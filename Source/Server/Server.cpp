#include "Server.hpp"

#include <QSettings>

// QtWebApp
#include "httplistener.h"

using namespace stefanfrings;

#include <Source/Global.hpp>
#include "RequestMapper.hpp"

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
    // FIXME: improve this asap
    // also find a workaround to make QSettings **NOT** to store its settings to disk
    // if there is no workaround make changes upstream in QtWebApp and port it away from QSettings
    // i'd like to avoid that though

    debugger->notice("Server: starting up...");

    QSettings *settings = new QSettings(this);
    settings->setValue("host", this->m_listeningAddress);
    settings->setValue("port", this->m_listeningPort);

    settings->setValue("minThreads", 4);
    settings->setValue("maxThreads", 100);
    settings->setValue("cleanupInterval", 60000);
    settings->setValue("readTimeout", 60000);
    settings->setValue("maxRequestSize", 16000);
    settings->setValue("maxMultiPartSize", 10000000);

    // note about settings
    // all settings are used, except this 2
    //  - sslKeyFile
    //  - sslCertFile
    // we don't need that, we recommend to setup a reverse proxy on a more professional web server app

    new HttpListener(settings, new RequestMapper(this), this);
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
