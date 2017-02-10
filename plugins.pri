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

INCLUDEPATH += $$PWD/Plugins

SOURCES += \ 
    $$PWD/Plugins/TestPlugin/TestPlugin.cpp \
    $$PWD/Plugins/CommandProcessor/CommandProcessor.cpp \
    $$PWD/Plugins/CommandProcessor/AbstractCommand.cpp \
    $$PWD/Plugins/CommandProcessor/TestCommand.cpp

HEADERS += \ 
    $$PWD/Plugins/TestPlugin/TestPlugin.hpp \
    $$PWD/Plugins/CommandProcessor/CommandProcessor.hpp \
    $$PWD/Plugins/CommandProcessor/AbstractCommand.hpp \
    $$PWD/Plugins/CommandProcessor/TestCommand.hpp
