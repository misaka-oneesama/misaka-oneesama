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

#include "DiscordEventHandler.hpp"

#include <Global.hpp>

DiscordEventHandler::DiscordEventHandler(QDiscord *discord, QObject *parent)
    : QObject(parent)
{
    this->m_discord = discord;

    QObject::connect(this->m_discord->state(), &QDiscordStateComponent::selfCreated,
                     this, &DiscordEventHandler::selfCreated);

    // Register Message Signals
    QObject::connect(this->m_discord->state(), &QDiscordStateComponent::messageCreated,
                     this, &DiscordEventHandler::messageReceived);
    QObject::connect(this->m_discord->state(), &QDiscordStateComponent::messageUpdated,
                     this, &DiscordEventHandler::messageUpdated);
    QObject::connect(this->m_discord->state(), &QDiscordStateComponent::messageDeleted,
                     this, &DiscordEventHandler::messageDeleted);
}

DiscordEventHandler::~DiscordEventHandler()
{
    this->m_discord = nullptr;
}

void DiscordEventHandler::selfCreated(QSharedPointer<QDiscordUser> user)
{
    debugger->notice(QString("Bot Account Details: [Username: %1#%2, ID: %3]").arg(
                             user->username(),
                             user->discriminator(),
                             user->id()));
}

void DiscordEventHandler::messageReceived(const QDiscordMessage &message)
{
    debugger->notice("DiscordEventHandler: message received -> " + message.content());
}

void DiscordEventHandler::messageUpdated(const QDiscordMessage &message, const QDateTime &timestamp)
{
    debugger->notice("DiscordEventHandler: message updated -> " + message.content() + " | @" + timestamp.toString());
}

void DiscordEventHandler::messageDeleted(const QDiscordMessage &message)
{
    debugger->notice("DiscordEventHandler: message deleted -> " + message.id());
}
