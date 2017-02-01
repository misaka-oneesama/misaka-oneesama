#ifndef GLOBAL_HPP
#define GLOBAL_HPP

#include "Core/Debugger.hpp"
#include "Core/ConfigManager.hpp"
#include "Core/IpcProcess.hpp"

#include "Server/Server.hpp"
#include "Core/BotManager.hpp"

extern Debugger *debugger;
extern ConfigManager *configManager;

extern IpcProcess *ipcServer;
extern IpcProcess *ipcBot;

extern Server *server;
extern BotManager *botManager;

extern const QString dbus_service_name;

#endif // GLOBAL_HPP
