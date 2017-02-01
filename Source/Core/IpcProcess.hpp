#ifndef IPCPROCESS_HPP
#define IPCPROCESS_HPP

#include <QProcess>
#include <QMutexLocker>
#include <QMutex>

class IpcProcess : public QProcess
{
    Q_OBJECT

public:
    explicit IpcProcess(QObject *parent = nullptr);
    virtual ~IpcProcess();

    enum class InstanceType : quint8 {
        Master = 0,
        Server = 1,
        Bot    = 2
    };

    static auto instanceName(const InstanceType &type)
    {
        switch (type)
        {
            case InstanceType::Master: return "Master"; break;
            case InstanceType::Server: return "Server"; break;
            case InstanceType::Bot:    return "Bot"; break;
        }

        return "";
    }

    void setIdentifier(const InstanceType &id);

    // if set to true the child process output will be redirected to the parent
    // uses a mutex to block, recommended to disable for Production/Release builds
    void redirectOutput(bool);

public slots:
    void terminate();

    void processStandardOutput();
    void processStandardError();
    void processFinished(int, QProcess::ExitStatus);
    void handleError(QProcess::ProcessError);

private:
    QMutex m_mutex;
    InstanceType m_id = InstanceType::Master;
    bool m_redirectOutput = false;
};

#endif // IPCPROCESS_HPP
