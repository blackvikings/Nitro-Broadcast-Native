#pragma once

#include <QString>

namespace nitro {

struct EncoderConfig {
    QString codec = QStringLiteral("auto");
    int width = 1920;
    int height = 1080;
    int fps = 60;
    int bitrateKbps = 6000;
};

struct RecordingConfig {
    QString container = QStringLiteral("mkv");
    QString outputPath;
    EncoderConfig video;
};

struct StreamingConfig {
    QString rtmpUrl;
    // stream key must never be logged
    QString streamKey;
    EncoderConfig video;
    bool reconnect = true;
};

/// Abstraction placeholder around FFmpeg. No libav linkage in this phase.
class FFmpegManager {
public:
    FFmpegManager() = default;

    bool isAvailable() const { return false; }
    QString statusMessage() const
    {
        return QStringLiteral("FFmpeg not linked yet — configuration stubs only");
    }

    void setEncoderConfig(const EncoderConfig& config) { encoderConfig_ = config; }
    void setRecordingConfig(const RecordingConfig& config) { recordingConfig_ = config; }
    void setStreamingConfig(const StreamingConfig& config) { streamingConfig_ = config; }

    const EncoderConfig& encoderConfig() const { return encoderConfig_; }
    const RecordingConfig& recordingConfig() const { return recordingConfig_; }
    const StreamingConfig& streamingConfig() const { return streamingConfig_; }

private:
    EncoderConfig encoderConfig_;
    RecordingConfig recordingConfig_;
    StreamingConfig streamingConfig_;
};

} // namespace nitro
