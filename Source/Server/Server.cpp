#include "Server.hpp"
#include <Global.hpp>

#include <QThread>
#include <QTcpSocket>

Server::Server(QObject *parent)
    : QObject(parent)
{
    this->m_httpServerSettings.reset(new QtWebApp::HttpServerSettings);

    this->m_configured = true;
}

Server::Server(const QString &listeningAddress, quint16 listeningPort, QObject *parent)
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

bool Server::isRunning() const
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
        debugger->notice("Server: set listening address to " + address);
        emit listeningAddressChanged();
    }

    else
    {
        debugger->error("Server: not a valid listening address");
        debugger->notice("Server: listening address unchanged");
    }
}

void Server::setListeningPort(const quint16 &port)
{
    // do nothing if port is unchanged
    if (port == this->m_listeningPort)
    {
        return;
    }

    // lets trust the magic of `uint16` here ;)
    if (port != 0 && this->isPortAvailable(port))
    {
        this->m_mutex.lock();
        this->m_listeningPort = port;
        this->m_httpServerSettings->port = port;
        this->m_mutex.unlock();
        debugger->notice("Server: set listening port to " + QString::number(port));
        emit listeningPortChanged();
    }

    else
    {
        debugger->error(QString("Server: %1 = invalid listening port or port is already taken").arg(QString::number(port)));
        debugger->notice("Server: listening port unchanged");
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
        if (!this->m_configured || this->m_listeningAddress.size() == 0 || this->m_listeningPort == 0)
        {
            debugger->error("Server: can not start a unconfigured server, please specify a valid address and port number");
            emit error(ServerNotConfigured);
            return;
        }

        this->p_startPrivate();
        emit started();
    }

    else
    {
        debugger->warning(QString("Server: server is already running on %1:%2").arg(this->m_listeningAddress, QString::number(this->m_listeningPort)));
    }
}

void Server::p_startPrivate()
{
    debugger->notice("Server: starting up...");

    if (!this->m_httpListener && !this->m_requestMapper)
    {
        this->m_requestMapper.reset(new RequestMapper(this));
        this->m_httpListener.reset(new HttpListener(this->m_httpServerSettings.get(), this->m_requestMapper.get(), this));
        QObject::connect(this->m_requestMapper.get(), &RequestMapper::shutdown, this, &Server::stop);
        this->m_isRunning = true;
        debugger->notice("Server: started");
    }
}

void Server::stop()
{
    if (this->m_isRunning)
    {
        this->p_stopPrivate();
        emit stopped();
    }

    else
    {
        debugger->warning("Server: server already stopped");
    }
}

void Server::p_stopPrivate()
{
    debugger->notice("Server: shutting down...");

    if (this->m_httpListener && this->m_requestMapper)
    {
        QObject::disconnect(this->m_requestMapper.get(), &RequestMapper::shutdown, this, &Server::stop);
        this->m_requestMapper.reset();

        this->m_httpListener->close();
        this->m_httpListener.reset();

        this->m_isRunning = false;
        debugger->notice("Server: stopped");
    }
}

void Server::restart()
{
    this->p_stopPrivate();
    this->p_startPrivate();
    emit restarted();
}


ServerDBusAdapter::ServerDBusAdapter(Server *server, QObject *parent)
    : QObject(parent)
{
    this->d = server;
}

ServerDBusAdapter::~ServerDBusAdapter()
{
    this->d = nullptr;
}

void ServerDBusAdapter::start()
{
    this->d->start();
}

void ServerDBusAdapter::stop()
{
    this->d->stop();
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
        debugger->notice("D-Bus: Server: address may not be empty!");
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

    if (Server::isPortAvailable(port))
    {
        this->d->setListeningPortUnsafe(port);
        debugger->notice("D-Bus: Server port changed. Please reload the server for the changes to take effect.");
        return true;
    }

    else
    {
        debugger->warning(QString("D-Bus: Server: port %1 is taken or bind error occurred. Try another port.").arg(QString::number(port)));
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
