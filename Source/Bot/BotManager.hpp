#ifndef BOTMANAGER_HPP
#define BOTMANAGER_HPP

#include <QObject>
#include <QMutex>
#include <QMutexLocker>
#include <QDBusVariant>

#include <QDiscord>
#include <Bot/DiscordEventHandler.hpp>

#include <memory>

class BotManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString token MEMBER m_token WRITE setOAuthToken)

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

    void setOAuthToken(const QString &token);
    const QString &token() const;

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
    QMutex m_mutex;

    std::unique_ptr<QDiscord> m_discord;
    std::unique_ptr<DiscordEventHandler> m_eventHandler;

    QString m_token;
};

class BotManagerDBusAdapter : public QObject
{
    Q_OBJECT

public:
    BotManagerDBusAdapter(BotManager *botManager, QObject *parent = nullptr);
    ~BotManagerDBusAdapter();

public slots:
    Q_NOREPLY void start();
    Q_NOREPLY void stop();
    Q_SCRIPTABLE bool reload();

    Q_NOREPLY void login();
    Q_NOREPLY void logout();

    Q_NOREPLY void setOAuthToken(const QString &token);

private:
    QMutex m_mutex;
    BotManager *d = nullptr;
};

#endif // BOTMANAGER_HPP
