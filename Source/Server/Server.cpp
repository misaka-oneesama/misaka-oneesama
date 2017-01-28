#include "Server.hpp"

#include <Source/Global.hpp>
#include <QThread>

Server::Server(QObject *parent)
    : QObject(parent)
{
    this->m_httpServerSettings = new HttpServerSettings;

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
    if (this->m_httpServerSettings)
    {
        delete this->m_httpServerSettings;
        this->m_httpServerSettings = nullptr;
    }
}

void Server::setListeningAddress(const QLatin1String &address)
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
    if (!this->m_configured || this->m_listeningAddress.size() == 0 || this->m_listeningPort == 0)
    {
        debugger->error("Server: can not start a unconfigured server, please specify a valid address and port number");
        emit error(ServerNotConfigured);
        return;
    }

    this->p_startPrivate();
    emit started();
}

void Server::p_startPrivate()
{
    debugger->notice("Server: starting up...");

    if (!this->m_httpListener && !this->m_requestMapper)
    {
        this->m_requestMapper = new RequestMapper(this);
        this->m_httpListener = new HttpListener(this->m_httpServerSettings, this->m_requestMapper, this);
        debugger->notice("Server: started");
    }

    else
    {
        debugger->warning(QString("Server: server is already running on %1:%2").arg(this->m_listeningAddress, QString::number(this->m_listeningPort)));
    }
}

void Server::stop()
{
    this->p_stopPrivate();
    emit stopped();
}

void Server::stopAndQuitThread()
{
    this->p_stopPrivate();

    static_cast<QThread*>(this->parent())->requestInterruption();
    static_cast<QThread*>(this->parent())->quit();
    emit stopped();
}

void Server::p_stopPrivate()
{
    debugger->notice("Server: shutting down...");

    if (this->m_httpListener && this->m_requestMapper)
    {
        this->m_httpListener->close();
        delete this->m_httpListener;
        this->m_httpListener = nullptr;

        delete this->m_requestMapper;
        this->m_requestMapper = nullptr;

        debugger->notice("Server: stopped");
    }

    else
    {
        debugger->warning("Server: server already stopped");
    }
}

void Server::restart()
{
    this->p_stopPrivate();
    this->p_startPrivate();
    emit restarted();
}
