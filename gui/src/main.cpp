
#include <QAction>
#include <QApplication>
#include <QIcon>
#include <QMenu>
#include <QMetaObject>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include <QQuickWindow>
#include <QStyle>
#include <QSystemTrayIcon>
#include <QtQml/qqml.h>

#include "adapter/vcpilot_adapter.hpp"

int main(int argc, char* argv[]) {
    QQuickStyle::setStyle("Basic");

    QApplication app(argc, argv);

    app.setQuitOnLastWindowClosed(false);

    VCPilotAdapter vcpilotAdapter;

    qmlRegisterSingletonInstance("VCPilot", 1, 0, "VCPilotAdapter", &vcpilotAdapter);

    QQmlApplicationEngine engine;

    QObject::connect(
        &engine, &QQmlApplicationEngine::objectCreationFailed, &app,
        []() { QCoreApplication::exit(-1); }, Qt::QueuedConnection);

    engine.loadFromModule("VCPilot", "Main");

    auto* mainWindow = qobject_cast<QQuickWindow*>(engine.rootObjects().value(0));

    if (!mainWindow)
        return -1;

    // System tray
    QSystemTrayIcon trayIcon;

    trayIcon.setIcon(app.style()->standardIcon(QStyle::SP_ComputerIcon));

    QMenu trayMenu;

    auto* openAction = trayMenu.addAction("Open VCPilot");
    trayMenu.addSeparator();
    auto* exitAction = trayMenu.addAction("Exit");

    trayIcon.setContextMenu(&trayMenu);

    auto showMainWindow = [&]() {
        mainWindow->show();
        mainWindow->raise();
        mainWindow->requestActivate();
    };

    QObject::connect(openAction, &QAction::triggered, &app, showMainWindow);

    QObject::connect(&trayIcon, &QSystemTrayIcon::activated, &app,
                     [&](QSystemTrayIcon::ActivationReason reason) {
                         if (reason == QSystemTrayIcon::DoubleClick)
                             showMainWindow();
                     });

    QObject::connect(exitAction, &QAction::triggered, &app, [&]() {
        // Persist and destroy dynamically created widget windows.
        QMetaObject::invokeMethod(mainWindow, "shutdownApplication", Qt::DirectConnection);

        trayIcon.hide();
        app.quit();
    });

    trayIcon.show();

    return app.exec();
}
