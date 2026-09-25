#include "nitro/audio/AudioResampler.hpp"

#include <algorithm>
#include <cmath>

namespace nitro {

void AudioResampler::configure(int inputRate, int outputRate, int channels)
{
    inRate_ = inputRate > 0 ? inputRate : 48000;
    outRate_ = outputRate > 0 ? outputRate : 48000;
    channels_ = std::max(1, channels);
    reset();
}

void AudioResampler::reset()
{
    srcPos_ = 0.0;
    history_.assign(static_cast<std::size_t>(channels_), 0.0f);
    hasHistory_ = false;
}

void AudioResampler::process(const float* inInterleaved, std::size_t inFrames,
                             std::vector<float>& outInterleaved)
{
    outInterleaved.clear();
    if (!inInterleaved || inFrames == 0) {
        return;
    }

    if (inRate_ == outRate_) {
        outInterleaved.assign(inInterleaved, inInterleaved + inFrames * static_cast<std::size_t>(channels_));
        // Keep last frame for future rate changes
        history_.assign(inInterleaved + (inFrames - 1) * channels_,
                        inInterleaved + inFrames * channels_);
        hasHistory_ = true;
        srcPos_ = 0.0;
        return;
    }

    // Build contiguous view: optional history frame + current packet
    std::vector<float> stream;
    stream.reserve((inFrames + 1) * static_cast<std::size_t>(channels_));
    if (hasHistory_) {
        stream.insert(stream.end(), history_.begin(), history_.end());
    }
    stream.insert(stream.end(), inInterleaved, inInterleaved + inFrames * channels_);

    const std::size_t streamFrames = stream.size() / static_cast<std::size_t>(channels_);
    if (streamFrames < 2) {
        history_.assign(stream.begin(), stream.end());
        hasHistory_ = !history_.empty();
        return;
    }

    // srcPos_ is relative to start of `stream` (0 = history frame or first in)
    const double step = static_cast<double>(inRate_) / static_cast<double>(outRate_);
    // How many output frames until we exhaust usable stream (need i1 < streamFrames)
    const double maxPos = static_cast<double>(streamFrames - 1) - 1e-9;
    std::size_t outCount = 0;
    double pos = srcPos_;
    while (pos <= maxPos) {
        ++outCount;
        pos += step;
    }

    outInterleaved.resize(outCount * static_cast<std::size_t>(channels_));
    pos = srcPos_;
    for (std::size_t o = 0; o < outCount; ++o) {
        const std::size_t i0 = static_cast<std::size_t>(pos);
        const std::size_t i1 = std::min(i0 + 1, streamFrames - 1);
        const float t = static_cast<float>(pos - static_cast<double>(i0));
        for (int c = 0; c < channels_; ++c) {
            const float a = stream[i0 * channels_ + c];
            const float b = stream[i1 * channels_ + c];
            outInterleaved[o * channels_ + c] = a * (1.0f - t) + b * t;
        }
        pos += step;
    }

    // Advance source position relative to NEXT packet (without history prefix)
    // Consumed through `pos` in stream coords; history was 1 frame if present.
    const double historyOffset = hasHistory_ ? 1.0 : 0.0;
    srcPos_ = pos - historyOffset - static_cast<double>(inFrames);
    // Keep last input frame as history for next call
    history_.assign(inInterleaved + (inFrames - 1) * channels_,
                    inInterleaved + inFrames * channels_);
    hasHistory_ = true;
    // Clamp fractional remainder into [0, 1)
    if (srcPos_ < 0.0) {
        srcPos_ = 0.0;
    }
    while (srcPos_ >= 1.0) {
        srcPos_ -= 1.0;
    }
}

void AudioResampler::flush(std::vector<float>& outInterleaved)
{
    outInterleaved.clear();
    reset();
}

} // namespace nitro
