#include "Global.hpp"

#include <Core/IpcProcess.hpp>
#include <QObject>

static const QString dbus_service_name = "moe.misaka_oneesama";
static const QString dbus_service_name_server = dbus_service_name + ".Server";
static const QString dbus_service_name_bot = dbus_service_name + ".Bot";

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

IpcProcess *ipcServer = nullptr;
IpcProcess *ipcBot = nullptr;

void Global::createIpcProcess(const InstanceType &type, QObject *parent)
{
    switch (type)
    {
        case InstanceType::Server: ipcServer = new IpcProcess(parent); break;
        case InstanceType::Bot:    ipcBot    = new IpcProcess(parent); break;
        default: break;
    }
}

IpcProcess *Global::ipc(const InstanceType &type)
{
    switch (type)
    {
        case InstanceType::Server: return ipcServer; break;
        case InstanceType::Bot:    return ipcBot; break;
        default: return nullptr; break;
    }
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
