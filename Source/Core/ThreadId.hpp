#ifndef THREADID_HPP
#define THREADID_HPP

#include <QString>
#include <QObjectUserData>

class ThreadId : public QObjectUserData
{
public:
    ThreadId(const QString &namedId);
    ~ThreadId();

    const QString &getNamedId() const;

private:
    QString m_namedId;
};

#endif // THREADID_HPP
