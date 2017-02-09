#ifndef COMMANDPROCESSOR_HPP
#define COMMANDPROCESSOR_HPP

#include <Core/PluginInterface.hpp>

#include <QMutex>

class CommandProcessor : public PluginInterface
{
    Q_OBJECT

public:
    CommandProcessor(QDiscord *discord, const QString &prefix, bool deleteCommandMessage = true,
                     const Qt::CaseSensitivity &prefix_sensitivity = Qt::CaseSensitive,
                     const Qt::CaseSensitivity &cmd_sensitivity = Qt::CaseSensitive);
    ~CommandProcessor();

    void setPrefix(const QString &prefix);
    void setPrefixCaseSensitivity(const Qt::CaseSensitivity &sensitivity);
    void setCommandCaseSensitivity(const Qt::CaseSensitivity &sensitivity);
    void deleteCommandMessage(bool);

    void messageReceived(const QDiscordMessage &message);

signals:
    void sendMessage(const QString &content, const QDiscordID &channel, bool tts = false);
    void deleteMessage(const QDiscordMessage &message);

private:
    QMutex m_mutex;

    QString m_prefix;
    Qt::CaseSensitivity m_prefix_sensitivity = Qt::CaseSensitive;
    Qt::CaseSensitivity m_cmd_sensitivity = Qt::CaseSensitive;
    bool m_deleteCmdMsg = true;
};

#endif // COMMANDPROCESSOR_HPP
