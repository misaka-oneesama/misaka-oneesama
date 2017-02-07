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

#include "IpcProcess.hpp"

#include <QMetaMethod>

#include <iostream>

IpcProcess::IpcProcess(QObject *parent)
    : QProcess(parent)
{
    QObject::connect(this, static_cast<void(IpcProcess::*)(int, IpcProcess::ExitStatus)>(&IpcProcess::finished),
                     this, &IpcProcess::processFinished);
    QObject::connect(this, &IpcProcess::errorOccurred, this, &IpcProcess::handleError);
}

IpcProcess::~IpcProcess()
{
}

void IpcProcess::setIdentifier(const InstanceType &id)
{
    QMutexLocker(&this->m_mutex);
    this->m_id = id;
}

void IpcProcess::redirectOutput(bool enabled)
{
    QMutexLocker(&this->m_mutex);

    if (enabled && !this->isSignalConnected(QMetaMethod::fromSignal(&IpcProcess::readyReadStandardOutput))
                && !this->isSignalConnected(QMetaMethod::fromSignal(&IpcProcess::readyReadStandardError)))
    {
        QObject::connect(this, &IpcProcess::readyReadStandardOutput, this, &IpcProcess::processStandardOutput);
        QObject::connect(this, &IpcProcess::readyReadStandardError, this, &IpcProcess::processStandardError);
    }

    else
    {
        QObject::disconnect(this, &IpcProcess::readyReadStandardOutput, this, &IpcProcess::processStandardOutput);
        QObject::disconnect(this, &IpcProcess::readyReadStandardError, this, &IpcProcess::processStandardError);
    }
}

bool IpcProcess::isRunning() const
{
    return (this->state() == IpcProcess::Starting || this->state() == IpcProcess::Running);
}

void IpcProcess::terminate()
{
    QMutexLocker(&this->m_mutex);

    debugger->notice(QString("IpcProcess: process %1 received SIGTERM").arg(instanceName(this->m_id)));

    QProcess::terminate();
    if (!this->waitForFinished())
    {
        this->kill();
    }
}

void IpcProcess::processStandardOutput()
{
    QMutexLocker(&this->m_mutex);
    std::cout << instanceName(this->m_id) << ": " << this->readAllStandardOutput().constData() << std::flush;
}

void IpcProcess::processStandardError()
{
    QMutexLocker(&this->m_mutex);
    std::cerr << instanceName(this->m_id) << ": " << this->readAllStandardError().constData() << std::flush;
}

void IpcProcess::processFinished(int exitCode, QProcess::ExitStatus status)
{
    (void) status;
    debugger->notice(QString("IpcProcess: process %1 exited with status %2").arg(
                         instanceName(this->m_id),
                         QString::number(exitCode)));
}

void IpcProcess::handleError(QProcess::ProcessError error)
{
    debugger->error(QString("IpcProcess: error occurred in process %1").arg(instanceName(this->m_id)));

    if (error == QProcess::Crashed)
    {
        // todo: respawn process
    }
}
