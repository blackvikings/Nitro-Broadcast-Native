# Architecture

## 1. System overview

```
┌──────────────────────────── NitroBroadcast.exe ─────────────────────────────┐
│                                                                             │
│  Qt Quick / QML UI                                                          │
│       │ commands / property bindings          preview · metrics · events ▲  │
│       ▼                                                                  │  │
│  Application Core (C++)  ─────────── Qt models / signals ────────────────┘  │
│  Session · Profiles · Scenes · Sources · Destinations · Hotkeys · Auth UI   │
│       │                                                                     │
│       │  IMediaEngine (commands / events)                                   │
│       ▼                                                                     │
│  ┌────────────────────── Media Engine (C++ module) ───────────────────────┐ │
│  │ Devices · Capture · Audio · Video · Compositor · Encoder               │ │
│  │ Recording · Streaming · Clock · Frame pools · Metrics                  │ │
│  └────────────────────────────────────────────────────────────────────────┘ │
│                                                                             │
│  platform/windows  →  WGC · DXGI · MF · WASAPI · Win32 · DPAPI              │
│  adapters/ffmpeg   →  libav* · HW device wrappers                           │
└─────────────────────────────────────────────────────────────────────────────┘
```

### Layering rules

| Layer | May use | Must not contain |
|-------|---------|------------------|
| QML | View-models / models only | FFmpeg, capture SDKs, encode, mux |
| Application Core | Engine interfaces, config, Qt | Vendor SDK headers |
| Media Engine | Platform + FFmpeg adapters | QML, OAuth UI, dialogs |
| Platform / Adapters | OS and vendor APIs | Product/business policy |

**UI sends commands and receives state. C++ owns all media.**

## 2. Crash isolation

| Model | Performance | Stability | Isolation | IPC | Debug |
|-------|-------------|-----------|-----------|-----|-------|
| Single process (UI + engine threads) | Best | Crash kills app | None | None | Easiest |
| Separate `nitro-engine` process | Slightly lower | UI can survive | Strong | Named pipe / SHM | Harder |

**Recommendation**

- **Phase 1–2:** Single process with a hard `IMediaEngine` boundary and disciplined threading.  
- **Phase 3+:** Optional out-of-process engine behind the **same** interface if crash isolation becomes required.

Do not pay IPC cost before the pipeline exists. Design interfaces so extraction is possible later.

## 3. C++ module boundaries

| Module | Responsibility |
|--------|----------------|
| `core` | IDs, `Result`/`Error`, clock, `VideoFrame` / `AudioBuffer`, non-owning views |
| `app` | Session lifecycle, wires UI ↔ engine |
| `scenes` | Scene objects, ordering, serialization |
| `sources` | Source descriptors, transforms, visibility, lock |
| `capture` | Capture source runtime (`ICaptureSource`) |
| `audio` | Inputs, mix, meters, monitor, resample |
| `video` | Color, scale, crop, filters |
| `compositor` | Scene → Preview/Program frames; transitions |
| `encoder` | Soft + HW encoders; capability probe |
| `recording` | File recording + replay buffer |
| `streaming` | Output sessions, reconnect |
| `destinations` | `IDestination` implementations |
| `devices` | Enumeration + hotplug |
| `hotkeys` | Global / app hotkeys |
| `config` | Profiles, scene collections, settings store |
| `diagnostics` | Logging (redacted), metrics |
| `platform/windows` | OS implementations of portable interfaces |
| `ui` | QObject models and preview item for QML |

**Dependency direction:** `ui → app → scenes/sources/config → engine modules → platform/adapters`.

## 4. Media engine interfaces (design)

Conceptual API (not implemented in this phase):

```cpp
class IMediaEngine {
  virtual Result initialize(const EngineConfig&) = 0;
  virtual void shutdown() = 0;
  virtual EngineCapabilities capabilities() const = 0;
  virtual EngineStatus status() const = 0;
};

class ICaptureSource {
  virtual Result start() = 0;
  virtual void stop() = 0;
  virtual Result update(const CaptureSettings&) = 0;
};

class ICompositor {
  virtual void setPreviewScene(SceneId) = 0;
  virtual void setProgramScene(SceneId) = 0;
  virtual Result transition(TransitionType, Duration) = 0;
};

class IAudioEngine {
  virtual Result addInput(AudioInputId, DeviceId) = 0;
  virtual void setGain(AudioInputId, float linearOrDb) = 0;
  virtual void setMute(AudioInputId, bool) = 0;
  virtual void setMonitor(AudioInputId, MonitorMode) = 0;
  virtual MeterSnapshot meters() const = 0;
};

class IEncoder {
  virtual Result open(const EncoderConfig&) = 0;
  virtual void close() = 0;
};

class IRecordingEngine {
  virtual Result start(const RecordConfig&) = 0;
  virtual Result stop() = 0;
  virtual Result startReplayBuffer(const ReplayConfig&) = 0;
  virtual Result saveReplay() = 0;
};

class IStreamingEngine {
  virtual Result createOutput(OutputId, const OutputConfig&) = 0;
  virtual Result startOutput(OutputId) = 0;
  virtual Result stopOutput(OutputId) = 0;
  virtual Result reconnectOutput(OutputId) = 0;
  virtual OutputStatus status(OutputId) const = 0;
};

class IDestination {
  virtual Result connect() = 0;
  virtual Result disconnect() = 0;
  virtual Result start() = 0;
  virtual Result stop() = 0;
  virtual DestinationStatus status() const = 0;
};
```

### Ownership and lifecycle

- Engine owns runtime frames, device handles, encoder sessions.  
- App owns scene/profile documents.  
- UI owns no media buffers.  
- Prefer `std::unique_ptr` / `std::shared_ptr` with clear owners; no raw owning pointers.  
- Errors: explicit `Result` / `std::expected`-style across thread boundaries; no exception-driven media path.  
- Validate every command originating from UI (ranges, paths, enums).

## 5. Threading model

```
Qt UI thread
    │ async command queue
Engine control thread
    ├── Capture (WGC/MF callbacks or pump threads)
    ├── Audio realtime (WASAPI) → lock-free rings
    ├── Compose thread (GPU preferred)
    ├── Encoder thread(s)
    └── Output thread(s) (file / RTMP)
```

Rules:

- Never block the UI thread on encode or capture.  
- Bounded frame queues; on overflow drop oldest and count drops.  
- Audio callback: no blocking locks, no heap allocation.  
- Frame/audio buffer pools (RAII).  
- Shutdown: stop outputs → encoders → compose → capture → audio → join with timeout.  
- A/V sync: audio as master clock; video drift correction (drop/dupe as needed).

## 6. Security baseline

- Never log OAuth tokens or stream keys.  
- Secrets in OS secure storage (e.g. DPAPI / credential store), not plain JSON.  
- No arbitrary process execution from UI strings.  
- Browser/overlay sources run sandboxed; untrusted content.  
- Canonicalize and restrict file paths for recording/assets.  
- Allowlist external URLs for account login.  
- Prefer FFmpeg **library** APIs over shelling out with user-built CLI strings.

## 7. Performance principles

- Avoid unnecessary frame copies; prefer GPU textures where practical.  
- No unbounded queues.  
- No global mutable media state.  
- Profile before optimizing.  
- Measure: CPU, GPU, RAM, latency, drops, encode latency, audio RTL, startup, stream/record stability at 720p30 → 4K60.
