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

#include "Server.hpp"
#include <Global.hpp>

#include <QThread>
#include <QTcpSocket>

#include <chrono>
#include <random>

Server::Server(QObject *parent)
    : QObject(parent)
{
    this->m_httpServerSettings.reset(new QtWebApp::HttpServerSettings);
}

Server::Server(const QString &listeningAddress, const quint16 &listeningPort, QObject *parent)
    : Server(parent)
{
    this->m_listeningAddress = listeningAddress;
    this->m_listeningPort = listeningPort;

    this->m_httpServerSettings->host = listeningAddress;
    this->m_httpServerSettings->port = listeningPort;
}

Server::~Server()
{
    this->m_httpServerSettings.reset();
    this->m_requestMapper.reset();
    this->m_httpListener.reset();
}

bool Server::isPortAvailable(const quint16 &port)
{
    static QMutex mutex;
    mutex.lock();

    QTcpSocket socket;

    if (socket.bind(port))
    {
        socket.close();
        mutex.unlock();
        return true;
    }

    mutex.unlock();
    return false;
}

const bool &Server::isRunning() const
{
    return this->m_isRunning;
}

const QString &Server::listeningAddress() const
{
    return this->m_listeningAddress;
}

const quint16 &Server::listeningPort() const
{
    return this->m_listeningPort;
}

void Server::setListeningAddress(const QString &address)
{
    if (address.size() != 0)
    {
        this->m_mutex.lock();
        this->m_listeningAddress = address;
        this->m_httpServerSettings->host = address;
        this->m_mutex.unlock();
        debugger->notice(QString("Server: set listening address to '%1'").arg(address));
        emit listeningAddressChanged();
    }

    else
    {
        debugger->error(QString("Server: '%1' is not a valid listening address.").arg(address));
        debugger->notice("Server: listening address unchanged.");
    }
}

void Server::setListeningPort(const quint16 &port)
{
    // do nothing if port is unchanged
    if (port == this->m_listeningPort && port == this->m_httpServerSettings->port)
    {
        debugger->notice(QString("Server: listening port is already %1").arg(QString::number(port)));
        return;
    }

    if (/*port > 0 && port <= 65535 && */this->isPortAvailable(port))
    {
        this->m_mutex.lock();
        this->setPortInternal(port);
        this->m_mutex.unlock();
        emit listeningPortChanged();
    }

    else if (port == 0)
    {
        static std::random_device rd;
        static std::mt19937 mt(rd());
        static std::uniform_int_distribution<quint16> dist(1, 65535);

        this->m_mutex.lock();
        quint16 random_port = dist(mt);

        if (this->isPortAvailable(random_port))
        {
            this->setPortInternal(random_port);
            this->m_mutex.unlock();
            emit listeningPortChanged();
        }
    }

    else
    {
        debugger->error(QString("Server: %1 = invalid listening port or port is already taken").arg(QString::number(port)));
        debugger->notice("Server: listening port unchanged.");
    }
}

void Server::setListeningPortUnsafe(const quint16 &port)
{
    QMutexLocker(&this->m_mutex);
    this->m_listeningPort = port;
    this->m_httpServerSettings->port = port;
}

void Server::start()
{
    if (!this->m_isRunning)
    {
        if (this->m_listeningAddress.size() == 0 || this->m_listeningPort == 0)
        {
            debugger->error("Server: can not start server, please specify a valid address and port number");
            emit error(InvalidAddressOrPort);
            return;
        }

        this->startInternal();
        emit started();
    }

    else
    {
        debugger->warning(QString("Server: server is already running on %1:%2").arg(this->m_listeningAddress, QString::number(this->m_listeningPort)));
    }
}

void Server::setPortInternal(const quint16 &port)
{
    this->m_listeningPort = port;
    this->m_httpServerSettings->port = port;
    debugger->notice(QString("Server: listening port changed to %1").arg(QString::number(port)));
}

