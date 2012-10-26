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
#include <errno.h>
#include <fcntl.h>              /* low-level i/o */
#include <linux/input.h>

#include <QDebug>

#include "inputEvent.h"

InputEvent::InputEvent()
{
    mFd = -1;
    mPause = 0;
    mDevName = NULL;
}

InputEvent::~InputEvent()
{
    if (mDevName != NULL) {
        closeDevice();
        free(mDevName);
    }
}

void InputEvent::SetDevice(const QString& dev)
{
    if (mDevName != NULL) {
        closeDevice();
        free(mDevName);
    }
    mDevName = (char *)malloc(100);
    strcpy(mDevName, dev.toLocal8Bit().data());
    openDevice();
    qDebug() << "SetDevice:" << dev;
}

void InputEvent::run()
{
    //exec(); /* start the event loop */
    while (1) {
        QThread::msleep((int)(10));
        if (!mPause)
            mainloop();
    }
}

void InputEvent::pause()
{
    mPause = 1;
}

void InputEvent::resume()
{
    mPause = 0;
}

bool InputEvent::isPaused()
{
    if (mPause)
        return true;
    return false;
}

void InputEvent::mainloop(void)
{
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

        if (readDevice())
            break;
        if (EAGAIN != r)
            break;
        /* EAGAIN - continue select loop. */
    }
}

void InputEvent::emitError(const char *s)
{
    qDebug() << "[Error] " << QString("%1").arg(s);
    emit error(QString("%1").arg(s));
}

int InputEvent::readDevice()
{
    struct input_event event;
    memset(&event, 0, sizeof(event));

    if (-1 == read(mFd, &event, sizeof(event))) {
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

    if (event.code == KEY_POWER)
        emit powerPressed();
    else if (event.code == KEY_VOLUMEDOWN)
        emit volumDownPressed();
    else if (event.code == KEY_VOLUMEUP)
        emit volumUpPressed();

    return 0;
}

void InputEvent::closeDevice(void)
{
    if (-1 == close(mFd))
        emitError("close");

    mFd = -1;
    qDebug() << "Device close";
}

int InputEvent::openDevice(void)
{
    mFd = open(mDevName, O_RDWR /* required */ | O_NONBLOCK, 0);
    if (-1 == mFd) {
        fprintf(stderr, "Cannot open '%s': %d, %s\n",
                mDevName, errno, strerror(errno));
        return -1;
    }
    qDebug() << "Device open";
}
