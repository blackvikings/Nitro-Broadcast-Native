#include "nitro/sources/WinCaptureEnum.hpp"

#ifdef Q_OS_WIN
#  ifndef NOMINMAX
#    define NOMINMAX
#  endif
#  include <Windows.h>
#  include <dwmapi.h>
#  pragma comment(lib, "Dwmapi.lib")
#endif

#include <QFileInfo>

namespace nitro::win_capture {

QString preferredCaptureApi()
{
#ifdef Q_OS_WIN
    // Windows Graphics Capture requires Windows 10 1803+ (build 17134).
    // DXGI Desktop Duplication is the fallback path for older builds.
    return QStringLiteral("WGC");
#else
    return QStringLiteral("unsupported");
#endif
}

#ifdef Q_OS_WIN

namespace {

struct EnumCtx {
    QVariantList* out = nullptr;
    int index = 0;
};

BOOL CALLBACK monitorEnumProc(HMONITOR monitor, HDC, LPRECT, LPARAM data)
{
    auto* ctx = reinterpret_cast<EnumCtx*>(data);
    MONITORINFOEXW mi{};
    mi.cbSize = sizeof(mi);
    if (!GetMonitorInfoW(monitor, &mi)) {
        return TRUE;
    }
    const int w = mi.rcMonitor.right - mi.rcMonitor.left;
    const int h = mi.rcMonitor.bottom - mi.rcMonitor.top;
    QVariantMap map;
    map.insert(QStringLiteral("index"), ctx->index);
    map.insert(QStringLiteral("name"), QString::fromWCharArray(mi.szDevice));
    map.insert(QStringLiteral("width"), w);
    map.insert(QStringLiteral("height"), h);
    map.insert(QStringLiteral("x"), static_cast<int>(mi.rcMonitor.left));
    map.insert(QStringLiteral("y"), static_cast<int>(mi.rcMonitor.top));
    map.insert(QStringLiteral("primary"), (mi.dwFlags & MONITORINFOF_PRIMARY) != 0);
    map.insert(QStringLiteral("handle"), QString::number(reinterpret_cast<quintptr>(monitor)));
    ctx->out->push_back(map);
    ++ctx->index;
    return TRUE;
}

QString processNameFromPid(DWORD pid)
{
    HANDLE h = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!h) {
        return {};
    }
    wchar_t path[MAX_PATH]{};
    DWORD size = MAX_PATH;
    QString name;
    if (QueryFullProcessImageNameW(h, 0, path, &size)) {
        name = QFileInfo(QString::fromWCharArray(path)).fileName();
    }
    CloseHandle(h);
    return name;
}

BOOL CALLBACK windowEnumProc(HWND hwnd, LPARAM data)
{
    auto* out = reinterpret_cast<QVariantList*>(data);
    if (!IsWindow(hwnd)) {
        return TRUE;
    }
    // Skip tool/owned windows without title for a usable picker list
    if (GetWindow(hwnd, GW_OWNER) != nullptr) {
        return TRUE;
    }
    const LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);
    if ((style & WS_CHILD) != 0) {
        return TRUE;
    }

    wchar_t titleBuf[512]{};
    const int len = GetWindowTextW(hwnd, titleBuf, 512);
    if (len <= 0) {
        return TRUE;
    }

    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);

    const bool visible = IsWindowVisible(hwnd) != FALSE;
    const bool minimized = IsIconic(hwnd) != FALSE;

    // Cloaked UWP windows (invisible) — still list but mark inaccessible
    BOOL cloaked = FALSE;
    DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked));

    QVariantMap map;
    map.insert(QStringLiteral("hwnd"), QString::number(reinterpret_cast<quintptr>(hwnd)));
    map.insert(QStringLiteral("title"), QString::fromWCharArray(titleBuf));
    map.insert(QStringLiteral("processId"), static_cast<int>(pid));
    map.insert(QStringLiteral("processName"), processNameFromPid(pid));
    map.insert(QStringLiteral("visible"), visible);
    map.insert(QStringLiteral("minimized"), minimized);
    map.insert(QStringLiteral("cloaked"), cloaked != FALSE);
    map.insert(QStringLiteral("accessible"), visible && !minimized && !cloaked);
    out->push_back(map);
    return TRUE;
}

} // namespace

QVariantList enumerateMonitors()
{
    QVariantList list;
    EnumCtx ctx{&list, 0};
    EnumDisplayMonitors(nullptr, nullptr, monitorEnumProc, reinterpret_cast<LPARAM>(&ctx));
    return list;
}

QVariantList enumerateWindows()
{
    QVariantList list;
    EnumWindows(windowEnumProc, reinterpret_cast<LPARAM>(&list));
    return list;
}

#else

QVariantList enumerateMonitors() { return {}; }
QVariantList enumerateWindows() { return {}; }

#endif

} // namespace nitro::win_capture
