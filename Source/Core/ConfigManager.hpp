#ifndef CONFIGMANAGER_HPP
#define CONFIGMANAGER_HPP

#include <QString>
#include <QFile>
#include <QDataStream>

class ConfigManager
{
public:
    ConfigManager();
    ~ConfigManager();

    // Checks if the config manager can read/write the configuration directory
    bool isValid() const;

    // reads the configuration file and stores the configuration into memory
    // calling this function again after modifying the config, restores the config from the file
    void loadConfig();

    // restores the hardcoded default configuration
    // call `saveConfig` to make it permanent
    void resetConfig();

    // writes back the current configuration into the config file
    void saveConfig();

    // returns the absolute path to the configuration directory
    const QString &configPath() const;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// CONFIGURABLE OPTIONS
    ///////////////////////////////////////////////////////////////////////////////////////////////

    void setMaxLogFilesToKeep(quint16);
    quint16 maxLogFilesToKeep() const;

    void setOAuthToken(const QString &token);
    const QString &token() const;

    void setJoinedGuilds(const QList<quint64> &guildIds);
    void addJoinedGuild(const quint64 &guildId);
    void removeJoinedGuild(const quint64 &guildId);
    const QList<quint64> &joinedGuilds() const;

private:
    bool m_valid = false;
    QString m_configPath;
    QString m_configFilePath;

    QFile *m_configFile = nullptr;
    QDataStream m_configStream;

    // settings.bin header
    QString m_cfgHeader;

    // version of the configuration file
    quint32 m_cfgVersion;

    void p_resetStream(bool force = false);

    // CONFIGURABLE OPTIONS
    quint16 m_cfgMaxLogFilesToKeep = 0;
    QString m_cfgOAuthToken;
    QList<quint64> m_cfgJoinedGuilds;
};

#endif // CONFIGMANAGER_HPP
