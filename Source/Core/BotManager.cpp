#include "BotManager.hpp"

#include <Source/Global.hpp>

BotManager::BotManager(QObject *parent)
    : QObject(parent)
{
}

BotManager::~BotManager()
{
    if (this->m_discord)
    {
        delete this->m_discord;
        this->m_discord = nullptr;
    }
}

void BotManager::init()
{
    this->m_discord = new QDiscord();

    QObject::connect(this->m_discord, &QDiscord::loginSuccess, this, &BotManager::loginSuccess);
    QObject::connect(this->m_discord, &QDiscord::loginFailed, this, &BotManager::loginFailed);
    QObject::connect(this->m_discord, &QDiscord::loggedOut, this, &BotManager::loggedOut);
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
