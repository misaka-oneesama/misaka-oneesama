#ifndef PLUGININTERFACE_HPP
#define PLUGININTERFACE_HPP

#include <QDiscord>

class PluginInterface : public QObject
{
    Q_OBJECT

public:
    PluginInterface(QDiscord *discord);
    virtual ~PluginInterface() = 0;

    virtual void selfCreated(QSharedPointer<QDiscordUser> user) = 0;

    virtual void messageReceived(const QDiscordMessage &message) = 0;
    virtual void messageUpdated(const QDiscordMessage &message, const QDateTime &timestamp) = 0;
    virtual void messageDeleted(const QDiscordMessage &message) = 0;

private:
    QDiscord *d; // pointer to the active QDiscord components
};

#endif // PLUGININTERFACE_HPP
