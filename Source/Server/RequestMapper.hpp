#ifndef REQUESTMAPPER_HPP
#define REQUESTMAPPER_HPP

#include "httprequesthandler.h"

using namespace stefanfrings;

class RequestMapper : public HttpRequestHandler {
    Q_OBJECT
    Q_DISABLE_COPY(RequestMapper)

public:
    RequestMapper(QObject* parent = nullptr);
    ~RequestMapper();

    void service(HttpRequest &request, HttpResponse &response);
};

#endif // REQUESTMAPPER_HPP
