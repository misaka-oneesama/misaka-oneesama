#include "RequestMapper.hpp"

#include <Source/Global.hpp>

#include <iostream>

RequestMapper::RequestMapper(QObject* parent)
    : HttpRequestHandler(parent)
{
    debugger->notice("RequestMapper created");
}


RequestMapper::~RequestMapper()
{
    debugger->notice("RequestMapper destroyed");
}

void RequestMapper::service(HttpRequest &request, HttpResponse &response)
{
    std::cout << "................" << std::endl;
    QByteArray path = request.getPath();
    debugger->notice(QString("[HTTP %1] RequestMapper: (path=%2)").arg(request.getMethod().constData(), path.constData()));

    // todo: change to html when we got basic functionality working
    response.setHeader("Content-Type", "text/plain; charset=UTF-8");

    // note: all responses here are for experimenting right now
    // also the path handling should be more professional than this anyway

    if (path == "/")
    {
        response.setStatus(200, QString("OK").toUtf8());
        response.write(QString("It's working :D").toUtf8(), true);
    }

    else
    {
        response.setStatus(404, QString("Not Found").toUtf8());
        response.write(QString("No handler for URL '%1' found").arg(path.constData()).toUtf8());
    }

    debugger->notice(QString("[HTTP %1] RequestMapper: finished request for (path=%2)").arg(request.getMethod().constData(), path.constData()));
}
