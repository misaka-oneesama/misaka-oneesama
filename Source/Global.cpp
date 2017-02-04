#include "Global.hpp"

Debugger *debugger = nullptr;
ConfigManager *configManager = nullptr;

IpcProcess *ipcServer = nullptr;
IpcProcess *ipcBot = nullptr;

Server *server = nullptr;
BotManager *botManager = nullptr;

const QString dbus_service_name = "moe.misaka-oneesama";
const QString dbus_service_name_server = dbus_service_name + ".Server";
const QString dbus_service_name_bot = dbus_service_name + ".Bot";
