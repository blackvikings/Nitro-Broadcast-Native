#include "nitro/sources/SourceConfigs.hpp"

namespace nitro {

namespace {
int clampInt(int v, int lo, int hi) { return v < lo ? lo : (v > hi ? hi : v); }
}

QVariantMap DisplayCaptureConfig::toMap() const
{
    return {
        {QStringLiteral("monitorIndex"), monitorIndex},
        {QStringLiteral("captureCursor"), captureCursor},
        {QStringLiteral("fps"), fps},
        {QStringLiteral("api"), api}
    };
}

DisplayCaptureConfig DisplayCaptureConfig::fromMap(const QVariantMap& m)
{
    DisplayCaptureConfig c;
    c.monitorIndex = m.value(QStringLiteral("monitorIndex"), 0).toInt();
    c.captureCursor = m.value(QStringLiteral("captureCursor"), true).toBool();
    c.fps = m.value(QStringLiteral("fps"), 60).toInt();
    c.api = m.value(QStringLiteral("api"), QStringLiteral("WGC")).toString();
    return c;
}

bool DisplayCaptureConfig::validate(QString* error) const
{
    if (monitorIndex < 0) {
        if (error) *error = QStringLiteral("monitorIndex must be >= 0");
        return false;
    }
    if (fps < 1 || fps > 240) {
        if (error) *error = QStringLiteral("fps must be 1..240");
        return false;
    }
    return true;
}

QVariantMap WindowCaptureConfig::toMap() const
{
    return {
        {QStringLiteral("windowTitle"), windowTitle},
        {QStringLiteral("hwnd"), hwnd},
        {QStringLiteral("processId"), processId},
        {QStringLiteral("processName"), processName},
        {QStringLiteral("api"), api}
    };
}

WindowCaptureConfig WindowCaptureConfig::fromMap(const QVariantMap& m)
{
    WindowCaptureConfig c;
    c.windowTitle = m.value(QStringLiteral("windowTitle")).toString();
    c.hwnd = m.value(QStringLiteral("hwnd")).toString();
    c.processId = m.value(QStringLiteral("processId")).toInt();
    c.processName = m.value(QStringLiteral("processName")).toString();
    c.api = m.value(QStringLiteral("api"), QStringLiteral("WGC")).toString();
    return c;
}

bool WindowCaptureConfig::validate(QString* error) const
{
    (void)error;
    return true; // empty title allowed until picker selects
}

QVariantMap CameraConfig::toMap() const
{
    return {
        {QStringLiteral("deviceId"), deviceId},
        {QStringLiteral("resolution"), resolution},
        {QStringLiteral("fps"), fps},
        {QStringLiteral("format"), format}
    };
}

CameraConfig CameraConfig::fromMap(const QVariantMap& m)
{
    CameraConfig c;
    c.deviceId = m.value(QStringLiteral("deviceId")).toString();
    c.resolution = m.value(QStringLiteral("resolution"), QStringLiteral("1280x720")).toString();
    c.fps = m.value(QStringLiteral("fps"), 30).toInt();
    c.format = m.value(QStringLiteral("format"), QStringLiteral("auto")).toString();
    return c;
}

bool CameraConfig::validate(QString* error) const
{
    if (fps < 1 || fps > 240) {
        if (error) *error = QStringLiteral("camera fps must be 1..240");
        return false;
    }
    return true;
}

QVariantMap ImageConfig::toMap() const
{
    return {{QStringLiteral("filePath"), filePath}};
}

ImageConfig ImageConfig::fromMap(const QVariantMap& m)
{
    ImageConfig c;
    c.filePath = m.value(QStringLiteral("filePath")).toString();
    return c;
}

bool ImageConfig::validate(QString* error) const
{
    (void)error;
    return true;
}

QVariantMap TextConfig::toMap() const
{
    return {
        {QStringLiteral("content"), content},
        {QStringLiteral("fontFamily"), fontFamily},
        {QStringLiteral("fontSize"), fontSize},
        {QStringLiteral("fontWeight"), fontWeight},
        {QStringLiteral("color"), color},
        {QStringLiteral("alignment"), alignment},
        {QStringLiteral("outline"), outline},
        {QStringLiteral("shadow"), shadow}
    };
}

TextConfig TextConfig::fromMap(const QVariantMap& m)
{
    TextConfig c;
    c.content = m.value(QStringLiteral("content"), QStringLiteral("Nitro Broadcast")).toString();
    c.fontFamily = m.value(QStringLiteral("fontFamily"), QStringLiteral("Segoe UI")).toString();
    c.fontSize = clampInt(m.value(QStringLiteral("fontSize"), 48).toInt(), 1, 512);
    c.fontWeight = m.value(QStringLiteral("fontWeight"), QStringLiteral("Bold")).toString();
    c.color = m.value(QStringLiteral("color"), QStringLiteral("#E8EAED")).toString();
    c.alignment = m.value(QStringLiteral("alignment"), QStringLiteral("left")).toString();
    c.outline = m.value(QStringLiteral("outline")).toBool();
    c.shadow = m.value(QStringLiteral("shadow")).toBool();
    return c;
}

bool TextConfig::validate(QString* error) const
{
    if (fontSize < 1) {
        if (error) *error = QStringLiteral("fontSize must be >= 1");
        return false;
    }
    return true;
}

QVariantMap AudioSourceConfig::toMap() const
{
    return {
        {QStringLiteral("deviceId"), deviceId},
        {QStringLiteral("gainDb"), gainDb},
        {QStringLiteral("mute"), mute},
        {QStringLiteral("monitor"), monitor},
        {QStringLiteral("mixerChannel"), mixerChannel}
    };
}

AudioSourceConfig AudioSourceConfig::fromMap(const QVariantMap& m)
{
    AudioSourceConfig c;
    c.deviceId = m.value(QStringLiteral("deviceId")).toString();
    c.gainDb = m.value(QStringLiteral("gainDb")).toDouble();
    c.mute = m.value(QStringLiteral("mute")).toBool();
    c.monitor = m.value(QStringLiteral("monitor")).toBool();
    c.mixerChannel = m.value(QStringLiteral("mixerChannel")).toString();
    return c;
}

bool AudioSourceConfig::validate(QString* error) const
{
    if (gainDb < -60.0 || gainDb > 12.0) {
        if (error) *error = QStringLiteral("gainDb must be -60..+12");
        return false;
    }
    return true;
}

QVariantMap BrowserSourceConfig::toMap() const
{
    return {
        {QStringLiteral("url"), url},
        {QStringLiteral("width"), width},
        {QStringLiteral("height"), height},
        {QStringLiteral("fps"), fps},
        {QStringLiteral("sandbox"), sandbox},
        {QStringLiteral("allowFileAccess"), allowFileAccess},
        {QStringLiteral("allowNativeBridge"), allowNativeBridge}
    };
}

BrowserSourceConfig BrowserSourceConfig::fromMap(const QVariantMap& m)
{
    BrowserSourceConfig c;
    c.url = m.value(QStringLiteral("url"), QStringLiteral("https://")).toString();
    c.width = m.value(QStringLiteral("width"), 1920).toInt();
    c.height = m.value(QStringLiteral("height"), 1080).toInt();
    c.fps = m.value(QStringLiteral("fps"), 30).toInt();
    c.sandbox = m.value(QStringLiteral("sandbox"), true).toBool();
    c.allowFileAccess = m.value(QStringLiteral("allowFileAccess"), false).toBool();
    c.allowNativeBridge = m.value(QStringLiteral("allowNativeBridge"), false).toBool();
    return c;
}

bool BrowserSourceConfig::validate(QString* error) const
{
    if (allowNativeBridge) {
        if (error) *error = QStringLiteral("native bridge is forbidden");
        return false;
    }
    if (!sandbox && allowFileAccess) {
        if (error) *error = QStringLiteral("file access requires sandbox");
        return false;
    }
    return true;
}

} // namespace nitro
