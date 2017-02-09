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

void RequestMapper::api(const QString &endpoint)
{
    // Just for reference: Don't implement Server::halt() endpoint here
    if (endpoint == "/shutdown")
    {
        this->json(200, true, "Server is shutting down...");
        emit shutdown();
    }

    else if (endpoint == "/reload")
    {
        this->json(200, true, "Server is reloading...");;
        emit reload();
    }

    else if (endpoint.startsWith("/bot"))
    {
        this->apiBot(endpoint.mid(4));
    }

    else
    {
        this->apiEndpointError();
    }
}
