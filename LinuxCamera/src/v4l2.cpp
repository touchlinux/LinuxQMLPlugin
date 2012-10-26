/*
 * LinuxQMLPlugin - QML/C++ plugin for linux kernel
 *
 * Copyright (C) 2012 Byungho Min <bhminjames@gmail.com>, TouchLinux
 * (LinuxInput is released under the GNU Lesser General Public License.)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <getopt.h>             /* getopt_long() */
#include <fcntl.h>              /* low-level i/o */
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>

#include <QDebug>
#include "v4l2.h"

V4L2::V4L2(int width, int height)
{
    mWidth = width;
    mHeight = height;
    mIO = IO_METHOD_MMAP;
    mFd = -1;
    mOutBuf = 1;
    mFrameCount = 1;
    mForceFormat = 1;
    mPause = 0;
    mFps = 30;
    mDevName = NULL;
}

V4L2::~V4L2()
{
    if (mDevName != NULL) {
        StopCapture();
        free(mDevName);
    }
}

void V4L2::SetDevice(const QString& dev)
{
    if (mDevName != NULL) {
        StopCapture();
        free(mDevName);
    }
    mDevName = (char *)malloc(100);
    strcpy(mDevName, dev.toLocal8Bit().data());
    StartCapture();
    qDebug() << "SetDevice:" << dev;
}

void V4L2::StopCapture()
{
    stop_capture_device();
    uninit_device();
    close_device();
}

void V4L2::StartCapture()
{
    if (open_device() < 0)
        goto open_err;
    if (init_device() < 0)
        goto init_err;
    if (start_capture_device() < 0)
        goto start_err;
    return; /* sucess */
start_err:
init_err:
    close_device();
open_err:
    mDevName = NULL;
}

void V4L2::run()
{
    //exec(); /* start the event loop */
    while (1) {
        QThread::msleep((int)(1000/mFps));
        if (!mPause)
            mainloop();
    }
}

void V4L2::pause()
{
    mPause = 1;
}

void V4L2::resume()
{
    mPause = 0;
}

bool V4L2::isPaused()
{
    if (mPause)
        return true;
    return false;
}

void V4L2::emitError(const char *s)
{
    qDebug() << "[Error] " << QString("%1").arg(s);
    emit error(QString("%1").arg(s));
}

int V4L2::xioctl(int fh, int request, void *arg)
{
    int r;

    do {
        r = ioctl(fh, request, arg);
    } while (-1 == r && EINTR == errno);

    return r;
}

QImage V4L2::convertFromYUV422(const QByteArray &array, int w, int h)
{
    QImage result(w, h, QImage::Format_RGB32);
    // TODO: bilinearly interpolate the U and V channels for better result

    for (int y = 0; y < h; ++y) {
        uint *sp = (uint *)result.scanLine(y);
        const uchar *yp = (const uchar *)(array.constData() + y * w * 2);
        const uchar *up = (const uchar *)(array.constData() + y * w * 2 + 1);
        const uchar *vp = (const uchar *)(array.constData() + y * w * 2 + 3);

        for (int x = 0; x < w; ++x) {
            const int sy = *yp;
            const int su = *up;
            const int sv = *vp;
            const int R = int(1.164 * (sy - 16) + 1.596 * (sv - 128));
            const int G = int(1.164 * (sy - 16) - 0.813 * (sv - 128) - 0.391 * (su - 128));
            const int B = int(1.164 * (sy - 16) + 2.018 * (su - 128));

            *sp = qRgb(qBound(0, R, 255),
                       qBound(0, G, 255),
                       qBound(0, B, 255));
            ++sp;

            yp += 2;
            if (x & 1) {
                up += 4;
                vp += 4;
            }
        }
    }

    return result;
}

void V4L2::process_image(const void *p, int size)
{
    if (mOutBuf) {
        QByteArray array;
        array.append((const char *)p, size);
        mImage = convertFromYUV422(array, mWidth, mHeight);
        emit captured(mImage);
    }
}

