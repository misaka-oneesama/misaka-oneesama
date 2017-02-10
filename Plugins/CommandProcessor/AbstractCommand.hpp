#ifndef ABSTRACTCOMMAND_HPP
#define ABSTRACTCOMMAND_HPP

#include <QDiscord>
#include <QObject>
#include <QString>

class AbstractCommand
{
public:
    AbstractCommand();
    virtual ~AbstractCommand();

    void setEnabled(bool enabled) { this->m_enabled = enabled; }
    bool isEnabled() const { return this->m_enabled; }

    void setName(const QString &name) { this->m_name = name; }
    const QString &name() const { return this->m_name; }

    bool operator== (const AbstractCommand &other)
    { return QString::compare(this->name(), other.name(), Qt::CaseSensitive) == 0; }
    bool operator== (const AbstractCommand *other)
    { if (other) return operator== (*other); return false; }

    virtual QString exec(const QString &arguments, const QDiscordMessage &message) = 0;

private:
    bool m_enabled = true;
    QString m_name;
};

#endif // ABSTRACTCOMMAND_HPP
