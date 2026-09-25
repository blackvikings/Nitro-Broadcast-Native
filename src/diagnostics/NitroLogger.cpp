#include "nitro/diagnostics/NitroLogger.hpp"

#include <QDateTime>
#include <QDebug>
#include <QRegularExpression>

namespace nitro {

NitroLogger& NitroLogger::instance()
{
    static NitroLogger logger;
    return logger;
}

void NitroLogger::setMinLevel(LogLevel level)
{
    std::lock_guard lock(mutex_);
    minLevel_ = level;
}

LogLevel NitroLogger::minLevel() const
{
    std::lock_guard lock(mutex_);
    return minLevel_;
}

QString NitroLogger::levelToString(LogLevel level)
{
    switch (level) {
    case LogLevel::Debug: return QStringLiteral("DEBUG");
    case LogLevel::Info: return QStringLiteral("INFO");
    case LogLevel::Warning: return QStringLiteral("WARN");
    case LogLevel::Error: return QStringLiteral("ERROR");
    }
    return QStringLiteral("INFO");
}

QString NitroLogger::sanitize(const QString& message)
{
    // Never log secrets even if callers accidentally include them.
    QString cleaned = message;
    static const QRegularExpression tokenRe(
        QStringLiteral(R"((?i)(token|password|secret|authorization|api[_-]?key|stream[_-]?key)\s*[=:]\s*\S+)"));
    cleaned.replace(tokenRe, QStringLiteral("\\1=***REDACTED***"));
    return cleaned;
}

void NitroLogger::log(LogLevel level, const QString& module, const QString& message)
{
    std::lock_guard lock(mutex_);
    if (static_cast<int>(level) < static_cast<int>(minLevel_)) {
        return;
    }

    const QString timestamp = QDateTime::currentDateTimeUtc().toString(Qt::ISODate);
    const QString line = QStringLiteral("[%1] [%2] [%3] %4")
                             .arg(timestamp, levelToString(level), module, sanitize(message));

    recent_.push_back(line);
    if (recent_.size() > kMaxRecent) {
        recent_.erase(recent_.begin(), recent_.begin() + static_cast<std::ptrdiff_t>(recent_.size() - kMaxRecent));
    }

    switch (level) {
    case LogLevel::Debug:
        qDebug().noquote() << line;
        break;
    case LogLevel::Info:
        qInfo().noquote() << line;
        break;
    case LogLevel::Warning:
        qWarning().noquote() << line;
        break;
    case LogLevel::Error:
        qCritical().noquote() << line;
        break;
    }
}

void NitroLogger::debug(const QString& module, const QString& message)
{
    log(LogLevel::Debug, module, message);
}

void NitroLogger::info(const QString& module, const QString& message)
{
    log(LogLevel::Info, module, message);
}

void NitroLogger::warning(const QString& module, const QString& message)
{
    log(LogLevel::Warning, module, message);
}

void NitroLogger::error(const QString& module, const QString& message)
{
    log(LogLevel::Error, module, message);
}

QStringList NitroLogger::recentLines(int maxCount) const
{
    std::lock_guard lock(mutex_);
    QStringList out;
    const int start = std::max(0, static_cast<int>(recent_.size()) - maxCount);
    for (int i = start; i < static_cast<int>(recent_.size()); ++i) {
        out.push_back(recent_[static_cast<std::size_t>(i)]);
    }
    return out;
}

} // namespace nitro
