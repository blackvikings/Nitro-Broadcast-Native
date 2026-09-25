#include "nitro/core/NitroEngine.hpp"

#include "nitro/diagnostics/NitroLogger.hpp"

namespace nitro {

NitroEngine::NitroEngine()
    : capture_(std::make_unique<NullCaptureEngine>())
    , audio_(std::make_unique<NullAudioEngine>())
    , video_(std::make_unique<NullVideoEngine>())
    , encoder_(std::make_unique<NullEncoder>())
    , recorder_(std::make_unique<NullRecorder>())
    , streamer_(std::make_unique<NullStreamer>())
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
    NITRO_LOG_INFO(QStringLiteral("Engine"),
                   QStringLiteral("Initialized with null media backends (capture/encode not implemented)"));
    initialized_ = true;
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
    audio_->stop();
    capture_->stop();
    initialized_ = false;
    NITRO_LOG_INFO(QStringLiteral("Engine"), QStringLiteral("Shutdown complete"));
}

} // namespace nitro
