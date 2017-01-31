#ifndef DISCORDEVENTHANDLER_HPP
#define DISCORDEVENTHANDLER_HPP

#include <QDiscord>
#include <QObject>

class DiscordEventHandler : public QObject
{
    Q_OBJECT

public:
    explicit DiscordEventHandler(QDiscord *discord, QObject *parent = nullptr);
    ~DiscordEventHandler();

private slots:
    void messageReceived(const QDiscordMessage &message);
    void messageUpdated(const QDiscordMessage &message, const QDateTime &timestamp);
    void messageDeleted(const QDiscordMessage &message);

private:
    QDiscord *m_discord = nullptr;
};

#endif // DISCORDEVENTHANDLER_HPP
