#include "ConfigManager.hpp"

#include <QCoreApplication>
#include <QStandardPaths>
#include <QDir>

#include <iostream>

#ifdef MISAKA_DEBUG
#define MISAKA_CONFIG_FILENAME "/settings.debug.bin"
#else
#define MISAKA_CONFIG_FILENAME "/settings.bin"
#endif

ConfigManager::ConfigManager(bool output)
    : m_valid(false)
    , m_output(output)
{
    const QString cfgBasePath = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
    this->m_configPath = cfgBasePath + '/' + qApp->applicationName();
    this->m_configFilePath = this->m_configPath + MISAKA_CONFIG_FILENAME;

    if (QDir(cfgBasePath).mkdir(qApp->applicationName()) ||
        QDir(this->m_configPath).exists())
    {
        if (this->m_output)
            std::cerr << "ConfigManager: configuration directory found" << std::endl;
        this->m_valid = true;
    }

    else
    {
        if (this->m_output)
            std::cerr << "ConfigManager: unable to create or access the configuration directory" << std::endl;
    }

    // set file header
    this->m_cfgHeader = QLatin1String("misaka-cfg");

    // set expected config file version
    // TODO:
    //   check this value during reading and update the settings file and reload it
    //   to prevent corrupting or losing the configuration. during some tests the file
    //   was resized to 0 during reading.
    //   also add a note that this value needs to be raised everytime the signature
    //   or the stream changes by either adding new values or removing existing ones.
    // NOTE:
    //   if this causes too much problems in the future port the configuration system
    //   to a JSON or INI file.
    this->m_cfgVersion = 0x00000100;

    // initialize default configuration
    this->resetConfig();
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
    QMutexLocker(&this->m_mutex);

    this->m_configFile.reset(new QFile(this->m_configFilePath));

    if (this->m_configFile->exists())
    {
        if (this->m_configFile->open(QFile::ReadWrite))
        {
            if (this->m_output)
                std::cerr << "ConfigManager: reading from settings.bin" << std::endl;
            this->m_configStream.setDevice(this->m_configFile.get());

            this->m_configStream >> this->m_cfgHeader;

            if (this->m_cfgHeader != QLatin1String("misaka-cfg"))
            {
                if (this->m_output)
                {
                    std::cerr << "ConfigManager: not a valid configuration file" << std::endl;
                    std::cerr << "ConfigManager: [WARNING] using default configuration" << std::endl;
                }

                this->resetConfig();
                this->p_resetStream();
                return;
            }

            this->m_configStream >> this->m_cfgVersion;

            this->m_configStream >> this->m_cfgMaxLogFilesToKeep;
            this->m_configStream >> this->m_cfgDebuggerPrintToTerminal;
            this->m_configStream >> this->m_cfgServerListeningAddress;
            this->m_configStream >> this->m_cfgServerListeningPort;
            this->m_configStream >> this->m_cfgOAuthToken;
            this->m_configStream >> this->m_cfgJoinedGuilds;
        }

        else
        {
            if (this->m_output)
            {
                std::cerr << "ConfigManager: unable to read from or write to settings.bin" << std::endl;
                std::cerr << "ConfigManager: current configuration hasn't been changed" << std::endl;
            }
        }
    }

    else
    {
        this->p_resetStream();

        this->resetConfig();
        this->saveConfig();
    }
}

void ConfigManager::resetConfig()
{
    QMutexLocker(&this->m_mutex);

    this->m_cfgHeader = QLatin1String("misaka-cfg");
    this->m_cfgVersion = 0x00000100;

    this->m_cfgMaxLogFilesToKeep = 5;
#ifdef MISAKA_DEBUG
    this->m_cfgDebuggerPrintToTerminal = true;
#else
    this->m_cfgDebuggerPrintToTerminal = false;
#endif
    this->m_cfgServerListeningAddress = QLatin1String("127.0.0.1");
    this->m_cfgServerListeningPort = 4555U;
    this->m_cfgJoinedGuilds.clear();
}

