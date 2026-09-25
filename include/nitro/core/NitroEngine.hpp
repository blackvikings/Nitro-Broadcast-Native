#pragma once

#include "nitro/core/NullMedia.hpp"

#include <memory>

namespace nitro {

/// Media engine façade. Owns interfaces; real backends not wired yet.
class NitroEngine {
public:
    NitroEngine();
    ~NitroEngine();

    NitroEngine(const NitroEngine&) = delete;
    NitroEngine& operator=(const NitroEngine&) = delete;

    bool initialize();
    void shutdown();
    bool isInitialized() const { return initialized_; }

    ICaptureEngine& capture() { return *capture_; }
    IAudioEngine& audio() { return *audio_; }
    IVideoEngine& video() { return *video_; }
    IEncoder& encoder() { return *encoder_; }
    IRecorder& recorder() { return *recorder_; }
    IStreamer& streamer() { return *streamer_; }

private:
    bool initialized_ = false;
    std::unique_ptr<ICaptureEngine> capture_;
    std::unique_ptr<IAudioEngine> audio_;
    std::unique_ptr<IVideoEngine> video_;
    std::unique_ptr<IEncoder> encoder_;
    std::unique_ptr<IRecorder> recorder_;
    std::unique_ptr<IStreamer> streamer_;
};

} // namespace nitro
