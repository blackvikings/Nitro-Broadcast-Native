#pragma once

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace nitro {

/// Single-producer / single-consumer float sample ring (interleaved frames).
/// Capacity is in frames; each frame has `channels` floats.
class AudioRingBuffer {
public:
    AudioRingBuffer() = default;
    explicit AudioRingBuffer(std::size_t capacityFrames, int channels);

    void reset(std::size_t capacityFrames, int channels);

    int channels() const { return channels_; }
    std::size_t capacityFrames() const { return capacityFrames_; }

    /// Returns frames written (may be less if full — drops oldest on overflow policy optional).
    std::size_t write(const float* interleaved, std::size_t frames);
    /// Overwrite-oldest policy: always makes room.
    std::size_t writeOverwrite(const float* interleaved, std::size_t frames);

    std::size_t read(float* interleavedOut, std::size_t frames);
    std::size_t availableFrames() const;
    std::size_t freeFrames() const;

    void clear();

private:
    std::size_t capacityFrames_ = 0;
    int channels_ = 2;
    std::vector<float> data_;
    std::atomic<std::size_t> writePos_{0}; // in frames
    std::atomic<std::size_t> readPos_{0};
};

} // namespace nitro
