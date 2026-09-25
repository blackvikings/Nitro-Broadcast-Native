#pragma once

namespace nitro {

class ICaptureEngine {
public:
    virtual ~ICaptureEngine() = default;
    virtual bool start() = 0;
    virtual void stop() = 0;
    virtual bool isRunning() const = 0;
};

class IAudioEngine {
public:
    virtual ~IAudioEngine() = default;
    virtual bool start() = 0;
    virtual void stop() = 0;
    virtual bool isRunning() const = 0;
};

class IVideoEngine {
public:
    virtual ~IVideoEngine() = default;
    virtual bool start() = 0;
    virtual void stop() = 0;
    virtual bool isRunning() const = 0;
};

class IEncoder {
public:
    virtual ~IEncoder() = default;
    virtual bool open() = 0;
    virtual void close() = 0;
    virtual bool isOpen() const = 0;
};

class IRecorder {
public:
    virtual ~IRecorder() = default;
    virtual bool start() = 0;
    virtual void stop() = 0;
    virtual bool isRunning() const = 0;
};

class IStreamer {
public:
    virtual ~IStreamer() = default;
    virtual bool start() = 0;
    virtual void stop() = 0;
    virtual bool isRunning() const = 0;
};

class IDestination {
public:
    virtual ~IDestination() = default;
    virtual bool connect() = 0;
    virtual void disconnect() = 0;
    virtual bool start() = 0;
    virtual void stop() = 0;
    virtual bool isConnected() const = 0;
    virtual bool isLive() const = 0;
};

} // namespace nitro
