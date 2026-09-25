#pragma once

#include "nitro/audio/AudioClock.hpp"
#include "nitro/audio/AudioFormat.hpp"

#include <algorithm>
#include <cstddef>
#include <vector>

namespace nitro {

/// Interleaved float32 PCM block with timing metadata.
struct AudioBuffer {
    AudioFormat format = engineAudioFormat();
    AudioTimestamp timestamp;
    std::vector<float> samples; // interleaved
    std::size_t frameCount = 0;

    void resize(std::size_t frames, int channels)
    {
        frameCount = frames;
        format.channels = channels;
        samples.assign(frames * static_cast<std::size_t>(channels), 0.0f);
    }

    void clear()
    {
        std::fill(samples.begin(), samples.end(), 0.0f);
    }
};

} // namespace nitro
