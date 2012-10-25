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

#ifndef LINUXINPUT_H
#define LINUXINPUT_H

#include <QObject>
#include <QtDeclarative/QDeclarativeItem>

#include "inputEvent.h"

class LinuxInput : public QDeclarativeItem
{
    Q_OBJECT
    Q_DISABLE_COPY(LinuxInput)
    Q_PROPERTY(QString devName READ DevName WRITE SetDevName)
public:
    LinuxInput(QDeclarativeItem *parent = 0);
    ~LinuxInput();
#if QT_VERSION >= 0x040700
    static void registerQMLTypes()
    {
        qmlRegisterType<LinuxInput>("LinuxInput", 1, 0, "LinuxInput");
    }
#endif
public slots:
    void shutdown(int);
    void SetDevName(const QString &devName);
    QString DevName();
signals:
    void powerPressed();
    void volumDownPressed();
    void volumUpPressed();
private:
    InputEvent *mEvent;
    QString mDevName;
};

QML_DECLARE_TYPE(LinuxInput)

#endif // QSYSTEM_H

