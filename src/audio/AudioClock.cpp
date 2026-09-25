#include "nitro/audio/AudioClock.hpp"

#ifdef _WIN32
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
#endif

namespace nitro {

AudioClock::AudioClock()
{
#ifdef _WIN32
    LARGE_INTEGER freq{};
    LARGE_INTEGER counter{};
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&counter);
    freq_ = freq.QuadPart > 0 ? freq.QuadPart : 1;
    startCounter_ = counter.QuadPart;
#else
    freq_ = 1'000'000'000;
    startCounter_ = 0;
#endif
}

std::int64_t AudioClock::nowNs() const
{
#ifdef _WIN32
    LARGE_INTEGER counter{};
    QueryPerformanceCounter(&counter);
    // (delta * 1e9) / freq
    const auto delta = counter.QuadPart - startCounter_;
    return static_cast<std::int64_t>((delta * 1'000'000'000LL) / freq_);
#else
    return 0;
#endif
}

std::int64_t AudioClock::framesToNs(std::int64_t frames, int sampleRate) const
{
    if (sampleRate <= 0) {
        return 0;
    }
    return (frames * 1'000'000'000LL) / sampleRate;
}

} // namespace nitro
