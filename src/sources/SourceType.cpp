#include "nitro/sources/SourceType.hpp"

namespace nitro {

QString sourceTypeToString(SourceType type)
{
    switch (type) {
    case SourceType::DisplayCapture: return QStringLiteral("display");
    case SourceType::WindowCapture: return QStringLiteral("window");
    case SourceType::GameCapture: return QStringLiteral("game");
    case SourceType::ApplicationCapture: return QStringLiteral("application");
    case SourceType::Camera: return QStringLiteral("camera");
    case SourceType::MediaSource: return QStringLiteral("media");
    case SourceType::VlcSource: return QStringLiteral("vlc");
    case SourceType::Browser: return QStringLiteral("browser");
    case SourceType::AudioInput: return QStringLiteral("audio_input");
    case SourceType::AudioOutput: return QStringLiteral("audio_output");
    case SourceType::ApplicationAudio: return QStringLiteral("application_audio");
    case SourceType::Image: return QStringLiteral("image");
    case SourceType::ImageSlideshow: return QStringLiteral("image_slideshow");
    case SourceType::Color: return QStringLiteral("color");
    case SourceType::Text: return QStringLiteral("text");
    case SourceType::AnimatedMedia: return QStringLiteral("animated_media");
    case SourceType::Scene: return QStringLiteral("scene");
    case SourceType::Group: return QStringLiteral("group");
    case SourceType::Ndi: return QStringLiteral("ndi");
    case SourceType::DeckLink: return QStringLiteral("decklink");
    case SourceType::Plugin: return QStringLiteral("plugin");
    }
    return QStringLiteral("display");
}

SourceType sourceTypeFromString(const QString& value)
{
    // Legacy aliases
    if (value == QLatin1String("microphone")) return SourceType::AudioInput;
    if (value == QLatin1String("desktop_audio")) return SourceType::AudioOutput;

    if (value == QLatin1String("window")) return SourceType::WindowCapture;
    if (value == QLatin1String("game")) return SourceType::GameCapture;
    if (value == QLatin1String("application")) return SourceType::ApplicationCapture;
    if (value == QLatin1String("camera")) return SourceType::Camera;
    if (value == QLatin1String("media")) return SourceType::MediaSource;
    if (value == QLatin1String("vlc")) return SourceType::VlcSource;
    if (value == QLatin1String("browser")) return SourceType::Browser;
    if (value == QLatin1String("audio_input")) return SourceType::AudioInput;
    if (value == QLatin1String("audio_output")) return SourceType::AudioOutput;
    if (value == QLatin1String("application_audio")) return SourceType::ApplicationAudio;
    if (value == QLatin1String("image")) return SourceType::Image;
    if (value == QLatin1String("image_slideshow")) return SourceType::ImageSlideshow;
    if (value == QLatin1String("color")) return SourceType::Color;
    if (value == QLatin1String("text")) return SourceType::Text;
    if (value == QLatin1String("animated_media")) return SourceType::AnimatedMedia;
    if (value == QLatin1String("scene")) return SourceType::Scene;
    if (value == QLatin1String("group")) return SourceType::Group;
    if (value == QLatin1String("ndi")) return SourceType::Ndi;
    if (value == QLatin1String("decklink")) return SourceType::DeckLink;
    if (value == QLatin1String("plugin")) return SourceType::Plugin;
    return SourceType::DisplayCapture;
}

QString sourceTypeDisplayName(SourceType type)
{
    switch (type) {
    case SourceType::DisplayCapture: return QStringLiteral("Display Capture");
    case SourceType::WindowCapture: return QStringLiteral("Window Capture");
    case SourceType::GameCapture: return QStringLiteral("Game Capture");
    case SourceType::ApplicationCapture: return QStringLiteral("Application Capture");
    case SourceType::Camera: return QStringLiteral("Camera");
    case SourceType::MediaSource: return QStringLiteral("Media Source");
    case SourceType::VlcSource: return QStringLiteral("VLC / Playlist");
    case SourceType::Browser: return QStringLiteral("Browser");
    case SourceType::AudioInput: return QStringLiteral("Audio Input Capture");
    case SourceType::AudioOutput: return QStringLiteral("Audio Output Capture");
    case SourceType::ApplicationAudio: return QStringLiteral("Application Audio Capture");
    case SourceType::Image: return QStringLiteral("Image");
    case SourceType::ImageSlideshow: return QStringLiteral("Image Slideshow");
    case SourceType::Color: return QStringLiteral("Color Source");
    case SourceType::Text: return QStringLiteral("Text");
    case SourceType::AnimatedMedia: return QStringLiteral("Animated Media");
    case SourceType::Scene: return QStringLiteral("Scene");
    case SourceType::Group: return QStringLiteral("Group");
    case SourceType::Ndi: return QStringLiteral("NDI Source");
    case SourceType::DeckLink: return QStringLiteral("DeckLink Capture");
    case SourceType::Plugin: return QStringLiteral("Plugin Source");
    }
    return QStringLiteral("Source");
}

QString sourceCategoryName(SourceCategory category)
{
    switch (category) {
    case SourceCategory::Video: return QStringLiteral("VIDEO");
    case SourceCategory::Audio: return QStringLiteral("AUDIO");
    case SourceCategory::Graphics: return QStringLiteral("GRAPHICS");
    case SourceCategory::Composition: return QStringLiteral("COMPOSITION");
    case SourceCategory::Advanced: return QStringLiteral("ADVANCED");
    }
    return QStringLiteral("OTHER");
}

SourceCategory sourceCategoryFor(SourceType type)
{
    switch (type) {
    case SourceType::DisplayCapture:
    case SourceType::WindowCapture:
    case SourceType::GameCapture:
    case SourceType::ApplicationCapture:
    case SourceType::Camera:
    case SourceType::MediaSource:
    case SourceType::VlcSource:
    case SourceType::Browser:
        return SourceCategory::Video;
    case SourceType::AudioInput:
    case SourceType::AudioOutput:
    case SourceType::ApplicationAudio:
        return SourceCategory::Audio;
    case SourceType::Image:
    case SourceType::ImageSlideshow:
    case SourceType::Color:
    case SourceType::Text:
    case SourceType::AnimatedMedia:
        return SourceCategory::Graphics;
    case SourceType::Scene:
    case SourceType::Group:
        return SourceCategory::Composition;
    case SourceType::Ndi:
    case SourceType::DeckLink:
    case SourceType::Plugin:
        return SourceCategory::Advanced;
    }
    return SourceCategory::Video;
}

QString sourceCapabilityToString(SourceCapability c)
{
    switch (c) {
    case SourceCapability::Available: return QStringLiteral("Available");
    case SourceCapability::Experimental: return QStringLiteral("Experimental");
    case SourceCapability::NotAvailable: return QStringLiteral("Not Available");
    case SourceCapability::NotImplemented: return QStringLiteral("Not Implemented");
    }
    return QStringLiteral("Unknown");
}

} // namespace nitro
