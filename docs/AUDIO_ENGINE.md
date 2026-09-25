#pragma once

/**
 * Nitro Broadcast — Audio Engine Architecture
 *
 * Internal format
 * ---------------
 * Sample rate : 48 kHz (engine); device rates are resampled linearly
 * Channels    : Stereo interleaved
 * Sample type : float32 (-1..+1)
 * Block size  : 480 frames (10 ms)
 *
 * Signal chain
 * ------------
 * Physical Device (WASAPI)
 *   → Capture thread (WasapiCapture)
 *   → AudioRingBuffer (SPSC, overwrite-oldest on overrun)
 *   → Process thread (AudioEngine::processLoop)
 *       → Gain (-60 dB .. +12 dB)
 *       → Channel Meter (Peak / RMS / Peak-hold)  ← UI reads this
 *       → Mute (silence into mix; meter still shows pre-mute)
 *       → Sum → Master Gain → Master Mute → Master Meter
 *       → IAudioOutput (Null for now; future monitor/record/stream)
 *
 * Channel kinds
 * -------------
 * MIC      : WASAPI capture (input device)
 * DESKTOP  : WASAPI loopback (render device)
 * GAME / MUSIC / BROWSER : UI strips without a physical source yet
 *               (meters stay at silence until a source is attached)
 *
 * Mute behaviour
 * --------------
 * Pre-mute metering (broadcast style): muted channels still show level,
 * but contribute silence to the master bus.
 *
 * Threading
 * ---------
 * - Capture: one OS thread per WasapiCapture (MMCSS "Pro Audio")
 * - Mix: dedicated process thread (~10 ms cadence)
 * - UI: QTimer ~30 Hz copies atomic meter snapshots into QML models
 * Real-time paths avoid heap allocation and Qt calls.
 */
