#ifndef DBUSINTERFACE_HPP
#define DBUSINTERFACE_HPP

#include <QObject>
#include <QMutex>
#include <QMutexLocker>
#include <QDBusVariant>

#include <Core/IpcProcess.hpp>

class DBusInterface : public QObject
{
    Q_OBJECT

public:
    DBusInterface(QObject *parent = nullptr);
    ~DBusInterface();

    void setServerProcess(IpcProcess *server);
    void setBotProcess(IpcProcess *bot);

public slots:
    Q_NOREPLY void startServer();
    Q_NOREPLY void stopServer();
    Q_NOREPLY void restartServer();
    Q_SCRIPTABLE bool isServerRunning();

    Q_NOREPLY void startBot();
    Q_NOREPLY void stopBot();
    Q_NOREPLY void restartBot();
    Q_SCRIPTABLE bool isBotRunning();

private:
    QMutex m_mutex;

    IpcProcess *p_server = nullptr;
    IpcProcess *p_bot = nullptr;
};

#endif // DBUSINTERFACE_HPP
