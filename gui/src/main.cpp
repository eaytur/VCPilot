#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QtQml/qqml.h>

#include "adapter/vcpilot_adapter.hpp"

int main(int argc, char* argv[]) {
    QQuickStyle::setStyle("Basic");

    QGuiApplication app(argc, argv);

    VCPilotAdapter vcpilotAdapter;

    qmlRegisterSingletonInstance("VCPilot", 1, 0, "VCPilotAdapter", &vcpilotAdapter);

    QQmlApplicationEngine engine;

    QObject::connect(
        &engine, &QQmlApplicationEngine::objectCreationFailed, &app,
        []() { QCoreApplication::exit(-1); }, Qt::QueuedConnection);

    engine.loadFromModule("VCPilot", "Main");

    return app.exec();
}