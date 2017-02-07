/*****************************************************************************
 * 御坂ーお姉さま
 * Copyright (C) 2017 マギルゥ一ベルベット
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

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
    Debugger();
    Debugger(const QString &logDir);
    Debugger(const QString &logDir, bool enabled);
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
