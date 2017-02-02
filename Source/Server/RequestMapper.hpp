#ifndef REQUESTMAPPER_HPP
#define REQUESTMAPPER_HPP

#include "HttpRequestHandler.hpp"

using namespace QtWebApp::HttpServer;

class RequestMapper : public HttpRequestHandler
{
    Q_OBJECT
    Q_DISABLE_COPY(RequestMapper)

public:
    RequestMapper(QObject *parent = nullptr);
    ~RequestMapper();

    void service(HttpRequest &request, HttpResponse &response);

signals:
    void shutdown();
};

#endif // REQUESTMAPPER_HPP
