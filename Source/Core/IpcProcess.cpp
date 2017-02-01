#include "IpcProcess.hpp"

#include <iostream>

#include <Source/Global.hpp>

IpcProcess::IpcProcess(QObject *parent)
    : QProcess(parent)
{
    QObject::connect(this, &IpcProcess::readyReadStandardOutput, this, &IpcProcess::processStandardOutput);
    QObject::connect(this, &IpcProcess::readyReadStandardError, this, &IpcProcess::processStandardError);

    QObject::connect(this, static_cast<void(IpcProcess::*)(int, IpcProcess::ExitStatus)>(&IpcProcess::finished),
                     this, &IpcProcess::processFinished);
    QObject::connect(this, &IpcProcess::errorOccurred, this, &IpcProcess::handleError);
}

IpcProcess::~IpcProcess()
{
}

void IpcProcess::setIdentifier(const InstanceType &id)
{
    this->m_id = id;
}

void IpcProcess::redirectOutput(bool enabled)
{
    this->m_redirectOutput = enabled;
}

void IpcProcess::terminate()
{
    debugger->notice(QString("IpcProcess: process %1 received SIGTERM").arg(
                         this->instanceName(this->m_id)));

    QProcess::terminate();
    if (!this->waitForFinished())
    {
        this->kill();
    }
}

void IpcProcess::processStandardOutput()
{
    if (this->m_redirectOutput)
    {
        QMutexLocker(&this->m_mutex);
        std::cout << this->instanceName(this->m_id) << ": " << this->readAllStandardOutput().constData() << std::flush;
    }
}

void IpcProcess::processStandardError()
{
    if (this->m_redirectOutput)
    {
        QMutexLocker(&this->m_mutex);
        std::cerr << this->instanceName(this->m_id) << ": " << this->readAllStandardError().constData() << std::flush;
    }
}

void IpcProcess::processFinished(int exitCode, QProcess::ExitStatus status)
{
    (void) status;
    debugger->notice(QString("IpcProcess: process %1 exited with status %2").arg(
                         this->instanceName(this->m_id),
                         QString::number(exitCode)));
}

void IpcProcess::handleError(QProcess::ProcessError error)
{
    debugger->error(QString("IpcProcess: error occurred in process %1").arg(
                        this->instanceName(this->m_id)));

    if (error == QProcess::Crashed)
    {
        // todo: respawn process
    }
}
