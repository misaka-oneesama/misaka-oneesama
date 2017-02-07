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

#include "ConfigManager.hpp"

#include <QCoreApplication>
#include <QStandardPaths>
#include <QDir>

#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>

#include <QDebug>

#include <iostream>

#ifdef MISAKA_DEBUG
#define MISAKA_DATABASE_FILENAME "/settings.debug.sqlite"
#else
#define MISAKA_DATABASE_FILENAME "/settings.sqlite"
#endif

ConfigDirectory::ConfigDirectory(bool output)
{
    this->m_cfgPath = ConfigDirectory::configBasePath() + '/' + qApp->applicationName();

    if (QDir(ConfigDirectory::configBasePath()).mkdir(qApp->applicationName()) ||
        QDir(this->m_cfgPath).exists())
    {
        if (output)
        {
            std::cerr << "ConfigDirectory: configuration directory found" << std::endl;
        }

        this->m_valid = true;
    }

    else
    {
        if (output)
        {
            std::cerr << "ConfigDirectory: unable to create or access the configuration directory" << std::endl;
        }
    }
}

ConfigDirectory::~ConfigDirectory()
{
    this->m_cfgPath.clear();
}

const QString &ConfigDirectory::configBasePath()
{
    static const QString m_cfgBasePath = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
    return m_cfgBasePath;
}

const QString &ConfigDirectory::configPath() const
{
    return this->m_cfgPath;
}

bool ConfigDirectory::isValid() const
{
    return this->m_valid;
}


ConfigManager::ConfigManager(bool output)
{
    this->m_output = output;

    // determine config directory and filename for the database
    this->m_cfgDir.reset(new ConfigDirectory(output));
    this->m_valid = this->m_cfgDir->isValid();
    this->m_configPath = this->m_cfgDir->configPath();
    this->m_configDatabaseFilename = this->m_configPath + MISAKA_DATABASE_FILENAME;

    // initialize default configuration
    this->resetConfigSoft();

    // initialize database connection
    this->m_db = QSqlDatabase::addDatabase("QSQLITE");
    this->m_db.setDatabaseName(this->m_configDatabaseFilename);

    if (this->m_db.open())
    {
        if (this->m_output)
        {
            std::cerr << "ConfigManager: SQLite3 database opened." << std::endl;
        }

        if (!this->isValidDatabase())
        {
            if (this->m_output)
            {
                std::cerr << "ConfigManager: Creating/Fixing SQLite3 database..." << std::endl;
            }

            this->createDatabaseLayout();
        }

        else
        {
            if (this->m_output)
            {
                std::cerr << "ConfigManager: Database check passed." << std::endl;
            }
        }

        this->m_validDb = true;
    }

    else
    {
        if (this->m_output)
        {
            std::cerr << "ConfigManager: unable to open the SQLite3 database!" << std::endl;
            std::cerr << "ConfigManager: the hardcoded default configuration will be used instead." << std::endl;
        }
    }
}

ConfigManager::~ConfigManager()
{
    this->m_db.close();
    this->m_cfgDir.reset();
}

bool ConfigManager::isValid() const
{
    return this->m_valid && this->m_validDb;
}

void ConfigManager::createDatabaseLayout()
{
    if (this->m_output)
    {
        std::cerr << "ConfigManager: Creating tables..." << std::endl;
    }

    this->m_db.exec("CREATE TABLE `Core` ("
                    "`key` TEXT NOT NULL, "
                    "`value` BLOB NOT NULL "
                    ");");
    this->m_db.exec("CREATE TABLE `Server` ("
                    "`key` TEXT NOT NULL, "
                    "`value` BLOB NOT NULL "
                    ");");
    this->m_db.exec("CREATE TABLE `Bot` ("
                    "`key` TEXT NOT NULL, "
                    "`value` BLOB NOT NULL "
                    ");");
    this->m_db.exec("CREATE TABLE `JoinedGuilds` ("
                    "`id` INTEGER NOT NULL"
                    ");");

    std::cerr << "ConfigManager: Tables created." << std::endl;
}

