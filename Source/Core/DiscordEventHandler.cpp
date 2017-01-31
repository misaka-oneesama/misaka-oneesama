#include "DiscordEventHandler.hpp"

#include <Source/Global.hpp>

DiscordEventHandler::DiscordEventHandler(QDiscord *discord, QObject *parent)
    : QObject(parent)
{
    this->m_discord = discord;

    //qRegisterMetaType<QDiscordMessage>("QDiscordMessage");

    QObject::connect(this->m_discord->state(), &QDiscordStateComponent::messageCreated,
                     this, &DiscordEventHandler::messageReceived);
    QObject::connect(this->m_discord->state(), &QDiscordStateComponent::messageUpdated,
                     this, &DiscordEventHandler::messageUpdated);
    QObject::connect(this->m_discord->state(), &QDiscordStateComponent::messageDeleted,
                     this, &DiscordEventHandler::messageDeleted);
}

DiscordEventHandler::~DiscordEventHandler()
{
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
    debugger->notice("DiscordEventHandler: message deleted -> " + message.content());
}