void Server::startInternal()
{
    debugger->notice("Server: starting up...");

    if (!this->m_httpListener && !this->m_requestMapper)
    {
        this->m_requestMapper.reset(new RequestMapper(this));
        this->m_httpListener.reset(new HttpListener(this->m_httpServerSettings.get(), this->m_requestMapper.get(), this));
        QObject::connect(this->m_requestMapper.get(), &RequestMapper::shutdown, this, &Server::stop);
        QObject::connect(this->m_requestMapper.get(), &RequestMapper::reload, this, &Server::restart);
        this->m_isRunning = true;
        debugger->notice("Server: started.");
    }
}

void Server::stop()
{
    if (this->m_isRunning)
    {
        this->stopInternal();
        emit stopped();
    }

    else
    {
        debugger->warning("Server: server already stopped.");
    }
}

void Server::stopInternal()
{
    debugger->notice("Server: shutting down...");

    if (this->m_httpListener && this->m_requestMapper)
    {
        QObject::disconnect(this->m_requestMapper.get(), &RequestMapper::shutdown, this, &Server::stop);
        QObject::disconnect(this->m_requestMapper.get(), &RequestMapper::reload, this, &Server::restart);
        this->m_requestMapper.reset();

        this->m_httpListener->close();
        this->m_httpListener.reset();

        this->m_isRunning = false;
        debugger->notice("Server: stopped.");
    }
}

void Server::halt()
{
    if (this->m_isRunning)
    {
        this->stopInternal();
        debugger->notice("Server: server halted.");
        emit halted();
    }

    else
    {
        debugger->warning("Server: server not running.");
    }
}

void Server::restart()
{
    this->stopInternal();
    this->startInternal();
    emit restarted();
}


ServerDBusAdapter::ServerDBusAdapter(Server *s, QObject *parent)
    : QObject(parent)
{
    this->d = s;
}

ServerDBusAdapter::~ServerDBusAdapter()
{
    this->d = nullptr;
}

void ServerDBusAdapter::start()
{
    QMutexLocker(&this->m_mutex);
    this->d->start();
}

void ServerDBusAdapter::stop()
{
    QMutexLocker(&this->m_mutex);
    this->d->stop();
}

void ServerDBusAdapter::halt()
{
    QMutexLocker(&this->m_mutex);
    this->d->halt();
}

bool ServerDBusAdapter::reload()
{
    QMutexLocker(&this->m_mutex);
    this->d->restart();
    return this->d->isRunning();
}

bool ServerDBusAdapter::isPortAvailable(const quint16 &port)
{
    QMutexLocker(&this->m_mutex);
    return Server::isPortAvailable(port);
}

bool ServerDBusAdapter::setAddress(const QString &address)
{
    debugger->notice("D-Bus: Trying to change server address...");

    if (!address.isEmpty())
    {
        this->d->setListeningAddress(address);
        debugger->notice("D-Bus: Server address changed. Please reload the server for the changes to take effect.");
        return true;
    }

    else
    {
        debugger->notice("D-Bus: Server address may not be empty!");
    }

    return false;
}

bool ServerDBusAdapter::setPort(const quint16 &port)
{
    QMutexLocker(&this->m_mutex);

    debugger->notice(QString("D-Bus: Trying to change server port to %1...").arg(QString::number(port)));

    if (port == this->d->listeningPort())
    {
        debugger->notice("D-Bus: Port not changed. Ignoring.");
        return true;
    }

    quint16 random_port = 0;
    if (port == 0)
    {
        static std::random_device rd;
        static std::mt19937 mt(rd());
        static std::uniform_int_distribution<quint16> dist(1, 65535);

        debugger->notice("D-Bus: A port of 0 is not possible. Generating a random 16-bit uint now...");

        random_port = dist(mt);
    }

    else
    {
        random_port = port;
    }

    if (Server::isPortAvailable(random_port))
    {
        this->d->setListeningPortUnsafe(random_port);
        debugger->notice(QString("D-Bus: Server port changed to %1. Please reload the server for the changes to take effect.")
                         .arg(QString::number(random_port)));
        return true;
    }

    else
    {
        debugger->warning(QString("D-Bus: Server: port %1 is already taken or bind error occurred. Try another port.").arg(QString::number(port)));
    }

    return false;
}

QString ServerDBusAdapter::address()
{
    return this->d->listeningAddress();
}

quint16 ServerDBusAdapter::port()
{
    return this->d->listeningPort();
}
