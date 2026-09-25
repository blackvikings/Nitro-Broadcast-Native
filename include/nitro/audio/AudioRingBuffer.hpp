#pragma once

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace nitro {

/**
 * Lock-free SPSC float ring (interleaved frames).
 *
 * Ownership:
 *  - Producer alone mutates write index / writes slots
 *  - Consumer alone mutates read index / reads slots
 *  - On overrun the producer DROPS new input (never advances readPos)
 *  - On underrun the consumer returns fewer frames and increments underruns
 *
 * No heap allocation after reset(). No mutex. Safe for WASAPI callback + process thread.
 */
class AudioRingBuffer {
public:
    AudioRingBuffer() = default;
    explicit AudioRingBuffer(std::size_t capacityFrames, int channels);

    void reset(std::size_t capacityFrames, int channels);

    int channels() const { return channels_; }
    std::size_t capacityFrames() const { return capacityFrames_; }

    /// Writes up to `frames`. Returns frames accepted. Excess increments overruns and is dropped.
    std::size_t write(const float* interleaved, std::size_t frames);

    /// Reads up to `frames`. Returns frames produced. Short read increments underruns if frames>0 requested.
    std::size_t read(float* interleavedOut, std::size_t frames);

    std::size_t availableFrames() const;
    std::size_t freeFrames() const;

    void clear();

    std::uint64_t overrunCount() const { return overruns_.load(std::memory_order_relaxed); }
    std::uint64_t underrunCount() const { return underruns_.load(std::memory_order_relaxed); }
    void resetCounters();

    /// @deprecated Prefer write() — overwrite-oldest violated SPSC ownership.
    [[deprecated("Use write(); producer must not advance readPos")]]
    std::size_t writeOverwrite(const float* interleaved, std::size_t frames);

private:
    std::size_t capacityFrames_ = 0;
    int channels_ = 2;
    std::vector<float> data_;
    // Monotonic frame counters — slot = pos % capacity
    alignas(64) std::atomic<std::uint64_t> writePos_{0};
    alignas(64) std::atomic<std::uint64_t> readPos_{0};
    std::atomic<std::uint64_t> overruns_{0};
    std::atomic<std::uint64_t> underruns_{0};
};

} // namespace nitro
