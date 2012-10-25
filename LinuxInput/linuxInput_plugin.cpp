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

#include "linuxInput_plugin.h"
#include "linuxInput.h"

void LinuxInputPlugin::registerTypes(const char *uri)
{
    // @uri com.touchlinux.qmlcomponents
    qmlRegisterType<LinuxInput>(uri, 1, 0, "LinuxInput");
}

Q_EXPORT_PLUGIN2(LinuxInput, LinuxInputPlugin)

