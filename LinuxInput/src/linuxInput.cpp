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

#include <QProcess>
#include <QtDeclarative/qdeclarative.h>

#include "linuxInput.h"

LinuxInput::LinuxInput(QDeclarativeItem *parent):
    QDeclarativeItem(parent)
{
    // By default, QDeclarativeItem does not draw anything. If you subclass
    // QDeclarativeItem to create a visual item, you will need to uncomment the
    // following line:
    //setFlag(ItemHasNoContents, false);

    mEvent = new InputEvent;
    connect(mEvent, SIGNAL(powerPressed()), this, SIGNAL(powerPressed()));
    connect(mEvent, SIGNAL(volumDownPressed()), this, SIGNAL(volumDownPressed()));
    connect(mEvent, SIGNAL(volumUpPressed()), this, SIGNAL(volumUpPressed()));
    mEvent->start();
}

LinuxInput::~LinuxInput()
{
    mEvent->terminate();
    mEvent->wait();
    delete mEvent;
}

QString LinuxInput::DevName()
{
    return mDevName;
}

void LinuxInput::SetDevName(const QString &devName)
{
    mDevName = devName;
    mEvent->SetDevice(mDevName);
}

void LinuxInput::shutdown(int afterSec)
{
    //QString cmd = QString("shutdown -h -t %1").arg(afterSec);
    QString cmd = QString("halt");
    //system(cmd.toStdString().c_str());
    QProcess::execute(cmd);
}
