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
    Q_NOREPLY void terminate();

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
