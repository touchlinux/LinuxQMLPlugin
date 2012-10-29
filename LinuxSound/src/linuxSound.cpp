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
#include <QProcess>
#include "linuxSound.h"

LinuxSound::LinuxSound(QDeclarativeItem *parent):
    QDeclarativeItem(parent),
    mThread(NULL)
{
    // By default, QDeclarativeItem does not draw anything. If you subclass
    // QDeclarativeItem to create a visual item, you will need to uncomment the
    // following line:
    
    // setFlag(ItemHasNoContents, false);
}

LinuxSound::~LinuxSound()
{
    if (mThread != NULL)
        delete mThread;
}

void LinuxSound::setSource(QString source)
{
    mSource = source;
}

void LinuxSound::setRate(int rate)
{
    if (rate == 22050 || rate == 44100) {
        mRate = rate;
    }
}

void LinuxSound::play()
{
    QStringList arguments;
    arguments << QString("-r %1").arg(mRate) <<  mSource;

    mThread = new SoundThread(QString("aplay"), arguments);
    mThread->start();
}
