#include "PluginInterface.hpp"

PluginInterface::PluginInterface(QDiscord *discord)
{
    this->d = discord;
}

PluginInterface::~PluginInterface()
{
}
