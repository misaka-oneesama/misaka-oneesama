#include "Debugger.hpp"

#include <QCoreApplication>
#include <QFile>
#include <QFileDevice>
#include <QFileInfo>
#include <QFileInfoList>
#include <QDateTime>
#include <QThread>
#include <QMutexLocker>

#include "ThreadId.hpp"

#include <iostream>

Debugger::Debugger(bool output)
    : m_output(output)
{
}

Debugger::Debugger(const QString &logDir)
    : Debugger()
{
    this->setLogDir(logDir);
}

Debugger::Debugger(const QString &logDir, bool enabled)
    : Debugger(logDir)
{
    this->m_enabled = enabled;
}

Debugger::~Debugger()
{
    if (this->m_logFile)
    {
        this->m_logFile->close();
        this->m_logFile.reset();
    }

    this->m_logStream.setDevice(0);
}

bool Debugger::setLogDir(const QString &logDir)
{
    QMutexLocker(&this->m_mutex);

    this->m_logDir = QDir(logDir);

    if (!this->m_logDir.exists())
    {
        QDir parent = this->m_logDir;
        if (parent.cdUp())
        {
            if (parent.mkdir(this->m_logDir.dirName()))
            {
                if (this->m_output)
                {
                    std::cerr << "Debugger: log directory created" << std::endl;
                }

                this->m_valid = true;
            }
        }
    }

    else
    {
        this->m_valid = true;
    }

    if (this->m_valid)
    {
        if (this->m_output)
        {
            std::cerr << "Debugger: log directory set to '" << qUtf8Printable(this->m_logDir.absolutePath()) << "'" << std::endl;
        }
    }

    else
    {
        if (this->m_output)
        {
            std::cerr << "Debugger: given log directory is not valid: " << qUtf8Printable(this->m_logDir.absolutePath()) << std::endl;
        }
    }

    return this->m_valid;
}

const QString Debugger::logDir() const
{
    return this->m_logDir.absolutePath();
}

void Debugger::setEnabled(bool enabled)
{
    QMutexLocker(&this->m_mutex);

    this->m_enabled = enabled;

    if (this->m_enabled)
    {
        QFileInfoList logs = this->m_logDir.entryInfoList(QStringList(this->m_prefix + '*'), QDir::Files | QDir::NoDotAndDotDot, QDir::Name);
        if (logs.count() >= this->m_maxLogFiles)
        {
            for (int i = 0; i < logs.count() - this->m_maxLogFiles; i++)
            {
                std::cout << "Old log file removed: " << qUtf8Printable(logs.at(i).absoluteFilePath()) << std::endl;
                QFile::remove(logs.at(i).absoluteFilePath());
            }
        }

        this->m_logFile.reset(new QFile(this->m_logDir.absolutePath() + '/' +
                                        this->m_prefix + QDateTime::currentDateTimeUtc().toString("yyyy-MM-dd--hh-mm-ss") + ".log"));
        if (this->m_logFile->open(QFile::WriteOnly | QFile::Text))
        {
            this->m_logStream.setDevice(this->m_logFile.get());
            std::cerr << "Debugger: new log file " << qUtf8Printable(this->m_logFile->fileName()) << std::endl;

            this->m_logStream << qApp->applicationName() << " " << qApp->applicationVersion();

            if (this->m_prefix.isEmpty())
            {
                this->m_logStream << "\n\n";
            }

            else
            {
                this->m_logStream << QString::fromUtf8(" 〔") << this->m_prefix << QString::fromUtf8("〕") << "\n\n";
            }

            this->m_logStream << "Logging started at " << QDateTime::currentDateTimeUtc().toString("yyyy-MM-dd hh:mm:ss") << " UTC\n";
            this->m_logStream.flush();
            this->m_logFile->flush();
        }

        else
        {
            this->m_enabled = false;

            this->m_logFile->close();
            this->m_logFile.reset();

            std::cerr << "Debugger: unable to create log file" << std::endl;
        }
    }
}

void Debugger::setFilenamePrefix(const QString &prefix)
{
    QMutexLocker(&this->m_mutex);
    prefix.isEmpty() ? this->m_prefix = prefix : this->m_prefix = prefix + '-';
}

void Debugger::setMaxLogFilesToKeep(quint16 c)
{
    QMutexLocker(&this->m_mutex);
    this->m_maxLogFiles = c;
}

void Debugger::printToTerminal(bool enabled)
{
    QMutexLocker(&this->m_mutex);
    this->m_printToTerminal = enabled;
}

void Debugger::notice(const QString &message)
{
    this->log(message, Notice);
}

void Debugger::warning(const QString &message)
{
    this->log(message, Warning);
}

void Debugger::error(const QString &message)
{
    this->log(message, Error);
}

void Debugger::log(const QString &message, MessageType type)
{
    QMutexLocker(&this->m_mutex);

    QString formated = message;

    switch (type)
    {
        case Notice:
            break;

        case Warning:
            formated.prepend("⚠ WARNING! ─ ");
            break;

        case Error:
            formated.prepend(">ERROR! ─ ");
            break;
    }

    if (this->m_printToTerminal)
    {
        std::cerr << qUtf8Printable(formated) << std::endl;
    }

    if (this->m_valid && this->m_enabled)
    {
        this->m_logStream << QDateTime::currentDateTimeUtc().toString("yyyy-MM-dd hh:mm:ss") << " | "
                          << "@thread " << this->currentThreadName() << " | "
                          << formated << "\n";
        this->m_logStream.flush();
        this->m_logFile->flush();
    }

    formated.clear();
}

QString Debugger::currentThreadName()
{
    return QThread::currentThread()->userData(0)
         ? static_cast<ThreadId*>(QThread::currentThread()->userData(0))->getNamedId()
         : QString("0x%1").arg(reinterpret_cast<quintptr>(QThread::currentThreadId()),
                               QT_POINTER_SIZE * 2, 16, QChar('0'));
}
