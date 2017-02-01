#ifndef GLOBAL_HPP
#define GLOBAL_HPP

#include "Core/Debugger.hpp"
#include "Core/ConfigManager.hpp"
#include "Core/IpcProcess.hpp"

#include "Server/Server.hpp"
#include "Core/BotManager.hpp"

extern Debugger *debugger;              // can be used in every instance (unique)
extern ConfigManager *configManager;    // can ONLY be used in the [Master] instance

extern IpcProcess *ipcServer;           // [Master] only
extern IpcProcess *ipcBot;              // [Master] only

extern Server *server;                  // [Server] only
extern BotManager *botManager;          // [Bot] only

extern const QString dbus_service_name; // every instance

#endif // GLOBAL_HPP
