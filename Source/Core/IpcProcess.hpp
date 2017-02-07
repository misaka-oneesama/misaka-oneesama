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

#ifndef IPCPROCESS_HPP
#define IPCPROCESS_HPP

#include <Global.hpp>

#include <QProcess>
#include <QMutexLocker>
#include <QMutex>

class IpcProcess : public QProcess
{
    Q_OBJECT

public:
    explicit IpcProcess(QObject *parent = nullptr);
    virtual ~IpcProcess();

    void setIdentifier(const InstanceType &id);

    // if set to true the child process output will be redirected to the parent
    // uses a mutex to block, recommended to disable for Production/Release builds
    void redirectOutput(bool);

    bool isRunning() const;

public slots:
    void terminate();

    void processStandardOutput();
    void processStandardError();
    void processFinished(int, QProcess::ExitStatus);
    void handleError(QProcess::ProcessError);

private:
    QMutex m_mutex;
    InstanceType m_id = InstanceType::None;
};

#endif // IPCPROCESS_HPP
