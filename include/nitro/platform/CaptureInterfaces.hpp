#pragma once

#include <QString>
#include <memory>

namespace nitro::platform {

class IScreenCapture {
public:
    virtual ~IScreenCapture() = default;
    virtual bool start(const QString& deviceId) = 0;
    virtual void stop() = 0;
    virtual bool isRunning() const = 0;
};

class IWindowCapture {
public:
    virtual ~IWindowCapture() = default;
    virtual bool start(const QString& windowId) = 0;
    virtual void stop() = 0;
    virtual bool isRunning() const = 0;
};

class ICameraCapture {
public:
    virtual ~ICameraCapture() = default;
    virtual bool start(const QString& deviceId) = 0;
    virtual void stop() = 0;
    virtual bool isRunning() const = 0;
};

class IAudioCapture {
public:
    virtual ~IAudioCapture() = default;
    virtual bool start(const QString& deviceId) = 0;
    virtual void stop() = 0;
    virtual bool isRunning() const = 0;
};

} // namespace nitro::platform
