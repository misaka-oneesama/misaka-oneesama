#include "Global.hpp"

Debugger *debugger = nullptr;
ConfigManager *configManager = nullptr;

IpcProcess *ipcServer = nullptr;
IpcProcess *ipcBot = nullptr;

Server *server = nullptr;
BotManager *botManager = nullptr;

const QString dbus_service_name = "moe.misaka-oneesama.discordbot";
