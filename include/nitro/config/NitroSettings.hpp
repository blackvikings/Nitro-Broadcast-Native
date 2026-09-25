#pragma once

#include <QJsonObject>
#include <QObject>
#include <QString>

namespace nitro {

class NitroSettings : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString theme READ theme WRITE setTheme NOTIFY themeChanged)
    Q_PROPERTY(int videoWidth READ videoWidth WRITE setVideoWidth NOTIFY videoChanged)
    Q_PROPERTY(int videoHeight READ videoHeight WRITE setVideoHeight NOTIFY videoChanged)
    Q_PROPERTY(int fps READ fps WRITE setFps NOTIFY videoChanged)
    Q_PROPERTY(QString encoder READ encoder WRITE setEncoder NOTIFY videoChanged)
    Q_PROPERTY(int videoBitrateKbps READ videoBitrateKbps WRITE setVideoBitrateKbps NOTIFY videoChanged)
    Q_PROPERTY(int audioSampleRate READ audioSampleRate WRITE setAudioSampleRate NOTIFY audioChanged)
    Q_PROPERTY(int audioChannels READ audioChannels WRITE setAudioChannels NOTIFY audioChanged)
    Q_PROPERTY(QString microphoneDeviceId READ microphoneDeviceId WRITE setMicrophoneDeviceId NOTIFY audioChanged)
    Q_PROPERTY(QString desktopDeviceId READ desktopDeviceId WRITE setDesktopDeviceId NOTIFY audioChanged)
    Q_PROPERTY(QString currentProfile READ currentProfile WRITE setCurrentProfile NOTIFY profileChanged)

public:
    explicit NitroSettings(QObject* parent = nullptr);

    QString theme() const { return theme_; }
    void setTheme(const QString& theme);

    int videoWidth() const { return videoWidth_; }
    void setVideoWidth(int w);

    int videoHeight() const { return videoHeight_; }
    void setVideoHeight(int h);

    int fps() const { return fps_; }
    void setFps(int fps);

    QString encoder() const { return encoder_; }
    void setEncoder(const QString& encoder);

    int videoBitrateKbps() const { return videoBitrateKbps_; }
    void setVideoBitrateKbps(int kbps);

    int audioSampleRate() const { return audioSampleRate_; }
    void setAudioSampleRate(int rate);

    int audioChannels() const { return audioChannels_; }
    void setAudioChannels(int channels);

    QString microphoneDeviceId() const { return microphoneDeviceId_; }
    void setMicrophoneDeviceId(const QString& id);

    QString desktopDeviceId() const { return desktopDeviceId_; }
    void setDesktopDeviceId(const QString& id);

    QString currentProfile() const { return currentProfile_; }
    void setCurrentProfile(const QString& profile);

    Q_INVOKABLE bool loadFromFile(const QString& path);
    Q_INVOKABLE bool saveToFile(const QString& path) const;

    QJsonObject toJson() const;
    void fromJson(const QJsonObject& obj);

signals:
    void themeChanged();
    void videoChanged();
    void audioChanged();
    void profileChanged();

private:
    QString theme_ = QStringLiteral("dark");
    int videoWidth_ = 1920;
    int videoHeight_ = 1080;
    int fps_ = 60;
    QString encoder_ = QStringLiteral("auto");
    int videoBitrateKbps_ = 6000;
    int audioSampleRate_ = 48000;
    int audioChannels_ = 2;
    QString microphoneDeviceId_;
    QString desktopDeviceId_;
    QString currentProfile_ = QStringLiteral("Gaming 1080p60");
};

} // namespace nitro
