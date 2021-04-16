#include <QApplication>
#include <QAction>
#include <QtWidgets>
#include <QVBoxLayout>

#include <KTextEdit>
#include <KLocalizedString>
#include <KActionCollection>
#include <KStandardAction>

#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : KXmlGuiWindow(parent) {
//    QVBoxLayout *mainLayout = new QVBoxLayout();
    QLabel *portLabel = new QLabel(i18n("Server Port"));
    listenPort = new QLineEdit;
    QGridLayout *layout = new QGridLayout;
    layout->addWidget(portLabel,1,0);
    layout->addWidget(listenPort,1,1);
    QGroupBox *gridGroup = new QGroupBox;
    gridGroup->setLayout(layout);
    setCentralWidget(gridGroup);
//    mainLayout->addWidget(gridGroup);
//    setLayout(mainLayout);

    setupActions();
}

void MainWindow::setupActions() {

    KStandardAction::quit(qApp, &QCoreApplication::quit, actionCollection());

    setupGUI (Default, "pelancurwebui.rc");
}

bool MainWindow::queryClose(){
    server->stopServer();
    // Force exit, I know it's not good practice.. but it works for my purposes, and you're welcome to make it nicer.
    // Need to Record the new port in here
    settings->beginGroup("serverPort");
    settings->setValue("port", listenPort->text());
    settings->sync();
    exit(0);
    return true;
}