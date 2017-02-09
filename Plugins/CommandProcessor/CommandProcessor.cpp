#include "CommandProcessor.hpp"

#include <Global.hpp>

CommandProcessor::CommandProcessor(QDiscord *discord, const QString &prefix, bool deleteCommandMessage,
                                   const Qt::CaseSensitivity &prefix_sensitivity,
                                   const Qt::CaseSensitivity &cmd_sensitivity)
    : PluginInterface(discord)
{
    this->m_prefix = prefix;
    this->m_prefix_sensitivity = prefix_sensitivity;
    this->m_cmd_sensitivity = cmd_sensitivity;
    this->m_deleteCmdMsg = deleteCommandMessage;
}

CommandProcessor::~CommandProcessor()
{
}

void CommandProcessor::setPrefix(const QString &prefix)
{
    QMutexLocker(&this->m_mutex);
    this->m_prefix = prefix;
}

void CommandProcessor::setPrefixCaseSensitivity(const Qt::CaseSensitivity &sensitivity)
{
    QMutexLocker(&this->m_mutex);
    this->m_prefix_sensitivity = sensitivity;
}

void CommandProcessor::setCommandCaseSensitivity(const Qt::CaseSensitivity &sensitivity)
{
    QMutexLocker(&this->m_mutex);
    this->m_cmd_sensitivity = sensitivity;
}

void CommandProcessor::deleteCommandMessage(bool enabled)
{
    QMutexLocker(&this->m_mutex);
    this->m_deleteCmdMsg = enabled;
}

void CommandProcessor::messageReceived(const QDiscordMessage &message)
{
    if (message.content().startsWith(this->m_prefix, this->m_prefix_sensitivity))
    {
        QString command = message.content().mid(this->m_prefix.size(), message.content().indexOf(' '));
        QString arguments = message.content().mid(command.size() + 1);

        emit sendMessage(QString("**Command received**\n**`Command:`** %1\n**`Arguments:`** %2")
                        .arg(command, arguments), message.channelId());

        if (this->m_deleteCmdMsg)
        {
            emit deleteMessage(message);
        }
    }
}
