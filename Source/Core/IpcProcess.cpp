#include "IpcProcess.hpp"
#include <Global.hpp>

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

void IpcProcess::terminate()
{
    QMutexLocker(&this->m_mutex);

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
    QMutexLocker(&this->m_mutex);
    std::cout << this->instanceName(this->m_id) << ": " << this->readAllStandardOutput().constData() << std::flush;
}

void IpcProcess::processStandardError()
{
    QMutexLocker(&this->m_mutex);
    std::cerr << this->instanceName(this->m_id) << ": " << this->readAllStandardError().constData() << std::flush;
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
