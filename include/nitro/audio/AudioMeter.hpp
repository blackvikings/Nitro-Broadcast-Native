#pragma once

#include <atomic>
#include <cmath>
#include <cstddef>
#include <cstdint>

namespace nitro {

struct MeterLevels {
    float peakLinear = 0.0f;
    float rmsLinear = 0.0f;
    float peakHoldLinear = 0.0f;
    float peakDb = -120.0f;
    float rmsDb = -120.0f;
    float peakHoldDb = -120.0f;
    float peakLeft = 0.0f;
    float peakRight = 0.0f;
    bool clipped = false;
};

inline float linearToDb(float linear)
{
    constexpr float kMin = 1.0e-7f;
    if (linear < kMin) {
        return -120.0f;
    }
    return 20.0f * std::log10(linear);
}

inline float dbToLinear(float db)
{
    if (db <= -120.0f) {
        return 0.0f;
    }
    return std::pow(10.0f, db / 20.0f);
}

/// Real-time meter: Peak, RMS, Peak hold, clip. Thread-safe snapshot for UI.
class AudioMeter {
public:
    void process(const float* interleaved, std::size_t frames, int channels);
    void reset();

    MeterLevels snapshot() const;
    float uiLevel() const;

    void setPeakHoldMs(float ms) { peakHoldMs_ = ms; }
    void setSampleRate(int sr) { sampleRate_ = sr > 0 ? sr : 48000; }
    void setClipThreshold(float t) { clipThreshold_ = t; }

    bool clipped() const { return clipped_.load(std::memory_order_relaxed); }
    void clearClip() { clipped_.store(false, std::memory_order_relaxed); }

private:
    std::atomic<float> peak_{0.0f};
    std::atomic<float> rms_{0.0f};
    std::atomic<float> peakHold_{0.0f};
    std::atomic<float> peakL_{0.0f};
    std::atomic<float> peakR_{0.0f};
    std::atomic<bool> clipped_{false};
    float peakHoldMs_ = 800.0f;
    int sampleRate_ = 48000;
    float holdTimerFrames_ = 0.0f;
    float clipThreshold_ = 1.0f;
    // Envelope coefficients (approx attack/release for displayed peak)
    float attack_ = 0.35f;
    float release_ = 0.08f;
    float envPeak_ = 0.0f;
};

} // namespace nitro
