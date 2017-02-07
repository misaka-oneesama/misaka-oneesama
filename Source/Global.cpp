/*****************************************************************************
 * 御坂ーお姉さま
 * Copyright (C) 2017 マギルゥ一ベルベット
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "Global.hpp"

#include <Core/IpcProcess.hpp>
#include <Core/ConfigManager.hpp>

#include <QCoreApplication>
#include <QObject>
#include <QMap>

static const QString dbus_service_name = "moe.misaka_oneesama";
static const QString dbus_service_name_server = dbus_service_name + ".Server";
static const QString dbus_service_name_bot = dbus_service_name + ".Bot";

quint8 Global::reasonToInt(const TerminateReason &reason)
{
    return static_cast<quint8>(reason);
}

const char *Global::instanceName(const InstanceType &type)
{
    switch (type)
    {
        case InstanceType::Master: return "Master"; break;
        case InstanceType::Server: return "Server"; break;
        case InstanceType::Bot:    return "Bot"; break;
        default: return ""; break;
    }
}

Debugger *Global::debugger = nullptr;
ConfigManager *Global::configManager = nullptr;
ConfigDirectory *Global::configDir = nullptr;

QMap<InstanceType, IpcProcess*> ipcMap;

void Global::createIpcProcess(const InstanceType &type, ConfigManager *cfg, QObject *parent)
{
    // create new process instance
    if (!ipcMap.contains(type))
    {
        ipcMap.insert(type, new IpcProcess(parent));

        // configure
        ipcMap.value(type)->setProgram(qApp->arguments().at(0));
        ipcMap.value(type)->setIdentifier(type);
        ipcMap.value(type)->redirectOutput(cfg->debuggerPrintToTerminal());

        // signal connections
        QObject::connect(static_cast<QCoreApplication*>(parent), &QCoreApplication::aboutToQuit, ipcMap.value(type), &IpcProcess::terminate);
        QObject::connect(static_cast<QCoreApplication*>(parent), &QCoreApplication::aboutToQuit, ipcMap.value(type), &IpcProcess::deleteLater);
    }

}

IpcProcess *Global::ipc(const InstanceType &type)
{
    return ipcMap.contains(type) ? ipcMap.value(type) : nullptr;
}

Server *Global::server = nullptr;
BotManager *Global::botManager = nullptr;

const QString &Global::DBusServiceName(const InstanceType &type)
{
    static const QString invalid;

    switch (type)
    {
        case InstanceType::Master: return dbus_service_name; break;
        case InstanceType::Server: return dbus_service_name_server; break;
        case InstanceType::Bot: return dbus_service_name_bot; break;
        default: return invalid; break;
    }
}
