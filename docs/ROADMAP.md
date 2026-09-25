# Features checklist and first tasks

## 15. Migration / features checklist (old → native)

| Feature | Electron | Native target | Priority |
|---------|----------|---------------|----------|
| Scenes add/delete | Yes | + rename/dup/reorder/save | P0 |
| Preview / Program + Cut | Yes | + Fade | P0 |
| Display / Window / Camera | Chromium | WGC / MF | P0 |
| Mic + Desktop audio | Yes | WASAPI + mixer | P0 |
| Mixer gain / mute / monitor / meters | Partial | Peak + RMS | P0 |
| Destinations UI | Yes | Native + real outputs | P0 |
| Real RTMP streaming | **No** | Yes | P0 |
| Recording | **No** | Yes | P0 |
| Settings res / FPS / theme | Partial | Profiles + encoder | P0 |
| HW encoder detection | No | NVENC / AMF / QSV + software | P0 |
| Image / Text / Browser sources | Stub | Real | P1 |
| OAuth / accounts | Fragile | Secure storage + browser | P1 |
| Replay buffer | No | Yes | P1 |
| Hotkeys | No | Yes | P1 |
| Vertical canvas | Yes | Profiles | P1 |
| Chat | Placeholder | Platform chat | P2 |
| Alerts / overlays advanced | No | Later | P2 |
| AV1 | No | Future | P3 |
| Electron / JS stack | Yes | **Removed** | — |

## 16. First 10 implementation tasks

> Do **not** start these until the design package is approved.

| ID | Task | Notes |
|----|------|-------|
| **NB-001** | CMake skeleton | `nitro_core` + `NitroBroadcast` exe, presets, README build section |
| **NB-002** | Qt 6 Quick app shell | Empty window, dark `Theme` singleton, `qml.qrc` |
| **NB-003** | Design system components | Buttons, panels, list rows, meters, status dots |
| **NB-004** | Main Studio layout | Top bar, scenes, Preview/Program placeholders, sources, mixer, status, Stream/Record |
| **NB-005** | C++ list models + mock data | Default scenes Starting Soon…Ending; mock mixer channels |
| **NB-006** | `IMediaEngine` + `NullMediaEngine` | Fake capabilities/metrics; proves UI↔core boundary |
| **NB-007** | Profile schema load/save | JSON profile with no media |
| **NB-008** | Diagnostics logger | Secret redaction; version/about |
| **NB-009** | `docs/FFMPEG.md` tooling notes | vcpkg integration plan still **without** encode |
| **NB-010** | Unit tests | Scene collection serialize + transform math |

### Explicitly deferred (not in first 10)

- Capture prototypes  
- FFmpeg encode / RTMP  
- Recording / replay  
- Authentication / OAuth  

## Progressive integration (after first 10)

1. First WGC capture prototype  
2. Native video frame path + compositor stub  
3. WASAPI audio + meters  
4. Encoder capability probe + first encode  
5. Recording  
6. Streaming  
7. Destinations + health  
8. Replace placeholders; keep app usable throughout  

## Approval gate

Approve or adjust before NB-001:

1. Qt Quick (not Widgets)  
2. Single-process engine first (isolatable later)  
3. vcpkg for Qt/FFmpeg  
4. Design tokens / accent (`#5B8CFF`)  
5. Task order NB-001…NB-010  