bool ConfigManager::isValidDatabase() const
{
    if (this->findTable("Core") &&
        this->findTable("Server") &&
        this->findTable("Bot") &&
        this->findTable("JoinedGuilds"))
    {
        return (
            this->verifyFields("Core", TableFormat::KeyValuePair) &&
            this->verifyFields("Server", TableFormat::KeyValuePair) &&
            this->verifyFields("Bot", TableFormat::KeyValuePair) &&
            this->verifyFields("JoinedGuilds", TableFormat::IdList)
        );
    }

    return false;
}

bool ConfigManager::findTable(const QString &tableName) const
{
    QSqlQuery q = this->m_db.exec("SELECT count(*) FROM sqlite_master WHERE type = 'table' AND name = '" + tableName + "'");
    return (q.next() ? q.value(0).toBool() : false);
}

bool ConfigManager::verifyFields(const QString &tableName, const TableFormat &format) const
{
    QSqlQuery q = this->m_db.exec("PRAGMA table_info(" + tableName +")");

    if (q.next())
    {
        QSqlRecord r = q.record();

        if (format == TableFormat::KeyValuePair)
        {
            bool first = (
                r.field("cid").value() == QVariant(0) &&
                r.field("name").value() == QVariant("key") &&
                r.field("type").value() == QVariant("TEXT") &&
                r.field("notnull").value() == QVariant(1) &&
                r.field("dflt_value").value().isNull() &&
                r.field("pk").value() == QVariant(0)
            );

            if (first && q.next())
            {
                r = q.record();
                return (
                    r.field("cid").value() == QVariant(1) &&
                    r.field("name").value() == QVariant("value") &&
                    r.field("type").value() == QVariant("BLOB") &&
                    r.field("notnull").value() == QVariant(1) &&
                    r.field("dflt_value").value().isNull() &&
                    r.field("pk").value() == QVariant(0)
                );
            }
        }

        else if (format == TableFormat::IdList)
        {
            return (
                r.field("cid").value() == QVariant(0) &&
                r.field("name").value() == QVariant("id") &&
                r.field("type").value() == QVariant("INTEGER") &&
                r.field("notnull").value() == QVariant(1) &&
                r.field("dflt_value").value().isNull() &&
                r.field("pk").value() == QVariant(0)
            );
        }
    }

    return false;
}

bool ConfigManager::tableHasKey(const QString &tableName, const QString &keyName) const
{
    QSqlQuery q = this->m_db.exec("SELECT EXISTS(SELECT 1 FROM '" + tableName + "' WHERE key=\"" + keyName + "\" LIMIT 1)");

    if (q.next())
    {
        return q.value(0).toBool();
    }

    return false;
}

QVariant ConfigManager::tableValue(const QString &tableName, const QString &keyName, const QVariant &fallback) const
{
    if (this->tableHasKey(tableName, keyName))
    {
        QSqlQuery q = this->m_db.exec("SELECT value FROM '" + tableName + "' WHERE key=\"" + keyName + "\" LIMIT 1");

        if (q.next())
        {
            return (q.value(0).isValid() ? q.value(0) : fallback);
        }
    }

    return fallback;
}

QList<quint64> ConfigManager::tableIdList(const QString &tableName) const
{
    QList<quint64> ids;
    QSqlQuery q = this->m_db.exec("SELECT id FROM '" + tableName + "'");

    while (q.next())
    {
        ids << q.value(0).toULongLong();
    }

    return ids;
}

void ConfigManager::modifyValueAt(const QString &tableName, const QString &keyName, const QVariant &value)
{
    if (this->tableHasKey(tableName, keyName))
    {
        this->m_db.exec("UPDATE '" + tableName + "' SET value=\"" + value.toString() + "\" WHERE key=\"" + keyName + "\"");
    }
}

void ConfigManager::modifyOrInsertValueAt(const QString &tableName, const QString &keyName, const QVariant &value)
{
    if (this->tableHasKey(tableName, keyName))
    {
        this->modifyValueAt(tableName, keyName, value);
    }

    else
    {
        this->m_db.exec("INSERT INTO '" + tableName + "' (key, value) VALUES (\"" + keyName + "\", \"" + value.toString() + "\")");
    }
}

