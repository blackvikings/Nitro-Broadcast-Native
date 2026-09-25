# Media pipeline strategies

## Module boundaries (FFmpeg)

| Directory | Role |
|-----------|------|
| `include/nitro/ffmpeg/` | FFmpegManager + module boundary stubs |
| `encoder/` (future) | Encode sessions — no UI includes |
| `recording/` (future) | File mux/recording |
| `streaming/` (future) | RTMP/HLS etc. |

Scene/UI modules must not include `libav*` headers. Prepare for:
`libavformat`, `libavcodec`, `libavutil`, `libswscale`, `libswresample`.

## Rendering boundary

`IVideoFrame` / `ICompositor` / `PreviewRenderer` / `ProgramRenderer`
live under `include/nitro/render/`. QML only displays composed surfaces;
frame processing stays in C++.

## 10. FFmpeg integration strategy

| Approach | Performance | Complexity | Stability | Debug | HW encode | Scale |
|----------|-------------|------------|-----------|-------|-----------|-------|
| Subprocess CLI | Weak for multi-output | Lowest | Process isolation | Easy | Via builds | Poor control |
| Libraries (`libav*`) | Best | Highest | Shared process | Harder | Direct / HW devices | Best |
| **Hybrid** | Best practical | Medium | Engine-contained | Medium | Vendor + FFmpeg | Best |

### Decision: Hybrid (libraries in-process inside the media engine)

- Use **libavformat / libavcodec / libavutil / libswscale / libswresample** for mux, software encode, resample/scale, and RTMP(S) where appropriate.  
- Use **NVENC / AMF / Quick Sync** via vendor SDKs or FFmpeg HW devices with **runtime capability detection** (never NVIDIA-only hard-coding).  
- Optional CLI FFmpeg only for diagnostics — not the hot path.  
- Prefer shared LGPL FFmpeg builds; document redistribution; decide GPL posture before shipping GPL-only codecs.

Do **not** put FFmpeg calls in QML.

## 11. Windows capture strategy

| Use case | Primary | Fallback |
|----------|---------|----------|
| Screen / display | **Windows Graphics Capture (WGC)** | DXGI Desktop Duplication |
| Window | **WGC** | Legacy GDI/BitBlt only if unavoidable |
| Webcam | **Media Foundation** | DirectShow |
| Microphone | **WASAPI** capture | — |
| Desktop audio | **WASAPI loopback** | — |

All implementations live under `platform/windows` and expose portable `ICaptureSource` / audio device interfaces. Core engine must not `#include` WinRT/WASAPI headers.

## 12. Audio architecture

```
WASAPI devices
  → capture rings
  → per-input: gain, mute, (future: NS, compressor, limiter, EQ)
  → mixer → master bus
  → optional monitor sink
  → encoder (e.g. AAC)
  → meters (peak + RMS) → UI ~30–60 Hz
```

Requirements:

- Multiple input and output devices  
- Sample-rate conversion and buffer management  
- A/V synchronization (audio master clock)  
- Named UI channels (Mic, Desktop, Game, Music, Browser) map to buses/inputs — not hard-wired hardware IDs  

## 13. Video architecture

```
Capture
  → Frame queue (bounded pool)
  → Scene composition
  → Scale / crop / filters
  → Frame sync
  → Hardware or software encoder
  → Recording and/or streaming outputs
       ↳ Preview/Program taps for UI (not the encode path itself)
```

Resolutions: 720p, 1080p, 1440p, 4K.  
Frame rates: 30, 60 (architecture allows higher where hardware permits).  
Color: Rec.709 first; HDR later.

## 14. Streaming and recording architecture

### Destinations

```
IDestination
  ├── YouTubeDestination
  ├── TwitchDestination
  ├── FacebookDestination
  └── CustomRTMPDestination
```

Each: connect, disconnect, start, stop, status, errors, reconnect.

`StreamingEngine` owns transport; destinations supply ingest endpoints and platform metadata APIs. Multi-destination: start with shared encode + multiple RTMP outputs when compatible; document per-destination encode as a later option.

Health: bitrate, FPS, dropped frames, connection state (Idle / Connecting / Live / Reconnecting / Error).

### Recording

- Local MP4 / MKV  
- Separate recording profiles vs stream settings  
- Multiple audio tracks where container/encoder allow  
- Replay buffer + save replay  

### Profiles (config)

Examples: Gaming 1080p60, Gaming 1440p60, Vertical, Recording, Streaming, Low Bandwidth.

A profile contains: resolution, FPS, encoder, bitrate, audio setup, destinations, scene collection reference.
