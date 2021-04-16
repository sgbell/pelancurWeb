#include <cstdlib>


#include <QApplication>
#include <QCommandLineParser>

#include <KAboutData>
#include <KLocalizedString>

#include "mainwindow.h"
#include "WebServer.h"
#include "RootController.h"

int main (int argc, char *argv[]){
    QApplication app(argc, argv);
    KLocalizedString::setApplicationDomain("pelancurWeb");

    KAboutData aboutData(
        // The program name used internally.
        QStringLiteral("pelancurWeb"),
        // A displayable program name string
        i18n("pelancurWeb"),
        // The program version string
        QStringLiteral("1.0"),
        // Short description of what the app does
        i18n("This allows you to start and stop applications running in a desktop via a web interface"),
        // The License this code is released under
        KAboutLicense::GPL,
        // Copyright Statement
        i18n("(c) 2021"),
        // The program homepage
        QStringLiteral("https://github.com/sgbel2/pelancurWeb"),
        // The bug report email address
        QStringLiteral("info@mimpidev.com")
    );

    aboutData.addAuthor(i18n("Sam Bell"),
                        i18n("Developer"),
                        QStringLiteral("sam@mimpidev.com"),
                        QStringLiteral("http://mimpidev.com"),
                        QStringLiteral(""));

    KAboutData::setApplicationData(aboutData);

    QCommandLineParser parser;
    aboutData.setupCommandLine(&parser);
    parser.process(app);
    aboutData.processCommandLine(&parser);

    MainWindow *window = new MainWindow();
    window->show();

    QFile *etcConf = new QFile("/etc/pelancurweb.conf");
    QSettings *settings = nullptr;
    if (etcConf->exists()){
        settings = new QSettings("/etc/pelancurweb.conf", QSettings::NativeFormat);
    } else {
        settings = new QSettings(std::string(std::getenv("HOME")).append("/.config/pelancurweb.conf").c_str(), QSettings::NativeFormat);
    }

    settings->beginGroup("serverPort");
    int serverPort = settings->value("port", 11080).toInt();
    settings->endGroup();
    window->setSettings(settings);
    WebServer *server= new WebServer(serverPort);
    Controller *myPage = new RootController();
    server->addController(myPage);
    myPage = new ExecController();
    server->addController(myPage);
    myPage = new AssetsController();
    server->addController(myPage);
    myPage = new ActivityController();
    server->addController(myPage);
    server->start();
    window->setServer(server);

    WebServer::connect(&app, &QApplication::aboutToQuit, server, [server](){server->stopServer();});

    return app.exec();
}