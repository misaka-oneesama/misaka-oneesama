#ifndef GLOBAL_HPP
#define GLOBAL_HPP

#include "Core/Debugger.hpp"
#include "Core/ConfigManager.hpp"

#include "Server/Server.hpp"
#include "Bot/BotManager.hpp"

class IpcProcess;
class QObject;

namespace Global
{

    enum class InstanceType : quint8 {
        Master = 0x0,
        Server = 0x1,
        Bot    = 0x2,

        None   = 0xF
    };

    // """""forward declare types""""" (get it? ¯\_(ツ)_/¯)
    // using `using Alias = Enum::Value` fails
#define ItMaster InstanceType::Master
#define ItServer InstanceType::Server
#define ItBot    InstanceType::Bot
#define ItNone   InstanceType::None

    const char *instanceName(const InstanceType &type);

    // can be used in every instance (every instance has its unique copy)
    extern Debugger *debugger;

    // can ONLY be used in the [Master] instance
    extern ConfigManager *configManager;

    // returns a pointer to the IPC process object, can ONLY be used in the [Master] instance
    void createIpcProcess(const InstanceType &type, QObject *parent = nullptr);
    IpcProcess *ipc(const InstanceType &type);

    // can ONLY be used in the [Server] instance
    extern Server *server;

    // can ONLY be used in the [Bot] instance
    extern BotManager *botManager;

    // read-only strings of the D-Bus service names
    const QString &DBusServiceName(const InstanceType &type);

} // namespace Global

using namespace Global;

#endif // GLOBAL_HPP
