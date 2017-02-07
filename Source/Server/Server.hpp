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

#ifndef SERVER_HPP
#define SERVER_HPP

#include <QObject>
#include <QString>
#include <QMutex>
#include <QDBusVariant>

#include <memory>

#include <QtWebApp/HttpServer/HttpListener>
#include <Server/RequestMapper.hpp>

using namespace QtWebApp::HttpServer;

class Server : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString listeningAddress MEMBER m_listeningAddress WRITE setListeningAddress NOTIFY listeningAddressChanged)
    Q_PROPERTY(quint16 listeningPort MEMBER m_listeningPort WRITE setListeningPort NOTIFY listeningPortChanged)

public:
    Server(QObject *parent = nullptr);
    Server(const QString &listeningAddress, const quint16 &listeningPort, QObject *parent = nullptr);
    ~Server();

    enum ErrorCode : quint8 {
        InvalidAddressOrPort
    };

    static bool isPortAvailable(const quint16 &port);
    const bool &isRunning() const;

    const QString &listeningAddress() const;
    const quint16 &listeningPort() const;

public slots:
    void setListeningAddress(const QString &address);
    void setListeningPort(const quint16 &port);
    void setListeningPortUnsafe(const quint16 &port); // prevent usage, doesn't check if port is available for bind

    void start();
    void stop();
    void halt();
    void restart(); // wrapper around `stop` and `start` with its own signal

signals:
    void listeningAddressChanged();
    void listeningPortChanged();

    void started();
    void stopped();
    void halted();
    void restarted();

    void error(ErrorCode);

private:
    QMutex m_mutex;

    bool m_isRunning = false;

    std::unique_ptr<QtWebApp::HttpServerSettings> m_httpServerSettings;
    std::unique_ptr<HttpListener> m_httpListener;
    std::unique_ptr<RequestMapper> m_requestMapper;

    QString m_listeningAddress = "127.0.0.1";
    quint16 m_listeningPort = 4555;

    void setPortInternal(const quint16 &port);
    void startInternal();
    void stopInternal();
};

class ServerDBusAdapter : public QObject
{
    Q_OBJECT

public:
    ServerDBusAdapter(Server *server, QObject *parent = nullptr);
    ~ServerDBusAdapter();

public slots:
    Q_NOREPLY void start();
    Q_NOREPLY void stop();
    Q_NOREPLY void halt(); // stop Server without terminating process
    Q_SCRIPTABLE bool reload();

    Q_SCRIPTABLE bool isPortAvailable(const quint16 &port);

    Q_SCRIPTABLE bool setAddress(const QString &address);
    Q_SCRIPTABLE bool setPort(const quint16 &port);

    Q_SCRIPTABLE QString address();
    Q_SCRIPTABLE quint16 port();

private:
    QMutex m_mutex;
    Server *d = nullptr;
};

#endif // SERVER_HPP
