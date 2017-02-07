/*****************************************************************************
 * 御坂ーお姉さま
 * Copyright (C) 2017 マギルゥ一ベルベット
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

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
