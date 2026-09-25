#include "nitro/audio/AudioEngine.hpp"

#include "nitro/audio/AudioMixer.hpp"
#include "nitro/diagnostics/NitroLogger.hpp"

#include <QtMath>
#include <chrono>

namespace nitro {

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
    }

    connect(&uiMeterTimer_, &QTimer::timeout, this, &AudioEngine::publishMeters);
    uiMeterTimer_.setInterval(33); // ~30 FPS UI updates
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

bool AudioEngine::startCaptures()
{
    stopCaptures();

    auto* mic = findChannel(QStringLiteral("mic"));
    auto* desktop = findChannel(QStringLiteral("desktop"));

    if (mic) {
        mic->capture = std::make_unique<WasapiCapture>();
        if (!mic->capture->start(micDeviceId_, WasapiCaptureMode::Microphone, &clock_)) {
            emit errorOccurred(QStringLiteral("Failed to start microphone capture"));
        }
        if (devices_ && mixer_) {
            if (auto* ui = mixer_->channelAt(0)) {
                ui->setSourceName(devices_->deviceNameForId(micDeviceId_));
            }
        }
    }

    if (desktop) {
        desktop->capture = std::make_unique<WasapiCapture>();
        if (!desktop->capture->start(desktopDeviceId_, WasapiCaptureMode::Loopback, &clock_)) {
            emit errorOccurred(QStringLiteral("Failed to start desktop loopback capture"));
        }
        if (devices_ && mixer_) {
            // desktop is index 2 in default layout
            if (auto* ui = mixer_->channelAt(2)) {
                ui->setSourceName(devices_->deviceNameForId(desktopDeviceId_)
                                  + QStringLiteral(" (Loopback)"));
            }
        }
    }

    // Unassigned channels keep "No source yet"
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
    if (processThread_.joinable()) {
        processThread_.join();
    }
    stopCaptures();
    monitorOut_->stop();
    running_.store(false, std::memory_order_release);
    setStatus(QStringLiteral("Audio stopped"));
    emit runningChanged();
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
    std::vector<float> temp(block * static_cast<std::size_t>(chCount), 0.0f);

    using clock = std::chrono::steady_clock;
    auto next = clock::now();

    while (!stopProcess_.load(std::memory_order_acquire)) {
        std::fill(mix.begin(), mix.end(), 0.0f);

        for (auto& channel : channels_) {
            if (channel.scratch.size() < block * static_cast<std::size_t>(chCount)) {
                channel.scratch.resize(block * static_cast<std::size_t>(chCount));
            }
            std::fill(channel.scratch.begin(),
                      channel.scratch.begin() + static_cast<std::ptrdiff_t>(block * chCount),
                      0.0f);

            std::size_t got = 0;
            if (channel.capture && channel.capture->isRunning()) {
                got = channel.capture->pull(channel.scratch.data(), block);
                // If underrun, remaining samples stay 0
                Q_UNUSED(got);
            }

            const float gain = channel.gainLinear.load(std::memory_order_relaxed);
            for (std::size_t i = 0; i < block * static_cast<std::size_t>(chCount); ++i) {
                channel.scratch[i] *= gain;
            }

            // Meter post-gain, pre-mute (professional: see signal while muted)
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
                // Soft clip protect
                s = std::clamp(s, -1.0f, 1.0f);
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

        next += std::chrono::microseconds((block * 1'000'000ULL) / static_cast<unsigned>(format_.sampleRate));
        std::this_thread::sleep_until(next);
        // If we fell behind, reset schedule
        if (clock::now() > next + std::chrono::milliseconds(50)) {
            next = clock::now();
        }
    }
}

void AudioEngine::publishMeters()
{
    const auto master = masterMeter_.snapshot();
    masterUiLevel_ = masterMeter_.uiLevel();
    masterPeakDb_ = master.peakDb;

    // Propagate device names / capture errors to UI channels
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
