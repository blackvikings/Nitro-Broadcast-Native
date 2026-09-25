#include "nitro/audio/AudioMeter.hpp"

#include <algorithm>

namespace nitro {

void AudioMeter::process(const float* interleaved, std::size_t frames, int channels)
{
    if (!interleaved || frames == 0 || channels <= 0) {
        return;
    }

    float peak = 0.0f;
    float peakL = 0.0f;
    float peakR = 0.0f;
    double sumSq = 0.0;
    bool clip = false;
    const std::size_t total = frames * static_cast<std::size_t>(channels);
    for (std::size_t i = 0; i < total; ++i) {
        const float s = interleaved[i];
        const float a = std::abs(s);
        peak = std::max(peak, a);
        if (a >= clipThreshold_) {
            clip = true;
        }
        sumSq += static_cast<double>(a) * static_cast<double>(a);
        if (channels >= 2) {
            if ((i % static_cast<std::size_t>(channels)) == 0) {
                peakL = std::max(peakL, a);
            } else if ((i % static_cast<std::size_t>(channels)) == 1) {
                peakR = std::max(peakR, a);
            }
        } else {
            peakL = peakR = peak;
        }
    }
    const float rms = static_cast<float>(std::sqrt(sumSq / static_cast<double>(total)));

    // Stable envelope
    if (peak > envPeak_) {
        envPeak_ += (peak - envPeak_) * attack_;
    } else {
        envPeak_ += (peak - envPeak_) * release_;
    }

    peak_.store(envPeak_, std::memory_order_relaxed);
    rms_.store(rms, std::memory_order_relaxed);
    peakL_.store(peakL, std::memory_order_relaxed);
    peakR_.store(peakR, std::memory_order_relaxed);
    if (clip) {
        clipped_.store(true, std::memory_order_relaxed);
    }

    float hold = peakHold_.load(std::memory_order_relaxed);
    if (peak >= hold) {
        hold = peak;
        holdTimerFrames_ = peakHoldMs_ * 0.001f * static_cast<float>(sampleRate_);
    } else {
        holdTimerFrames_ -= static_cast<float>(frames);
        if (holdTimerFrames_ <= 0.0f) {
            hold = std::max(peak, hold * 0.92f);
        }
    }
    peakHold_.store(hold, std::memory_order_relaxed);
}

void AudioMeter::reset()
{
    peak_.store(0.0f, std::memory_order_relaxed);
    rms_.store(0.0f, std::memory_order_relaxed);
    peakHold_.store(0.0f, std::memory_order_relaxed);
    peakL_.store(0.0f, std::memory_order_relaxed);
    peakR_.store(0.0f, std::memory_order_relaxed);
    clipped_.store(false, std::memory_order_relaxed);
    holdTimerFrames_ = 0.0f;
    envPeak_ = 0.0f;
}

MeterLevels AudioMeter::snapshot() const
{
    MeterLevels m;
    m.peakLinear = peak_.load(std::memory_order_relaxed);
    m.rmsLinear = rms_.load(std::memory_order_relaxed);
    m.peakHoldLinear = peakHold_.load(std::memory_order_relaxed);
    m.peakLeft = peakL_.load(std::memory_order_relaxed);
    m.peakRight = peakR_.load(std::memory_order_relaxed);
    m.clipped = clipped_.load(std::memory_order_relaxed);
    m.peakDb = linearToDb(m.peakLinear);
    m.rmsDb = linearToDb(m.rmsLinear);
    m.peakHoldDb = linearToDb(m.peakHoldLinear);
    return m;
}

float AudioMeter::uiLevel() const
{
    const float db = linearToDb(peak_.load(std::memory_order_relaxed));
    const float norm = (db + 60.0f) / 60.0f;
    return std::clamp(norm, 0.0f, 1.0f);
}

} // namespace nitro
