#pragma once

#include "nitro/platform/CaptureInterfaces.hpp"

namespace nitro::platform::windows {

/// Windows stubs — WGC / MF / WASAPI implementations come later.
class WindowsScreenCapture final : public IScreenCapture {
public:
    bool start(const QString&) override { return false; }
    void stop() override { running_ = false; }
    bool isRunning() const override { return running_; }
private:
    bool running_ = false;
};

class WindowsWindowCapture final : public IWindowCapture {
public:
    bool start(const QString&) override { return false; }
    void stop() override { running_ = false; }
    bool isRunning() const override { return running_; }
private:
    bool running_ = false;
};

class WindowsCameraCapture final : public ICameraCapture {
public:
    bool start(const QString&) override { return false; }
    void stop() override { running_ = false; }
    bool isRunning() const override { return running_; }
private:
    bool running_ = false;
};

class WindowsAudioCapture final : public IAudioCapture {
public:
    bool start(const QString&) override { return false; }
    void stop() override { running_ = false; }
    bool isRunning() const override { return running_; }
private:
    bool running_ = false;
};

} // namespace nitro::platform::windows