int V4L2::read_frame(void)
{
    struct v4l2_buffer buf;
    unsigned int i;

    switch (mIO) {
    case IO_METHOD_READ:
        if (-1 == read(mFd, mBuffer[0].start, mBuffer[0].length)) {
            switch (errno) {
            case EAGAIN:
                return 0;

            case EIO:
                /* Could ignore EIO, see spec. */

                /* fall through */

            default:
                emitError("read");
                return -1;
            }
        }

        process_image(mBuffer[0].start, mBuffer[0].length);
        break;

    case IO_METHOD_MMAP:
        CLEAR(buf);

        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;

        if (-1 == xioctl(mFd, VIDIOC_DQBUF, &buf)) {
            switch (errno) {
            case EAGAIN:
                return 0;

            case EIO:
                /* Could ignore EIO, see spec. */

                /* fall through */

            default:
                emitError("VIDIOC_DQBUF");
                return -1;
            }
        }

        assert(buf.index < mNumBuffers);

        process_image(mBuffer[buf.index].start, buf.bytesused);

        if (-1 == xioctl(mFd, VIDIOC_QBUF, &buf)) {
            emitError("VIDIOC_QBUF");
            return -1;
        }
        break;

    case IO_METHOD_USERPTR:
        CLEAR(buf);

        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_USERPTR;

        if (-1 == xioctl(mFd, VIDIOC_DQBUF, &buf)) {
            switch (errno) {
            case EAGAIN:
                return 0;

            case EIO:
                /* Could ignore EIO, see spec. */

                /* fall through */

            default:
                emitError("VIDIOC_DQBUF");
                return -1;
            }
        }

        for (i = 0; i < mNumBuffers; ++i)
            if (buf.m.userptr == (unsigned long)mBuffer[i].start
                    && buf.length == mBuffer[i].length)
                break;

        assert(i < mNumBuffers);

        process_image((void *)buf.m.userptr, buf.bytesused);

        if (-1 == xioctl(mFd, VIDIOC_QBUF, &buf)) {
            emitError("VIDIOC_QBUF");
            return -1;
        }
        break;
    }

    return 1;
}

void V4L2::mainloop(void)
{
    unsigned int count;
    count = mFrameCount;

    for (;;) {
        if (mFd == -1)
            continue;
        fd_set fds;
        struct timeval tv;
        int r;

        FD_ZERO(&fds);
        FD_SET(mFd, &fds);

        /* Timeout. */
        tv.tv_sec = 2;
        tv.tv_usec = 0;

        r = select(mFd + 1, &fds, NULL, NULL, &tv);

        if (-1 == r) {
            if (EINTR == errno)
                continue;
            emitError("select");
            return;
        }

        if (0 == r) {
            emitError("select timeout");
            return;
        }

        if (read_frame())
            break;
        if (EAGAIN != r)
            break;
        /* EAGAIN - continue select loop. */
    }
}

int V4L2::stop_capture_device(void)
{
    enum v4l2_buf_type type;

    switch (mIO) {
    case IO_METHOD_READ:
        /* Nothing to do. */
        break;

    case IO_METHOD_MMAP:
    case IO_METHOD_USERPTR:
        type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        if (-1 == xioctl(mFd, VIDIOC_STREAMOFF, &type)) {
            fprintf(stderr, "VIDIOC_STREAMOFF");
            return -1;
        }
        break;
    }
    return 0;
}

int V4L2::start_capture_device(void)
{
    unsigned int i;
    enum v4l2_buf_type type;

    switch (mIO) {
    case IO_METHOD_READ:
        /* Nothing to do. */
        break;

    case IO_METHOD_MMAP:
        for (i = 0; i < mNumBuffers; ++i) {
            struct v4l2_buffer buf;

            CLEAR(buf);
            buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            buf.memory = V4L2_MEMORY_MMAP;
            buf.index = i;

            if (-1 == xioctl(mFd, VIDIOC_QBUF, &buf)) {
                fprintf(stderr, "VIDIOC_QBUF");
                return -1;
            }
        }
        type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        if (-1 == xioctl(mFd, VIDIOC_STREAMON, &type)) {
            fprintf(stderr, "VIDIOC_STREAMON");
            return -1;
        }
        break;

    case IO_METHOD_USERPTR:
        for (i = 0; i < mNumBuffers; ++i) {
            struct v4l2_buffer buf;

            CLEAR(buf);
            buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            buf.memory = V4L2_MEMORY_USERPTR;
            buf.index = i;
            buf.m.userptr = (unsigned long)mBuffer[i].start;
            buf.length = mBuffer[i].length;

            if (-1 == xioctl(mFd, VIDIOC_QBUF, &buf)) {
                fprintf(stderr, "VIDIOC_QBUF");
                return -1;
            }
        }
        type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        if (-1 == xioctl(mFd, VIDIOC_STREAMON, &type)) {
            fprintf(stderr, "VIDIOC_STREAMON");
            return -1;
        }
        break;
    }
    return 0;
}

