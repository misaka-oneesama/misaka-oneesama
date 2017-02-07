#ifndef REQUESTMAPPER_HPP
#define REQUESTMAPPER_HPP

#include <QDBusInterface>
#include <memory>

#include <QtWebApp/HttpServer/HttpRequestHandler>

using namespace QtWebApp::HttpServer;

class RequestMapper : public HttpRequestHandler
{
    Q_OBJECT
    Q_DISABLE_COPY(RequestMapper)

public:
    RequestMapper(QObject *parent = nullptr);
    ~RequestMapper();

    void service(HttpRequest &request, HttpResponse &response);

private:
    void api(HttpRequest &request, HttpResponse &response, const QString &endpoint);
    void apiBot(HttpRequest &request, HttpResponse &response, const QString &endpoint);

    void dbusError(HttpResponse &response);

signals:
    void shutdown();
    void reload();

private:
    std::unique_ptr<QDBusInterface> m_ifaceMaster;
    std::unique_ptr<QDBusInterface> m_ifaceServer;
    std::unique_ptr<QDBusInterface> m_ifaceBot;
};

#endif // REQUESTMAPPER_HPP
