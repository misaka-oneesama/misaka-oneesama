#include <Server/RequestMapper.hpp>
#include <Global.hpp>

#include <QStandardPaths>
#include <QDBusReply>

#include <iostream>

void RequestMapper::api(HttpRequest &request, HttpResponse &response, const QString &endpoint)
{
    // Just for reference: Don't implement Server::halt() endpoint here
    if (endpoint == "/shutdown")
    {
        response.setStatus(200);
        response.write(QByteArray("Shutting down..."), true);

        emit shutdown();
    }

    else if (endpoint == "/reload")
    {
        response.setStatus(200);
        response.write(QByteArray("Reloading server..."), true);

        emit reload();
    }

    else if (endpoint.startsWith("/bot"))
    {
        this->apiBot(request, response, endpoint.mid(4));
    }

    else
    {
        response.setStatus(400);
        response.write(QString("No such endpoint: %1").arg(request.getPath().constData()).toUtf8(), true);
    }
}
