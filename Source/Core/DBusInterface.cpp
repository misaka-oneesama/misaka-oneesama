#include "DBusInterface.hpp"

#include <QCoreApplication>
#include <Global.hpp>

DBusInterface::DBusInterface(QObject *parent)
    : QObject(parent)
{
}

DBusInterface::~DBusInterface()
{
}

void DBusInterface::setServerProcess(IpcProcess *server)
{
    QMutexLocker(&this->m_mutex);
    this->p_server = server;
}

void DBusInterface::setBotProcess(IpcProcess *bot)
{
    QMutexLocker(&this->m_mutex);
    this->p_bot = bot;
}

void DBusInterface::terminate()
{
    QMutexLocker(&this->m_mutex);
    debugger->notice("D-Bus: Terminating application...");
    qApp->quit();
}

void DBusInterface::startServer()
{
    QMutexLocker(&this->m_mutex);

    if (!this->p_server->isRunning())
    {
        debugger->notice("D-Bus: Starting server...");
        this->p_server->start(IpcProcess::ReadOnly);
        debugger->notice("D-Bus: Server started.");
    }

    else
    {
        debugger->notice("D-Bus: Server is already running.");
    }
}

void DBusInterface::stopServer()
{
    QMutexLocker(&this->m_mutex);

    if (this->p_server->isRunning())
    {
        debugger->notice("D-Bus: Stopping server...");
        this->p_server->terminate();
        debugger->notice("D-Bus: Server stopped.");
    }

    else
    {
        debugger->notice("D-Bus: Server is not running.");
    }
}

void DBusInterface::restartServer()
{
    QMutexLocker(&this->m_mutex);
    this->stopServer();
    this->startServer();
}

bool DBusInterface::isServerRunning()
{
    return this->p_server->isRunning();
}

void DBusInterface::startBot()
{
    QMutexLocker(&this->m_mutex);

    if (!this->p_bot->isRunning())
    {
        debugger->notice("D-Bus: Starting bot...");
        this->p_bot->start(IpcProcess::ReadOnly);
        debugger->notice("D-Bus: Bot started.");
    }

    else
    {
        debugger->notice("D-Bus: Bot is already running.");
    }
}

void DBusInterface::stopBot()
{
    QMutexLocker(&this->m_mutex);

    if (this->p_bot->isRunning())
    {
        debugger->notice("D-Bus: Stopping bot...");
        this->p_bot->terminate();
        debugger->notice("D-Bus: Bot stopped.");
    }

    else
    {
        debugger->notice("D-Bus: Bot is not running.");
    }
}

void DBusInterface::restartBot()
{
    QMutexLocker(&this->m_mutex);
    this->stopBot();
    this->startBot();
}

bool DBusInterface::isBotRunning()
{
    return this->p_bot->isRunning();
}
