#pragma once

#include <QJsonObject>
#include <QString>
#include <QVariantMap>
#include <QVariantList>

namespace nitro {

struct DisplayCaptureConfig {
    int monitorIndex = 0;
    bool captureCursor = true;
    int fps = 60;
    QString api = QStringLiteral("WGC");

    QVariantMap toMap() const;
    static DisplayCaptureConfig fromMap(const QVariantMap& m);
    bool validate(QString* error = nullptr) const;
};

struct WindowCaptureConfig {
    QString windowTitle;
    QString hwnd;
    int processId = 0;
    QString processName;
    QString api = QStringLiteral("WGC");

    QVariantMap toMap() const;
    static WindowCaptureConfig fromMap(const QVariantMap& m);
    bool validate(QString* error = nullptr) const;
};

struct CameraConfig {
    QString deviceId;
    QString resolution = QStringLiteral("1280x720");
    int fps = 30;
    QString format = QStringLiteral("auto");

    QVariantMap toMap() const;
    static CameraConfig fromMap(const QVariantMap& m);
    bool validate(QString* error = nullptr) const;
};

struct ImageConfig {
    QString filePath;

    QVariantMap toMap() const;
    static ImageConfig fromMap(const QVariantMap& m);
    bool validate(QString* error = nullptr) const;
};

struct TextConfig {
    QString content = QStringLiteral("Nitro Broadcast");
    QString fontFamily = QStringLiteral("Segoe UI");
    int fontSize = 48;
    QString fontWeight = QStringLiteral("Bold");
    QString color = QStringLiteral("#E8EAED");
    QString alignment = QStringLiteral("left");
    bool outline = false;
    bool shadow = false;

    QVariantMap toMap() const;
    static TextConfig fromMap(const QVariantMap& m);
    bool validate(QString* error = nullptr) const;
};

struct AudioSourceConfig {
    QString deviceId;
    double gainDb = 0.0;
    bool mute = false;
    bool monitor = false;
    QString mixerChannel;

    QVariantMap toMap() const;
    static AudioSourceConfig fromMap(const QVariantMap& m);
    bool validate(QString* error = nullptr) const;
};

struct BrowserSourceConfig {
    QString url = QStringLiteral("https://");
    int width = 1920;
    int height = 1080;
    int fps = 30;
    bool sandbox = true;
    bool allowFileAccess = false;
    bool allowNativeBridge = false;

    QVariantMap toMap() const;
    static BrowserSourceConfig fromMap(const QVariantMap& m);
    bool validate(QString* error = nullptr) const;
};

} // namespace nitro
