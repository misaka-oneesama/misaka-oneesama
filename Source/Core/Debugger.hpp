#ifndef DEBUGGER_HPP
#define DEBUGGER_HPP

#include <QString>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QMutex>

#include <memory>

class Debugger
{
public:
    Debugger(bool output = true);
    Debugger(const QString &logDir, bool output = true);
    Debugger(const QString &logDir, bool enabled, bool output = true);
    ~Debugger();

    bool setLogDir(const QString &logDir);
    const QString logDir() const;

    void setEnabled(bool);
    void setFilenamePrefix(const QString &prefix);
    void setMaxLogFilesToKeep(quint16);

    void printToTerminal(bool);

    void notice(const QString&);
    void warning(const QString&);
    void error(const QString&);

private:
    bool m_output = true;
    QDir m_logDir;
    std::unique_ptr<QFile> m_logFile;
    QTextStream m_logStream;

    bool m_printToTerminal = false;

    QMutex m_mutex;

    bool m_valid = false;
    bool m_enabled = false;
    QString m_prefix;
    quint16 m_maxLogFiles = 5; // +1 for the newly created one (means a total of 6 log files)

    enum MessageType : quint8 {
        Notice = 0,
        Warning,
        Error
    };

    void log(const QString &message, MessageType);
    static QString currentThreadName();
};

#endif // DEBUGGER_HPP
