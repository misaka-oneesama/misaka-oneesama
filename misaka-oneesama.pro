QT += core network websockets
QT -= gui

CONFIG += c++14

TARGET = misaka-oneesama
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

# Source files
include(depends.pri)

# Disables and refuse to compile all APIs deprecated before Qt 6.0.0 (the future is now ;P)
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

# Define DEBUG and RELEASE macros
debug {
    DEFINES += MISAKA_DEBUG
}
release {
    DEFINES += MISAKA_RELEASE
}
profile {
    DEFINES += MISAKA_PROFILING
}

# QtWebApp (C++ HTTP Server written in Qt)
include(External/QtWebApp/logging/logging.pri)
include(External/QtWebApp/httpserver/httpserver.pri)
include(External/QtWebApp/templateengine/templateengine.pri)

# QDiscord (C++ Discord API wrapper written in Qt)
include(External/QDiscord/QDiscord/src/depends.pri)
INCLUDEPATH += External/QDiscord/QDiscord/src
