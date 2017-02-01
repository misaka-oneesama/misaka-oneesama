#ifndef CONFIGMANAGER_HPP
#define CONFIGMANAGER_HPP

#include <QString>
#include <QFile>
#include <QDataStream>
#include <QMutexLocker>
#include <QMutex>

#include <memory>

class ConfigManager
{
public:
    ConfigManager(bool output = true);
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
    const quint16 &maxLogFilesToKeep() const;

    void setDebuggerPrintToTerminal(bool);
    const bool &debuggerPrintToTerminal() const;

    void setServerListeningAddress(const QString &address);
    const QString &serverListeningAddress() const;

    void setServerListeningPort(const quint16 &port);
    const quint16 &serverListeningPort() const;

    void setOAuthToken(const QString &token);
    const QString &token() const;

    void setJoinedGuilds(const QList<quint64> &guildIds);
    void addJoinedGuild(const quint64 &guildId);
    void removeJoinedGuild(const quint64 &guildId);
    const QList<quint64> &joinedGuilds() const;

private:
    bool m_valid = false;
    bool m_output = true;
    QMutex m_mutex;
    QString m_configPath;
    QString m_configFilePath;

    std::unique_ptr<QFile> m_configFile;
    QDataStream m_configStream;

    void p_resetStream();

    // settings.bin header
    QString m_cfgHeader;

    // version of the configuration file
    quint32 m_cfgVersion;

    // CONFIGURABLE OPTIONS
    quint16 m_cfgMaxLogFilesToKeep = 0;
    bool m_cfgDebuggerPrintToTerminal;
    QString m_cfgServerListeningAddress;
    quint16 m_cfgServerListeningPort;
    QString m_cfgOAuthToken;
    QList<quint64> m_cfgJoinedGuilds;
};

#endif // CONFIGMANAGER_HPP
