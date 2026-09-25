#pragma once

#include "nitro/sources/WinCaptureEnum.hpp"

#include <QObject>
#include <QVariantList>

namespace nitro {

/// QML-facing helpers for display/window pickers (enumeration only — no frames).
class CaptureDeviceHelper : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString preferredApi READ preferredApi CONSTANT)

public:
    explicit CaptureDeviceHelper(QObject* parent = nullptr)
        : QObject(parent)
    {
    }

    QString preferredApi() const { return win_capture::preferredCaptureApi(); }

    Q_INVOKABLE QVariantList monitors() const { return win_capture::enumerateMonitors(); }
    Q_INVOKABLE QVariantList windows() const { return win_capture::enumerateWindows(); }
};

} // namespace nitro
