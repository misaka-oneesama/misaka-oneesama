/*****************************************************************************
 * 御坂ーお姉さま
 * Copyright (C) 2017 マギルゥ一ベルベット
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <QCoreApplication>
#include <QCommandLineParser>
#include <QThread>

#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusServiceWatcher>
#include <QDBusVariant>
#include <QProcess>
#include <QFile>

#include <iostream>
#include <cstdio>
#include <csignal>
#include <memory>

#include <Global.hpp>
#include <Core/IpcProcess.hpp>
#include <Core/ThreadId.hpp>
#include <Core/DBusInterface.hpp>

std::unique_ptr<QCoreApplication> a;
InstanceType instance = ItMaster;

QThread *threadMain = nullptr;
ThreadId *threadId = new ThreadId("main");

void signal_terminate(int)
{
    std::cout << "---" << instanceName(instance) << ": signal received, preparing to terminate...." << std::endl;

    if (server) server->stop();
    if (botManager) botManager->stop();

    a->quit();
}

void signal_sigsegv(int)
{
    std::cout << "---" << instanceName(instance) << ": bad things happened, saving your life now...." << std::endl;
    static quint16 max_terminate_tries = 10;

    if (server) server->stop();
    if (botManager) botManager->stop();

    a->quit();

    max_terminate_tries--;
    std::cout << "---" << instanceName(instance) << ": failed to terminate normally! Trying again (" << max_terminate_tries << ")/0" << std::endl;

    if (max_terminate_tries <= 0)
    {
        std::cout << "---" << instanceName(instance) << ": shit's fucked yo. Force killing process!" << std::endl;
        std::exit(128 + SIGSEGV);
        std::raise(SIGKILL); // just in case™
    }
}

Q_NORETURN void terminate(quint8 exitCode)
{
    a.reset();
    delete configManager;
    delete configDir;
    delete debugger;
    delete threadId;
    threadId = nullptr;
    threadMain->setUserData(0, nullptr);
    std::exit(exitCode);
}

Q_NORETURN void terminate(TerminateReason reason)
{
    terminate(static_cast<quint8>(reason));
}

#ifdef MISAKA_RELEASE
void disable_qdebug(QtMsgType, const QMessageLogContext&, const QString&)
{
//    context.file, context.line, context.function
//    case QtDebugMsg:
//    case QtInfoMsg:
//    case QtWarningMsg:
//    case QtCriticalMsg:
//    case QtFatalMsg:
}
#endif

int main(int argc, char **argv)
{
    // Close input stream
    std::fclose(stdin);

#ifdef MISAKA_RELEASE
    qInstallMessageHandler(disable_qdebug);
#endif

    // Initialize QCoreApplication
    a.reset(new QCoreApplication(argc, argv));
    a->setApplicationName(QString::fromUtf8("御坂ーお姉さま"));
    a->setApplicationVersion(QLatin1String("v0.0.3"));
    a->setOrganizationName(QString::fromUtf8("マギルゥーベルベット"));
    a->setOrganizationDomain(QLatin1String("magiruuvelvet.gdn"));

    threadMain = QThread::currentThread();
    threadMain->setUserData(0, threadId);

    QCommandLineParser parser;
    parser.setApplicationDescription("Legend: [a]=All Instances, [i]=Only childs of master, [m]=Master, [s]=Server, [b]=Bot");
    parser.addOptions({
        /// Explicitly override the "help" and "version" options. The parser handles them internally,
        /// even when not calling `add{Help,Version}Option` they are added anyway. Because QCoreApplication
        /// is created at the heap and is managed by a smart pointer, the parser just makes the app SIGSEGV.
        {{"h", "help"},             "Displays this help."},
        {{"v", "version"},          "Displays version information."},

        /// Program options
        {{"i", "instance"},         "[a] Starts the appropriate instance. Valid instances are `master` (default), `server` and `bot`. "
                                    "If no instance type is given `master` is selected.", "type"},
        {"skip-server",             "[m] Don't start the server process during startup."},
        {"skip-bot",                "[m] Don't start the bot process during startup."},
        {{"n", "no-init-messages"}, "[a] Hides all unimportant startup messages."},
        {"terminal-logging",        "[i] Prints all messages to the terminal. Warning! Degrades performance! [Synced and Blocking]"},
        {"listen",                  "[s] Server listening address.", "address"},
        {"port",                    "[s] Server listening port.", "port"},
        {"token",                   "[b] Discord OAuth token. Must be a Bot token!", "token"},
        {"log-max",                 "[i] Maximum number of old log files to keep. [autoremove]", "number"},
        {"log-dir",                 "[i] Directory where to store log files. Must be an existing and writable directory.", "directory"}
    });

    if (!parser.parse(a->arguments()))
    {
        std::cout << qUtf8Printable(parser.errorText()) << std::endl;
        a.reset();
        return reasonToInt(TerminateReason::CommandLineParsingError);
    }

    if (parser.isSet("help"))
    {
        std::cout << qUtf8Printable(parser.helpText()) << std::endl;
        a.reset();
        return 0;
    }

    bool quitAfterShowVersion = parser.isSet("version");
    bool silent = parser.isSet("no-init-messages");

    if (!silent || quitAfterShowVersion)
    {
        std::cout << "〔" << a->applicationName().toUtf8().constData() << "〕─"
                  << "〔" << a->applicationVersion().toUtf8().constData() << "〕\n"
                  << std::endl;

        if (quitAfterShowVersion)
        {
            a.reset();
            return 0;
        }
    }

    // Determine instance type
    if (parser.isSet("instance"))
    {
        const QString &inst = parser.value("instance");

        if (QString::compare(inst, "master", Qt::CaseInsensitive) == 0)
        {
            instance = ItMaster;
        }

        else if (QString::compare(inst, "server", Qt::CaseInsensitive) == 0)
        {
            instance = ItServer;
            configDir = new ConfigDirectory(false);
        }

        else if (QString::compare(inst, "bot", Qt::CaseInsensitive) == 0)
        {
            instance = ItBot;
            configDir = new ConfigDirectory(false);
        }

        else
        {
            std::cerr << "---App: unkown instance type '" << qUtf8Printable(inst) << "'" << std::endl;
            terminate(TerminateReason::UnknownInstanceType);
        }
    }

    std::cout << "---App: instance [" << instanceName(instance) << "] selected." << std::endl;

    // Check if the D-Bus daemon is running
    if (!QDBusConnection::sessionBus().isConnected()) {
        std::cerr << "---" << instanceName(instance) << ": unable to connect to the message bus!" << '\n'
                  << "---" << instanceName(instance) << ": make sure that the D-Bus daemon is running." << std::endl;
        std::cerr << "---" << instanceName(instance) << qUtf8Printable(QDBusConnection::sessionBus().lastError().message()) << std::endl;

        terminate(TerminateReason::DBusConnectionError);
    }

    // Register signal handlers
    std::cout << "---" << instanceName(instance) << ": registering signals SIGINT, SIGTERM, SIGQUIT, SIGSEGV and SIGILL..." << std::endl;
    std::signal(SIGINT, signal_terminate);
    std::signal(SIGTERM, signal_terminate);
    std::signal(SIGQUIT, signal_terminate);
    std::signal(SIGSEGV, signal_sigsegv);
    std::signal(SIGILL, signal_terminate);

    // Initialize debugger
    std::cout << "---" << instanceName(instance) << ": Initializing Debugger..." << std::endl;

    debugger = new Debugger();
    debugger->setFilenamePrefix(instanceName(instance));

    if (instance == ItMaster)
    {
        // Initialize configuration
        if (!silent)
        {
            std::cout << "---App: Initializing ConfigManager..." << std::endl;
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

        if (!hasToken || configManager->token().size() < 30) // TODO/RESEARCH: is the token always 59 chars long, can it be any shorter or longer?
        {
            debugger->error("No OAuth token found!");
            std::cerr << "---App: No OAuth token found!" << "\n\n"
                      << " Please paste your bot's token in the following file, create it if it doesn't exists." << '\n'
                      << " --> " << qUtf8Printable(configManager->configPath()) << "/token" << '\n'
                      << " Recommended: Make sure the file doesn't end with a new line and is UTF-8 encoded."
                      << std::endl;

            terminate(TerminateReason::OAuthTokenNotFound);
        }

        std::cout << "---App: starting master process..." << std::endl;

        // Register D-Bus service
        debugger->notice("Registering D-Bus service...");
        if (!QDBusConnection::sessionBus().registerService(DBusServiceName(ItMaster)))
        {
            debugger->error(QDBusConnection::sessionBus().lastError().message());
            terminate(TerminateReason::DBusServiceRegistrationFailed);
        }

        // Shared command line arguments across all instances
        const QStringList sharedArgs({
            "--no-init-messages",
            configManager->debuggerPrintToTerminal() ? "--terminal-logging" : "",
            "--log-max=" + QString::number(configManager->maxLogFilesToKeep())
        });

        ///
        /// Server
        ///

        debugger->notice("IPC: Initializing server instance...");
        createIpcProcess(ItServer, configManager, a.get());

        ipc(ItServer)->setArguments(QStringList({"--instance=server",
            "--listen=" + configManager->serverListeningAddress(),
            "--port=" + QString::number(configManager->serverListeningPort())
        }) << sharedArgs);

        if (!parser.isSet("skip-server"))
        {
            ipc(ItServer)->start(IpcProcess::ReadOnly);
            debugger->notice("IPC: Server instance started.");
        }

        else
        {
            debugger->notice("IPC: Server instance startup skipped. Use D-Bus to start it at some later point.");
        }

        ///
        /// Bot
        ///

        debugger->notice("IPC: Initializing bot instance...");
        createIpcProcess(ItBot, configManager, a.get());

        ipc(ItBot)->setArguments(QStringList({"--instance=bot",
            "--token=" + configManager->token()
        }) << sharedArgs);

        if (!parser.isSet("skip-bot"))
        {
            //ipc(ItBot)->start(IpcProcess::ReadOnly);
            //debugger->notice("IPC: Bot instance started.");
        }

        else
        {
            debugger->notice("IPC: Bot instance startup skipped. Use D-Bus to start it at some later point.");
        }

        ///
        /// D-Bus
        ///

        // Register D-Bus interface
        debugger->notice("Registering D-Bus interface...");
        DBusInterface *dbus_iface = new DBusInterface();
        dbus_iface->setServerProcess(ipc(ItServer));
        dbus_iface->setBotProcess(ipc(ItBot));

        if (!QDBusConnection::sessionBus().registerObject("/", DBusServiceName(ItMaster), dbus_iface, DBusFlags))
        {
            debugger->error(QDBusConnection::sessionBus().lastError().message());
            delete dbus_iface;
            terminate(TerminateReason::DBusObjectRegistrationFailed);
        }
    }

    else if (instance == ItServer)
    {
        std::cout << "---App: starting master process..." << std::endl;

        // Validate command line arguments
        std::cout << "---" << instanceName(instance) << ": validating command line arguments..." << std::endl;

        if (argc < 4)
        {
            std::cerr << instanceName(instance) << ": Too few command line arguments!" << std::endl;
            std::cerr << instanceName(instance) << ": Please don't run the instance directly. Use D-Bus instead." << std::endl;
            terminate(TerminateReason::UncategorizedError);
        }

        if (!parser.isSet("listen"))
        {
            std::cerr << instanceName(instance) << ": Listening address missing." << std::endl;
            terminate(TerminateReason::UncategorizedError);
        }

        if (!parser.isSet("port"))
        {
            std::cerr << instanceName(instance) << ": Listening port missing." << std::endl;
            terminate(TerminateReason::UncategorizedError);
        }

        debugger->setMaxLogFilesToKeep(parser.isSet("log-max") ? parser.value("log-max").toUShort() : 5);
        debugger->setLogDir(parser.isSet("log-dir") ? parser.value("log-dir") : configDir->configPath() + "/logs");
        debugger->printToTerminal(parser.isSet("terminal-logging"));
        debugger->setEnabled(true);

        // Check if the master process is running and connected to D-Bus
        std::cout << "---" << instanceName(instance) << ": validating D-Bus connection..." << std::endl;
        QDBusInterface iface(DBusServiceName(ItMaster), "/", "", QDBusConnection::sessionBus());
        if (!iface.isValid())
        {
            debugger->error("D-Bus: the master process interface couldn't be found!");
            debugger->error("D-Bus: refusing to start!");
            debugger->error("D-Bus: " + QDBusConnection::sessionBus().lastError().message());
            terminate(TerminateReason::DBusServiceConnectionError);
        }

        std::cout << "---" << instanceName(instance) << ": Initializing server..." << std::endl;
        server = new Server();
        ServerDBusAdapter *server_dbus = new ServerDBusAdapter(server);
        server->setListeningAddress(parser.value("listen"));
        server->setListeningPort(parser.value("port").toUShort());
        QObject::connect(server, &Server::stopped, server, &Server::deleteLater);
        QObject::connect(server, &Server::stopped, a.get(), &QCoreApplication::quit);

        std::cout << "---" << instanceName(instance) << ": Registering D-Bus service..." << std::endl;
        if (!QDBusConnection::sessionBus().registerService(DBusServiceName(ItServer)))
        {
            debugger->error(QDBusConnection::sessionBus().lastError().message());
            a.reset();
            delete server_dbus;
            delete server;
            terminate(TerminateReason::DBusServiceRegistrationFailed);
        }

        std::cout << "---" << instanceName(instance) << ": Registering D-Bus interface..." << std::endl;
        if (!QDBusConnection::sessionBus().registerObject("/", DBusServiceName(ItServer), server_dbus, DBusFlags))
        {
            debugger->error(QDBusConnection::sessionBus().lastError().message());
            a.reset();
            delete server_dbus;
            delete server;
            terminate(TerminateReason::DBusObjectRegistrationFailed);
        }

        std::cout << "---" << instanceName(instance) << ": Starting server..." << std::endl;
        QTimer::singleShot(0, server, &Server::start);
    }

    else if (instance == ItBot)
    {
        std::cout << "---App: starting bot process..." << std::endl;

        // Validate command line arguments
        std::cout << "---" << instanceName(instance) << ": validating command line arguments..." << std::endl;

        if (argc < 2)
        {
            std::cerr << instanceName(instance) << ": Too few command line arguments!" << std::endl;
            std::cerr << instanceName(instance) << ": Please don't run the instance directly. Use D-Bus instead." << std::endl;
            terminate(TerminateReason::UncategorizedError);
        }

        if (!parser.isSet("token"))
        {
            std::cerr << instanceName(instance) << ": OAuth token is missing." << std::endl;
            terminate(TerminateReason::UncategorizedError);
        }

        debugger->setMaxLogFilesToKeep(parser.isSet("log-max") ? parser.value("log-max").toUShort() : 5);
        debugger->setLogDir(parser.isSet("log-dir") ? parser.value("log-dir") : configDir->configPath() + "/logs");
        debugger->printToTerminal(parser.isSet("terminal-logging"));
        debugger->setEnabled(true);

        // Check if the master process is running and connected to D-Bus
        QDBusInterface iface(DBusServiceName(ItMaster), "/", "", QDBusConnection::sessionBus());
        if (!iface.isValid())
        {
            debugger->error("D-Bus: the master process interface couldn't be found!");
            debugger->error("D-Bus: refusing to start!");
            debugger->error("D-Bus: " + QDBusConnection::sessionBus().lastError().message());
            terminate(TerminateReason::DBusServiceConnectionError);
        }

        std::cout << "---" << instanceName(instance) << ": Initializing bot..." << std::endl;
        botManager = new BotManager();
        BotManagerDBusAdapter *botManager_dbus = new BotManagerDBusAdapter(botManager);
        botManager->setOAuthToken(a->arguments().at(2).mid(8));

        // FIXME! is this secure enough?
        // BUG: argv[2] hardcoded, `--token=` could be anywhere in the array
        //
        // remove OAuth token from argv (hides it from /proc/self/exe, ps ax, and other sources)
        std::size_t argvlen = strlen(argv[2]);
        for (std::size_t i = 8; i < argvlen; i++)
        {
            argv[2][i] = '*';
        }

        QObject::connect(botManager, &BotManager::stopped, botManager, &BotManager::deleteLater);
        QObject::connect(botManager, &BotManager::stopped, a.get(), &QCoreApplication::quit);

        std::cout << "---" << instanceName(instance) << ": Registering D-Bus service..." << std::endl;
        if (!QDBusConnection::sessionBus().registerService(DBusServiceName(ItBot)))
        {
            debugger->error(QDBusConnection::sessionBus().lastError().message());
            a.reset();
            delete botManager_dbus;
            delete botManager;
            terminate(TerminateReason::DBusServiceRegistrationFailed);
        }

        std::cout << "---" << instanceName(instance) << ": Registering D-Bus interface..." << std::endl;
        if (!QDBusConnection::sessionBus().registerObject("/", DBusServiceName(ItBot), botManager_dbus, DBusFlags))
        {
            debugger->error(QDBusConnection::sessionBus().lastError().message());
            a.reset();
            delete botManager_dbus;
            delete botManager;
            terminate(TerminateReason::DBusObjectRegistrationFailed);
        }

        std::cout << "---" << instanceName(instance) << ": Starting bot..." << std::endl;
        QTimer::singleShot(0, botManager, static_cast<void(BotManager::*)()>(&BotManager::login));
    }

    int status = a->exec();
    a.reset();

    delete configManager;
    delete configDir;
    delete debugger;
    delete threadId;
    threadId = nullptr;
    threadMain->setUserData(0, nullptr);

    std::cout << "---" << instanceName(instance) << ": process exited with code " << status << std::endl;
    return status;
}
