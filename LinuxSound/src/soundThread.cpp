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

#include <QDebug>
#include "soundThread.h"

SoundThread::SoundThread(const QString program, const QStringList arguments) :
    mProgram(program),
    mArguments(arguments)
{
    //qDebug() << __FUNCTION__ << "is called";
    connect(this, SIGNAL(finished()), this, SLOT(destroyItself()));
}

SoundThread::~SoundThread()
{
    //qDebug() << __FUNCTION__ << "is called";
    if (mProcess != NULL)
        delete mProcess;
}

void SoundThread::run()
{
    //qDebug() << __FUNCTION__ << "is called";
    mProcess = new QProcess;
    mProcess->start(mProgram, mArguments);
    mProcess->waitForFinished();
    /* playing */
}

void SoundThread::destroyItself()
{
    quit();
    delete this;
}
