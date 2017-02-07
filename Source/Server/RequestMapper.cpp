#include "RequestMapper.hpp"
#include <Global.hpp>

#include <QStandardPaths>
#include <QDBusReply>

#include <iostream>

#include <QtWebApp/HttpServer/StaticFileController>

RequestMapper::RequestMapper(QObject *parent)
    : HttpRequestHandler(parent)
{
    this->m_ifaceMaster.reset(new QDBusInterface(DBusServiceName(InstanceType::Master), "/", "", QDBusConnection::sessionBus()));
    this->m_ifaceServer.reset(new QDBusInterface(DBusServiceName(InstanceType::Server), "/", "", QDBusConnection::sessionBus()));
    this->m_ifaceBot.reset(new QDBusInterface(DBusServiceName(InstanceType::Bot), "/", "", QDBusConnection::sessionBus()));

    debugger->notice("RequestMapper created");
}

RequestMapper::~RequestMapper()
{
    this->m_ifaceMaster.reset();
    this->m_ifaceServer.reset();
    this->m_ifaceBot.reset();

    debugger->notice("RequestMapper destroyed");
}

void RequestMapper::service(HttpRequest &request, HttpResponse &response)
{
    QByteArray path = request.getPath();
    debugger->notice(QString("[HTTP %1] RequestMapper: (path=%2)").arg(request.getMethod().constData(), path.constData()));

    // todo: change to html when we got basic functionality working
    // API responses should be always plain though
    // todo(api): implement json responses??
    response.setHeader("Content-Type", "text/plain; charset=UTF-8");

    if (path == "/")
    {
        response.setStatus(200);
        response.write(QString("It's working :D").toUtf8(), true);
    }

    else if (path.startsWith("/api"))
    {
        this->api(request, response, path.mid(4));
    }

    else
    {
        //static StaticFileController *staticFileController = new StaticFileController(
        //            new QtWebApp::StaticFileControllerConfig(QStandardPaths::writableLocation(QStandardPaths::HomeLocation)));
        //staticFileController->setContentTypeEncoding("UTF-8");
        //staticFileController->service(request, response);

        response.setStatus(404);
        response.write(QString("No handler for URL '%1' found").arg(path.constData()).toUtf8(), true);
    }

    debugger->notice(QString("[HTTP %1] RequestMapper: finished request for (path=%2)").arg(request.getMethod().constData(), path.constData()));
}

void RequestMapper::dbusError(HttpResponse &response)
{
    debugger->warning("RequestMapper: Can't connect to the D-Bus Interface.");
    response.setStatus(503);
    response.write(QByteArray("Can't connect to the D-Bus Interface."), true);
}
