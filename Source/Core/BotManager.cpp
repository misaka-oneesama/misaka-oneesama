#include "BotManager.hpp"

#include <QThread>
#include "ThreadId.hpp"

#include <Source/Global.hpp>

BotManager::BotManager(QObject *parent)
    : QObject(parent)
{
}

BotManager::~BotManager()
{
    this->m_eventHandler.reset();
    this->m_discord.reset();
}

void BotManager::init()
{
    // TODO: move the event handler to its own thread
    // currently runs on the `main` thread
    this->m_discord.reset(new QDiscord());
    this->m_eventHandler.reset(new DiscordEventHandler(this->m_discord.get()));

    QObject::connect(this->m_discord.get(), &QDiscord::loginSuccess, this, &BotManager::loginSuccess);
    QObject::connect(this->m_discord.get(), &QDiscord::loginFailed, this, &BotManager::loginFailed);
    QObject::connect(this->m_discord.get(), &QDiscord::loggedOut, this, &BotManager::loggedOut);
    QObject::connect(this->m_discord.get(), &QDiscord::disconnected, this, &BotManager::disconnected);
}

void BotManager::setOAuthToken(const QString &token)
{
    this->m_token = token;
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
    debugger->notice("BotManager: WebSocket closed by host. Session no longer valid or other network error");
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
