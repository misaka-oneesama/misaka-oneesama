#include <QCoreApplication>
#include <QThread>

#include <QDebug>
#include <iostream>

#include <Source/Global.hpp>

#include "Server/Server.hpp"

int main(int argc, char** argv)
{
    QCoreApplication a(argc, argv);
    a.setApplicationName(QString::fromUtf8("御坂ーお姉さま"));
    a.setApplicationVersion(QLatin1String("v0.0.1"));
    a.setOrganizationName(QString::fromUtf8("マギルゥーベルベット"));
    a.setOrganizationDomain(QLatin1String("magiruuvelvet.gdn"));

    // Initialize configuration
    configManager = new ConfigManager();
    configManager->loadConfig();

    // Initialize debugger
    debugger = new Debugger();
    debugger->setMaxLogFilesToKeep(configManager->maxLogFilesToKeep());
    debugger->setLogDir(configManager->configPath() + "/logs");
    debugger->setEnabled(true);
    debugger->printToTerminal(true); // for better debugging during development

    // Initialize Server Thread
    QThread *serverThread = new QThread;
    Server *server = new Server();
    server->moveToThread(serverThread);

    QObject::connect(serverThread, &QThread::started, server, &Server::start);
    QObject::connect(server, &Server::stopped, serverThread, &QThread::quit);
    QObject::connect(server, &Server::stopped, server, &Server::deleteLater);
    QObject::connect(serverThread, &QThread::finished, serverThread, &QThread::deleteLater);

    QObject::connect(server, &Server::started, server, &Server::stop);

    serverThread->start();

    // notice: segfauls on termination using UNIX signal
    // fixme: implement a proper event loop for the code in here

    int status = a.exec();

    delete configManager;
    delete debugger;

    return status;
}
