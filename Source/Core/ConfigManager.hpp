#ifndef CONFIGMANAGER_HPP
#define CONFIGMANAGER_HPP

#include <QString>

class ConfigManager
{
public:
    ConfigManager();
    ~ConfigManager();

    bool isValid() const;

    void loadConfig();
    void saveConfig();

    const QString &configPath() const;

private:
    bool m_valid = false;
    QString m_configPath;
};

#endif // CONFIGMANAGER_HPP
