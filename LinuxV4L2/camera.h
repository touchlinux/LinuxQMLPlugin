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

#ifndef V4L2_H
#define V4L2_H

#include <QObject>
#include <QImage>
#include <QLabel>
#include <QThread>

#define CLEAR(x) memset(&(x), 0, sizeof(x))

enum io_method {
    IO_METHOD_READ,
    IO_METHOD_MMAP,
    IO_METHOD_USERPTR
};

struct buffer {
    void   *start;
    size_t  length;
};

class Camera : public QThread
{
    Q_OBJECT
public:
    Camera(int width = 640, int height = 480);
    ~Camera();
    void SetDevice(const QString& dev);
    void StartCapture();
    void StopCapture();

    void mainloop(void);
    void pause(void);
    void resume(void);
    bool isPaused();
    void set_frame_per_sec(int fps) { mFps = fps; }
    int get_frame_per_sec() { return mFps; }
    QImage mImage;
protected:
    void run();
signals:
    void error(const QString&);
    void captured(const QImage&);
private:
    void emitError(const char *s);
    int xioctl(int fh, int request, void *arg);
    void process_image(const void *p, int size);
    int read_frame(void);
    void uninit_device();
    void init_read(unsigned int buffer_size);
    void init_mmap(void);
    void init_userp(unsigned int buffer_size);
    int init_device(void);
    void close_device(void);
    int open_device(void);
    int stop_capture_device(void);
    int start_capture_device(void);
    QImage convertFromYUV422(const QByteArray &array, int mWidth, int mHeight);

    int mPause;
    char *mDevName;
    enum io_method mIO;
    int mFd;
    buffer *mBuffer;
    unsigned int mNumBuffers;
    int mOutBuf;
    int mForceFormat;
    int mFrameCount;
    int mWidth, mHeight;
    int mFps;
};

#endif // V4L2_H
