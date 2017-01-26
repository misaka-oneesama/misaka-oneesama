#ifndef DEBUGGER_HPP
#define DEBUGGER_HPP

#include <QString>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QMutex>

class Debugger
{
public:
    Debugger();
    Debugger(const QString &logDir);
    Debugger(const QString &logDir, bool enabled);
    ~Debugger();

    bool setLogDir(const QString &logDir);
    void setEnabled(bool);
    void setMaxLogFilesToKeep(quint16);

    void printToTerminal(bool);

    void notice(const QString&);
    void warning(const QString&);
    void error(const QString&);

private:
    QDir m_logDir;
    QFile *m_logFile = nullptr;
    QTextStream m_logStream;

    bool m_printToTerminal = false;

    QMutex m_mutex;

    bool m_valid = false;
    bool m_enabled = false;
    quint16 m_maxLogFiles = 5;
};

#endif // DEBUGGER_HPP
