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

#ifndef INPUTEVENT_H
#define INPUTEVENT_H

#include <QObject>
#include <QImage>
#include <QLabel>
#include <QThread>

class InputEvent : public QThread
{
    Q_OBJECT
    enum event_type {PWR_BTN_PRESSED, VOLUP_BTN_PRESSED, VOLDOWN_BTN_PRESSED};
public:
    InputEvent();
    ~InputEvent();
    void SetDevice(const QString& dev);
    void mainloop(void);
    void pause(void);
    void resume(void);
    bool isPaused();
protected:
    void run();
signals:
    void error(const QString&);
    void powerPressed();
    void volumDownPressed();
    void volumUpPressed();
private:
    void emitError(const char *s);
    int readDevice(void);
    void closeDevice(void);
    int openDevice(void);
    int mFd;
    int mPause;
    char *mDevName;
};

#endif // INPUTEVENT_H
