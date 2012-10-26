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

#include <QtDeclarative/qdeclarative.h>
#include "linuxV4L2.h"

#define FILENAME    "/tmp/image"

LinuxV4L2::LinuxV4L2(QDeclarativeItem *parent):
    QDeclarativeItem(parent)
{
    // By default, QDeclarativeItem does not draw anything. If you subclass
    // QDeclarativeItem to create a visual item, you will need to uncomment the
    // following line:
    setFlag(ItemHasNoContents, false);

    mProxy = new QGraphicsProxyWidget(this);
    mProxy->setWidget(mView = new QLabel);

    mWidth = 640;
    mHeight = 480;

    SetScreenSize(mWidth, mHeight);
    mCamera = new Camera(mWidth, mHeight);
    connect(mCamera, SIGNAL(captured(QImage)), this, SLOT(UpdateImage(QImage)));
    mCamera->start();
}

LinuxV4L2::~LinuxV4L2()
{
    mCamera->terminate();
    mCamera->wait();
    delete mCamera;
}

void LinuxV4L2::SetScreenSize(int width, int height)
{
    QRect screen_size(0, 0, width, height);

    screen_size.moveTo(0, 0);
    QSizeF background_size;
    background_size.setHeight(screen_size.height());
    background_size.setWidth(screen_size.width());
    setSize(background_size);

    mView->setGeometry(screen_size);
    mProxy->setGeometry(screen_size);
}

QString LinuxV4L2::CapturedImage()
{
    return mImageFile;
}

void LinuxV4L2::UpdateImage(const QImage& image)
{
    static int order = 0;
    order = (order + 1) % 5;
    mImageFile = QString(FILENAME) + QString("%1").arg(order);
    image.save(mImageFile, "BMP");
    mView->setPixmap(QPixmap::fromImage(image));
    emit imageCaptured();
}

void LinuxV4L2::SetDevName(const QString &devName)
{
    mDevName = devName;
    mCamera->SetDevice(mDevName);
}

QString LinuxV4L2::DevName()
{
    return mDevName;
}

void LinuxV4L2::SetWidth(int width)
{
    mWidth = width;
    SetScreenSize(mWidth, mHeight);
}

int LinuxV4L2::Width()
{
    return mWidth;
}

void LinuxV4L2::SetHeight(int height)
{
    mHeight = height;
    SetScreenSize(mWidth, mHeight);
}

int LinuxV4L2::Height()
{
    return mHeight;
}

void LinuxV4L2::SetFramePerSec(int fps)
{
    mCamera->set_frame_per_sec(fps);
}

int LinuxV4L2::FramePerSec()
{
    return mCamera->get_frame_per_sec();
}

void LinuxV4L2::SetPause(bool flag)
{
    if (flag == true)
        mCamera->pause();
    else
        mCamera->resume();
}

bool LinuxV4L2::Pause()
{
    return mCamera->isPaused();
}
