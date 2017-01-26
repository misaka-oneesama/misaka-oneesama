#include "Server.hpp"

#include <Source/Global.hpp>

Server::Server(QObject *parent)
    : QObject(parent)
{
    this->m_configured = true;
}

Server::Server(const QLatin1String &listeningAddress, quint16 listeningPort, QObject *parent)
    : Server(parent)
{
    this->m_listeningAddress = listeningAddress;
    this->m_listeningPort = listeningPort;

    //this->m_configured = true;
}

Server::~Server()
{
}

void Server::setListeningAddress(const QLatin1String &address)
{
    if (address.size() != 0)
    {
        this->m_listeningAddress = address;
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
    if (port == 0)
    {
        this->m_listeningPort = port;
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

    // todo: initialize and start server here
    this->p_startPrivate();
    emit started();
}

void Server::p_startPrivate()
{
    debugger->notice("Server: starting up...");
}

void Server::stop()
{
    // todo: deinitialize and stop server here
    this->p_stopPrivate();
    emit stopped();
}

void Server::p_stopPrivate()
{
    debugger->notice("Server: shutting down...");
}

void Server::restart()
{
    this->p_stopPrivate();
    this->p_startPrivate();
    emit restarted();
}
