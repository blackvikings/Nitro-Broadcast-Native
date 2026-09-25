#pragma once

#include <atomic>
#include <cstdint>

namespace nitro {

/// Monotonic engine clock in nanoseconds (QueryPerformanceCounter based on Windows).
class AudioClock {
public:
    AudioClock();

    /// Nanoseconds since clock construction (steady).
    std::int64_t nowNs() const;

    /// Convert host sample position to engine time.
    std::int64_t framesToNs(std::int64_t frames, int sampleRate) const;

private:
    std::int64_t freq_ = 1;
    std::int64_t startCounter_ = 0;
};

struct AudioTimestamp {
    std::int64_t captureNs = 0;   // when buffer was captured
    std::int64_t hostFrames = 0;  // device position if known
};

} // namespace nitro
