#include "RequestMapper.hpp"

#include <Source/Global.hpp>
#include "Server.hpp"

#include "StaticFileController.hpp"

#include <iostream>
#include <QStandardPaths>

RequestMapper::RequestMapper(QObject *parent)
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
    QByteArray path = request.getPath();
    debugger->notice(QString("[HTTP %1] RequestMapper: (path=%2)").arg(request.getMethod().constData(), path.constData()));

    // todo: change to html when we got basic functionality working
    response.setHeader("Content-Type", "text/plain; charset=UTF-8");

    if (path == "/")
    {
        response.setStatus(200);
        response.write(QString("It's working :D").toUtf8(), true);
    }

    else if (path.startsWith("/api"))
    {
        this->api(request, response, path.mid(4));
    }

    else
    {
        //static StaticFileController *staticFileController = new StaticFileController(
        //            new QtWebApp::StaticFileControllerConfig(QStandardPaths::writableLocation(QStandardPaths::HomeLocation)));
        //staticFileController->setContentTypeEncoding("UTF-8");
        //staticFileController->service(request, response);

        response.setStatus(404);
        response.write(QString("No handler for URL '%1' found").arg(path.constData()).toUtf8(), true);
    }

    debugger->notice(QString("[HTTP %1] RequestMapper: finished request for (path=%2)").arg(request.getMethod().constData(), path.constData()));
}

void RequestMapper::api(HttpRequest &request, HttpResponse &response, const QString &endpoint)
{
    if (endpoint == "/shutdown")
    {
        response.setStatus(200);
        response.write(QByteArray("Shutting down..."), true);

        emit shutdown();
    }

    else
    {
        response.setStatus(400);
        response.write(QString("No such endpoint: %1").arg(request.getPath().constData()).toUtf8(), true);
    }
}
