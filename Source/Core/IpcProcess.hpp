#ifndef IPCPROCESS_HPP
#define IPCPROCESS_HPP

#include <QProcess>

class IpcProcess : public QProcess
{
    Q_OBJECT

public:
    explicit IpcProcess(QObject *parent = nullptr);
    virtual ~IpcProcess();

    void setIdentifier(quint8);

public slots:
    void processStandardOutput();
    void processStandardError();
    void processFinished(int, QProcess::ExitStatus);
    void handleError(QProcess::ProcessError);

private:
    quint8 m_id = 0U;
};

#endif // IPCPROCESS_HPP
