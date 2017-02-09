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

const QString RequestMapper::dbus_error_message = QString::fromUtf8("Can't connect to the D-Bus Interface.");

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
    this->request = &request;
    this->response = &response;

    this->path = this->request->getPath();
    this->method = this->request->getMethod();

    debugger->notice(QString("[HTTP %1] RequestMapper: (path=%2)")
                     .arg(this->method.constData(),
                          this->path.constData()));

    // set default content-type to plain text
    response.setHeader("Content-Type", "text/plain; charset=UTF-8");

    if (this->path == "/")
    {
        response.setStatus(200);
        response.write(QString::fromUtf8("It's working 👌").toUtf8(), true);
    }

    else if (this->path.startsWith("/api"))
    {
        this->api(path.mid(4));
    }

    else
    {
        //static StaticFileController *staticFileController = new StaticFileController(
        //            new QtWebApp::StaticFileControllerConfig(QStandardPaths::writableLocation(QStandardPaths::HomeLocation)));
        //staticFileController->setContentTypeEncoding("UTF-8");
        //staticFileController->service(request, response);

        response.setStatus(404);
        response.write(QString::fromUtf8("No handler for URL '%1' found").arg(this->path.constData()).toUtf8(), true);
    }

    debugger->notice(QString::fromUtf8("[HTTP %1] RequestMapper: finished request for (path=%2)")
                     .arg(this->method.constData(), this->path.constData()).toUtf8());

    // reset data and pointers
    this->path.clear();
    this->method.clear();
    this->request = nullptr;
    this->response = nullptr;
}

void RequestMapper::dbusCall(QDBusInterface *interface, const QString &method)
{
    if (interface->isValid())
    {
        interface->call(method);
        this->json(200, true, QString::fromUtf8("D-Bus call to '%1' was successful.").arg(method));
    }

    else
    {
        debugger->warning("RequestMapper: " + this->dbus_error_message);
        this->json(503, false, this->dbus_error_message);
    }
}

void RequestMapper::dbusCallReply(QDBusInterface *interface, const QString &method)
{
    if (interface->isValid())
    {
        QDBusReply<bool> reply = interface->call(method);
        this->json(200, reply, QString::fromUtf8("D-Bus call to '%1' was successful.").arg(method));
    }

    else
    {
        debugger->warning("RequestMapper: " + this->dbus_error_message);
        this->json(503, false, this->dbus_error_message);
    }
}

void RequestMapper::json(const quint16 &httpStatus, bool success, const QString &message)
{
    QJsonObject obj;
    obj["status"] = httpStatus;
    obj["success"] = success;
    obj["message"] = message;
    QJsonDocument d(obj);
    this->response->setStatus(httpStatus);
    this->response->setHeader("Content-Type", "application/json; charset=UTF-8");
    this->response->write(d.toJson(QJsonDocument::Compact), true);
}

void RequestMapper::apiEndpointError()
{
    this->json(400, false, QString::fromUtf8("No such endpoint: %1").arg(this->path.constData()));
}
