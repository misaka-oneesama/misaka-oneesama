#ifndef TESTCOMMAND_HPP
#define TESTCOMMAND_HPP

#include "AbstractCommand.hpp"

class TestCommand : public AbstractCommand
{
public:
    TestCommand();
    TestCommand(const QString &name);

    QString exec(const QString &arguments, const QDiscordMessage &message) override;
};

#endif // TESTCOMMAND_HPP
