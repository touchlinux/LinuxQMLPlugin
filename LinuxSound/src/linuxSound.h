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

#ifndef LINUXSOUND_H
#define LINUXSOUND_H

#include <QtDeclarative/QDeclarativeItem>
#include <QtMultimedia/QAudioOutput>
#include <QFile>
#include <QTimer>
#include <QProcess>
#include "soundThread.h"

class LinuxSound : public QDeclarativeItem
{
    Q_OBJECT
    Q_DISABLE_COPY(LinuxSound)
    Q_PROPERTY(QString source READ source WRITE setSource)
    Q_PROPERTY(int rate READ rate WRITE setRate)
    Q_PROPERTY(bool overlap READ overlap WRITE setOverlap)
    Q_PROPERTY(int gap READ gap WRITE setGap)
    
public:
    LinuxSound(QDeclarativeItem *parent = 0);
    ~LinuxSound();
#if QT_VERSION >= 0x040700
    static void registerQMLTypes()
    {
        qmlRegisterType<LinuxSound>("LinuxSound", 1, 0, "LinuxSound");
    }
#endif
    QString source() { return mSource; }
    void setSource(QString source);
    int rate() { return mRate; }
    void setRate(int rate);
    bool overlap() { return mOverlap; }
    void setOverlap(bool overlap);
    int gap() { return mGapMsec; }
    void setGap(int msec);
    Q_INVOKABLE void play();
public slots:
    void processTimeout();
signals:
    void playStarted();
    void playFinished();

private:
    QString mSource;
    int mRate;
    bool mOverlap;
    SoundThread *mThread;
    QTimer *mGapTimer;
    int mGapMsec;
};

QML_DECLARE_TYPE(LinuxSound)

#endif // LINUXSOUND_H

