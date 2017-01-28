#include "ThreadId.hpp"

ThreadId::ThreadId(const QString &namedId)
{
    this->m_namedId = namedId;
}

ThreadId::~ThreadId()
{
    this->m_namedId.clear();
}

const QString &ThreadId::getNamedId() const
{
    return this->m_namedId;
}
