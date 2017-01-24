#include <QCoreApplication>

#include "httplistener.h"
#include "templatecache.h"
#include "httpsessionstore.h"
#include "staticfilecontroller.h"
#include "filelogger.h"
//#include "requestmapper.h"

#include <Source/global.hpp>

using namespace stefanfrings;

TemplateCache *templateCache = nullptr;
HttpSessionStore *sessionStore = nullptr;
StaticFileController *staticFileController = nullptr;
FileLogger *logger = nullptr;

int main(int argc, char** argv)
{
    QCoreApplication a(argc, argv);
    a.setApplicationName(QString::fromUtf8("御坂ーお姉さま"));
    a.setApplicationVersion(QLatin1String("v0.0.1"));
    a.setOrganizationName(QString::fromUtf8("マギルゥーベルベット"));
    a.setOrganizationDomain(QLatin1String("magiruuvelvet.gdn"));

    // Initialize configuration
    configManager = new ConfigManager();

    // Initialize debugger
    debugger = new Debugger();
    debugger->setMaxLogFilesToKeep(5); // todo: make use configurable
    debugger->setLogDir(configManager->configPath() + "/logs");
    debugger->setEnabled(true);

    //int status = a.exec();

    delete configManager;
    delete debugger;

    return 0; //status;
}
