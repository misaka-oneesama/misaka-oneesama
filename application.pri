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
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

INCLUDEPATH += $$PWD/Source

# FIXME: this is getting ridiculous, find a way to GLOB recursive this easily (**/*.cpp)
# On the other side I plan to switch to CMake soon™ anyway.
SOURCES += \
    $$PWD/Source/main.cpp \
    $$PWD/Source/Global.cpp \
    \
    $$PWD/Source/Core/Debugger.cpp \
    $$PWD/Source/Core/ConfigManager.cpp \
    $$PWD/Source/Core/ThreadId.cpp \
    $$PWD/Source/Core/IpcProcess.cpp \
    $$PWD/Source/Core/DBusInterface.cpp \
    $$PWD/Source/Core/PluginInterface.cpp \
    \
    $$PWD/Source/Server/Server.cpp \
    $$PWD/Source/Server/RequestMapper.cpp \
    $$PWD/Source/Server/API/Base.cpp \
    $$PWD/Source/Server/API/BotController.cpp \
    \
    $$PWD/Source/Bot/BotManager.cpp \
    $$PWD/Source/Bot/DiscordEventHandler.cpp

HEADERS += \
    $$PWD/Source/Global.hpp \
    \
    $$PWD/Source/Core/Debugger.hpp \
    $$PWD/Source/Core/ConfigManager.hpp \
    $$PWD/Source/Core/ThreadId.hpp \
    $$PWD/Source/Core/IpcProcess.hpp \
    $$PWD/Source/Core/DBusInterface.hpp \
    $$PWD/Source/Core/PluginInterface.hpp \
    \
    $$PWD/Source/Server/Server.hpp \
    $$PWD/Source/Server/RequestMapper.hpp \
    \
    $$PWD/Source/Bot/BotManager.hpp \
    $$PWD/Source/Bot/DiscordEventHandler.hpp
