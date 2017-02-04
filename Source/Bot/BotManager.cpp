#include "BotManager.hpp"
#include <Global.hpp>

#include <QThread>
#include <Core/ThreadId.hpp>

BotManager::BotManager(QObject *parent)
    : QObject(parent)
{
    this->m_discord.reset(new QDiscord());
    this->m_eventHandler.reset(new DiscordEventHandler(this->m_discord.get()));

    QObject::connect(this->m_discord.get(), &QDiscord::loginSuccess, this, &BotManager::loginSuccess);
    QObject::connect(this->m_discord.get(), &QDiscord::loginFailed, this, &BotManager::loginFailed);
    QObject::connect(this->m_discord.get(), &QDiscord::loggedOut, this, &BotManager::loggedOut);
    QObject::connect(this->m_discord.get(), &QDiscord::disconnected, this, &BotManager::disconnected);
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

void BotManager::logout()
{
    this->m_discord->logout();
}

void BotManager::stop()
{
    this->logout();
    emit stopped();
}

void BotManager::loginSuccess()
{
    debugger->notice("BotManager: logged in");
    emit notify(LoginSuccess);
}

void BotManager::loginFailed()
{
    debugger->error("BotManager: failed to login");
    emit error(LoginFailed);
}

void BotManager::loggedOut()
{
    debugger->notice("BotManager: logged out");
    emit notify(LoggedOut);
}

void BotManager::disconnected()
{
    debugger->notice("BotManager: WebSocket closed by host. Session no longer valid or other network error.");
    debugger->notice("BotManager: Reconnecting...");
    emit notify(Disconnected);
}


BotManagerDBusAdapter::BotManagerDBusAdapter(BotManager *botManager, QObject *parent)
    : QObject(parent)
{
    this->d = botManager;
}

BotManagerDBusAdapter::~BotManagerDBusAdapter()
{
    this->d = nullptr;
}

void BotManagerDBusAdapter::start()
{
    this->d->login();
}

void BotManagerDBusAdapter::stop()
{
    this->d->stop();
}

// FIXME: doesn't work, delay/sleep doesn't fixes the problem
bool BotManagerDBusAdapter::reload()
{
//    QMutexLocker(&this->m_mutex);
//    this->d->logout();
//    this->d->login();
//    return this->d->isConnected();
    debugger->warning("D-Bus: Bot: method '.reload' is unimplemented. Use '.logout' + '.login' instead.");
    return false;
}

void BotManagerDBusAdapter::login()
{
    this->d->login();
}

void BotManagerDBusAdapter::logout()
{
    this->d->logout();
}

bool BotManagerDBusAdapter::isConnected()
{
    return this->d->isConnected();
}

void BotManagerDBusAdapter::setOAuthToken(const QString &token)
{
    this->d->setOAuthToken(token);
}
