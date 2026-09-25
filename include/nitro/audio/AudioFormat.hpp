#pragma once

#include <cstdint>
#include <string>

namespace nitro {

enum class AudioSampleFormat {
    Float32,
    Int16,
    Unknown
};

struct AudioFormat {
    int sampleRate = 48000;
    int channels = 2;
    AudioSampleFormat sampleFormat = AudioSampleFormat::Float32;
    int framesPerBuffer = 480; // 10 ms @ 48 kHz

    int bytesPerSample() const
    {
        switch (sampleFormat) {
        case AudioSampleFormat::Float32: return 4;
        case AudioSampleFormat::Int16: return 2;
        default: return 0;
        }
    }

    int bytesPerFrame() const { return bytesPerSample() * channels; }
};

/// Internal engine format: 48 kHz, stereo, float32 interleaved.
inline AudioFormat engineAudioFormat()
{
    return AudioFormat{48000, 2, AudioSampleFormat::Float32, 480};
}

enum class AudioSourceKind {
    None,
    Microphone,      // WASAPI capture (input device)
    DesktopLoopback, // WASAPI loopback (playback device)
    Application,     // Future: app-specific capture
    MediaFile,       // Future
    Browser          // Future
};

inline const char* audioSourceKindName(AudioSourceKind kind)
{
    switch (kind) {
    case AudioSourceKind::None: return "None";
    case AudioSourceKind::Microphone: return "Microphone";
    case AudioSourceKind::DesktopLoopback: return "Desktop Loopback";
    case AudioSourceKind::Application: return "Application";
    case AudioSourceKind::MediaFile: return "Media File";
    case AudioSourceKind::Browser: return "Browser";
    }
    return "Unknown";
}

} // namespace nitro
