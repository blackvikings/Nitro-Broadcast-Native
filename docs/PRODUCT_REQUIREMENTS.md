# Product requirements (from Electron Nitro Broadcast)

**Source:** `blackvikings/Nitro-Broadcast-` (reference only).  
**Rule:** Extract product intent. Do **not** copy UI, Electron architecture, or `renderer.js` logic.

## Summary

The Electron app is a **studio UI prototype** with Chromium-based capture preview. Real encoding, RTMP streaming, and recording were **not** implemented. Dependencies `ffmpeg-static`, `fluent-ffmpeg`, and `axios` were declared but unused in source.

## Feature inventory

| Area | What existed | Gaps / native target |
|------|--------------|----------------------|
| **Scenes** | Named list; add/delete; Preview vs Program selection | Rename, duplicate, reorder, persistence, import/export |
| **Sources** | Display, Window, Camera, Mic, Desktop Audio | Image, Text, Browser were UI stubs only |
| **Preview / Program** | Dual viewports; Cut | Fade and full transition system |
| **Audio mixer** | Per-source gain, monitor, simple meters | Named buses, RMS, master, filters |
| **Capture** | `desktopCapturer` + `getUserMedia` | Replace with WGC / MF / WASAPI |
| **Destinations** | YouTube, Twitch, Facebook, Custom RTMP fields | Real connect/start/stop/reconnect |
| **Streaming** | Start/Stop toggled UI “live” state only | Real RTMP(S), health, multi-output |
| **Recording** | RECORD button with no handler | Local MP4/MKV, profiles, replay buffer |
| **Settings** | Resolution (incl. vertical), FPS 30/60, themes | Encoder, bitrate, audio, profiles |
| **Auth** | Browser OAuth + `nitro://` deep link | Secure storage; never log tokens |
| **Chat** | Placeholder panel | Platform chat integration |
| **Persistence** | None (in-memory) | Profiles + scene collections on disk |

## Product capabilities to preserve (conceptually)

1. Multi-scene studio with Preview editing and Program live path  
2. Cut from Preview → Program  
3. Multiple source types (expand stubs into real sources)  
4. Per-channel mixer with mute / monitor / meters  
5. Multi-destination streaming concept  
6. Resolution + FPS output settings including vertical canvases  
7. Account connection flow for platforms (redesign securely)  
8. Stream and record as primary actions  

## Explicitly out of scope for porting

- Electron main/preload/renderer split  
- Canvas 2D composition in the UI process  
- Web Audio as the production mix path  
- HTML/CSS theming model  
- `nitro://` deep-link token handling as currently implemented (insecure)
