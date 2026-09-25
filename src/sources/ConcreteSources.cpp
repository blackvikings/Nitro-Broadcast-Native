#include "nitro/sources/ConcreteSources.hpp"

#include "nitro/sources/SourceFactory.hpp"
#include "nitro/sources/WinCaptureEnum.hpp"

namespace nitro {

namespace {

void applyDefaults(SourceBase& src, SourceType type)
{
    switch (type) {
    case SourceType::DisplayCapture:
        src.setProperty(QStringLiteral("monitorIndex"), 0);
        src.setProperty(QStringLiteral("captureCursor"), true);
        src.setProperty(QStringLiteral("fps"), 60);
        src.setProperty(QStringLiteral("api"), win_capture::preferredCaptureApi());
        break;
    case SourceType::WindowCapture:
        src.setProperty(QStringLiteral("windowTitle"), QString());
        src.setProperty(QStringLiteral("hwnd"), QString());
        src.setProperty(QStringLiteral("processId"), 0);
        src.setProperty(QStringLiteral("processName"), QString());
        src.setProperty(QStringLiteral("api"), win_capture::preferredCaptureApi());
        src.setProperty(QStringLiteral("captureState"), QStringLiteral("idle"));
        break;
    case SourceType::GameCapture:
        src.setProperty(QStringLiteral("processName"), QString());
        src.setProperty(QStringLiteral("hookMethod"), QStringLiteral("pending"));
        src.setProperty(QStringLiteral("dx11"), true);
        src.setProperty(QStringLiteral("dx12"), false);
        src.setProperty(QStringLiteral("vulkan"), false);
        break;
    case SourceType::ApplicationCapture:
        src.setProperty(QStringLiteral("processId"), 0);
        src.setProperty(QStringLiteral("processName"), QString());
        break;
    case SourceType::Camera:
        src.setProperty(QStringLiteral("deviceId"), QString());
        src.setProperty(QStringLiteral("resolution"), QStringLiteral("1280x720"));
        src.setProperty(QStringLiteral("fps"), 30);
        src.setProperty(QStringLiteral("format"), QStringLiteral("auto"));
        break;
    case SourceType::MediaSource:
        src.setProperty(QStringLiteral("filePath"), QString());
        src.setProperty(QStringLiteral("loop"), true);
        src.setProperty(QStringLiteral("restartOnActivate"), true);
        src.setProperty(QStringLiteral("autoStart"), false);
        src.setProperty(QStringLiteral("volume"), 1.0);
        src.setProperty(QStringLiteral("speed"), 1.0);
        src.setProperty(QStringLiteral("playing"), false);
        src.setProperty(QStringLiteral("positionMs"), 0);
        src.setAudioEnabled(true);
        break;
    case SourceType::VlcSource:
        src.setProperty(QStringLiteral("playlist"), QVariantList());
        src.setProperty(QStringLiteral("shuffle"), false);
        src.setProperty(QStringLiteral("loopPlaylist"), true);
        break;
    case SourceType::Browser:
        src.setProperty(QStringLiteral("url"), QStringLiteral("https://"));
        src.setProperty(QStringLiteral("width"), 1920);
        src.setProperty(QStringLiteral("height"), 1080);
        src.setProperty(QStringLiteral("fps"), 30);
        src.setProperty(QStringLiteral("sandbox"), true);
        src.setProperty(QStringLiteral("allowFileAccess"), false);
        src.setProperty(QStringLiteral("allowNativeBridge"), false);
        break;
    case SourceType::AudioInput:
        src.setProperty(QStringLiteral("deviceId"), QString());
        src.setProperty(QStringLiteral("gainDb"), 0.0);
        src.setProperty(QStringLiteral("mute"), false);
        src.setProperty(QStringLiteral("monitor"), false);
        src.setProperty(QStringLiteral("mixerChannel"), QStringLiteral("mic"));
        src.setAudioEnabled(true);
        src.transform().width = 0;
        src.transform().height = 0;
        break;
    case SourceType::AudioOutput:
        src.setProperty(QStringLiteral("deviceId"), QString());
        src.setProperty(QStringLiteral("gainDb"), 0.0);
        src.setProperty(QStringLiteral("mute"), false);
        src.setProperty(QStringLiteral("mixerChannel"), QStringLiteral("desktop"));
        src.setAudioEnabled(true);
        src.transform().width = 0;
        src.transform().height = 0;
        break;
    case SourceType::ApplicationAudio:
        src.setProperty(QStringLiteral("processId"), 0);
        src.setProperty(QStringLiteral("processName"), QString());
        src.setAudioEnabled(true);
        src.transform().width = 0;
        src.transform().height = 0;
        break;
    case SourceType::Image:
        src.setProperty(QStringLiteral("filePath"), QString());
        break;
    case SourceType::ImageSlideshow:
        src.setProperty(QStringLiteral("files"), QVariantList());
        src.setProperty(QStringLiteral("durationMs"), 5000);
        src.setProperty(QStringLiteral("loop"), true);
        src.setProperty(QStringLiteral("random"), false);
        src.setProperty(QStringLiteral("transition"), QStringLiteral("fade"));
        src.setProperty(QStringLiteral("paused"), false);
        src.setProperty(QStringLiteral("currentIndex"), 0);
        break;
    case SourceType::Color:
        src.setProperty(QStringLiteral("color"), QStringLiteral("#1C1F26"));
        src.transform().width = 1920;
        src.transform().height = 1080;
        break;
    case SourceType::Text:
        src.setProperty(QStringLiteral("content"), QStringLiteral("Nitro Broadcast"));
        src.setProperty(QStringLiteral("fontFamily"), QStringLiteral("Segoe UI"));
        src.setProperty(QStringLiteral("fontSize"), 48);
        src.setProperty(QStringLiteral("fontWeight"), QStringLiteral("Bold"));
        src.setProperty(QStringLiteral("color"), QStringLiteral("#E8EAED"));
        src.setProperty(QStringLiteral("alignment"), QStringLiteral("left"));
        src.setProperty(QStringLiteral("outline"), false);
        src.setProperty(QStringLiteral("outlineColor"), QStringLiteral("#000000"));
        src.setProperty(QStringLiteral("shadow"), false);
        src.setProperty(QStringLiteral("background"), false);
        src.setProperty(QStringLiteral("backgroundColor"), QStringLiteral("#00000000"));
        src.transform().width = 800;
        src.transform().height = 120;
        break;
    case SourceType::AnimatedMedia:
        src.setProperty(QStringLiteral("filePath"), QString());
        src.setProperty(QStringLiteral("loop"), true);
        src.setProperty(QStringLiteral("alphaSupported"), false); // honest: no fake alpha
        break;
    case SourceType::Scene:
        src.setProperty(QStringLiteral("sceneId"), QString());
        break;
    case SourceType::Group:
        src.setProperty(QStringLiteral("childIds"), QVariantList());
        break;
    case SourceType::Ndi:
        src.setProperty(QStringLiteral("sourceName"), QString());
        src.setProperty(QStringLiteral("sdkPresent"), false);
        break;
    case SourceType::DeckLink:
        src.setProperty(QStringLiteral("deviceIndex"), 0);
        src.setProperty(QStringLiteral("sdkPresent"), false);
        break;
    case SourceType::Plugin:
        src.setProperty(QStringLiteral("pluginId"), QString());
        break;
    }
}

template <typename T>
std::shared_ptr<T> makeTyped(SourceType type, const QString& name)
{
    auto src = std::make_shared<T>(name.isEmpty() ? SourceFactory::defaultName(type) : name);
    applyDefaults(*src, type);
    return src;
}

} // namespace

DisplayCaptureSource::DisplayCaptureSource(QString name)
    : SourceBase(SourceType::DisplayCapture,
                 name.isEmpty() ? SourceFactory::defaultName(SourceType::DisplayCapture) : std::move(name),
                 SourceFactory::capabilityFor(SourceType::DisplayCapture),
                 SourceFactory::capabilityNoteFor(SourceType::DisplayCapture))
{
}

QVariantList DisplayCaptureSource::enumerateMonitors() const
{
    return win_capture::enumerateMonitors();
}

WindowCaptureSource::WindowCaptureSource(QString name)
    : SourceBase(SourceType::WindowCapture,
                 name.isEmpty() ? SourceFactory::defaultName(SourceType::WindowCapture) : std::move(name),
                 SourceFactory::capabilityFor(SourceType::WindowCapture),
                 SourceFactory::capabilityNoteFor(SourceType::WindowCapture))
{
}

QVariantList WindowCaptureSource::enumerateWindows() const
{
    return win_capture::enumerateWindows();
}

GameCaptureSource::GameCaptureSource(QString name)
    : SourceBase(SourceType::GameCapture,
                 name.isEmpty() ? SourceFactory::defaultName(SourceType::GameCapture) : std::move(name),
                 SourceFactory::capabilityFor(SourceType::GameCapture),
                 SourceFactory::capabilityNoteFor(SourceType::GameCapture))
{
}

ApplicationCaptureSource::ApplicationCaptureSource(QString name)
    : SourceBase(SourceType::ApplicationCapture,
                 name.isEmpty() ? SourceFactory::defaultName(SourceType::ApplicationCapture) : std::move(name),
                 SourceFactory::capabilityFor(SourceType::ApplicationCapture),
                 SourceFactory::capabilityNoteFor(SourceType::ApplicationCapture))
{
}

CameraSource::CameraSource(QString name)
    : SourceBase(SourceType::Camera,
                 name.isEmpty() ? SourceFactory::defaultName(SourceType::Camera) : std::move(name),
                 SourceFactory::capabilityFor(SourceType::Camera),
                 SourceFactory::capabilityNoteFor(SourceType::Camera))
{
}

MediaSource::MediaSource(QString name)
    : SourceBase(SourceType::MediaSource,
                 name.isEmpty() ? SourceFactory::defaultName(SourceType::MediaSource) : std::move(name),
                 SourceFactory::capabilityFor(SourceType::MediaSource),
                 SourceFactory::capabilityNoteFor(SourceType::MediaSource))
{
}

VlcSource::VlcSource(QString name)
    : SourceBase(SourceType::VlcSource,
                 name.isEmpty() ? SourceFactory::defaultName(SourceType::VlcSource) : std::move(name),
                 SourceFactory::capabilityFor(SourceType::VlcSource),
                 SourceFactory::capabilityNoteFor(SourceType::VlcSource))
{
}

BrowserSource::BrowserSource(QString name)
    : SourceBase(SourceType::Browser,
                 name.isEmpty() ? SourceFactory::defaultName(SourceType::Browser) : std::move(name),
                 SourceFactory::capabilityFor(SourceType::Browser),
                 SourceFactory::capabilityNoteFor(SourceType::Browser))
{
}

ImageSource::ImageSource(QString name)
    : SourceBase(SourceType::Image,
                 name.isEmpty() ? SourceFactory::defaultName(SourceType::Image) : std::move(name),
                 SourceFactory::capabilityFor(SourceType::Image),
                 SourceFactory::capabilityNoteFor(SourceType::Image))
{
}

ImageSlideshowSource::ImageSlideshowSource(QString name)
    : SourceBase(SourceType::ImageSlideshow,
                 name.isEmpty() ? SourceFactory::defaultName(SourceType::ImageSlideshow) : std::move(name),
                 SourceFactory::capabilityFor(SourceType::ImageSlideshow),
                 SourceFactory::capabilityNoteFor(SourceType::ImageSlideshow))
{
}

ColorSource::ColorSource(QString name)
    : SourceBase(SourceType::Color,
                 name.isEmpty() ? SourceFactory::defaultName(SourceType::Color) : std::move(name),
                 SourceFactory::capabilityFor(SourceType::Color),
                 SourceFactory::capabilityNoteFor(SourceType::Color))
{
}

TextSource::TextSource(QString name)
    : SourceBase(SourceType::Text,
                 name.isEmpty() ? SourceFactory::defaultName(SourceType::Text) : std::move(name),
                 SourceFactory::capabilityFor(SourceType::Text),
                 SourceFactory::capabilityNoteFor(SourceType::Text))
{
}

AnimatedMediaSource::AnimatedMediaSource(QString name)
    : SourceBase(SourceType::AnimatedMedia,
                 name.isEmpty() ? SourceFactory::defaultName(SourceType::AnimatedMedia) : std::move(name),
                 SourceFactory::capabilityFor(SourceType::AnimatedMedia),
                 SourceFactory::capabilityNoteFor(SourceType::AnimatedMedia))
{
}

AudioInputSource::AudioInputSource(QString name)
    : SourceBase(SourceType::AudioInput,
                 name.isEmpty() ? SourceFactory::defaultName(SourceType::AudioInput) : std::move(name),
                 SourceFactory::capabilityFor(SourceType::AudioInput),
                 SourceFactory::capabilityNoteFor(SourceType::AudioInput))
{
}

AudioOutputSource::AudioOutputSource(QString name)
    : SourceBase(SourceType::AudioOutput,
                 name.isEmpty() ? SourceFactory::defaultName(SourceType::AudioOutput) : std::move(name),
                 SourceFactory::capabilityFor(SourceType::AudioOutput),
                 SourceFactory::capabilityNoteFor(SourceType::AudioOutput))
{
}

ApplicationAudioSource::ApplicationAudioSource(QString name)
    : SourceBase(SourceType::ApplicationAudio,
                 name.isEmpty() ? SourceFactory::defaultName(SourceType::ApplicationAudio) : std::move(name),
                 SourceFactory::capabilityFor(SourceType::ApplicationAudio),
                 SourceFactory::capabilityNoteFor(SourceType::ApplicationAudio))
{
}

SceneSource::SceneSource(QString name)
    : SourceBase(SourceType::Scene,
                 name.isEmpty() ? SourceFactory::defaultName(SourceType::Scene) : std::move(name),
                 SourceFactory::capabilityFor(SourceType::Scene),
                 SourceFactory::capabilityNoteFor(SourceType::Scene))
{
}

GroupSource::GroupSource(QString name)
    : SourceBase(SourceType::Group,
                 name.isEmpty() ? SourceFactory::defaultName(SourceType::Group) : std::move(name),
                 SourceFactory::capabilityFor(SourceType::Group),
                 SourceFactory::capabilityNoteFor(SourceType::Group))
{
}

NdiSource::NdiSource(QString name)
    : SourceBase(SourceType::Ndi,
                 name.isEmpty() ? SourceFactory::defaultName(SourceType::Ndi) : std::move(name),
                 SourceFactory::capabilityFor(SourceType::Ndi),
                 SourceFactory::capabilityNoteFor(SourceType::Ndi))
{
}

DeckLinkSource::DeckLinkSource(QString name)
    : SourceBase(SourceType::DeckLink,
                 name.isEmpty() ? SourceFactory::defaultName(SourceType::DeckLink) : std::move(name),
                 SourceFactory::capabilityFor(SourceType::DeckLink),
                 SourceFactory::capabilityNoteFor(SourceType::DeckLink))
{
}

PluginSource::PluginSource(QString name)
    : SourceBase(SourceType::Plugin,
                 name.isEmpty() ? SourceFactory::defaultName(SourceType::Plugin) : std::move(name),
                 SourceFactory::capabilityFor(SourceType::Plugin),
                 SourceFactory::capabilityNoteFor(SourceType::Plugin))
{
}

// Factory helpers used by SourceFactory.cpp
namespace concrete_factory {

std::shared_ptr<ISource> createTyped(SourceType type, const QString& name)
{
    switch (type) {
    case SourceType::DisplayCapture: return makeTyped<DisplayCaptureSource>(type, name);
    case SourceType::WindowCapture: return makeTyped<WindowCaptureSource>(type, name);
    case SourceType::GameCapture: return makeTyped<GameCaptureSource>(type, name);
    case SourceType::ApplicationCapture: return makeTyped<ApplicationCaptureSource>(type, name);
    case SourceType::Camera: return makeTyped<CameraSource>(type, name);
    case SourceType::MediaSource: return makeTyped<MediaSource>(type, name);
    case SourceType::VlcSource: return makeTyped<VlcSource>(type, name);
    case SourceType::Browser: return makeTyped<BrowserSource>(type, name);
    case SourceType::AudioInput: return makeTyped<AudioInputSource>(type, name);
    case SourceType::AudioOutput: return makeTyped<AudioOutputSource>(type, name);
    case SourceType::ApplicationAudio: return makeTyped<ApplicationAudioSource>(type, name);
    case SourceType::Image: return makeTyped<ImageSource>(type, name);
    case SourceType::ImageSlideshow: return makeTyped<ImageSlideshowSource>(type, name);
    case SourceType::Color: return makeTyped<ColorSource>(type, name);
    case SourceType::Text: return makeTyped<TextSource>(type, name);
    case SourceType::AnimatedMedia: return makeTyped<AnimatedMediaSource>(type, name);
    case SourceType::Scene: return makeTyped<SceneSource>(type, name);
    case SourceType::Group: return makeTyped<GroupSource>(type, name);
    case SourceType::Ndi: return makeTyped<NdiSource>(type, name);
    case SourceType::DeckLink: return makeTyped<DeckLinkSource>(type, name);
    case SourceType::Plugin: return makeTyped<PluginSource>(type, name);
    }
    return makeTyped<DisplayCaptureSource>(SourceType::DisplayCapture, name);
}

} // namespace concrete_factory

} // namespace nitro
