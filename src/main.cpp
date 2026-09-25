#include "nitro/app/NitroApplication.hpp"
#include "nitro/diagnostics/NitroLogger.hpp"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlError>
#include <QQuickStyle>
#include <QUrl>
#include <cstdio>

int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);
    QGuiApplication::setOrganizationName(QStringLiteral("Nitro"));
    QGuiApplication::setApplicationName(QStringLiteral("Nitro Broadcast"));
    QGuiApplication::setApplicationVersion(QStringLiteral("0.1.0"));

    QQuickStyle::setStyle(QStringLiteral("Basic"));

    nitro::NitroApplication nitroApp;
    if (!nitroApp.initialize()) {
        return 1;
    }

    QQmlApplicationEngine engine;
    nitroApp.exposeToQml(engine);

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::warnings,
        &app,
        [](const QList<QQmlError>& warnings) {
            for (const auto& w : warnings) {
                fprintf(stderr, "QML: %s\n", qPrintable(w.toString()));
                NITRO_LOG_WARN(QStringLiteral("UI"), w.toString());
            }
        });

    const QUrl url(QStringLiteral("qrc:/qt/qml/NitroBroadcast/Main.qml"));
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() {
            fprintf(stderr, "Failed to create QML root object\n");
            NITRO_LOG_ERROR(QStringLiteral("UI"), QStringLiteral("Failed to create QML root object"));
            QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);

    engine.load(url);
    if (engine.rootObjects().isEmpty()) {
        fprintf(stderr, "No QML root objects for %s\n", qPrintable(url.toString()));
        NITRO_LOG_ERROR(QStringLiteral("UI"), QStringLiteral("No QML root objects"));
        return -1;
    }

    NITRO_LOG_INFO(QStringLiteral("UI"), QStringLiteral("Main window loaded"));
    const int code = app.exec();
    nitroApp.shutdown();
    return code;
}
