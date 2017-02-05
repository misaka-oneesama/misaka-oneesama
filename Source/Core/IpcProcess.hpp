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