void V4L2::uninit_device(void)
{
    unsigned int i;

    switch (mIO) {
    case IO_METHOD_READ:
        free(mBuffer[0].start);
        break;

    case IO_METHOD_MMAP:
        for (i = 0; i < mNumBuffers; ++i)
            if (-1 == munmap(mBuffer[i].start, mBuffer[i].length))
                emitError("munmap");
        break;

    case IO_METHOD_USERPTR:
        for (i = 0; i < mNumBuffers; ++i)
            free(mBuffer[i].start);
        break;
    }

    free(mBuffer);
}

void V4L2::init_read(unsigned int buffer_size)
{
    mBuffer = (struct buffer *)calloc(1, sizeof(*mBuffer));

    if (!mBuffer) {
        fprintf(stderr, "Out of memory\n");
        exit(EXIT_FAILURE);
    }

    mBuffer[0].length = buffer_size;
    mBuffer[0].start = malloc(buffer_size);

    if (!mBuffer[0].start) {
        fprintf(stderr, "Out of memory\n");
        exit(EXIT_FAILURE);
    }
}

void V4L2::init_mmap(void)
{
    struct v4l2_requestbuffers req;

    CLEAR(req);

    req.count = 4;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    if (-1 == xioctl(mFd, VIDIOC_REQBUFS, &req)) {
        if (EINVAL == errno) {
            fprintf(stderr, "%s does not support "
                    "memory mapping\n", mDevName);
            exit(EXIT_FAILURE);
        } else {
            emitError("VIDIOC_REQBUFS");
        }
    }

    if (req.count < 2) {
        fprintf(stderr, "Insufficient buffer memory on %s\n",
                mDevName);
        exit(EXIT_FAILURE);
    }

    mBuffer = (struct buffer *)calloc(req.count, sizeof(*mBuffer));
    if (!mBuffer) {
        fprintf(stderr, "Out of memory\n");
        exit(EXIT_FAILURE);
    }

    for (mNumBuffers = 0; mNumBuffers < req.count; ++mNumBuffers) {
        struct v4l2_buffer buf;

        CLEAR(buf);

        buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory      = V4L2_MEMORY_MMAP;
        buf.index       = mNumBuffers;

        if (-1 == xioctl(mFd, VIDIOC_QUERYBUF, &buf))
            emitError("VIDIOC_QUERYBUF");

        mBuffer[mNumBuffers].length = buf.length;
        mBuffer[mNumBuffers].start =
                mmap(NULL /* start anywhere */,
                     buf.length,
                     PROT_READ | PROT_WRITE /* required */,
                     MAP_SHARED /* recommended */,
                     mFd, buf.m.offset);

        if (MAP_FAILED == mBuffer[mNumBuffers].start)
            emitError("mmap");
    }
}

void V4L2::init_userp(unsigned int buffer_size)
{
    struct v4l2_requestbuffers req;

    CLEAR(req);

    req.count  = 4;
    req.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_USERPTR;

    if (-1 == xioctl(mFd, VIDIOC_REQBUFS, &req)) {
        if (EINVAL == errno) {
            fprintf(stderr, "%s does not support "
                    "user pointer i/o\n", mDevName);
            exit(EXIT_FAILURE);
        } else {
            emitError("VIDIOC_REQBUFS");
        }
    }

    mBuffer = (struct buffer *)calloc(4, sizeof(*mBuffer));

    if (!mBuffer) {
        fprintf(stderr, "Out of memory\n");
        exit(EXIT_FAILURE);
    }

    for (mNumBuffers = 0; mNumBuffers < 4; ++mNumBuffers) {
        mBuffer[mNumBuffers].length = buffer_size;
        mBuffer[mNumBuffers].start = malloc(buffer_size);

        if (!mBuffer[mNumBuffers].start) {
            fprintf(stderr, "Out of memory\n");
            exit(EXIT_FAILURE);
        }
    }
}

