#include "nitro/audio/AudioEngine.hpp"

#include "nitro/audio/AudioMixer.hpp"
#include "nitro/diagnostics/NitroLogger.hpp"

#include <QtMath>
#include <chrono>

namespace nitro {

namespace {
QString connectionStateName(AudioDeviceConnectionState s)
{
    switch (s) {
    case AudioDeviceConnectionState::Connected: return QStringLiteral("Connected");
    case AudioDeviceConnectionState::Disconnected: return QStringLiteral("Disconnected");
    case AudioDeviceConnectionState::Reconnecting: return QStringLiteral("Reconnecting");
    }
    return QStringLiteral("Unknown");
}
} // namespace

AudioEngine::AudioEngine(AudioDeviceManager* devices, QObject* parent)
    : QObject(parent)
    , devices_(devices)
    , monitorOut_(std::make_unique<NullAudioOutput>())
{
    const struct {
        const char* id;
        const char* name;
        AudioSourceKind kind;
    } defs[] = {
        {"mic", "MIC", AudioSourceKind::Microphone},
        {"game", "GAME", AudioSourceKind::Application},
        {"desktop", "DESKTOP", AudioSourceKind::DesktopLoopback},
        {"music", "MUSIC", AudioSourceKind::MediaFile},
        {"browser", "BROWSER", AudioSourceKind::Browser},
    };

    for (std::size_t i = 0; i < channels_.size(); ++i) {
        channels_[i].id = QString::fromUtf8(defs[i].id);
        channels_[i].name = QString::fromUtf8(defs[i].name);
        channels_[i].kind = defs[i].kind;
        channels_[i].meter.setSampleRate(format_.sampleRate);
        channels_[i].gainLinear.store(1.0f, std::memory_order_relaxed);
        channels_[i].muted.store(false, std::memory_order_relaxed);
        channels_[i].monitoring.store(false, std::memory_order_relaxed);
        channels_[i].scratch.resize(static_cast<std::size_t>(format_.framesPerBuffer) * format_.channels);
    }
    masterMeter_.setSampleRate(format_.sampleRate);

    if (devices_) {
        micDeviceId_ = devices_->defaultInputId();
        desktopDeviceId_ = devices_->defaultOutputId();
        connect(devices_, &AudioDeviceManager::devicesChanged, this, &AudioEngine::onDevicesChanged);
        connect(devices_, &AudioDeviceManager::defaultDeviceChanged, this, &AudioEngine::onDevicesChanged);
        connect(devices_, &AudioDeviceManager::deviceRemoved, this, [this](const QString& id) {
            if (id == micDeviceId_ || id == desktopDeviceId_) {
                if (auto* mic = findChannel(QStringLiteral("mic")); mic && id == micDeviceId_) {
                    mic->connection = AudioDeviceConnectionState::Disconnected;
                }
                if (auto* desk = findChannel(QStringLiteral("desktop")); desk && id == desktopDeviceId_) {
                    desk->connection = AudioDeviceConnectionState::Disconnected;
                }
                emit connectionStateChanged();
                emit errorOccurred(QStringLiteral("Audio device disconnected"));
                // Do not crash — stop captures for that device on next refresh
                if (isRunning()) {
                    refreshAndRestart();
                }
            }
        });
    }

    connect(&uiMeterTimer_, &QTimer::timeout, this, &AudioEngine::publishMeters);
    uiMeterTimer_.setInterval(33);
}

AudioEngine::~AudioEngine()
{
    stop();
}

void AudioEngine::attachMixer(AudioMixer* mixer)
{
    mixer_ = mixer;
    if (mixer_) {
        mixer_->setEngine(this);
    }
}

void AudioEngine::setStatus(const QString& text)
{
    if (statusText_ == text) {
        return;
    }
    statusText_ = text;
    emit statusChanged();
}

QString AudioEngine::micConnectionState() const
{
    if (const auto* ch = findChannel(QStringLiteral("mic"))) {
        return connectionStateName(ch->connection);
    }
    return QStringLiteral("Disconnected");
}

QString AudioEngine::desktopConnectionState() const
{
    if (const auto* ch = findChannel(QStringLiteral("desktop"))) {
        return connectionStateName(ch->connection);
    }
    return QStringLiteral("Disconnected");
}

AudioEngine::ChannelState* AudioEngine::findChannel(const QString& id)
{
    for (auto& ch : channels_) {
        if (ch.id == id) {
            return &ch;
        }
    }
    return nullptr;
}

const AudioEngine::ChannelState* AudioEngine::findChannel(const QString& id) const
{
    for (const auto& ch : channels_) {
        if (ch.id == id) {
            return &ch;
        }
    }
    return nullptr;
}

void AudioEngine::setMicDeviceId(const QString& id)
{
    if (micDeviceId_ == id) {
        return;
    }
    micDeviceId_ = id;
    emit devicesBoundChanged();
    if (isRunning()) {
        refreshAndRestart();
    }
}

void AudioEngine::setDesktopDeviceId(const QString& id)
{
    if (desktopDeviceId_ == id) {
        return;
    }
    desktopDeviceId_ = id;
    emit devicesBoundChanged();
    if (isRunning()) {
        refreshAndRestart();
    }
}

void AudioEngine::setMasterGainDb(double db)
{
    db = std::clamp(db, -60.0, 12.0);
    masterGainDb_ = db;
    masterGainLinear_.store(dbToLinear(static_cast<float>(db)), std::memory_order_relaxed);
    emit masterChanged();
}

void AudioEngine::setMasterMuted(bool muted)
{
    masterMuted_.store(muted, std::memory_order_relaxed);
    emit masterChanged();
}

void AudioEngine::clearMasterClip()
{
    masterMeter_.clearClip();
    masterClipped_ = false;
    emit metersUpdated();
}

void AudioEngine::setChannelGainDb(const QString& channelId, double db)
{
    if (auto* ch = findChannel(channelId)) {
        ch->gainLinear.store(dbToLinear(static_cast<float>(std::clamp(db, -60.0, 12.0))),
                             std::memory_order_relaxed);
    }
}

void AudioEngine::setChannelMuted(const QString& channelId, bool muted)
{
    if (auto* ch = findChannel(channelId)) {
        ch->muted.store(muted, std::memory_order_relaxed);
    }
}

void AudioEngine::setChannelMonitoring(const QString& channelId, bool monitoring)
{
    if (auto* ch = findChannel(channelId)) {
        ch->monitoring.store(monitoring, std::memory_order_relaxed);
    }
}

MeterLevels AudioEngine::channelMeter(const QString& channelId) const
{
    if (const auto* ch = findChannel(channelId)) {
        return ch->meter.snapshot();
    }
    return {};
}

float AudioEngine::channelUiLevel(const QString& channelId) const
{
    if (const auto* ch = findChannel(channelId)) {
        return ch->meter.uiLevel();
    }
    return 0.0f;
}

void AudioEngine::notifyDataReady()
{
    dataPending_.store(true, std::memory_order_release);
    wakeCv_.notify_one();
}

void AudioEngine::onDevicesChanged()
{
    emit connectionStateChanged();
}

bool AudioEngine::startCaptures()
{
    stopCaptures();

    auto* mic = findChannel(QStringLiteral("mic"));
    auto* desktop = findChannel(QStringLiteral("desktop"));
    auto wake = [this]() { notifyDataReady(); };

    if (mic) {
        mic->connection = AudioDeviceConnectionState::Reconnecting;
        mic->capture = std::make_unique<WasapiCapture>();
        mic->capture->setDataReadyCallback(wake);
        if (!mic->capture->start(micDeviceId_, WasapiCaptureMode::Microphone, &clock_)) {
            mic->connection = AudioDeviceConnectionState::Disconnected;
            emit errorOccurred(QStringLiteral("Failed to start microphone capture"));
        } else {
            mic->connection = AudioDeviceConnectionState::Connected;
        }
        if (devices_ && mixer_) {
            if (auto* ui = mixer_->channelAt(0)) {
                ui->setSourceName(devices_->deviceNameForId(micDeviceId_));
            }
        }
    }

    if (desktop) {
        desktop->connection = AudioDeviceConnectionState::Reconnecting;
        desktop->capture = std::make_unique<WasapiCapture>();
        desktop->capture->setDataReadyCallback(wake);
        if (!desktop->capture->start(desktopDeviceId_, WasapiCaptureMode::Loopback, &clock_)) {
            desktop->connection = AudioDeviceConnectionState::Disconnected;
            emit errorOccurred(QStringLiteral("Failed to start desktop loopback capture"));
        } else {
            desktop->connection = AudioDeviceConnectionState::Connected;
        }
        if (devices_ && mixer_) {
            if (auto* ui = mixer_->channelAt(2)) {
                ui->setSourceName(devices_->deviceNameForId(desktopDeviceId_)
                                  + QStringLiteral(" (Loopback)"));
            }
        }
    }

    emit connectionStateChanged();
    return true;
}

void AudioEngine::stopCaptures()
{
    for (auto& ch : channels_) {
        if (ch.capture) {
            ch.capture->stop();
            ch.capture.reset();
        }
        ch.meter.reset();
        ch.connection = AudioDeviceConnectionState::Disconnected;
    }
}

bool AudioEngine::start()
{
    if (isRunning()) {
        return true;
    }

    if (devices_) {
        devices_->refresh();
        if (micDeviceId_.isEmpty()) {
            micDeviceId_ = devices_->defaultInputId();
        }
        if (desktopDeviceId_.isEmpty()) {
            desktopDeviceId_ = devices_->defaultOutputId();
        }
    }

    startCaptures();
    monitorOut_->start();

    stopProcess_.store(false, std::memory_order_release);
    running_.store(true, std::memory_order_release);
    processThread_ = std::thread(&AudioEngine::processLoop, this);
    uiMeterTimer_.start();

    setStatus(QStringLiteral("Audio engine running (WASAPI)"));
    emit runningChanged();
    NITRO_LOG_INFO(QStringLiteral("Audio"), QStringLiteral("AudioEngine started"));
    return true;
}

void AudioEngine::stop()
{
    if (!isRunning() && !processThread_.joinable()) {
        stopCaptures();
        return;
    }

    uiMeterTimer_.stop();
    stopProcess_.store(true, std::memory_order_release);
    wakeCv_.notify_all();
    if (processThread_.joinable()) {
        processThread_.join();
    }
    stopCaptures();
    monitorOut_->stop();
    running_.store(false, std::memory_order_release);
    setStatus(QStringLiteral("Audio stopped"));
    emit runningChanged();
    emit connectionStateChanged();
    NITRO_LOG_INFO(QStringLiteral("Audio"), QStringLiteral("AudioEngine stopped"));
}

void AudioEngine::refreshAndRestart()
{
    const bool was = isRunning();
    stop();
    if (devices_) {
        devices_->refresh();
    }
    if (was) {
        start();
    }
}

void AudioEngine::processLoop()
{
    const std::size_t block = static_cast<std::size_t>(format_.framesPerBuffer);
    const int chCount = format_.channels;
    std::vector<float> mix(block * static_cast<std::size_t>(chCount), 0.0f);

    // Capture-driven with bounded latency: wake on data, or timeout to emit silence.
    constexpr auto kMaxWait = std::chrono::milliseconds(5);

    while (!stopProcess_.load(std::memory_order_acquire)) {
        {
            std::unique_lock lock(wakeMutex_);
            wakeCv_.wait_for(lock, kMaxWait, [this]() {
                return stopProcess_.load(std::memory_order_acquire)
                    || dataPending_.load(std::memory_order_acquire);
            });
            dataPending_.store(false, std::memory_order_release);
        }
        if (stopProcess_.load(std::memory_order_acquire)) {
            break;
        }

        std::fill(mix.begin(), mix.end(), 0.0f);

        for (auto& channel : channels_) {
            if (channel.scratch.size() < block * static_cast<std::size_t>(chCount)) {
                channel.scratch.resize(block * static_cast<std::size_t>(chCount));
            }
            std::fill(channel.scratch.begin(),
                      channel.scratch.begin() + static_cast<std::ptrdiff_t>(block * chCount),
                      0.0f);

            if (channel.capture && channel.capture->isRunning()) {
                channel.capture->pull(channel.scratch.data(), block);
            }

            const float gain = channel.gainLinear.load(std::memory_order_relaxed);
            for (std::size_t i = 0; i < block * static_cast<std::size_t>(chCount); ++i) {
                channel.scratch[i] *= gain;
            }

            channel.meter.process(channel.scratch.data(), block, chCount);

            if (channel.muted.load(std::memory_order_relaxed)) {
                continue;
            }

            for (std::size_t i = 0; i < block * static_cast<std::size_t>(chCount); ++i) {
                mix[i] += channel.scratch[i];
            }
        }

        const float masterGain = masterGainLinear_.load(std::memory_order_relaxed);
        const bool masterMute = masterMuted_.load(std::memory_order_relaxed);
        if (masterMute) {
            std::fill(mix.begin(), mix.end(), 0.0f);
        } else {
            for (float& s : mix) {
                s *= masterGain;
                // No hard clipping — preserve headroom; limiter boundary later.
                // Clip is detected by the meter (threshold 1.0).
            }
        }

        masterMeter_.process(mix.data(), block, chCount);

        AudioBuffer buf;
        buf.format = format_;
        buf.timestamp.captureNs = clock_.nowNs();
        buf.frameCount = block;
        buf.samples = mix;
        if (monitorOut_ && monitorOut_->isRunning()) {
            monitorOut_->push(buf);
        }
    }
}

void AudioEngine::publishMeters()
{
    const auto master = masterMeter_.snapshot();
    masterUiLevel_ = masterMeter_.uiLevel();
    masterPeakDb_ = master.peakDb;
    const bool wasClipped = masterClipped_;
    masterClipped_ = master.clipped;
    if (masterClipped_ && !wasClipped) {
        emit clipDetected();
    }

    if (mixer_ && devices_) {
        if (auto* micUi = mixer_->channelAt(0)) {
            QString name = devices_->deviceNameForId(micDeviceId_);
            if (auto* mic = findChannel(QStringLiteral("mic")); mic && mic->capture) {
                const QString err = mic->capture->lastError();
                if (!err.isEmpty() && !mic->capture->isRunning()) {
                    name = QStringLiteral("Error: check device");
                }
            }
            micUi->setSourceName(name);
        }
        if (auto* deskUi = mixer_->channelAt(2)) {
            deskUi->setSourceName(devices_->deviceNameForId(desktopDeviceId_)
                                  + QStringLiteral(" (Loopback)"));
        }
        mixer_->refreshFromEngine();
    }

    emit metersUpdated();
}

} // namespace nitro