void ConfigManager::saveConfig()
{
    QMutexLocker(&this->m_mutex);

    this->m_configFile.reset(new QFile(this->m_configFilePath));

    if (this->m_configFile->exists())
    {
        this->m_configFile->remove();
    }

    if (this->m_configFile->open(QFile::WriteOnly))
    {
        if (this->m_output)
            std::cerr << "ConfigManager: saving to settings.bin" << std::endl;
        this->m_configStream.setDevice(this->m_configFile.get());

        this->m_configStream << this->m_cfgHeader;
        this->m_configStream << this->m_cfgVersion;

        this->m_configStream << this->m_cfgMaxLogFilesToKeep;
        this->m_configStream << this->m_cfgDebuggerPrintToTerminal;
        this->m_configStream << this->m_cfgServerListeningAddress;
        this->m_configStream << this->m_cfgServerListeningPort;
        this->m_configStream << this->m_cfgOAuthToken;
        this->m_configStream << this->m_cfgJoinedGuilds;

        this->m_configFile->flush();
    }

    else
    {
        if (this->m_output)
        {
            std::cerr << "ConfigManager: unable to write to settings.bin" << std::endl;
            std::cerr << "ConfigManager: current configuration hasn't been saved" << std::endl;
        }
    }

    this->p_resetStream();
}

const QString &ConfigManager::configPath() const
{
    return this->m_configPath;
}

void ConfigManager::setMaxLogFilesToKeep(quint16 max)
{
    QMutexLocker(&this->m_mutex);
    this->m_cfgMaxLogFilesToKeep = max;
}

const quint16 &ConfigManager::maxLogFilesToKeep() const
{
    return this->m_cfgMaxLogFilesToKeep;
}

void ConfigManager::setDebuggerPrintToTerminal(bool enabled)
{
    QMutexLocker(&this->m_mutex);
    this->m_cfgDebuggerPrintToTerminal = enabled;
}

const bool &ConfigManager::debuggerPrintToTerminal() const
{
    return this->m_cfgDebuggerPrintToTerminal;
}

void ConfigManager::setServerListeningAddress(const QString &address)
{
    QMutexLocker(&this->m_mutex);
    this->m_cfgServerListeningAddress = address;
}

const QString &ConfigManager::serverListeningAddress() const
{
    return this->m_cfgServerListeningAddress;
}

void ConfigManager::setServerListeningPort(const quint16 &port)
{
    QMutexLocker(&this->m_mutex);
    this->m_cfgServerListeningPort = port;
}

const quint16 &ConfigManager::serverListeningPort() const
{
    return this->m_cfgServerListeningPort;
}

void ConfigManager::setOAuthToken(const QString &token)
{
    QMutexLocker(&this->m_mutex);
    this->m_cfgOAuthToken = token;
}

const QString &ConfigManager::token() const
{
    return this->m_cfgOAuthToken;
}

void ConfigManager::setJoinedGuilds(const QList<quint64> &guildIds)
{
    QMutexLocker(&this->m_mutex);
    this->m_cfgJoinedGuilds = guildIds;
}

void ConfigManager::addJoinedGuild(const quint64 &guildId)
{
    QMutexLocker(&this->m_mutex);

    if (!this->m_cfgJoinedGuilds.contains(guildId))
    {
        this->m_cfgJoinedGuilds.append(guildId);
    }
}

void ConfigManager::removeJoinedGuild(const quint64 &guildId)
{
    QMutexLocker(&this->m_mutex);

    // there should be only one occurrence of guildId in the list
    // threrefore `removeOne` is faster than `removeAll`
    this->m_cfgJoinedGuilds.removeOne(guildId);
}

const QList<quint64> &ConfigManager::joinedGuilds() const
{
    return this->m_cfgJoinedGuilds;
}

void ConfigManager::p_resetStream()
{
    QMutexLocker(&this->m_mutex);

    if (this->m_configFile)
    {
        this->m_configFile->close();
        this->m_configFile.reset();
    }

    this->m_configStream.setDevice(0);
}
