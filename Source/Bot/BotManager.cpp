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

#include "BotManager.hpp"
#include <Global.hpp>

BotManager::BotManager(QObject *parent)
    : QObject(parent)
{
    this->m_discord.reset(new QDiscord());
    this->m_eventHandler.reset(new DiscordEventHandler(this->m_discord.get()));

    QObject::connect(this->m_discord.get(), &QDiscord::loginSuccess, this, &BotManager::internal_loginSuccess);
    QObject::connect(this->m_discord.get(), &QDiscord::loginFailed, this, &BotManager::internal_loginFailed);
    QObject::connect(this->m_discord.get(), &QDiscord::loggedOut, this, &BotManager::internal_loggedOut);
    QObject::connect(this->m_discord.get(), &QDiscord::disconnected, this, &BotManager::internal_disconnected);
}

BotManager::~BotManager()
{
    this->m_eventHandler.reset();
    this->m_discord.reset();
}

void BotManager::setOAuthToken(const QString &token)
{
    QMutexLocker(&this->m_mutex);
    this->m_token = token;
}

const QString &BotManager::token() const
{
    return this->m_token;
}

bool BotManager::isConnected() const
{
    return this->m_discord->isConnected();
}

void BotManager::login()
{
    this->m_discord->login(this->m_token, QDiscordTokenType::Bot);
}

void BotManager::login(const QString &token)
{
    this->m_discord->login(token, QDiscordTokenType::Bot);
}

void BotManager::login(const QString &token, const QDiscordTokenType &type)
{
    this->m_discord->login(token, type);
}

void BotManager::logout()
{
    this->m_discord->logout();
}

void BotManager::stop()
{
    this->m_mutex.lock();
    this->m_isStopping = true;
    this->m_mutex.unlock();
    this->logout();
}

void BotManager::reload()
{
    this->m_mutex.lock();
    this->m_isReloading = true;
    this->m_mutex.unlock();
    this->logout();
}

void BotManager::internal_loginSuccess()
{
    debugger->notice("BotManager: logged in.");
    emit loginSuccess();
}

void BotManager::internal_loginFailed()
{
    debugger->error("BotManager: failed to login.");
    emit loginFailed();
}

void BotManager::internal_loggedOut()
{
    debugger->notice("BotManager: logged out.");

    if (this->m_isStopping)
    {
        this->m_mutex.lock();
        this->m_isStopping = false;
        this->m_mutex.unlock();
        emit stopped();
    }

    else if (this->m_isReloading)
    {
        this->m_mutex.lock();
        this->m_isReloading = false;
        this->m_mutex.unlock();
        this->login();
        emit reloaded();
    }

    else
    {
        emit loggedOut();
    }
}

void BotManager::internal_disconnected()
{
    debugger->notice("BotManager: disconnected.");
    emit disconnected();
}

BotManagerDBusAdapter::BotManagerDBusAdapter(BotManager *bm, QObject *parent)
    : QObject(parent)
{
    this->d = bm;
}

BotManagerDBusAdapter::~BotManagerDBusAdapter()
{
    this->d = nullptr;
}

void BotManagerDBusAdapter::start()
{
    this->d->login();
}

void BotManagerDBusAdapter::login()
{
    this->d->login();
}

void BotManagerDBusAdapter::stop()
{
    this->d->stop();
}

void BotManagerDBusAdapter::logout()
{
    this->d->logout();
}

void BotManagerDBusAdapter::reload()
{
    this->d->reload();
}

bool BotManagerDBusAdapter::isConnected()
{
    return this->d->isConnected();
}

void BotManagerDBusAdapter::setOAuthToken(const QString &token)
{
    this->d->setOAuthToken(token);
}
