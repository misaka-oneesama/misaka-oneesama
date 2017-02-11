#include "CommandProcessor.hpp"

#include <Global.hpp>

#include "TestCommand.hpp"

CommandProcessor::CommandProcessor(QDiscord *discord, const QString &prefix, bool deleteCommandMessage,
                                   const Qt::CaseSensitivity &prefix_sensitivity,
                                   const Qt::CaseSensitivity &cmd_sensitivity)
    : PluginInterface(discord)
{
    this->m_prefix = prefix;
    this->m_prefix_sensitivity = prefix_sensitivity;
    this->m_cmd_sensitivity = cmd_sensitivity;
    this->m_deleteCmdMsg = deleteCommandMessage;

    this->installCommand(new TestCommand("test"));
}

CommandProcessor::~CommandProcessor()
{
    for (AbstractCommand *i : this->m_commands)
    {
        if (i) delete i;
    }
    this->m_commands.clear();
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

void CommandProcessor::installCommand(AbstractCommand *cmd, bool force)
{
    if (cmd->name().isEmpty())
    {
        debugger->warning("CommandProcessor: Can't attach command without a name. [skipped]");
        return;
    }

    else if (this->m_commands.contains(cmd) && !force)
    {
        debugger->warning(
            QString::fromUtf8(
                "CommandProcessor: A command '%1' with the name is already attached. "
                "Use `force` to override.")
            .arg(cmd->name())
        );
        return;
    }

    QMutexLocker(&this->m_mutex);
    debugger->notice(
        QString::fromUtf8("CommandProcessor: Command '%1' attached.")
        .arg(cmd->name())
    );
    this->m_commands.append(cmd);
}

void CommandProcessor::messageReceived(const QDiscordMessage &message)
{
    if (message.content().startsWith(this->m_prefix, this->m_prefix_sensitivity))
    {
        QString command = message.content().mid(this->m_prefix.size());
        command = command.left(command.indexOf(' '));
        QString arguments = message.content().mid(this->m_prefix.size() + command.size() + 1);

        for (AbstractCommand *i : this->m_commands)
        {
            if (i->isEnabled() && QString::compare(i->name(), command, this->m_cmd_sensitivity) == 0)
            {
                emit sendMessage(i->exec(arguments, message), message.channelId());
            }
        }

        if (this->m_deleteCmdMsg)
        {
            emit deleteMessage(message);
        }

        command.clear();
        arguments.clear();
    }
}
