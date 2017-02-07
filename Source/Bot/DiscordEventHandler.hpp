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

#ifndef DISCORDEVENTHANDLER_HPP
#define DISCORDEVENTHANDLER_HPP

#include <QDiscord>
#include <QObject>

class DiscordEventHandler : public QObject
{
    Q_OBJECT

public:
    explicit DiscordEventHandler(QDiscord *discord, QObject *parent = nullptr);
    ~DiscordEventHandler();

private slots:
    void messageReceived(const QDiscordMessage &message);
    void messageUpdated(const QDiscordMessage &message, const QDateTime &timestamp);
    void messageDeleted(const QDiscordMessage &message);

private:
    QDiscord *m_discord = nullptr;
};

#endif // DISCORDEVENTHANDLER_HPP
