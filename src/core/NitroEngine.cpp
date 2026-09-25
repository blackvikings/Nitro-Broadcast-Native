#include "nitro/core/NitroEngine.hpp"

#include "nitro/diagnostics/NitroLogger.hpp"

namespace nitro {

NitroEngine::NitroEngine()
    : capture_(std::make_unique<NullCaptureEngine>())
    , video_(std::make_unique<NullVideoEngine>())
    , encoder_(std::make_unique<NullEncoder>())
    , recorder_(std::make_unique<NullRecorder>())
    , streamer_(std::make_unique<NullStreamer>())
    , compositor_(std::make_unique<NullCompositor>())
{
}

NitroEngine::~NitroEngine()
{
    shutdown();
}

bool NitroEngine::initialize()
{
    if (initialized_) {
        return true;
    }

    audioDevices_ = std::make_unique<AudioDeviceManager>();
    mixer_ = std::make_unique<AudioMixer>();
    audioEngine_ = std::make_unique<AudioEngine>(audioDevices_.get());
    audioEngine_->attachMixer(mixer_.get());
    ffmpeg_ = std::make_unique<FFmpegManager>();
    transitions_ = std::make_unique<TransitionEngine>();

    compositor_->setCanvasSize(1920, 1080);
    previewRenderer_.setCompositor(compositor_.get());
    programRenderer_.setCompositor(compositor_.get());

    if (!audioEngine_->start()) {
        NITRO_LOG_WARN(QStringLiteral("Engine"),
                       QStringLiteral("WASAPI AudioEngine failed to start — continuing"));
    }

    initialized_ = true;
    NITRO_LOG_INFO(QStringLiteral("Engine"),
                   QStringLiteral("NitroEngine ready (single AudioEngine owner)"));
    return true;
}

void NitroEngine::shutdown()
{
    if (!initialized_) {
        return;
    }
    streamer_->stop();
    recorder_->stop();
    encoder_->close();
    video_->stop();
    capture_->stop();
    if (audioEngine_) {
        audioEngine_->stop();
    }
    audioEngine_.reset();
    mixer_.reset();
    audioDevices_.reset();
    transitions_.reset();
    ffmpeg_.reset();
    initialized_ = false;
    NITRO_LOG_INFO(QStringLiteral("Engine"), QStringLiteral("Shutdown complete"));
}

} // namespace nitro
