#include "Server.hpp"

#include <Source/Global.hpp>
#include <QThread>

Server::Server(QObject *parent)
    : QObject(parent)
{
    this->m_httpServerSettings.reset(new QtWebApp::HttpServerSettings);

    this->m_configured = true;
}

Server::Server(const QLatin1String &listeningAddress, quint16 listeningPort, QObject *parent)
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

void Server::setListeningAddress(const QString &address)
{
    if (address.size() != 0)
    {
        this->m_listeningAddress = address;
        this->m_httpServerSettings->host = address;
        debugger->notice("Server: set listening address to " + address);
        emit listeningAddressChanged();
    }

    else
    {
        debugger->error("Server: not a valid listening address");
        debugger->notice("Server: listening address unchanged");
    }
}

void Server::setListeningPort(quint16 port)
{
    // lets trust the magic of `uint16` here ;)
    if (port != 0)
    {
        this->m_listeningPort = port;
        this->m_httpServerSettings->port = port;
        debugger->notice("Server: set listening port to " + QString::number(port));
        emit listeningPortChanged();
    }

    else
    {
        debugger->error("Server: not a valid listening port");
        debugger->notice("Server: listening port unchanged");
    }
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
