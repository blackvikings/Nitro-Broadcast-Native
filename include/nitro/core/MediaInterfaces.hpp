#pragma once

#include "nitro/core/MediaResult.hpp"

#include <QVariantMap>

namespace nitro {

/// Empty config placeholder for modules that need no parameters yet.
struct EmptyConfig {
    QVariantMap toMap() const { return {}; }
    static EmptyConfig fromMap(const QVariantMap&) { return {}; }
};

class ICaptureEngine {
public:
    virtual ~ICaptureEngine() = default;
    virtual MediaResult start(const EmptyConfig& config = {}) = 0;
    virtual MediaResult stop() = 0;
    virtual MediaStatus status() const = 0;
    virtual MediaCapabilities capabilities() const = 0;
};

class IAudioEngineIface {
public:
    virtual ~IAudioEngineIface() = default;
    virtual MediaResult start(const EmptyConfig& config = {}) = 0;
    virtual MediaResult stop() = 0;
    virtual MediaStatus status() const = 0;
    virtual MediaCapabilities capabilities() const = 0;
};

class IVideoEngine {
public:
    virtual ~IVideoEngine() = default;
    virtual MediaResult start(const EmptyConfig& config = {}) = 0;
    virtual MediaResult stop() = 0;
    virtual MediaStatus status() const = 0;
    virtual MediaCapabilities capabilities() const = 0;
};

class IEncoder {
public:
    virtual ~IEncoder() = default;
    virtual MediaResult open(const EmptyConfig& config = {}) = 0;
    virtual MediaResult close() = 0;
    virtual MediaStatus status() const = 0;
    virtual MediaCapabilities capabilities() const = 0;
};

class IRecorder {
public:
    virtual ~IRecorder() = default;
    virtual MediaResult start(const EmptyConfig& config = {}) = 0;
    virtual MediaResult stop() = 0;
    virtual MediaStatus status() const = 0;
    virtual MediaCapabilities capabilities() const = 0;
};

class IStreamer {
public:
    virtual ~IStreamer() = default;
    virtual MediaResult start(const EmptyConfig& config = {}) = 0;
    virtual MediaResult stop() = 0;
    virtual MediaStatus status() const = 0;
    virtual MediaCapabilities capabilities() const = 0;
};

class IDestination {
public:
    virtual ~IDestination() = default;
    virtual MediaResult connect(const EmptyConfig& config = {}) = 0;
    virtual MediaResult disconnect() = 0;
    virtual MediaResult start(const EmptyConfig& config = {}) = 0;
    virtual MediaResult stop() = 0;
    virtual MediaStatus status() const = 0;
    virtual MediaCapabilities capabilities() const = 0;
};

} // namespace nitro
