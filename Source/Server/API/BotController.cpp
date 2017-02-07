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

void RequestMapper::apiBot(HttpRequest &request, HttpResponse &response, const QString &endpoint)
{
    Q_UNUSED(request);

    if (endpoint == "/start")
    {
        if (this->m_ifaceMaster->isValid())
        {
            this->m_ifaceMaster->call("startBot");
            response.setStatus(200);
            response.write(QByteArray("D-Bus call to 'startBot' was successful."), true);
        }

        else
        {
            this->dbusError(response);
        }
    }

    else if (endpoint == "/stop")
    {
        if (this->m_ifaceMaster->isValid())
        {
            this->m_ifaceMaster->call("stopBot");
            response.setStatus(200);
            response.write(QByteArray("D-Bus call to 'stopBot' was successful."), true);
        }

        else
        {
            this->dbusError(response);
        }
    }
}
