#include <Server/RequestMapper.hpp>
#include <Global.hpp>

#include <QStandardPaths>
#include <QDBusReply>

#include <iostream>

void RequestMapper::apiBot(HttpRequest &request, HttpResponse &response, const QString &endpoint)
{
    Q_UNUSED(request);

    if (endpoint == "/start")
    {
        if (this->m_ifaceMaster->isValid())
        {
            this->m_ifaceMaster->call("startBot");
            response.setStatus(200);
            response.write(QByteArray("D-Bus call to 'startBot' was successful."), true);
        }

        else
        {
            this->dbusError(response);
        }
    }

    else if (endpoint == "/stop")
    {
        if (this->m_ifaceMaster->isValid())
        {
            this->m_ifaceMaster->call("stopBot");
            response.setStatus(200);
            response.write(QByteArray("D-Bus call to 'stopBot' was successful."), true);
        }

        else
        {
            this->dbusError(response);
        }
    }
}
