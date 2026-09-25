#pragma once

#include "nitro/audio/AudioClock.hpp"
#include "nitro/audio/AudioDeviceManager.hpp"
#include "nitro/audio/AudioFormat.hpp"
#include "nitro/audio/AudioMeter.hpp"
#include "nitro/audio/IAudioOutput.hpp"
#include "nitro/audio/WasapiCapture.hpp"

#include <QObject>
#include <QString>
#include <QTimer>
#include <array>
#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

namespace nitro {

class AudioMixer;

enum class AudioDeviceConnectionState {
    Connected,
    Disconnected,
    Reconnecting
};

/**
 * Signal chain:
 *   Capture → RingBuffer → [Process Thread, data-driven]
 *     → Gain → Channel Meter (post-gain, pre-mute) → Mute → Sum
 *     → Master Gain → Master Meter (clip detect, no hard clip)
 *     → IAudioOutput sinks
 *
 * Scheduling: process thread waits on condition_variable woken by capture
 * data callbacks, with a short timeout for bounded latency / silence blocks.
 * Documented in docs/AUDIO_ENGINE.md.
 */
class AudioEngine : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool running READ isRunning NOTIFY runningChanged)
    Q_PROPERTY(double masterGainDb READ masterGainDb WRITE setMasterGainDb NOTIFY masterChanged)
    Q_PROPERTY(bool masterMuted READ masterMuted WRITE setMasterMuted NOTIFY masterChanged)
    Q_PROPERTY(double masterLevel READ masterLevel NOTIFY metersUpdated)
    Q_PROPERTY(double masterPeakDb READ masterPeakDb NOTIFY metersUpdated)
    Q_PROPERTY(bool masterClipped READ masterClipped NOTIFY metersUpdated)
    Q_PROPERTY(QString micDeviceId READ micDeviceId WRITE setMicDeviceId NOTIFY devicesBoundChanged)
    Q_PROPERTY(QString desktopDeviceId READ desktopDeviceId WRITE setDesktopDeviceId NOTIFY devicesBoundChanged)
    Q_PROPERTY(QString statusText READ statusText NOTIFY statusChanged)
    Q_PROPERTY(QString micConnectionState READ micConnectionState NOTIFY connectionStateChanged)
    Q_PROPERTY(QString desktopConnectionState READ desktopConnectionState NOTIFY connectionStateChanged)

public:
    explicit AudioEngine(AudioDeviceManager* devices, QObject* parent = nullptr);
    ~AudioEngine() override;

    Q_INVOKABLE bool start();
    Q_INVOKABLE void stop();
    bool isRunning() const { return running_.load(std::memory_order_acquire); }

    AudioDeviceManager* deviceManager() const { return devices_; }

    QString micDeviceId() const { return micDeviceId_; }
    void setMicDeviceId(const QString& id);

    QString desktopDeviceId() const { return desktopDeviceId_; }
    void setDesktopDeviceId(const QString& id);

    double masterGainDb() const { return masterGainDb_; }
    void setMasterGainDb(double db);

    bool masterMuted() const { return masterMuted_.load(std::memory_order_relaxed); }
    void setMasterMuted(bool muted);

    double masterLevel() const { return masterUiLevel_; }
    double masterPeakDb() const { return masterPeakDb_; }
    bool masterClipped() const { return masterClipped_; }

    QString statusText() const { return statusText_; }
    QString micConnectionState() const;
    QString desktopConnectionState() const;

    void attachMixer(AudioMixer* mixer);

    void setChannelGainDb(const QString& channelId, double db);
    void setChannelMuted(const QString& channelId, bool muted);
    void setChannelMonitoring(const QString& channelId, bool monitoring);

    MeterLevels channelMeter(const QString& channelId) const;
    float channelUiLevel(const QString& channelId) const;

    Q_INVOKABLE void refreshAndRestart();
    Q_INVOKABLE void clearMasterClip();

signals:
    void runningChanged();
    void metersUpdated();
    void masterChanged();
    void devicesBoundChanged();
    void statusChanged();
    void connectionStateChanged();
    void errorOccurred(const QString& message);
    void clipDetected();

private:
    struct ChannelState {
        QString id;
        QString name;
        AudioSourceKind kind = AudioSourceKind::None;
        std::unique_ptr<WasapiCapture> capture;
        std::atomic<float> gainLinear{1.0f};
        std::atomic<bool> muted{false};
        std::atomic<bool> monitoring{false};
        AudioMeter meter;
        std::vector<float> scratch;
        AudioDeviceConnectionState connection = AudioDeviceConnectionState::Disconnected;
    };

    void processLoop();
    void notifyDataReady();
    void publishMeters();
    void setStatus(const QString& text);
    ChannelState* findChannel(const QString& id);
    const ChannelState* findChannel(const QString& id) const;
    bool startCaptures();
    void stopCaptures();
    void onDevicesChanged();

    AudioDeviceManager* devices_ = nullptr;
    AudioMixer* mixer_ = nullptr;
    AudioClock clock_;
    AudioFormat format_ = engineAudioFormat();

    std::array<ChannelState, 5> channels_{};
    AudioMeter masterMeter_;
    std::unique_ptr<IAudioOutput> monitorOut_;

    QString micDeviceId_;
    QString desktopDeviceId_;
    double masterGainDb_ = 0.0;
    std::atomic<float> masterGainLinear_{1.0f};
    std::atomic<bool> masterMuted_{false};

    std::atomic<bool> running_{false};
    std::atomic<bool> stopProcess_{false};
    std::thread processThread_;
    std::mutex wakeMutex_;
    std::condition_variable wakeCv_;
    std::atomic<bool> dataPending_{false};

    QTimer uiMeterTimer_;
    float masterUiLevel_ = 0.0f;
    float masterPeakDb_ = -120.0f;
    bool masterClipped_ = false;
    QString statusText_ = QStringLiteral("Audio stopped");

    mutable std::mutex channelMutex_;
};

} // namespace nitro
