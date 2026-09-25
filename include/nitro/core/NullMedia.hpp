#pragma once

#include "nitro/core/MediaInterfaces.hpp"

namespace nitro {

class NullCaptureEngine final : public ICaptureEngine {
public:
    MediaResult start(const EmptyConfig&) override
    {
        return MediaResult::fail(MediaErrorCode::Unsupported,
                                 QStringLiteral("Capture engine not implemented"));
    }
    MediaResult stop() override { status_ = MediaStatus::Idle; return MediaResult::success(); }
    MediaStatus status() const override { return status_; }
    MediaCapabilities capabilities() const override
    {
        return {false, false, false, true, QStringLiteral("Null capture")};
    }
private:
    MediaStatus status_ = MediaStatus::Unavailable;
};

class NullVideoEngine final : public IVideoEngine {
public:
    MediaResult start(const EmptyConfig&) override
    {
        return MediaResult::fail(MediaErrorCode::Unsupported,
                                 QStringLiteral("Video engine not implemented"));
    }
    MediaResult stop() override { status_ = MediaStatus::Idle; return MediaResult::success(); }
    MediaStatus status() const override { return status_; }
    MediaCapabilities capabilities() const override
    {
        return {false, false, false, true, QStringLiteral("Null video")};
    }
private:
    MediaStatus status_ = MediaStatus::Unavailable;
};

class NullEncoder final : public IEncoder {
public:
    MediaResult open(const EmptyConfig&) override
    {
        return MediaResult::fail(MediaErrorCode::DependencyMissing,
                                 QStringLiteral("FFmpeg encoder not linked"));
    }
    MediaResult close() override { status_ = MediaStatus::Idle; return MediaResult::success(); }
    MediaStatus status() const override { return status_; }
    MediaCapabilities capabilities() const override
    {
        return {false, false, false, true, QStringLiteral("Encoder stub")};
    }
private:
    MediaStatus status_ = MediaStatus::Unavailable;
};

class NullRecorder final : public IRecorder {
public:
    MediaResult start(const EmptyConfig&) override
    {
        return MediaResult::fail(MediaErrorCode::Unsupported,
                                 QStringLiteral("Recorder not implemented"));
    }
    MediaResult stop() override { status_ = MediaStatus::Idle; return MediaResult::success(); }
    MediaStatus status() const override { return status_; }
    MediaCapabilities capabilities() const override
    {
        return {false, false, false, true, QStringLiteral("Recorder stub")};
    }
private:
    MediaStatus status_ = MediaStatus::Unavailable;
};

class NullStreamer final : public IStreamer {
public:
    MediaResult start(const EmptyConfig&) override
    {
        return MediaResult::fail(MediaErrorCode::Unsupported,
                                 QStringLiteral("Streamer not implemented"));
    }
    MediaResult stop() override { status_ = MediaStatus::Idle; return MediaResult::success(); }
    MediaStatus status() const override { return status_; }
    MediaCapabilities capabilities() const override
    {
        return {false, false, false, true, QStringLiteral("Streamer stub")};
    }
private:
    MediaStatus status_ = MediaStatus::Unavailable;
};

class NullDestination final : public IDestination {
public:
    MediaResult connect(const EmptyConfig&) override
    {
        return MediaResult::fail(MediaErrorCode::Unsupported,
                                 QStringLiteral("Destination not implemented"));
    }
    MediaResult disconnect() override { status_ = MediaStatus::Idle; return MediaResult::success(); }
    MediaResult start(const EmptyConfig&) override
    {
        return MediaResult::fail(MediaErrorCode::NotRunning, QStringLiteral("Not connected"));
    }
    MediaResult stop() override { return MediaResult::success(); }
    MediaStatus status() const override { return status_; }
    MediaCapabilities capabilities() const override
    {
        return {false, false, false, true, QStringLiteral("Destination stub")};
    }
private:
    MediaStatus status_ = MediaStatus::Unavailable;
};

} // namespace nitro
