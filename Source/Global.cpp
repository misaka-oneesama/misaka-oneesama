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
