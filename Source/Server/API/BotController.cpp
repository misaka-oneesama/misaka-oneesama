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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <Server/RequestMapper.hpp>
#include <Global.hpp>

#include <QStandardPaths>
#include <QDBusReply>

#include <iostream>

void RequestMapper::apiBot(const QString &endpoint)
{   
    if (endpoint == "/start")
    {
        this->dbusCall(this->m_ifaceMaster.get(), "startBot");
    }

    else if (endpoint == "/stop")
    {
        this->dbusCall(this->m_ifaceMaster.get(), "stopBot");
    }

    else if (endpoint == "/login")
    {
        this->dbusCall(this->m_ifaceBot.get(), "login");
    }

    else if (endpoint == "/logout")
    {
        this->dbusCall(this->m_ifaceBot.get(), "logout");
    }

    else if (endpoint == "/reload")
    {
        this->dbusCall(this->m_ifaceBot.get(), "reload");
    }

    else if (endpoint == "/isConnected")
    {
        this->dbusCallReply(this->m_ifaceBot.get(), "isConnected");
    }

    else
    {
        this->apiEndpointError();
    }
}
