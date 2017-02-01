#include <QCoreApplication>
#include <QThread>

#include <QDBusInterface>
#include <QDBusServiceWatcher>
#include <QDBusAbstractAdaptor>
#include <QDBusVariant>
#include <QProcess>

#include <iostream>
#include <csignal>
#include <memory>

/// temporarily included
#include <QFile>
///======================

#include <Source/Global.hpp>
#include "Core/IpcProcess.hpp"

std::unique_ptr<QCoreApplication> a;
IpcProcess::InstanceType instance;

void terminate(int)
{
    std::cout << "---" << IpcProcess::instanceName(instance) << ": signal received, preparing to terminate...." << std::endl;

    if (server) server->stop();
    if (botManager) botManager->stop();

    a->quit();
}

int main(int argc, char **argv)
{
    a.reset(new QCoreApplication(argc, argv));
    a->setApplicationName(QString::fromUtf8("御坂ーお姉さま"));
    a->setApplicationVersion(QLatin1String("v0.0.1"));
    a->setOrganizationName(QString::fromUtf8("マギルゥーベルベット"));
    a->setOrganizationDomain(QLatin1String("magiruuvelvet.gdn"));

    bool silent = a->arguments().contains("--silent");

    if (!silent)
    {
        std::cout << a->applicationName().toUtf8().constData() << ' '
                  << a->applicationVersion().toUtf8().constData() << '\n' << std::endl;
    }

    // Determine instance type
    if (a->arguments().size() > 1)
    {
        const QString inst = a->arguments().at(1);

        if (inst.size() > 11 && inst.startsWith("--instance="))
        {
            if (inst.mid(11) == "master")
                instance = IpcProcess::InstanceType::Master;
            else if (inst.mid(11) == "server")
                instance = IpcProcess::InstanceType::Server;
            else if (inst.mid(11) == "bot")
                instance = IpcProcess::InstanceType::Bot;
            else
            {
                std::cerr << "---App: unkown instance type '" << inst.mid(11).toUtf8().constData() << "'" << std::endl;
                a.reset();
                std::exit(1);
            }
        }

        else
        {
            instance = IpcProcess::InstanceType::Master;
        }
    }

    else
    {
        instance = IpcProcess::InstanceType::Master;
    }

    std::cout << "---App: instance [" << IpcProcess::instanceName(instance) << "] selected." << std::endl;

    // Check if the D-Bus daemon is running
    if (!QDBusConnection::sessionBus().isConnected()) {
        std::cerr << "---" << IpcProcess::instanceName(instance) << ": unable to connect to the message bus!" << '\n'
                  << "---" << IpcProcess::instanceName(instance) << ": make sure that the D-Bus daemon is running." << std::endl;
        a.reset();
        std::exit(2);
    }

    // Register signal handlers
    std::cout << "---" << IpcProcess::instanceName(instance) << ": registering signals SIGINT, SIGTERM and SIGQUIT..." << std::endl;
    signal(SIGINT, terminate);
    signal(SIGTERM, terminate);
    signal(SIGQUIT, terminate);

    // Initialize configuration
    if (!silent)
        std::cout << "---App: creating [class] ConfigManager..." << std::endl;
    configManager = new ConfigManager(!silent);
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
    if (!silent)
        std::cout << "---App: creating [class] Debugger..." << std::endl;
    debugger = new Debugger(!silent);
    debugger->setFilenamePrefix(IpcProcess::instanceName(instance));
    debugger->setMaxLogFilesToKeep(configManager->maxLogFilesToKeep());
    debugger->setLogDir(configManager->configPath() + "/logs");
    debugger->setEnabled(true);
    debugger->printToTerminal(configManager->debuggerPrintToTerminal()); // true for debug builds, false otherwise

    if (instance == IpcProcess::InstanceType::Master)
    {
        std::cout << "---App: starting master process..." << std::endl;

        debugger->notice("Registering D-Bus service...");
        QDBusServiceWatcher serviceWatcher(dbus_service_name, QDBusConnection::sessionBus(), QDBusServiceWatcher::WatchForRegistration);
        //QObject::connect(&serviceWatcher, &QDBusServiceWatcher::serviceRegistered, );

        debugger->notice("IPC: Starting server instance...");
        ipcServer = new IpcProcess(a.get());
        ipcServer->setIdentifier(IpcProcess::InstanceType::Server);
        ipcServer->redirectOutput(configManager->debuggerPrintToTerminal());
        QObject::connect(ipcServer, static_cast<void(IpcProcess::*)(int, IpcProcess::ExitStatus)>(&IpcProcess::finished), ipcServer, &IpcProcess::deleteLater);
        QObject::connect(a.get(), &QCoreApplication::aboutToQuit, ipcServer, &IpcProcess::terminate);
        QObject::connect(a.get(), &QCoreApplication::aboutToQuit, ipcServer, &IpcProcess::deleteLater);
        ipcServer->start(a->arguments().at(0), QStringList({"--instance=server", "--silent"}), IpcProcess::ReadOnly);
        debugger->notice("IPC: Server instance started.");

        debugger->notice("IPC: Starting bot instance...");
        ipcBot = new IpcProcess(a.get());
        ipcBot->setIdentifier(IpcProcess::InstanceType::Bot);
        ipcBot->redirectOutput(configManager->debuggerPrintToTerminal());
        QObject::connect(ipcBot, static_cast<void(IpcProcess::*)(int, IpcProcess::ExitStatus)>(&IpcProcess::finished), ipcBot, &IpcProcess::deleteLater);
        QObject::connect(a.get(), &QCoreApplication::aboutToQuit, ipcBot, &IpcProcess::terminate);
        QObject::connect(a.get(), &QCoreApplication::aboutToQuit, ipcBot, &IpcProcess::deleteLater);
        ipcBot->start(a->arguments().at(0), QStringList({"--instance=bot", "--silent"}), IpcProcess::ReadOnly);
        debugger->notice("IPC: Bot instance started.");
    }

    else if (instance == IpcProcess::InstanceType::Server)
    {
        std::cout << "---App: starting server process..." << std::endl;

        server = new Server();
        server->setListeningAddress(QLatin1String("127.0.0.1"));
        server->setListeningPort(4555);
        //QObject::connect(a.get(), &QCoreApplication::aboutToQuit, server, &Server::stop);
        QObject::connect(server, &Server::stopped, server, &Server::deleteLater);
        QTimer::singleShot(0, server, &Server::start);
    }

    else if (instance == IpcProcess::InstanceType::Bot)
    {
        std::cout << "---App: starting bot process..." << std::endl;

        botManager = new BotManager();
        botManager->setOAuthToken(configManager->token());
        botManager->init();
        //QObject::connect(a.get(), &QCoreApplication::aboutToQuit, botManager, &BotManager::stop);
        QObject::connect(botManager, &BotManager::stopped, botManager, &BotManager::deleteLater);
        QTimer::singleShot(0, botManager, static_cast<void(BotManager::*)()>(&BotManager::login));
    }

    int status = a->exec();
    a.reset();

    delete configManager;
    delete debugger;

    std::cout << "---" << IpcProcess::instanceName(instance) << ": procss exited with code " << status << std::endl;
    return status;
}
