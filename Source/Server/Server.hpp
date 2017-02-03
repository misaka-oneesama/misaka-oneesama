#ifndef SERVER_HPP
#define SERVER_HPP

#include <QObject>
#include <QString>

#include <QDBusAbstractAdaptor>
#include <QDBusVariant>

#include <memory>

#include "RequestMapper.hpp"

// QtWebApp
#include "HttpListener.hpp"

using namespace QtWebApp::HttpServer;

class Server : public QObject //QDBusAbstractAdaptor
{
    Q_OBJECT
    //Q_CLASSINFO("D-Bus Interface", "moe.misaka-oneesama.discordbot.Server")
    Q_PROPERTY(QString listeningAddress MEMBER m_listeningAddress WRITE setListeningAddress NOTIFY listeningAddressChanged)
    Q_PROPERTY(quint16 listeningPort MEMBER m_listeningPort WRITE setListeningPort NOTIFY listeningPortChanged)

public:
    Server(QObject *parent = nullptr);
    Server(const QLatin1String &listeningAddress, quint16 listeningPort, QObject *parent = nullptr);
    ~Server();

    enum ErrorCode : quint8 {
        ServerNotConfigured = 0
    };

public slots:
    void setListeningAddress(const QString &address);
    void setListeningPort(quint16 port);

    void start();
    void stop();
    void restart(); // wrapper around `stop` and `start`

signals:
    void listeningAddressChanged();
    void listeningPortChanged();
    void started();
    void stopped();
    void restarted();
    void error(ErrorCode);

private:
    bool m_configured = false;
    bool m_isRunning = false;

    std::unique_ptr<QtWebApp::HttpServerSettings> m_httpServerSettings;
    std::unique_ptr<HttpListener> m_httpListener;
    std::unique_ptr<RequestMapper> m_requestMapper;

    QString m_listeningAddress = QLatin1String("127.0.0.1");
    quint16 m_listeningPort = 4555; // default server port

    void p_startPrivate();
    void p_stopPrivate();
};

#endif // SERVER_HPP
