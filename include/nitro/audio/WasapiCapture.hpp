#pragma once

#include "nitro/audio/AudioClock.hpp"
#include "nitro/audio/AudioMeter.hpp"
#include "nitro/audio/AudioResampler.hpp"
#include "nitro/audio/AudioRingBuffer.hpp"

#include <QString>
#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>

namespace nitro {

enum class WasapiCaptureMode {
    Microphone,
    Loopback
};

/// Dedicated-thread WASAPI capture → float32 stereo ring @ engine rate (with resampling).
class WasapiCapture {
public:
    using DataReadyFn = std::function<void()>;

    WasapiCapture();
    ~WasapiCapture();

    WasapiCapture(const WasapiCapture&) = delete;
    WasapiCapture& operator=(const WasapiCapture&) = delete;

    bool start(const QString& deviceId, WasapiCaptureMode mode, AudioClock* clock);
    void stop();
    bool isRunning() const { return running_.load(std::memory_order_acquire); }

    void setDataReadyCallback(DataReadyFn fn) { dataReady_ = std::move(fn); }

    AudioRingBuffer& ring() { return ring_; }
    AudioMeter& meter() { return meter_; }
    QString lastError() const;

    /// Engine-format frames available.
    std::size_t pull(float* interleavedStereo, std::size_t frames);

private:
    void threadMain(QString deviceId, WasapiCaptureMode mode);

    std::thread thread_;
    std::atomic<bool> running_{false};
    std::atomic<bool> stopRequested_{false};
    AudioRingBuffer ring_;
    AudioMeter meter_;
    AudioResampler resampler_;
    AudioClock* clock_ = nullptr;
    DataReadyFn dataReady_;
    mutable std::mutex errorMutex_;
    QString lastError_;
    std::atomic<std::int64_t> framesCaptured_{0};
};

} // namespace nitro
