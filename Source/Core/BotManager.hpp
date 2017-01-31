#ifndef BOTMANAGER_HPP
#define BOTMANAGER_HPP

#include <QObject>
#include <QDiscord>

#include <memory>

#include "DiscordEventHandler.hpp"

class BotManager : public QObject
{
    Q_OBJECT

public:
    BotManager(QObject *parent = nullptr);
    ~BotManager();

    enum NotifyCode : quint8 {
        LoginSuccess = 0,
        LoggedOut,
        Disconnected
    };

    enum ErrorCode : quint8 {
        LoginFailed = 0
    };

    void init(); // call AFTER `QThread::moveToThread()` !!
    void setOAuthToken(const QString &token);

public slots:
    void login();
    void login(const QString &token);
    void logout();

    void stop();

private slots:
    void loginSuccess();
    void loginFailed();
    void loggedOut();
    void disconnected();

signals:
    void notify(NotifyCode);
    void error(ErrorCode);

    void stopped();

private:
    std::unique_ptr<QDiscord> m_discord;
    std::unique_ptr<DiscordEventHandler> m_eventHandler;

    QString m_token;
};

#endif // BOTMANAGER_HPP
