# Nitro Broadcast (Native)

Professional native desktop broadcasting application built with **C++20**, **Qt 6 Quick/QML**, and **CMake**.

Electron is **not** used. The old Electron project is a product reference only and is not part of this build.

## Status

Architecture hardening pass (v0.1.1):

- **Single NitroEngine ownership** of AudioEngine / devices / mixer / FFmpeg stubs / compositor / transitions
- Real **WASAPI** mic + desktop loopback (no duplicate AudioEngine)
- SPSC **AudioRingBuffer**, stateful **AudioResampler**, data-driven process loop
- Source factory + capability states, typed source configs
- Resizable SplitView studio layout, vector **NitroIcon** (no emoji)
- Metrics marked `metricsSimulated = true` until real counters exist
- Windows GitHub Actions CI workflow

Not yet: WGC frame capture, encode/stream, game capture, NDI/DeckLink.

## Requirements

| Tool | Version |
|------|---------|
| CMake | ≥ 3.21 (4.x recommended) |
| Qt | 6.5+ with Quick, Qml, QuickControls2, Test |
| Compiler | MSVC 2022/2026 (x64) with C++20 |
| Ninja | Recommended (or Visual Studio generator) |
| OS | Windows 10/11 x64 |

### Install Qt (recommended: aqtinstall)

```powershell
py -3 -m pip install aqtinstall
py -3 -m aqt install-qt windows desktop 6.8.3 win64_msvc2022_64 -O C:\Qt
```

Set prefix path:

```powershell
$env:CMAKE_PREFIX_PATH = "C:\Qt\6.8.3\msvc2022_64"
```

Modules needed at minimum: `qtbase`, `qtdeclarative` (includes Quick/QML).

### Install CMake

```powershell
winget install Kitware.CMake
```

## Configure

From a **x64 Native Tools** / VS developer environment (or after `vcvars64.bat`):

```powershell
cd NitroBroadcast
$env:CMAKE_PREFIX_PATH = "C:\Qt\6.8.3\msvc2022_64"
cmake --preset windows-msvc-debug
```

Visual Studio generator alternative:

```powershell
cmake --preset windows-vs2022 -DCMAKE_PREFIX_PATH="C:\Qt\6.8.3\msvc2022_64"
```

## Build

```powershell
cmake --build --preset windows-msvc-debug
cmake --build --preset windows-msvc-release
```

Or for the VS preset:

```powershell
cmake --build --preset windows-vs2022-debug
cmake --build --preset windows-vs2022-release
```

## Run

```powershell
# After Debug build (Ninja preset):
.\build\windows-msvc-debug\NitroBroadcast.exe
```

Deploy Qt DLLs if needed:

```powershell
& "C:\Qt\6.8.3\msvc2022_64\bin\windeployqt.exe" --qmldir .\qml .\build\windows-msvc-debug\NitroBroadcast.exe
```

## Tests

```powershell
ctest --preset windows-msvc-debug
# or (with Qt bin on PATH):
.\build\windows-msvc-debug\nitro_tests.exe
```

## Architecture docs

See `docs/` for the full native architecture package.

## Not implemented yet

Real screen/window/camera/mic capture, FFmpeg encode/stream, OAuth, multi-destination RTMP, hardware encoders.
