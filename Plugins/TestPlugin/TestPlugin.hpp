#ifndef TESTPLUGIN_HPP
#define TESTPLUGIN_HPP

#include <Core/PluginInterface.hpp>

class TestPlugin : public PluginInterface
{
    Q_OBJECT

public:
    TestPlugin(QDiscord *discord);
    ~TestPlugin();

    //void selfCreated(QSharedPointer<QDiscordUser> user);

    void messageReceived(const QDiscordMessage &message);
    //void messageUpdated(const QDiscordMessage &message, const QDateTime &timestamp);
    //void messageDeleted(const QDiscordMessage &message);
};

#endif // TESTPLUGIN_HPP
