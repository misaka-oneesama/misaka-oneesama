#include "AbstractCommand.hpp"

AbstractCommand::AbstractCommand()
{
}

AbstractCommand::~AbstractCommand()
{
    this->m_name.clear();
}
