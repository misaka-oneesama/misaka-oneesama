# 御坂ーお姉さま
# Copyright (C) 2017 マギルゥ一ベルベット
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.

QT += core concurrent network websockets dbus sql
QT -= gui

CONFIG += c++14

TARGET = misaka-oneesama
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

# Source files
include(application.pri)
include(plugins.pri)

# Disables and refuse to compile all APIs deprecated before Qt 6.0.0 (the future is now ;P)
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

# Define DEBUG and RELEASE macros
CONFIG (debug, debug|release) {
    DEFINES += MISAKA_DEBUG
} else {
    DEFINES += MISAKA_RELEASE
}

# QtWebApp (C++ HTTP Server written in Qt)
include(External/QtWebApp/HttpServer.pri)
#include(External/QtWebApp/templateengine/templateengine.pri)
#include(External/QtWebApp/logging/logging.pri)

# QDiscord (C++ Discord API wrapper written in Qt)
include(External/QDiscord/QDiscord/src/depends.pri)
INCLUDEPATH += External/QDiscord/QDiscord/src