int V4L2::init_device()
{
    struct v4l2_capability cap;
    struct v4l2_cropcap cropcap;
    struct v4l2_crop crop;
    struct v4l2_format fmt;
    unsigned int min;

    if (-1 == xioctl(mFd, VIDIOC_QUERYCAP, &cap)) {
        if (EINVAL == errno) {
            fprintf(stderr, "%s is no V4L2 device\n",
                    mDevName);
            return -1;
        } else {
            fprintf(stderr, "VIDIOC_QUERYCAP");
            return -1;
        }
    }

    if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
        fprintf(stderr, "%s is no video capture device\n",
                mDevName);
        return -1;
    }

    switch (mIO) {
    case IO_METHOD_READ:
        if (!(cap.capabilities & V4L2_CAP_READWRITE)) {
            fprintf(stderr, "%s does not support read i/o\n",
                    mDevName);
            return -1;
        }
        break;

    case IO_METHOD_MMAP:
    case IO_METHOD_USERPTR:
        if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
            fprintf(stderr, "%s does not support streaming i/o\n",
                    mDevName);
            return -1;
        }
        break;
    }

    /* Select video input, video standard and tune here. */


    CLEAR(cropcap);

    cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if (0 == xioctl(mFd, VIDIOC_CROPCAP, &cropcap)) {
        crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        crop.c = cropcap.defrect; /* reset to default */

        if (-1 == xioctl(mFd, VIDIOC_S_CROP, &crop)) {
            switch (errno) {
            case EINVAL:
                /* Cropping not supported. */
                break;
            default:
                /* Errors ignored. */
                break;
            }
        }
    } else {
        /* Errors ignored. */
    }

    CLEAR(fmt);

    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (mForceFormat) {
        fmt.fmt.pix.width       = mWidth;
        fmt.fmt.pix.height      = mHeight;
        fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV; /* each four bytes is two pixels */
        fmt.fmt.pix.field       = V4L2_FIELD_INTERLACED;

        if (-1 == xioctl(mFd, VIDIOC_S_FMT, &fmt)) {
            fprintf(stderr, "VIDIOC_S_FMT");
            return -1;
        }

        /* Note VIDIOC_S_FMT may change width and height. */
    } else {
        /* Preserve original settings as set by v4l2-ctl for example */
        if (-1 == xioctl(mFd, VIDIOC_G_FMT, &fmt)) {
            fprintf(stderr, "VIDIOC_G_FMT");
            return -1;
        }
    }

    /* Buggy driver paranoia. */
    min = fmt.fmt.pix.width * 2;
    if (fmt.fmt.pix.bytesperline < min)
        fmt.fmt.pix.bytesperline = min;
    min = fmt.fmt.pix.bytesperline * fmt.fmt.pix.height;
    if (fmt.fmt.pix.sizeimage < min)
        fmt.fmt.pix.sizeimage = min;

    switch (mIO) {
    case IO_METHOD_READ:
        init_read(fmt.fmt.pix.sizeimage);
        break;

    case IO_METHOD_MMAP:
        init_mmap();
        break;

    case IO_METHOD_USERPTR:
        init_userp(fmt.fmt.pix.sizeimage);
        break;
    }
}

void V4L2::close_device(void)
{
    if (-1 == close(mFd))
        emitError("close");

    mFd = -1;
    qDebug() << "Device close";
}

int V4L2::open_device(void)
{
    struct stat st;

    if (-1 == stat(mDevName, &st)) {
        fprintf(stderr, "Cannot identify '%s': %d, %s\n",
                mDevName, errno, strerror(errno));
        return -1;
    }

    if (!S_ISCHR(st.st_mode)) {
        fprintf(stderr, "%s is no device\n", mDevName);
        return -1;
    }

    mFd = open(mDevName, O_RDWR /* required */ | O_NONBLOCK, 0);
    if (-1 == mFd) {
        fprintf(stderr, "Cannot open '%s': %d, %s\n",
                mDevName, errno, strerror(errno));
        return -1;
    }
    qDebug() << "Device open";
}
