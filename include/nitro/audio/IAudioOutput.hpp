#pragma once

#include "nitro/audio/AudioBuffer.hpp"

#include <memory>
#include <string>

namespace nitro {

class IAudioOutput {
public:
    virtual ~IAudioOutput() = default;
    virtual std::string name() const = 0;
    virtual bool start() = 0;
    virtual void stop() = 0;
    virtual bool isRunning() const = 0;
    /// Push mixed engine-format PCM (float32 interleaved stereo @ engine rate).
    virtual void push(const AudioBuffer& buffer) = 0;
};

/// No-op sink used until recording/streaming/monitor backends exist.
class NullAudioOutput final : public IAudioOutput {
public:
    std::string name() const override { return "null"; }
    bool start() override { running_ = true; return true; }
    void stop() override { running_ = false; }
    bool isRunning() const override { return running_; }
    void push(const AudioBuffer&) override {}
private:
    bool running_ = false;
};

} // namespace nitro
