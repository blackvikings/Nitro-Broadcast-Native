#pragma once

#include <QString>

namespace nitro {

enum class SourceType {
    // Video / Capture
    DisplayCapture,
    WindowCapture,
    GameCapture,
    ApplicationCapture,
    Camera,
    MediaSource,
    VlcSource,
    Browser,
    // Audio
    AudioInput,
    AudioOutput,
    ApplicationAudio,
    // Graphics
    Image,
    ImageSlideshow,
    Color,
    Text,
    AnimatedMedia,
    // Composition
    Scene,
    Group,
    // Advanced
    Ndi,
    DeckLink,
    Plugin
};

enum class SourceCategory {
    Video,
    Audio,
    Graphics,
    Composition,
    Advanced
};

enum class SourceCapability {
    Available,       // Usable now (config and/or runtime)
    Experimental,    // Partial / foundation only
    NotAvailable,    // Missing optional dependency (NDI/VLC/DeckLink)
    NotImplemented   // Planned, not built yet
};

QString sourceTypeToString(SourceType type);
SourceType sourceTypeFromString(const QString& value);
QString sourceTypeDisplayName(SourceType type);
QString sourceCategoryName(SourceCategory category);
SourceCategory sourceCategoryFor(SourceType type);
QString sourceCapabilityToString(SourceCapability c);

} // namespace nitro
