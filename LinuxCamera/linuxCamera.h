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

#ifndef LINUXV4L2_H
#define LINUXV4L2_H

#include <QtGui>
#include <QtDeclarative/QDeclarativeItem>
#include "v4l2.h"

class LinuxCamera : public QDeclarativeItem
{
    Q_OBJECT
    Q_DISABLE_COPY(LinuxCamera)
    Q_PROPERTY(QString capturedImage READ CapturedImage NOTIFY imageCaptured)
    Q_PROPERTY(QString devName READ DevName WRITE SetDevName)
    Q_PROPERTY(int width READ Width WRITE SetWidth)
    Q_PROPERTY(int height READ Height WRITE SetHeight)
    Q_PROPERTY(int framePerSec READ FramePerSec WRITE SetFramePerSec)
    Q_PROPERTY(bool pause READ Pause WRITE SetPause)
public:
    LinuxCamera(QDeclarativeItem *parent = 0);
    ~LinuxCamera();
#if QT_VERSION >= 0x040700
    static void registerQMLTypes()
    {
        qmlRegisterType<LinuxCamera>("LinuxV4L2", 1, 0, "LinuxV4L2");
    }
#endif
    QString CapturedImage();

public slots:
    void UpdateImage(const QImage& image);
    void SetDevName(const QString &devName);
    QString DevName();
    void SetWidth(int);
    int Width();
    void SetHeight(int);
    int Height();
    void SetFramePerSec(int);
    int FramePerSec();
    void SetPause(bool);
    bool Pause();
signals:
    void imageCaptured();
private:
    int mWidth;
    int mHeight;    
    int mFramePerSec;
    QString mDevName;
    V4L2 *mV4L2;
    QLabel *mView;
    QGraphicsProxyWidget *mProxy;
    QString mImageFile;
    void SetScreenSize(int width, int height);
};

QML_DECLARE_TYPE(LinuxCamera)

#endif // QCAPTURE_H

