#pragma once

#include <QString>
#include <QVariantList>

namespace nitro {

/// Windows display / window enumeration helpers (no frame capture yet).
namespace win_capture {

struct MonitorInfo {
    int index = 0;
    QString name;
    int width = 0;
    int height = 0;
    int x = 0;
    int y = 0;
    bool primary = false;
};

struct WindowInfo {
    qulonglong hwnd = 0;
    QString title;
    quint32 processId = 0;
    QString processName;
    bool visible = false;
    bool minimized = false;
};

QVariantList enumerateMonitors();
QVariantList enumerateWindows();

QString preferredCaptureApi(); // "WGC" on Win10 1803+, else "DXGI"

} // namespace win_capture
} // namespace nitro
