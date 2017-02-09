#ifndef PLUGININTERFACE_HPP
#define PLUGININTERFACE_HPP

#include <QDiscord>

class PluginInterface : public QObject
{
    Q_OBJECT

public:
    PluginInterface(QDiscord *discord);
    virtual ~PluginInterface() = 0;

    virtual void selfCreated(QSharedPointer<QDiscordUser> user);

    virtual void messageReceived(const QDiscordMessage &message);
    virtual void messageUpdated(const QDiscordMessage &message, const QDateTime &timestamp);
    virtual void messageDeleted(const QDiscordMessage &message);

protected:
    QDiscord *d; // pointer to the active QDiscord components
};

#endif // PLUGININTERFACE_HPP
