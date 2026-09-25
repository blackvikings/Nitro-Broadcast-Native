#pragma once

#include <cstddef>
#include <vector>

namespace nitro {

/**
 * Stateful stereo float resampler → engine bus (48 kHz / float32 / stereo).
 *
 * Continuity: keeps fractional source position + last frame across packets
 * to avoid clicks at WASAPI buffer boundaries.
 *
 * Backend: linear for now. When FFmpeg is linked, swap implementation to
 * libswresample behind the same API (no call-site changes).
 */
class AudioResampler {
public:
    void configure(int inputRate, int outputRate, int channels = 2);
    void reset();

    int inputRate() const { return inRate_; }
    int outputRate() const { return outRate_; }
    int channels() const { return channels_; }

    /// Process interleaved float input → interleaved float output (appended/resized).
    void process(const float* inInterleaved, std::size_t inFrames, std::vector<float>& outInterleaved);

    /// Flush remaining fractional delay as silence-padded end (optional).
    void flush(std::vector<float>& outInterleaved);

private:
    int inRate_ = 48000;
    int outRate_ = 48000;
    int channels_ = 2;
    double srcPos_ = 0.0; // fractional frame position into input stream
    std::vector<float> history_; // last input frame for continuity
    bool hasHistory_ = false;
};

} // namespace nitro
