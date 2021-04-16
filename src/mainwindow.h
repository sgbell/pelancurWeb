#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <KXmlGuiWindow>

#include <QLineEdit>
#include <QSettings>

#include "WebServer.h"

class KTextEdit;

class MainWindow : public KXmlGuiWindow {
    public:
        explicit MainWindow (QWidget *parent = nullptr);
        void setServer(WebServer *_server) {
            server = _server;
            listenPort->setText(QString::number(server->getPort()));
        }
        void setSettings(QSettings *_settings) {
            settings = _settings;
        }
        bool queryClose();

    private:
        QLineEdit *listenPort;
        void setupActions();
        WebServer *server;
        QSettings *settings;
};

#endif // MAINWINDOW_H