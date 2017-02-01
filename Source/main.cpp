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
#include <QDebug>
#include <QFile>
///======================

#include <Source/Global.hpp>
#include "Core/IpcProcess.hpp"

#include "Server/Server.hpp"
#include "Core/BotManager.hpp"

std::unique_ptr<QCoreApplication> a;

enum class InstanceType : quint8 {
    Master = 0,
    Server = 1,
    Bot    = 2
} instance;

auto instanceName(const InstanceType &type)
{
    switch (type)
    {
        case InstanceType::Master: return "Master"; break;
        case InstanceType::Server: return "Server"; break;
        case InstanceType::Bot:    return "Bot"; break;
    }

    return "";
}

void terminate(int)
{
    std::cout << "---" << instanceName(instance) << ": signal received, preparing to terminate...." << std::endl;
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
                instance = InstanceType::Master;
            else if (inst.mid(11) == "server")
                instance = InstanceType::Server;
            else if (inst.mid(11) == "bot")
                instance = InstanceType::Bot;
            else
            {
                std::cerr << "---App: unkown instance type '" << inst.mid(11).toUtf8().constData() << "'" << std::endl;
                a.reset();
                std::exit(1);
            }
        }

        else
        {
            instance = InstanceType::Master;
        }
    }

    else
    {
        instance = InstanceType::Master;
    }

    std::cout << "---App: instance [" << instanceName(instance) << "] selected." << std::endl;

    // Check if the D-Bus daemon is running
    if (!QDBusConnection::sessionBus().isConnected()) {
        std::cerr << "---" << instanceName(instance) << ": unable to connect to the message bus!" << '\n'
                  << "---" << instanceName(instance) << ": make sure that the D-Bus daemon is running." << std::endl;
        a.reset();
        std::exit(2);
    }

    // Register signal handlers
    std::cout << "---" << instanceName(instance) << ": registering signals SIGINT, SIGTERM and SIGQUIT..." << std::endl;
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
    debugger->setFilenamePrefix(instanceName(instance));
    debugger->setMaxLogFilesToKeep(configManager->maxLogFilesToKeep());
    debugger->setLogDir(configManager->configPath() + "/logs");
    debugger->setEnabled(true);
    debugger->printToTerminal(configManager->debuggerPrintToTerminal()); // true for debug builds, false otherwise

    if (instance == InstanceType::Master)
    {
        std::cout << "---App: starting master process..." << std::endl;

        QDBusServiceWatcher serviceWatcher(dbus_service_name, QDBusConnection::sessionBus(), QDBusServiceWatcher::WatchForRegistration);
        //QObject::connect(&serviceWatcher, &QDBusServiceWatcher::serviceRegistered, );

        IpcProcess *server = new IpcProcess(a.get());
        server->setIdentifier(static_cast<quint8>(InstanceType::Server));
        QObject::connect(server, static_cast<void(IpcProcess::*)(int, IpcProcess::ExitStatus)>(&IpcProcess::finished), server, &IpcProcess::deleteLater);
        QObject::connect(a.get(), &QCoreApplication::aboutToQuit, server, &IpcProcess::terminate);
        QObject::connect(a.get(), &QCoreApplication::aboutToQuit, server, &IpcProcess::deleteLater);
        server->start(a->arguments().at(0), QStringList({"--instance=server", "--silent"}), IpcProcess::ReadOnly);

        IpcProcess *bot = new IpcProcess(a.get());
        server->setIdentifier(static_cast<quint8>(InstanceType::Bot));
        QObject::connect(bot, static_cast<void(IpcProcess::*)(int, IpcProcess::ExitStatus)>(&IpcProcess::finished), bot, &IpcProcess::deleteLater);
        QObject::connect(a.get(), &QCoreApplication::aboutToQuit, bot, &IpcProcess::terminate);
        QObject::connect(a.get(), &QCoreApplication::aboutToQuit, bot, &IpcProcess::deleteLater);
        bot->start(a->arguments().at(0), QStringList({"--instance=bot", "--silent"}), IpcProcess::ReadOnly);
    }

    else if (instance == InstanceType::Server)
    {
        std::cout << "---App: starting server process..." << std::endl;

        Server *server = new Server();
        server->setListeningAddress(QLatin1String("127.0.0.1"));
        server->setListeningPort(4555);
        QTimer::singleShot(0, server, &Server::start);
    }

    else if (instance == InstanceType::Bot)
    {
        std::cout << "---App: starting bot process..." << std::endl;

        BotManager *botManager = new BotManager();
        botManager->setOAuthToken(configManager->token());
        botManager->init();
        QTimer::singleShot(0, botManager, static_cast<void(BotManager::*)()>(&BotManager::login));
    }

    int status = a->exec();
    a.reset();

    std::cout << "---" << instanceName(instance) << ": procss exited with code " << status << std::endl;
    return status;
}
