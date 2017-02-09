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

#ifndef REQUESTMAPPER_HPP
#define REQUESTMAPPER_HPP

#include <QDBusInterface>
#include <memory>

#include <QtWebApp/HttpServer/HttpRequestHandler>

using namespace QtWebApp::HttpServer;

// Note: QtWebApp creates a unique copy in its own thread for each request
class RequestMapper : public HttpRequestHandler
{
    Q_OBJECT
    Q_DISABLE_COPY(RequestMapper)

public:
    RequestMapper(QObject *parent = nullptr);
    ~RequestMapper();

    void service(HttpRequest &request, HttpResponse &response);

private:
    void api(const QString &endpoint);
    void apiBot(const QString &endpoint);

    void dbusCall(QDBusInterface *interface, const QString &method);
    void dbusCallReply(QDBusInterface *interface, const QString &method);
    void json(const quint16 &httpStatus, bool success, const QString &message);
    void apiEndpointError();

signals:
    void shutdown();
    void reload();

private:
    std::unique_ptr<QDBusInterface> m_ifaceMaster;
    std::unique_ptr<QDBusInterface> m_ifaceServer;
    std::unique_ptr<QDBusInterface> m_ifaceBot;

    QByteArray path;
    QByteArray method;
    HttpRequest *request;
    HttpResponse *response;

    static const QString dbus_error_message;
};

#endif // REQUESTMAPPER_HPP
