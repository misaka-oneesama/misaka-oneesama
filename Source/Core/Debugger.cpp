#include "Debugger.hpp"

#include <QCoreApplication>
#include <QFile>
#include <QFileDevice>
#include <QFileInfo>
#include <QFileInfoList>
#include <QDateTime>

#include <iostream>

Debugger::Debugger()
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
        delete this->m_logFile;
    }

    this->m_logStream.setDevice(0);
}

bool Debugger::setLogDir(const QString &logDir)
{
    this->m_logDir = QDir(logDir);

    if (!this->m_logDir.exists())
    {
        QDir parent = this->m_logDir;
        if (parent.cdUp())
        {
            if (parent.mkdir(this->m_logDir.dirName()))
            {
                std::cerr << "Debugger: log directory created" << std::endl;
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
        std::cerr << "Debugger: log directory set to '" << qUtf8Printable(this->m_logDir.absolutePath()) << "'" << std::endl;
    }

    else
    {
        std::cerr << "Debugger: given log directory is not valid: " << qUtf8Printable(this->m_logDir.absolutePath()) << std::endl;
    }

    return this->m_valid;
}

void Debugger::setEnabled(bool enabled)
{
    this->m_enabled = enabled;

    if (this->m_enabled)
    {
        QFileInfoList logs = this->m_logDir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot, QDir::Time);
        if (logs.count() > this->m_maxLogFiles)
        {
            // FIXME: something is wrong here
            for (int i = 0; i < logs.count() - this->m_maxLogFiles + 1; i++)
            {
                //std::cout << qUtf8Printable(logs.at(i).absoluteFilePath()) << std::endl;
                QFile::remove(logs.at(i).absoluteFilePath());
            }
        }

        this->m_logFile = new QFile(this->m_logDir.absolutePath() + '/' + QDateTime::currentDateTimeUtc().toString("yyyy-MM-dd--hh-mm-ss") + ".log");
        if (this->m_logFile->open(QFile::WriteOnly | QFile::Text))
        {
            this->m_logStream.setDevice(this->m_logFile);
            std::cerr << "Debugger: new log file " << qUtf8Printable(this->m_logFile->fileName()) << std::endl;

            this->m_logStream << qApp->applicationName() << " " << qApp->applicationVersion() << "\n\n";
            this->m_logStream << "Logging started at " << QDateTime::currentDateTimeUtc().toString("yyyy-MM-dd hh:mm:ss") << " UTC\n";
            this->m_logStream.flush();
            this->m_logFile->flush();
        }

        else
        {
            this->m_enabled = false;

            this->m_logFile->close();
            delete this->m_logFile;

            std::cerr << "Debugger: unable to create log file" << std::endl;
        }
    }
}

void Debugger::setMaxLogFilesToKeep(int c)
{
    this->m_maxLogFiles = c;
}

void Debugger::notice(const QString &message)
{
    if (this->m_valid && this->m_enabled)
    {
        this->m_logStream << QDateTime::currentDateTimeUtc().toString("yyyy-MM-dd hh:mm:ss") << " | "
                          << message << "\n";
        this->m_logStream.flush();
        this->m_logFile->flush();
    }
}

void Debugger::warning(const QString &message)
{
    if (this->m_valid && this->m_enabled)
    {
        this->m_logStream << QDateTime::currentDateTimeUtc().toString("yyyy-MM-dd hh:mm:ss") << " | "
                          << "WARNING! " << message << "\n";
        this->m_logStream.flush();
        this->m_logFile->flush();
    }
}

void Debugger::error(const QString &message)
{
    if (this->m_valid && this->m_enabled)
    {
        this->m_logStream << QDateTime::currentDateTimeUtc().toString("yyyy-MM-dd hh:mm:ss") << " | "
                          << "ERROR! " << message << "\n";
        this->m_logStream.flush();
        this->m_logFile->flush();
    }
}
