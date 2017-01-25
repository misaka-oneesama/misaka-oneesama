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
    this->m_configFilePath = this->m_configPath + "/settings.bin";

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

    // set file header
    this->m_cfgHeader = QLatin1String("misaka-cfg");

    // set expected config file version
    this->m_cfgVersion = 0x00000100;
}

ConfigManager::~ConfigManager()
{
    this->p_resetStream();
}

bool ConfigManager::isValid() const
{
    return this->m_valid;
}

void ConfigManager::loadConfig()
{
    this->m_configFile = new QFile(this->m_configFilePath);

    if (this->m_configFile->exists())
    {
        if (this->m_configFile->open(QFile::ReadWrite))
        {
            std::cerr << "ConfigManager: reading from settings.bin" << std::endl;
            this->m_configStream.setDevice(this->m_configFile);

            this->m_configStream >> this->m_cfgHeader;

            if (this->m_cfgHeader != QLatin1String("misaka-cfg"))
            {
                std::cerr << "ConfigManager: not a valid configuration file" << std::endl;
                std::cerr << "ConfigManager: [WARNING] using default configuration" << std::endl;

                this->resetConfig();
                this->p_resetStream();
                return;
            }

            this->m_configStream >> this->m_cfgVersion;
        }

        else
        {
            std::cerr << "ConfigManager: unable to read from or write to settings.bin" << std::endl;
            std::cerr << "ConfigManager: current configuration hasn't been changed" << std::endl;
        }
    }

    else
    {
        this->p_resetStream(true);

        this->resetConfig();
        this->saveConfig();
    }
}

void ConfigManager::resetConfig()
{
    this->m_cfgHeader = QLatin1String("misaka-cfg");
    this->m_cfgVersion = 0x00000100;
}

void ConfigManager::saveConfig()
{
    this->m_configFile = new QFile(this->m_configFilePath);

    if (this->m_configFile->exists())
    {
        this->m_configFile->remove();
    }

    if (this->m_configFile->open(QFile::WriteOnly))
    {
        std::cerr << "ConfigManager: saving to settings.bin" << std::endl;
        this->m_configStream.setDevice(this->m_configFile);

        this->m_configStream << this->m_cfgHeader;
        this->m_configStream << this->m_cfgVersion;

        this->m_configFile->flush();
    }

    else
    {
        std::cerr << "ConfigManager: unable to write to settings.bin" << std::endl;
        std::cerr << "ConfigManager: current configuration hasn't been saved" << std::endl;
    }

    this->p_resetStream();
}

const QString &ConfigManager::configPath() const
{
    return this->m_configPath;
}

void ConfigManager::p_resetStream(bool force)
{
    if (this->m_configFile || force)
    {
        this->m_configFile->close();
        delete this->m_configFile;
        this->m_configFile = nullptr;
    }

    this->m_configStream.setDevice(0);
}
