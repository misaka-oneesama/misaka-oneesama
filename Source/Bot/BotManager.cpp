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

#include <Plugins/TestPlugin/TestPlugin.hpp>
#include <Plugins/CommandProcessor/CommandProcessor.hpp>

BotManager::BotManager(QObject *parent)
    : QObject(parent)
{
    this->m_discord.reset(new QDiscord());
    this->m_eventHandler.reset(new DiscordEventHandler(this->m_discord.get(), &this->m_plugins));

    QObject::connect(this->m_discord.get(), &QDiscord::loginSuccess, this, &BotManager::internal_loginSuccess);
    QObject::connect(this->m_discord.get(), &QDiscord::loginFailed, this, &BotManager::internal_loginFailed);
    QObject::connect(this->m_discord.get(), &QDiscord::loggedOut, this, &BotManager::internal_loggedOut);
    QObject::connect(this->m_discord.get(), &QDiscord::disconnected, this, &BotManager::internal_disconnected);

    // Install meta types for QDiscord
    qRegisterMetaType<QDiscordID>();
    qRegisterMetaType<QDiscordDiscriminator>();
    qRegisterMetaType<QDiscordMessage>();
    qRegisterMetaType<QDiscordChannel>();
    qRegisterMetaType<QDiscordGuild>();
    qRegisterMetaType<QDiscordGame>();
    qRegisterMetaType<QDiscordMember>();
    qRegisterMetaType<QDiscordUser>();

    // Install built-in plugins
    this->m_plugins << new TestPlugin(this->m_discord.get());

    CommandProcessor *commandProcessor = new CommandProcessor(this->m_discord.get(), ">");
    QObject::connect(commandProcessor, &CommandProcessor::sendMessage, this, &BotManager::sendMessage);
    QObject::connect(commandProcessor, &CommandProcessor::deleteMessage, this, &BotManager::deleteMessage);

    this->m_plugins << commandProcessor;

    // Destroy all plugins on BotManager termination
    for (PluginInterface *i : this->m_plugins)
    {
        QObject::connect(this, &BotManager::stopped, i, &PluginInterface::deleteLater);
    }
}

BotManager::~BotManager()
{
    debugger->notice("BotManager: destroying...");

    this->m_plugins.clear();

    // Waits until all threads have finished their work, see DiscordEventHandler::~DiscordEventHandler()
    this->m_eventHandler.reset();
    this->m_discord.reset();

    debugger->notice("BotManager: destroyed");
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
    return this->m_discord->isConnected() ||
           this->m_discord->isConnecting();
}

QDiscord *BotManager::discord()
{
    return this->m_discord.get();
}

void BotManager::login()
{
    this->login(QDiscordToken(this->m_token, QDiscordToken::Type::Bot));
}

void BotManager::login(const QString &token, const QDiscordToken::Type &type)
{
    this->login(QDiscordToken(token, type));
}

void BotManager::login(const QDiscordToken &token)
{
    debugger->notice("BotManager: logging in...");
    this->m_discord->login(token);
}

void BotManager::logout()
{
    debugger->notice("BotManager: logging out...");
    this->m_discord->logout();
}

void BotManager::stop()
{
    debugger->notice("BotManager: preparing to stop...");
    this->m_mutex.lock();
    this->m_isStopping = true;
    this->m_mutex.unlock();
    this->logout();
}

void BotManager::reload()
{
    debugger->notice("BotManager: preparing to reload...");
    this->m_mutex.lock();
    this->m_isReloading = true;
    this->m_mutex.unlock();
    this->logout();
}

void BotManager::sendMessage(const QString &content, const QDiscordID &channel, bool tts)
{
    this->m_discord->rest()->sendMessage(content, channel, tts);
}

void BotManager::editMessage(const QString &newContent, const QDiscordMessage &message)
{
    this->m_discord->rest()->editMessage(newContent, message);
}

void BotManager::deleteMessage(const QDiscordMessage &message)
{
    this->m_discord->rest()->deleteMessage(message);
}

void BotManager::deleteMessages(const QList<QDiscordMessage> &messages)
{
    this->m_discord->rest()->deleteMessages(messages);
}

void BotManager::deleteMessages(const QList<QDiscordID> &messages, const QDiscordID &channel)
{
    this->m_discord->rest()->deleteMessages(messages, channel);
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

void BotManagerDBusAdapter::sendMessage(const quint64 &channel, const QString &message)
{
    QMutexLocker(&this->m_mutex);
    this->d->discord()->rest()->sendMessage(message, channel);
}

void BotManagerDBusAdapter::editMessage(const quint64 &channel, const quint64 &messageId, const QString &content)
{
    QMutexLocker(&this->m_mutex);
    this->d->discord()->rest()->editMessage(content, channel, messageId);
}

void BotManagerDBusAdapter::deleteMessage(const quint64 &channel, const quint64 &messageId)
{
    QMutexLocker(&this->m_mutex);
    this->d->discord()->rest()->deleteMessage(channel, messageId);
}

// qDBusRegisterMetaType<QList<quint64>>()
//void BotManagerDBusAdapter::deleteMessages(const quint64 &channel, const QList<quint64> &messageIds)
//{
//    QMutexLocker(&this->m_mutex);
//
//    QList<QDiscordID> messages;
//    for (auto&& i : messageIds)
//    {
//        messages << QDiscordID(i);
//    }
//
//    this->d->discord()->rest()->deleteMessages(messages, channel);
//    messages.clear();
//}
