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

#ifndef CONFIGMANAGER_HPP
#define CONFIGMANAGER_HPP

#include <QMutexLocker>
#include <QMutex>
#include <QSqlDatabase>
#include <QString>
#include <QVariant>
#include <QList>

#include <memory>

class ConfigDirectory
{
public:
    ConfigDirectory(bool output = true);
    ~ConfigDirectory();

    // returns the absolute path of the operating systems users configuration directory
    static const QString &configBasePath();

    // returns the absolute path to the configuration directory
    const QString &configPath() const;

    // returns the current status of the configuration directory
    bool isValid() const;

public:
    bool m_valid = false;
    QString m_cfgPath;
};

#define DEFINE_SETTING(NAME_SET, NAME_GET, TYPE) \
    public: \
        void set##NAME_SET(const TYPE &value) \
        { \
            QMutexLocker(&this->m_mutex); \
            this->cfg_##NAME_SET = value; \
        } \
        const TYPE &NAME_GET() const \
        { \
            return this->cfg_##NAME_SET; \
        } \
    private: \
        TYPE cfg_##NAME_SET

class ConfigManager
{
public:
    ConfigManager(bool output = true);
    ~ConfigManager();

    // Checks if the config manager can read/write the configuration directory and database
    bool isValid() const;

    void loadConfig();
    void saveConfig();
    void resetConfig();
    void resetConfigSoft();

    // returns the absolute path to the configuration directory
    const QString &configPath() const;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// CONFIGURABLE OPTIONS
    ///////////////////////////////////////////////////////////////////////////////////////////////

    // Core Settings
    DEFINE_SETTING(MaxLogFilesToKeep, maxLogFilesToKeep, quint16);
    DEFINE_SETTING(DebuggerPrintToTerminal, debuggerPrintToTerminal, bool);

    // Server Settings
    DEFINE_SETTING(ServerListeningAddress, serverListeningAddress, QString);
    DEFINE_SETTING(ServerListeningPort, serverListeningPort, quint16);

    // Bot Settings
    DEFINE_SETTING(OAuthToken, token, QString);

    DEFINE_SETTING(JoinedGuilds, joinedGuilds, QList<quint64>);
public:
    void addJoinedGuild(const quint64 &guildId);
    void removeJoinedGuild(const quint64 &guildId);

private:
    void createDatabaseLayout();
    bool isValidDatabase() const;

    enum class TableFormat : quint8 {
        KeyValuePair = 0,
        IdList
    };

    bool findTable(const QString &tableName) const;
    bool verifyFields(const QString &tableName, const TableFormat &format) const;
    bool tableHasKey(const QString &tableName, const QString &keyName) const;
    QVariant tableValue(const QString &tableName, const QString &keyName, const QVariant &fallback = QVariant()) const;
    QList<quint64> tableIdList(const QString &tableName) const;
    void modifyValueAt(const QString &tableName, const QString &keyName, const QVariant &value);
    void modifyOrInsertValueAt(const QString &tableName, const QString &keyName, const QVariant &value);

private:
    std::unique_ptr<ConfigDirectory> m_cfgDir;

    bool m_valid = false;
    bool m_validDb = false;
    bool m_output = true;

    QMutex m_mutex;
    QString m_configPath;
    QString m_configDatabaseFilename;
    QSqlDatabase m_db;
};

#undef DEFINE_SETTING

#endif // CONFIGMANAGER_HPP
