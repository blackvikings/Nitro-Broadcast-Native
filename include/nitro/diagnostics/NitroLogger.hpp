#pragma once

#include <QString>
#include <chrono>
#include <mutex>
#include <vector>

namespace nitro {

enum class LogLevel {
    Debug,
    Info,
    Warning,
    Error
};

class NitroLogger {
public:
    static NitroLogger& instance();

    void setMinLevel(LogLevel level);
    LogLevel minLevel() const;

    void log(LogLevel level, const QString& module, const QString& message);

    void debug(const QString& module, const QString& message);
    void info(const QString& module, const QString& message);
    void warning(const QString& module, const QString& message);
    void error(const QString& module, const QString& message);

    /// Returns recent log lines (for diagnostics UI). Secrets must never be passed in.
    QStringList recentLines(int maxCount = 200) const;

    NitroLogger(const NitroLogger&) = delete;
    NitroLogger& operator=(const NitroLogger&) = delete;

private:
    NitroLogger() = default;

    static QString levelToString(LogLevel level);
    static QString sanitize(const QString& message);

    mutable std::mutex mutex_;
    LogLevel minLevel_ = LogLevel::Debug;
    std::vector<QString> recent_;
    static constexpr std::size_t kMaxRecent = 500;
};

} // namespace nitro

#define NITRO_LOG_DEBUG(module, msg) ::nitro::NitroLogger::instance().debug((module), (msg))
#define NITRO_LOG_INFO(module, msg)  ::nitro::NitroLogger::instance().info((module), (msg))
#define NITRO_LOG_WARN(module, msg)  ::nitro::NitroLogger::instance().warning((module), (msg))
#define NITRO_LOG_ERROR(module, msg) ::nitro::NitroLogger::instance().error((module), (msg))
