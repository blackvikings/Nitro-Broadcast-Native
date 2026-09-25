#pragma once

#include <atomic>
#include <cmath>
#include <cstdint>

namespace nitro {

struct MeterLevels {
    float peakLinear = 0.0f;
    float rmsLinear = 0.0f;
    float peakHoldLinear = 0.0f;
    float peakDb = -120.0f;
    float rmsDb = -120.0f;
    float peakHoldDb = -120.0f;
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

/// Real-time meter: Peak, RMS, Peak hold. Thread-safe snapshot for UI.
class AudioMeter {
public:
    void process(const float* interleaved, std::size_t frames, int channels);
    void reset();

    MeterLevels snapshot() const;
    /// UI-normalized 0..1 from peak dB (-60..0 → 0..1).
    float uiLevel() const;

    void setPeakHoldMs(float ms) { peakHoldMs_ = ms; }
    void setSampleRate(int sr) { sampleRate_ = sr > 0 ? sr : 48000; }

private:
    std::atomic<float> peak_{0.0f};
    std::atomic<float> rms_{0.0f};
    std::atomic<float> peakHold_{0.0f};
    float peakHoldMs_ = 800.0f;
    int sampleRate_ = 48000;
    float holdTimerFrames_ = 0.0f;
};

} // namespace nitro
