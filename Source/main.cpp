#include <QCoreApplication>
#include <QThread>

#include <iostream>
#include <csignal>

/// temporarily included
#include <QDebug>
#include <QFile>
///======================

#include <Source/Global.hpp>

#include "Server/Server.hpp"
#include "Core/BotManager.hpp"

QCoreApplication *a = nullptr;

#ifdef Q_OS_UNIX
void terminate(int)
{
    std::cout << "---App: signal received, preparing to terminate...." << std::endl;
    a->quit();
}
#endif

int main(int argc, char** argv)
{
    a = new QCoreApplication(argc, argv);
    a->setApplicationName(QString::fromUtf8("御坂ーお姉さま"));
    a->setApplicationVersion(QLatin1String("v0.0.1"));
    a->setOrganizationName(QString::fromUtf8("マギルゥーベルベット"));
    a->setOrganizationDomain(QLatin1String("magiruuvelvet.gdn"));

#ifdef Q_OS_UNIX
    std::cout << "---App: registering signals SIGINT, SIGTERM and SIGQUIT..." << std::endl;
    signal(SIGINT, terminate);
    signal(SIGTERM, terminate);
    signal(SIGQUIT, terminate);
#endif

    // Initialize configuration
    std::cout << "---App: creating [class] ConfigManager..." << std::endl;
    configManager = new ConfigManager();
    configManager->loadConfig();

     /// JUST FOR DEVELOPMENT =========================================================================================
      // MUST be a bot token, don't run this bot on a user account, note: QDiscord rejects user tokens anyway ;P
      QFile tokenFile(configManager->configPath() + "/token");
      if (tokenFile.exists() && tokenFile.open(QFile::ReadOnly | QFile::Text))
      {
          // create a file in "$config_path/token" and paste your bots token here
          // file MUST NOT END WITH A NEWLINE and MUST BE UTF-8 ENCODED!!!
          configManager->setOAuthToken(tokenFile.readAll().constData());
          tokenFile.close();
      }
     ///===============================================================================================================

    // Initialize debugger
    std::cout << "---App: creating [class] Debugger..." << std::endl;
    debugger = new Debugger();
    debugger->setMaxLogFilesToKeep(configManager->maxLogFilesToKeep());
    debugger->setLogDir(configManager->configPath() + "/logs");
    debugger->setEnabled(true);
    debugger->printToTerminal(configManager->debuggerPrintToTerminal()); // true for debug builds, false otherwise

    // Initialize Server Thread
    QThread *serverThread = new QThread;
    Server *server = new Server();
    server->setListeningAddress(QLatin1String("127.0.0.1")); //= todo: add to ConfigManager
    server->setListeningPort(4555);                          //=
    server->moveToThread(serverThread);

    QObject::connect(serverThread, &QThread::started, server, &Server::start);
    QObject::connect(server, &Server::stopped, serverThread, &QThread::quit);
    QObject::connect(server, &Server::stopped, server, &Server::deleteLater);
    QObject::connect(serverThread, &QThread::finished, serverThread, &QThread::deleteLater);

    serverThread->start();

    // Initialize Bot Thread
    QThread *botThread = new QThread;
    BotManager *botManager = new BotManager();
    botManager->moveToThread(botThread);
    botManager->setOAuthToken(configManager->token());
    botManager->init();

    QObject::connect(botThread, &QThread::started, botManager, static_cast<void(BotManager::*)()>(&BotManager::login));
    QObject::connect(botManager, &BotManager::stopped, botThread, &QThread::quit);
    QObject::connect(botManager, &BotManager::stopped, botManager, &BotManager::deleteLater);
    QObject::connect(botThread, &QThread::finished, botThread, &QThread::deleteLater);

    botThread->start();

    // notice: segfauls on termination using UNIX signal
    // fixme: implement a proper event loop for the code in here

    int status = a->exec();

    std::cout << "---App: cleaning up and freeing resources..." << std::endl;

    delete a;

    delete configManager;
    delete debugger;

    std::cout << "---App: exited with status code " << status << std::endl;

    return status;
}
