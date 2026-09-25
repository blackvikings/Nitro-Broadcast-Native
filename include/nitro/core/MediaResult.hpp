#pragma once

#include <QString>
#include <cstdint>

namespace nitro {

enum class MediaStatus {
    Idle,
    Starting,
    Running,
    Stopping,
    Error,
    Unavailable
};

enum class MediaErrorCode {
    Ok = 0,
    NotInitialized,
    AlreadyRunning,
    NotRunning,
    DeviceMissing,
    DeviceDisconnected,
    Unsupported,
    DependencyMissing,
    Internal,
    Cancelled
};

struct MediaError {
    MediaErrorCode code = MediaErrorCode::Ok;
    QString message;

    bool ok() const { return code == MediaErrorCode::Ok; }
};

struct MediaResult {
    MediaError error;

    static MediaResult success() { return {}; }
    static MediaResult fail(MediaErrorCode code, const QString& message)
    {
        MediaResult r;
        r.error = {code, message};
        return r;
    }

    bool ok() const { return error.ok(); }
    explicit operator bool() const { return ok(); }
};

struct MediaCapabilities {
    bool available = false;
    bool experimental = false;
    bool canStart = false;
    bool canStop = false;
    QString note;
};

inline QString mediaStatusToString(MediaStatus s)
{
    switch (s) {
    case MediaStatus::Idle: return QStringLiteral("Idle");
    case MediaStatus::Starting: return QStringLiteral("Starting");
    case MediaStatus::Running: return QStringLiteral("Running");
    case MediaStatus::Stopping: return QStringLiteral("Stopping");
    case MediaStatus::Error: return QStringLiteral("Error");
    case MediaStatus::Unavailable: return QStringLiteral("Unavailable");
    }
    return QStringLiteral("Unknown");
}

} // namespace nitro
