# Project structure and CMake strategy

## 2. Recommended tree

```
NitroBroadcast/
├── CMakeLists.txt
├── CMakePresets.json
├── README.md
├── LICENSE
├── cmake/
│   ├── FindFFmpeg.cmake
│   └── NitroWarnings.cmake
├── docs/
├── include/nitro/
│   ├── core/
│   ├── app/
│   ├── scenes/
│   ├── sources/
│   ├── capture/
│   ├── audio/
│   ├── video/
│   ├── compositor/
│   ├── encoder/
│   ├── recording/
│   ├── streaming/
│   ├── destinations/
│   ├── devices/
│   ├── hotkeys/
│   ├── config/
│   ├── diagnostics/
│   └── platform/          # portable interfaces only
├── src/
│   ├── main.cpp
│   ├── app/
│   ├── core/
│   ├── ui/                # view-models, preview item
│   ├── scenes/
│   ├── sources/
│   ├── capture/
│   ├── audio/
│   ├── video/
│   ├── compositor/
│   ├── encoder/
│   ├── recording/
│   ├── streaming/
│   ├── destinations/
│   ├── devices/
│   ├── hotkeys/
│   ├── config/
│   ├── diagnostics/
│   └── platform/
│       └── windows/
├── adapters/
│   └── ffmpeg/
├── qml/
│   ├── Main.qml
│   ├── Theme/
│   ├── components/
│   ├── studio/
│   ├── scenes/
│   ├── sources/
│   ├── mixer/
│   ├── preview/
│   ├── destinations/
│   ├── settings/
│   └── dialogs/
├── resources/
│   ├── icons/
│   ├── fonts/
│   ├── themes/
│   └── qml.qrc
└── tests/
    ├── unit/
    └── integration/
```

### Improvements vs the original sketch

- `adapters/ffmpeg` kept out of `src/` so FFmpeg stays swappable.  
- `platform/windows` isolated under `src/platform`.  
- `qml/Theme` for the design system singleton.  
- `docs/` for architecture decisions.  
- Existing empty `NitroEngine` VS stub is **not** the source of truth; CMake generates IDE projects.

## 3. CMake strategy

| Item | Choice |
|------|--------|
| Language | C++20 |
| Generators | Ninja / Visual Studio via presets |
| Qt | Qt 6.5+ (`Quick`, `QuickControls2`, `Qml`, `Gui`, `Network`, `Concurrent`) |
| Deps | **vcpkg** preferred (Qt + FFmpeg), or documented system Qt |
| Targets | `nitro_core` (static/shared), `nitro_ui`, `NitroBroadcast` exe, `nitro_tests` |
| Options | `NITRO_ENABLE_NVENC`, `NITRO_ENABLE_AMF`, `NITRO_ENABLE_QSV`, `NITRO_WITH_FFMPEG` |
| Packaging | `windeployqt` + FFmpeg DLLs beside exe |
| Quality | Warnings-as-errors in CI; optional sanitizers in Debug |

Presets (planned): `windows-msvc-debug`, `windows-msvc-release`.

**Not started yet:** CMakeLists and targets will land in task **NB-001** after design approval.
