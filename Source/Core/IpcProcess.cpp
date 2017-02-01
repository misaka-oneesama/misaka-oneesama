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

void IpcProcess::setIdentifier(quint8 id)
{
    this->m_id = id;
}

void IpcProcess::processStandardOutput()
{
    std::cout << this->readAllStandardOutput().constData() << std::endl;
}

void IpcProcess::processStandardError()
{
    std::cerr << this->readAllStandardError().constData() << std::endl;
}

void IpcProcess::processFinished(int exitCode, QProcess::ExitStatus status)
{
}

void IpcProcess::handleError(QProcess::ProcessError error)
{
    if (error == QProcess::Crashed)
    {
        // todo: respawn process
    }
}
