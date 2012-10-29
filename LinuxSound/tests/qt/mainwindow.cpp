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

#include <QLabel>
#include <QDebug>
#include <QFileDialog>
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(const QString &devName, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    mThread(NULL)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    if (mThread != NULL)
        delete mThread;
    delete ui;
}

void MainWindow::on_playButton_pressed()
{
    QStringList arguments;
    arguments << QString("-r ") + ui->rateComboBox->currentText() << ui->fileNameEdit->text();
    mThread = new SoundThread(QString("aplay"), arguments);
    mThread->start();
}

void MainWindow::on_fileButton_pressed()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Open wav"), "../files", tr("PCM Files(*.wav)"));
    ui->fileNameEdit->setText(filename);
}
