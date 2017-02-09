#include "TestPlugin.hpp"

#include <Global.hpp>

TestPlugin::TestPlugin(QDiscord *discord)
    : PluginInterface(discord)
{
}

TestPlugin::~TestPlugin()
{
}

//void TestPlugin::selfCreated(QSharedPointer<QDiscordUser> user)
//{

//}

void TestPlugin::messageReceived(const QDiscordMessage &message)
{
    debugger->notice(" <<<<<<<< TestPlugin: messageReceived: " + message.content());
    QThread::sleep(10);
    debugger->notice(" <<<<<<<< TestPlugin: WAITED 10 SECONDS");
}

//void TestPlugin::messageUpdated(const QDiscordMessage &message, const QDateTime &timestamp)
//{

//}

//void TestPlugin::messageDeleted(const QDiscordMessage &message)
//{

//}
