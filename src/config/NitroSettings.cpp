#include "nitro/config/NitroSettings.hpp"

#include "nitro/diagnostics/NitroLogger.hpp"

#include <QFile>
#include <QJsonDocument>
#include <QSaveFile>

namespace nitro {

NitroSettings::NitroSettings(QObject* parent)
    : QObject(parent)
{
}

void NitroSettings::setTheme(const QString& theme)
{
    if (theme_ == theme) {
        return;
    }
    theme_ = theme;
    emit themeChanged();
}

void NitroSettings::setVideoWidth(int w)
{
    if (videoWidth_ == w || w <= 0) {
        return;
    }
    videoWidth_ = w;
    emit videoChanged();
}

void NitroSettings::setVideoHeight(int h)
{
    if (videoHeight_ == h || h <= 0) {
        return;
    }
    videoHeight_ = h;
    emit videoChanged();
}

void NitroSettings::setFps(int fps)
{
    if (fps_ == fps || fps <= 0) {
        return;
    }
    fps_ = fps;
    emit videoChanged();
}

void NitroSettings::setEncoder(const QString& encoder)
{
    if (encoder_ == encoder) {
        return;
    }
    encoder_ = encoder;
    emit videoChanged();
}

void NitroSettings::setVideoBitrateKbps(int kbps)
{
    if (videoBitrateKbps_ == kbps || kbps <= 0) {
        return;
    }
    videoBitrateKbps_ = kbps;
    emit videoChanged();
}

void NitroSettings::setAudioSampleRate(int rate)
{
    if (audioSampleRate_ == rate || rate <= 0) {
        return;
    }
    audioSampleRate_ = rate;
    emit audioChanged();
}

void NitroSettings::setAudioChannels(int channels)
{
    if (audioChannels_ == channels || channels <= 0) {
        return;
    }
    audioChannels_ = channels;
    emit audioChanged();
}

void NitroSettings::setMicrophoneDeviceId(const QString& id)
{
    if (microphoneDeviceId_ == id) {
        return;
    }
    microphoneDeviceId_ = id;
    emit audioChanged();
}

void NitroSettings::setDesktopDeviceId(const QString& id)
{
    if (desktopDeviceId_ == id) {
        return;
    }
    desktopDeviceId_ = id;
    emit audioChanged();
}

void NitroSettings::setCurrentProfile(const QString& profile)
{
    if (currentProfile_ == profile) {
        return;
    }
    currentProfile_ = profile;
    emit profileChanged();
}

QJsonObject NitroSettings::toJson() const
{
    QJsonObject root;
    root.insert(QStringLiteral("theme"), theme_);
    root.insert(QStringLiteral("currentProfile"), currentProfile_);

    QJsonObject video;
    video.insert(QStringLiteral("width"), videoWidth_);
    video.insert(QStringLiteral("height"), videoHeight_);
    video.insert(QStringLiteral("fps"), fps_);
    video.insert(QStringLiteral("encoder"), encoder_);
    video.insert(QStringLiteral("bitrateKbps"), videoBitrateKbps_);
    root.insert(QStringLiteral("video"), video);

    QJsonObject audio;
    audio.insert(QStringLiteral("sampleRate"), audioSampleRate_);
    audio.insert(QStringLiteral("channels"), audioChannels_);
    audio.insert(QStringLiteral("microphoneDeviceId"), microphoneDeviceId_);
    audio.insert(QStringLiteral("desktopDeviceId"), desktopDeviceId_);
    root.insert(QStringLiteral("audio"), audio);

    return root;
}

void NitroSettings::fromJson(const QJsonObject& obj)
{
    setTheme(obj.value(QStringLiteral("theme")).toString(theme_));
    setCurrentProfile(obj.value(QStringLiteral("currentProfile")).toString(currentProfile_));

    const QJsonObject video = obj.value(QStringLiteral("video")).toObject();
    setVideoWidth(video.value(QStringLiteral("width")).toInt(videoWidth_));
    setVideoHeight(video.value(QStringLiteral("height")).toInt(videoHeight_));
    setFps(video.value(QStringLiteral("fps")).toInt(fps_));
    setEncoder(video.value(QStringLiteral("encoder")).toString(encoder_));
    setVideoBitrateKbps(video.value(QStringLiteral("bitrateKbps")).toInt(videoBitrateKbps_));

    const QJsonObject audio = obj.value(QStringLiteral("audio")).toObject();
    setAudioSampleRate(audio.value(QStringLiteral("sampleRate")).toInt(audioSampleRate_));
    setAudioChannels(audio.value(QStringLiteral("channels")).toInt(audioChannels_));
    setMicrophoneDeviceId(audio.value(QStringLiteral("microphoneDeviceId")).toString(microphoneDeviceId_));
    setDesktopDeviceId(audio.value(QStringLiteral("desktopDeviceId")).toString(desktopDeviceId_));
}

bool NitroSettings::loadFromFile(const QString& path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        NITRO_LOG_WARN(QStringLiteral("Settings"), QStringLiteral("Failed to open %1").arg(path));
        return false;
    }
    const auto doc = QJsonDocument::fromJson(file.readAll());
    if (!doc.isObject()) {
        NITRO_LOG_ERROR(QStringLiteral("Settings"), QStringLiteral("Invalid settings JSON"));
        return false;
    }
    fromJson(doc.object());
    NITRO_LOG_INFO(QStringLiteral("Settings"), QStringLiteral("Loaded settings from %1").arg(path));
    return true;
}

bool NitroSettings::saveToFile(const QString& path) const
{
    QSaveFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
        NITRO_LOG_ERROR(QStringLiteral("Settings"), QStringLiteral("Failed to write %1").arg(path));
        return false;
    }
    file.write(QJsonDocument(toJson()).toJson(QJsonDocument::Indented));
    if (!file.commit()) {
        NITRO_LOG_ERROR(QStringLiteral("Settings"), QStringLiteral("Failed to commit %1").arg(path));
        return false;
    }
    NITRO_LOG_INFO(QStringLiteral("Settings"), QStringLiteral("Saved settings to %1").arg(path));
    return true;
}

} // namespace nitro