void ConfigManager::loadConfig()
{
    QMutexLocker(&this->m_mutex);

    std::cout << "ConfigManager: --- BEGIN OF CONFIGURATION ---" << std::endl;

    QVariant maxLogFilesToKeep = this->tableValue("Core", "MaxLogFilesToKeep", this->cfg_MaxLogFilesToKeep);
    if (maxLogFilesToKeep.canConvert<quint16>())
        if (maxLogFilesToKeep.toInt() > 0)
            this->cfg_MaxLogFilesToKeep = maxLogFilesToKeep.toInt();
    maxLogFilesToKeep.clear();
    std::cout << "ConfigManager: MaxLogFilesToKeep = " << this->cfg_MaxLogFilesToKeep << std::endl;

    QVariant debuggerPrintToTerminal = this->tableValue("Core", "DebuggerPrintToTerminal", this->cfg_DebuggerPrintToTerminal);
    if (debuggerPrintToTerminal.canConvert<bool>())
        this->cfg_DebuggerPrintToTerminal = debuggerPrintToTerminal.toBool();
    debuggerPrintToTerminal.clear();
    std::cout << "ConfigManager: DebuggerPrintToTerminal = " << this->cfg_DebuggerPrintToTerminal << std::endl;

    QVariant serverListeningAddress = this->tableValue("Server", "ServerListeningAddress", this->cfg_ServerListeningAddress);
    if (serverListeningAddress.canConvert<QString>())
        this->cfg_ServerListeningAddress = serverListeningAddress.toString();
    serverListeningAddress.clear();
    std::cout << "ConfigManager: ServerListeningAddress = " << qUtf8Printable(this->cfg_ServerListeningAddress) << std::endl;

    QVariant serverListeningPort = this->tableValue("Server", "ServerListeningPort", this->cfg_ServerListeningPort);
    if (serverListeningPort.canConvert<quint16>())
        if (serverListeningPort.toUInt() > 0U)
            this->cfg_ServerListeningPort = serverListeningPort.toUInt();
    serverListeningPort.clear();
    std::cout << "ConfigManager: ServerListeningPort = " << this->cfg_ServerListeningPort << std::endl;

    this->cfg_JoinedGuilds = this->tableIdList("JoinedGuilds");
    std::cout << "ConfigManager: JoinedGuilds = {\n";
    for (auto&& gId : this->cfg_JoinedGuilds)
    {
        std::cout << "   " << gId << ",\n";
    }
    std::cout << '}' << std::endl;

    std::cout << "ConfigManager: --- END OF CONFIGURATION ---" << std::endl;
}

void ConfigManager::saveConfig()
{
    QMutexLocker(&this->m_mutex);

    this->modifyOrInsertValueAt("Core", "MaxLogFilesToKeep", this->cfg_MaxLogFilesToKeep);
    this->modifyOrInsertValueAt("Core", "DebuggerPrintToTerminal", this->cfg_DebuggerPrintToTerminal);

    this->modifyOrInsertValueAt("Server", "ServerListeningAddress", this->cfg_ServerListeningAddress);
    this->modifyOrInsertValueAt("Server", "ServerListeningPort", this->cfg_ServerListeningPort);

    // todo: JoinedGuilds
}

void ConfigManager::resetConfig()
{
    QMutexLocker(&this->m_mutex);

    this->resetConfigSoft();
    this->saveConfig();
}

void ConfigManager::resetConfigSoft()
{
    QMutexLocker(&this->m_mutex);

    // Core Settings
    this->cfg_MaxLogFilesToKeep = 5;
#ifdef MISAKA_DEBUG
    this->cfg_DebuggerPrintToTerminal = true;
#else
    this->cfg_DebuggerPrintToTerminal = false;
#endif

    // Server Settings
    this->cfg_ServerListeningAddress = "127.0.0.1";
    this->cfg_ServerListeningPort = 4555U;

    // Bot Settings
    this->cfg_OAuthToken.clear();
    this->cfg_JoinedGuilds.clear();
}

const QString &ConfigManager::configPath() const
{
    return this->m_configPath;
}

void ConfigManager::addJoinedGuild(const quint64 &guildId)
{
    QMutexLocker(&this->m_mutex);

    if (!this->cfg_JoinedGuilds.contains(guildId))
    {
        this->cfg_JoinedGuilds.append(guildId);
    }
}

void ConfigManager::removeJoinedGuild(const quint64 &guildId)
{
    QMutexLocker(&this->m_mutex);
    this->cfg_JoinedGuilds.removeOne(guildId);
}
