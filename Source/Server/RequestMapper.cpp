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
