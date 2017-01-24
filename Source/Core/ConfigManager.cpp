#include "ConfigManager.hpp"

#include <QCoreApplication>
#include <QStandardPaths>
#include <QDir>

#include <iostream>

ConfigManager::ConfigManager()
    : m_valid(false)
{
    const QString cfgBasePath = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
    this->m_configPath = cfgBasePath + '/' + qApp->applicationName();

    if (QDir(cfgBasePath).mkdir(qApp->applicationName()) ||
        QDir(this->m_configPath).exists())
    {
        std::cerr << "ConfigManager: configuration directory found" << std::endl;
        this->m_valid = true;
    }

    else
    {
        std::cerr << "ConfigManager: unable to create or access the configuration directory" << std::endl;
    }
}

ConfigManager::~ConfigManager()
{
}

bool ConfigManager::isValid() const
{
    return this->m_valid;
}

void ConfigManager::loadConfig()
{
}

void ConfigManager::saveConfig()
{
}

const QString &ConfigManager::configPath() const
{
    return this->m_configPath;
}
