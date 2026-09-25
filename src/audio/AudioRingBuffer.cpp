#include "nitro/audio/AudioRingBuffer.hpp"

#include <algorithm>
#include <cstring>

namespace nitro {

AudioRingBuffer::AudioRingBuffer(std::size_t capacityFrames, int channels)
{
    reset(capacityFrames, channels);
}

void AudioRingBuffer::reset(std::size_t capacityFrames, int channels)
{
    capacityFrames_ = capacityFrames;
    channels_ = std::max(1, channels);
    data_.assign(capacityFrames_ * static_cast<std::size_t>(channels_), 0.0f);
    writePos_.store(0, std::memory_order_relaxed);
    readPos_.store(0, std::memory_order_relaxed);
    resetCounters();
}

void AudioRingBuffer::resetCounters()
{
    overruns_.store(0, std::memory_order_relaxed);
    underruns_.store(0, std::memory_order_relaxed);
}

std::size_t AudioRingBuffer::availableFrames() const
{
    const auto w = writePos_.load(std::memory_order_acquire);
    const auto r = readPos_.load(std::memory_order_acquire);
    return static_cast<std::size_t>(w - r);
}

std::size_t AudioRingBuffer::freeFrames() const
{
    if (capacityFrames_ == 0) {
        return 0;
    }
    const auto avail = availableFrames();
    return avail >= capacityFrames_ ? 0 : (capacityFrames_ - avail);
}

void AudioRingBuffer::clear()
{
    // Only safe when producer+consumer are stopped.
    writePos_.store(0, std::memory_order_relaxed);
    readPos_.store(0, std::memory_order_relaxed);
}

std::size_t AudioRingBuffer::write(const float* interleaved, std::size_t frames)
{
    if (!interleaved || frames == 0 || capacityFrames_ == 0) {
        return 0;
    }

    const std::size_t free = freeFrames();
    std::size_t toWrite = std::min(frames, free);
    if (toWrite < frames) {
        overruns_.fetch_add(frames - toWrite, std::memory_order_relaxed);
    }
    if (toWrite == 0) {
        return 0;
    }

    auto w = writePos_.load(std::memory_order_relaxed);
    for (std::size_t i = 0; i < toWrite; ++i) {
        const std::size_t idx = static_cast<std::size_t>(w + i) % capacityFrames_;
        std::memcpy(
            data_.data() + idx * static_cast<std::size_t>(channels_),
            interleaved + i * static_cast<std::size_t>(channels_),
            sizeof(float) * static_cast<std::size_t>(channels_));
    }
    writePos_.store(w + toWrite, std::memory_order_release);
    return toWrite;
}

std::size_t AudioRingBuffer::writeOverwrite(const float* interleaved, std::size_t frames)
{
    // Deprecated path: drop NEW samples instead of stealing readPos (SPSC-safe).
    return write(interleaved, frames);
}

std::size_t AudioRingBuffer::read(float* interleavedOut, std::size_t frames)
{
    if (!interleavedOut || frames == 0 || capacityFrames_ == 0) {
        return 0;
    }

    const std::size_t avail = availableFrames();
    const std::size_t toRead = std::min(frames, avail);
    if (toRead < frames) {
        underruns_.fetch_add(frames - toRead, std::memory_order_relaxed);
    }
    if (toRead == 0) {
        return 0;
    }

    auto r = readPos_.load(std::memory_order_relaxed);
    for (std::size_t i = 0; i < toRead; ++i) {
        const std::size_t idx = static_cast<std::size_t>(r + i) % capacityFrames_;
        std::memcpy(
            interleavedOut + i * static_cast<std::size_t>(channels_),
            data_.data() + idx * static_cast<std::size_t>(channels_),
            sizeof(float) * static_cast<std::size_t>(channels_));
    }
    readPos_.store(r + toRead, std::memory_order_release);
    return toRead;
}

} // namespace nitro
