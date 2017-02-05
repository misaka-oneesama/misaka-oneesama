#include <QCoreApplication>
#include <QThread>

#include <QDBusInterface>
#include <QDBusServiceWatcher>
#include <QDBusVariant>
#include <QProcess>
#include <QFile>

#include <iostream>
#include <csignal>
#include <memory>
#include <cstdio>

#include <Global.hpp>
#include <Core/IpcProcess.hpp>
#include <Core/DBusInterface.hpp>

std::unique_ptr<QCoreApplication> a;
InstanceType instance;

void terminate(int)
{
    std::cout << "---" << instanceName(instance) << ": signal received, preparing to terminate...." << std::endl;

    if (server) server->stop();
    if (botManager) botManager->stop();

    a->quit();
}

int main(int argc, char **argv)
{
    a.reset(new QCoreApplication(argc, argv));
    a->setApplicationName(QString::fromUtf8("御坂ーお姉さま"));
    a->setApplicationVersion(QLatin1String("v0.0.2"));
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
        std::cerr << "---" << instanceName(instance) << qUtf8Printable(QDBusConnection::sessionBus().lastError().message()) << std::endl;

        a.reset();
        std::exit(2);
    }

    const QDBusConnection::RegisterOptions dbus_flags =
              QDBusConnection::ExportAllSlots
            | QDBusConnection::ExportAllSignals
            | QDBusConnection::ExportAllProperties
            | QDBusConnection::ExportAllInvokables
            | QDBusConnection::ExportAllContents;

    // Register signal handlers
    if (instance == InstanceType::Master)
    {
        std::cout << "---" << instanceName(instance) << ": registering signals SIGINT, SIGTERM, SIGQUIT, SIGSEGV and SIGILL..." << std::endl;
        signal(SIGINT, terminate);
        signal(SIGTERM, terminate);
        signal(SIGQUIT, terminate);
        signal(SIGSEGV, terminate);
        signal(SIGILL, terminate);
    }

    else
    {
        std::cout << "---" << instanceName(instance) << ": registering signals SIGINT, SIGTERM and SIGQUIT..." << std::endl;
        signal(SIGINT, terminate);
        signal(SIGTERM, terminate);
        signal(SIGQUIT, terminate);
    }

    // Initialize debugger
    if (!silent)
    {
        std::cout << "---App: creating [class] Debugger..." << std::endl;
    }

    debugger = new Debugger(!silent);
    debugger->setFilenamePrefix(instanceName(instance));

    if (instance == ItMaster)
    {
        // Initialize configuration
        if (!silent)
        {
            std::cout << "---App: creating [class] ConfigManager..." << std::endl;
        }

        configManager = new ConfigManager(!silent);
        configManager->loadConfig();

        debugger->setMaxLogFilesToKeep(configManager->maxLogFilesToKeep());
        debugger->setLogDir(configManager->configPath() + "/logs");
        debugger->printToTerminal(configManager->debuggerPrintToTerminal()); // true for debug builds, false otherwise
        debugger->setEnabled(true);

        // Load the OAuth token from an individual file ("$config_path/token")
        bool hasToken = false;
        QFile tokenFile(configManager->configPath() + "/token");
        if (tokenFile.exists() && tokenFile.open(QFile::ReadOnly | QFile::Text))
        {
            // fix permissions
            tokenFile.setPermissions(QFile::ReadOwner | QFile::WriteOwner);

            // contents will be trimmed, file should be UTF-8 encoded
            configManager->setOAuthToken(tokenFile.readAll().trimmed().constData());
            tokenFile.close();
            hasToken = true;
        }

        if (!hasToken || configManager->token().size() < 30) // TODO/RESEARCH: is the token always 59 chars long, can it be any shorter or larger?
        {
            debugger->error("No OAuth token found!");
            std::cerr << "---App: No OAuth token found!" << "\n\n"
                      << " Please paste your bot's token in the following file, create it if it doesn't exists." << '\n'
                      << " --> " << qUtf8Printable(configManager->configPath()) << "/token" << '\n'
                      << " Recommended: Make sure the file doesn't end with a new line and is UTF-8 encoded."
                      << std::endl;

            a.reset();
            delete configManager;
            delete debugger;
            std::exit(1);
        }

        std::cout << "---App: starting master process..." << std::endl;

        //debugger->notice("Registering D-Bus service...");
        //QDBusServiceWatcher serviceWatcher(dbus_service_name, QDBusConnection::sessionBus(), QDBusServiceWatcher::WatchForRegistration);
        //QObject::connect(&serviceWatcher, &QDBusServiceWatcher::serviceRegistered, );

        if (!QDBusConnection::sessionBus().registerService(DBusServiceName(ItMaster)))
        {
            debugger->error(QDBusConnection::sessionBus().lastError().message());

            a.reset();
            delete configManager;
            delete debugger;
            std::exit(1);
        }

        debugger->notice("IPC: Starting server instance...");
        createIpcProcess(ItServer, a.get());
        ipc(ItServer)->setIdentifier(ItServer);
        ipc(ItServer)->redirectOutput(configManager->debuggerPrintToTerminal());
        //QObject::connect(ipcServer, static_cast<void(IpcProcess::*)(int, IpcProcess::ExitStatus)>(&IpcProcess::finished), ipcServer, &IpcProcess::deleteLater);
        QObject::connect(a.get(), &QCoreApplication::aboutToQuit, ipc(ItServer), &IpcProcess::terminate);
        QObject::connect(a.get(), &QCoreApplication::aboutToQuit, ipc(ItServer), &IpcProcess::deleteLater);
        ipc(ItServer)->setProgram(a->arguments().at(0));
        ipc(ItServer)->setArguments(QStringList({"--instance=server",
            "--listen=" + configManager->serverListeningAddress(),
            "--port=" + QString::number(configManager->serverListeningPort()),
            "--silent",
            configManager->debuggerPrintToTerminal() ? "--terminal-logging" : "--no-terminal-logging",
            "--log-max=" + QString::number(configManager->maxLogFilesToKeep()),
            "--log-dir=" + debugger->logDir()}));
        ipc(ItServer)->start(IpcProcess::ReadOnly);
        debugger->notice("IPC: Server instance started.");

        debugger->notice("IPC: Starting bot instance...");
        createIpcProcess(ItBot, a.get());
        ipc(ItBot)->setIdentifier(ItBot);
        ipc(ItBot)->redirectOutput(configManager->debuggerPrintToTerminal());
        //QObject::connect(ipcBot, static_cast<void(IpcProcess::*)(int, IpcProcess::ExitStatus)>(&IpcProcess::finished), ipcBot, &IpcProcess::deleteLater);
        QObject::connect(a.get(), &QCoreApplication::aboutToQuit, ipc(ItBot), &IpcProcess::terminate);
        QObject::connect(a.get(), &QCoreApplication::aboutToQuit, ipc(ItBot), &IpcProcess::deleteLater);
        ipc(ItBot)->setProgram(a->arguments().at(0));
        ipc(ItBot)->setArguments(QStringList({"--instance=bot",
            "--token=" + configManager->token(),
            "--silent",
            configManager->debuggerPrintToTerminal() ? "--terminal-logging" : "--no-terminal-logging",
            "--log-max=" + QString::number(configManager->maxLogFilesToKeep()),
            "--log-dir=" + debugger->logDir()}));
        //ipc(ItBot)->start(IpcProcess::ReadOnly);
        debugger->notice("IPC: Bot instance started.");

        debugger->notice("Registering D-Bus service...");
        DBusInterface *dbus_iface = new DBusInterface();
        dbus_iface->setServerProcess(ipc(ItServer));
        dbus_iface->setBotProcess(ipc(ItBot));

        if (!QDBusConnection::sessionBus().registerObject("/", DBusServiceName(ItMaster), dbus_iface, dbus_flags))
        {
            debugger->error(QDBusConnection::sessionBus().lastError().message());
            a->quit();
        }
    }

    else if (instance == ItServer)
    {
        std::cout << "---App: starting server process..." << std::endl;
        std::fclose(stdin);

        bool failed = false;

        if (argc < 3) // FIXME!!
        {
            std::cerr << instanceName(instance) << ": Too few command line arguments! Please don't run the instance directly." << std::endl;
            a.reset();
            delete configManager;
            delete debugger;
            std::exit(1);
        }

        debugger->setMaxLogFilesToKeep(a->arguments().at(6).mid(10).toInt());
        debugger->setLogDir(a->arguments().at(7).mid(10));
        debugger->printToTerminal(a->arguments().contains("--terminal-logging"));
        debugger->setEnabled(true);

        QDBusInterface iface(DBusServiceName(ItMaster), "/", "", QDBusConnection::sessionBus());
        if (!iface.isValid())
        {
            debugger->error("D-Bus: the master process interface couldn't be found!");
            debugger->error("D-Bus: refusing to start!");
            debugger->error("D-Bus: " + QDBusConnection::sessionBus().lastError().message());
            a.reset();
            delete configManager;
            delete debugger;
            std::exit(1);
        }

        server = new Server();
        ServerDBusAdapter *server_dbus = new ServerDBusAdapter(server);
        server->setListeningAddress(a->arguments().at(2).mid(9));
        server->setListeningPort(a->arguments().at(3).mid(7).toInt());
        //QObject::connect(a.get(), &QCoreApplication::aboutToQuit, server, &Server::stop);
        QObject::connect(server, &Server::stopped, server, &Server::deleteLater);
        QObject::connect(server, &Server::stopped, a.get(), &QCoreApplication::quit);

        if (!QDBusConnection::sessionBus().registerService(DBusServiceName(ItServer)))
        {
            debugger->error(QDBusConnection::sessionBus().lastError().message());
            failed = true;
        }

        if (!QDBusConnection::sessionBus().registerObject("/", DBusServiceName(ItServer), server_dbus, dbus_flags))
        {
            debugger->error(QDBusConnection::sessionBus().lastError().message());
            failed = true;
        }

        if (failed)
        {
            a.reset();
            delete server_dbus;
            delete server;
            delete configManager;
            delete debugger;
            std::exit(1);
        }

        QTimer::singleShot(0, server, &Server::start);
    }

    else if (instance == ItBot)
    {
        std::cout << "---App: starting bot process..." << std::endl;
        std::fclose(stdin);

        bool failed = false;

        if (argc < 3) // FIXME!
        {
            std::cerr << instanceName(instance) << ": Too few command line arguments! Please don't run the instance directly." << std::endl;
            a.reset();
            delete configManager;
            delete debugger;
            std::exit(1);
        }

        debugger->setMaxLogFilesToKeep(a->arguments().at(5).mid(10).toInt());
        debugger->setLogDir(a->arguments().at(6).mid(10));
        debugger->printToTerminal(a->arguments().contains("--terminal-logging"));
        debugger->setEnabled(true);

        QDBusInterface iface(DBusServiceName(ItMaster), "/", "", QDBusConnection::sessionBus());
        if (!iface.isValid())
        {
            debugger->error("D-Bus: the master process interface couldn't be found!");
            debugger->error("D-Bus: refusing to start!");
            debugger->error("D-Bus: " + QDBusConnection::sessionBus().lastError().message());
            a.reset();
            delete configManager;
            delete debugger;
            std::exit(1);
        }

        botManager = new BotManager();
        BotManagerDBusAdapter *botManager_dbus = new BotManagerDBusAdapter(botManager);
        botManager->setOAuthToken(a->arguments().at(2).mid(8));

        // remove OAuth token from argv (hides it from /proc/self/exe, ps ax, and other sources)
        std::size_t argvlen = strlen(argv[2]);
        for (std::size_t i = 8; i < argvlen; i++)
        {
            argv[2][i] = '*';
        }

        //QObject::connect(a.get(), &QCoreApplication::aboutToQuit, botManager, &BotManager::stop);
        QObject::connect(botManager, &BotManager::stopped, botManager, &BotManager::deleteLater);
        QObject::connect(botManager, &BotManager::stopped, a.get(), &QCoreApplication::quit);

        if (!QDBusConnection::sessionBus().registerService(DBusServiceName(ItBot)))
        {
            debugger->error(QDBusConnection::sessionBus().lastError().message());
            failed = true;
        }

        if (!QDBusConnection::sessionBus().registerObject("/", DBusServiceName(ItBot), botManager_dbus, dbus_flags))
        {
            debugger->error(QDBusConnection::sessionBus().lastError().message());
            failed = true;
        }

        if (failed)
        {
            a.reset();
            delete botManager_dbus;
            delete botManager;
            delete configManager;
            delete debugger;
            std::exit(1);
        }

        QTimer::singleShot(0, botManager, static_cast<void(BotManager::*)()>(&BotManager::login));
    }

    int status = a->exec();
    a.reset();

    delete configManager;
    delete debugger;

    std::cout << "---" << instanceName(instance) << ": procss exited with code " << status << std::endl;
    return status;
}
