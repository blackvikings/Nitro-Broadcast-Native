# Audio Engine

## Internal format

| Property | Value |
|----------|-------|
| Sample rate | 48 kHz (engine bus) |
| Channels | Stereo interleaved |
| Sample type | float32 |
| Block size | 480 frames (10 ms) |

## Ownership

`NitroEngine` owns the single `AudioEngine` + `AudioDeviceManager` + `AudioMixer`.
`NitroApplication` exposes them to QML; it does not construct a second engine.

## Signal chain

```
WASAPI device
  → WasapiCapture thread
  → AudioResampler (stateful; future: libswresample)
  → AudioRingBuffer (SPSC — producer never advances readPos)
  → AudioEngine process thread (data-driven wake + 5 ms timeout)
       → Gain → Channel Meter (post-gain, pre-mute) → Mute → Sum
       → Master Gain → Master Meter (clip detect, NO hard clip)
       → IAudioOutput (Null monitor for now)
```

## Scheduling model

The process loop waits on a `condition_variable` woken by capture `dataReady`
callbacks. A short timeout produces silence blocks so the UI meters stay live
and latency stays bounded. There is no busy-wait and no UI-thread audio work.

## Ring buffer

- Bounded SPSC float frames
- Overrun: drop **new** samples, increment `overrunCount` (readPos untouched)
- Underrun: short read, increment `underrunCount`
- No mutex / no alloc on the realtime path after `reset()`

## Hotplug

`IMMNotificationClient` updates the device list without polling.
Connection states exposed: Connected / Disconnected / Reconnecting.
Device removal does not crash the app — captures are restarted safely.

## Master headroom

Master path no longer hard-clamps to [-1, 1]. Clip is detected and exposed
(`masterClipped` / `clipDetected`). A true limiter is a future DSP module.
