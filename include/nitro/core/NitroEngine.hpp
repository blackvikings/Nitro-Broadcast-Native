#pragma once

#include "nitro/audio/AudioDeviceManager.hpp"
#include "nitro/audio/AudioEngine.hpp"
#include "nitro/audio/AudioMixer.hpp"
#include "nitro/core/NullMedia.hpp"
#include "nitro/ffmpeg/FFmpegManager.hpp"
#include "nitro/render/RenderInterfaces.hpp"
#include "nitro/scenes/TransitionEngine.hpp"

#include <memory>

namespace nitro {

/**
 * Authoritative media engine.
 *
 * Ownership:
 *   NitroApplication
 *     └── NitroEngine
 *           ├── AudioDeviceManager + AudioEngine (real WASAPI)
 *           ├── Capture / Video / Compositor (stubs → future WGC)
 *           ├── Encoder / Recorder / Streamer (FFmpeg stubs)
 *           └── TransitionEngine
 *
 * UI must obtain AudioEngine only via NitroEngine — never a second instance.
 */
class NitroEngine {
public:
    NitroEngine();
    ~NitroEngine();

    NitroEngine(const NitroEngine&) = delete;
    NitroEngine& operator=(const NitroEngine&) = delete;

    bool initialize();
    void shutdown();
    bool isInitialized() const { return initialized_; }

    AudioDeviceManager* audioDevices() const { return audioDevices_.get(); }
    AudioEngine* audioEngine() const { return audioEngine_.get(); }
    AudioMixer* mixer() const { return mixer_.get(); }
    FFmpegManager* ffmpeg() const { return ffmpeg_.get(); }
    TransitionEngine* transitions() const { return transitions_.get(); }

    ICaptureEngine& capture() { return *capture_; }
    IVideoEngine& video() { return *video_; }
    IEncoder& encoder() { return *encoder_; }
    IRecorder& recorder() { return *recorder_; }
    IStreamer& streamer() { return *streamer_; }
    ICompositor& compositor() { return *compositor_; }
    PreviewRenderer& previewRenderer() { return previewRenderer_; }
    ProgramRenderer& programRenderer() { return programRenderer_; }

private:
    bool initialized_ = false;

    std::unique_ptr<AudioDeviceManager> audioDevices_;
    std::unique_ptr<AudioMixer> mixer_;
    std::unique_ptr<AudioEngine> audioEngine_;
    std::unique_ptr<FFmpegManager> ffmpeg_;
    std::unique_ptr<TransitionEngine> transitions_;

    std::unique_ptr<ICaptureEngine> capture_;
    std::unique_ptr<IVideoEngine> video_;
    std::unique_ptr<IEncoder> encoder_;
    std::unique_ptr<IRecorder> recorder_;
    std::unique_ptr<IStreamer> streamer_;
    std::unique_ptr<ICompositor> compositor_;
    PreviewRenderer previewRenderer_;
    ProgramRenderer programRenderer_;
};

} // namespace nitro
