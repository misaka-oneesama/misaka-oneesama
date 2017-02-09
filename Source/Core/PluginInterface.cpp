#include "PluginInterface.hpp"

PluginInterface::PluginInterface(QDiscord *discord)
    : QObject()
{
    this->d = discord;
}

PluginInterface::~PluginInterface()
{
}

void PluginInterface::selfCreated(QSharedPointer<QDiscordUser>) {}

void PluginInterface::messageReceived(const QDiscordMessage&) {}
void PluginInterface::messageUpdated(const QDiscordMessage&, const QDateTime&) {}
void PluginInterface::messageDeleted(const QDiscordMessage&) {}
