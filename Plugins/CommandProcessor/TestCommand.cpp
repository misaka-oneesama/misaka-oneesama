#include "TestCommand.hpp"

#include <Global.hpp>

TestCommand::TestCommand()
    : AbstractCommand()
{
}

TestCommand::TestCommand(const QString &name)
    : TestCommand()
{
    this->setName(name);
}

QString TestCommand::exec(const QString &arguments, const QDiscordMessage &message)
{
    return QString("__**Test Command**__\n\n"
                   "**`Arguments:`** `%1`\n"
                   "**`Channel:`** `%2`\n"
                   "**`ID of command message:`** `%3`\n"
                   "**`Guild ID:`** `%4`\n"
                   "**`Guild Name:`** %5\n"
                   "**`Channel:`** %6\n"
                   "**`Guild ID (from Channel):` `%7`")
                   .arg(arguments.isEmpty() ? "(no arguments given)" : arguments,
                        message.channelId().toString(),
                        message.id().toString(),
                        message.guild().isNull() ? "nullptr, but why :thinking:" : message.guild()->id().toString(),
                        message.guild().isNull() ? "nullptr, but why :thinking:" : message.guild()->name(),
                        message.channel().isNull() ? "nullptr" : message.channel()->name(),
                        message.channel().isNull() ? "nullptr" : (message.channel()->guild().isNull() ? "guild nullptr" : message.channel()->guild()->id().toString()));
}
