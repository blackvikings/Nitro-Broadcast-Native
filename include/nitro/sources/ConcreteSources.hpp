#pragma once

#include "nitro/sources/SourceBase.hpp"

#include <QVariantList>
#include <memory>

namespace nitro {

/// Typed source subclasses — each owns type-specific helpers; capture engines stay separate.

class DisplayCaptureSource final : public SourceBase {
public:
    explicit DisplayCaptureSource(QString name = {});
    QVariantList enumerateMonitors() const;
};

class WindowCaptureSource final : public SourceBase {
public:
    explicit WindowCaptureSource(QString name = {});
    QVariantList enumerateWindows() const;
};

class GameCaptureSource final : public SourceBase {
public:
    explicit GameCaptureSource(QString name = {});
};

class ApplicationCaptureSource final : public SourceBase {
public:
    explicit ApplicationCaptureSource(QString name = {});
};

class CameraSource final : public SourceBase {
public:
    explicit CameraSource(QString name = {});
};

class MediaSource final : public SourceBase {
public:
    explicit MediaSource(QString name = {});
};

class VlcSource final : public SourceBase {
public:
    explicit VlcSource(QString name = {});
};

class BrowserSource final : public SourceBase {
public:
    explicit BrowserSource(QString name = {});
};

class ImageSource final : public SourceBase {
public:
    explicit ImageSource(QString name = {});
};

class ImageSlideshowSource final : public SourceBase {
public:
    explicit ImageSlideshowSource(QString name = {});
};

class ColorSource final : public SourceBase {
public:
    explicit ColorSource(QString name = {});
};

class TextSource final : public SourceBase {
public:
    explicit TextSource(QString name = {});
};

class AnimatedMediaSource final : public SourceBase {
public:
    explicit AnimatedMediaSource(QString name = {});
};

class AudioInputSource final : public SourceBase {
public:
    explicit AudioInputSource(QString name = {});
};

class AudioOutputSource final : public SourceBase {
public:
    explicit AudioOutputSource(QString name = {});
};

class ApplicationAudioSource final : public SourceBase {
public:
    explicit ApplicationAudioSource(QString name = {});
};

class SceneSource final : public SourceBase {
public:
    explicit SceneSource(QString name = {});
};

class GroupSource final : public SourceBase {
public:
    explicit GroupSource(QString name = {});
};

class NdiSource final : public SourceBase {
public:
    explicit NdiSource(QString name = {});
};

class DeckLinkSource final : public SourceBase {
public:
    explicit DeckLinkSource(QString name = {});
};

class PluginSource final : public SourceBase {
public:
    explicit PluginSource(QString name = {});
};

namespace concrete_factory {
std::shared_ptr<ISource> createTyped(SourceType type, const QString& name);
}

} // namespace nitro
