#include <QCoreApplication>
#include <QDebug>
#include <iostream>

#include <Source/global.hpp>

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

    //int status = a.exec();

    delete configManager;
    delete debugger;

    return 0; //status;
}
