#pragma once

#include "nitro/core/MediaInterfaces.hpp"

#include <memory>

namespace nitro {

/// Null / stub implementations — media path not implemented yet.
class NullCaptureEngine final : public ICaptureEngine {
public:
    bool start() override { return false; }
    void stop() override { running_ = false; }
    bool isRunning() const override { return running_; }
private:
    bool running_ = false;
};

class NullAudioEngine final : public IAudioEngine {
public:
    bool start() override { return false; }
    void stop() override { running_ = false; }
    bool isRunning() const override { return running_; }
private:
    bool running_ = false;
};

class NullVideoEngine final : public IVideoEngine {
public:
    bool start() override { return false; }
    void stop() override { running_ = false; }
    bool isRunning() const override { return running_; }
private:
    bool running_ = false;
};

class NullEncoder final : public IEncoder {
public:
    bool open() override { return false; }
    void close() override { open_ = false; }
    bool isOpen() const override { return open_; }
private:
    bool open_ = false;
};

class NullRecorder final : public IRecorder {
public:
    bool start() override { return false; }
    void stop() override { running_ = false; }
    bool isRunning() const override { return running_; }
private:
    bool running_ = false;
};

class NullStreamer final : public IStreamer {
public:
    bool start() override { return false; }
    void stop() override { running_ = false; }
    bool isRunning() const override { return running_; }
private:
    bool running_ = false;
};

class NullDestination final : public IDestination {
public:
    bool connect() override { return false; }
    void disconnect() override { connected_ = false; live_ = false; }
    bool start() override { return false; }
    void stop() override { live_ = false; }
    bool isConnected() const override { return connected_; }
    bool isLive() const override { return live_; }
private:
    bool connected_ = false;
    bool live_ = false;
};

} // namespace nitro
