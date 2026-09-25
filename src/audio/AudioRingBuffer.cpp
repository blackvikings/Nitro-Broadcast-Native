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
}

std::size_t AudioRingBuffer::availableFrames() const
{
    const auto w = writePos_.load(std::memory_order_acquire);
    const auto r = readPos_.load(std::memory_order_relaxed);
    return w - r;
}

std::size_t AudioRingBuffer::freeFrames() const
{
    return capacityFrames_ - availableFrames();
}

void AudioRingBuffer::clear()
{
    writePos_.store(0, std::memory_order_relaxed);
    readPos_.store(0, std::memory_order_relaxed);
}

std::size_t AudioRingBuffer::write(const float* interleaved, std::size_t frames)
{
    if (!interleaved || frames == 0 || capacityFrames_ == 0) {
        return 0;
    }
    const std::size_t free = freeFrames();
    const std::size_t toWrite = std::min(frames, free);
    auto w = writePos_.load(std::memory_order_relaxed);
    for (std::size_t i = 0; i < toWrite; ++i) {
        const std::size_t idx = (w + i) % capacityFrames_;
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
    if (!interleaved || frames == 0 || capacityFrames_ == 0) {
        return 0;
    }
    // If not enough space, advance read pointer (drop oldest).
    const std::size_t free = freeFrames();
    if (frames > free) {
        const std::size_t drop = frames - free;
        auto r = readPos_.load(std::memory_order_relaxed);
        readPos_.store(r + drop, std::memory_order_release);
    }
    return write(interleaved, frames);
}

std::size_t AudioRingBuffer::read(float* interleavedOut, std::size_t frames)
{
    if (!interleavedOut || frames == 0 || capacityFrames_ == 0) {
        return 0;
    }
    const std::size_t avail = availableFrames();
    const std::size_t toRead = std::min(frames, avail);
    auto r = readPos_.load(std::memory_order_relaxed);
    for (std::size_t i = 0; i < toRead; ++i) {
        const std::size_t idx = (r + i) % capacityFrames_;
        std::memcpy(
            interleavedOut + i * static_cast<std::size_t>(channels_),
            data_.data() + idx * static_cast<std::size_t>(channels_),
            sizeof(float) * static_cast<std::size_t>(channels_));
    }
    readPos_.store(r + toRead, std::memory_order_release);
    return toRead;
}

} // namespace nitro
