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

#include <Server/RequestMapper.hpp>
#include <Global.hpp>

#include <QStandardPaths>
#include <QDBusReply>

#include <iostream>

void RequestMapper::api(HttpRequest &request, HttpResponse &response, const QString &endpoint)
{
    // Just for reference: Don't implement Server::halt() endpoint here
    if (endpoint == "/shutdown")
    {
        response.setStatus(200);
        response.write(QByteArray("Shutting down..."), true);

        emit shutdown();
    }

    else if (endpoint == "/reload")
    {
        response.setStatus(200);
        response.write(QByteArray("Reloading server..."), true);

        emit reload();
    }

    else if (endpoint.startsWith("/bot"))
    {
        this->apiBot(request, response, endpoint.mid(4));
    }

    else
    {
        response.setStatus(400);
        response.write(QString("No such endpoint: %1").arg(request.getPath().constData()).toUtf8(), true);
    }
}
