#ifndef BOTMANAGER_HPP
#define BOTMANAGER_HPP

#include <QObject>
#include <QDiscord>

class BotManager : public QObject
{
    Q_OBJECT

public:
    BotManager(QObject *parent = nullptr);
    ~BotManager();

    enum NotifyCode : quint8 {
        LoginSuccess = 0,
        LoggedOut
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

signals:
    void notify(NotifyCode);
    void error(ErrorCode);

    void stopped();

private:
    QDiscord *m_discord = nullptr;

    QString m_token;
};

#endif // BOTMANAGER_HPP
