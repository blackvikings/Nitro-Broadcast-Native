#include "nitro/sources/SourceFactory.hpp"

#include "nitro/sources/ConcreteSources.hpp"

#include <QJsonObject>

namespace nitro {

QString SourceFactory::defaultName(SourceType type)
{
    return sourceTypeDisplayName(type);
}

SourceCapability SourceFactory::capabilityFor(SourceType type)
{
    switch (type) {
    case SourceType::DisplayCapture:
    case SourceType::WindowCapture:
    case SourceType::Camera:
        return SourceCapability::Experimental;
    case SourceType::AudioInput:
    case SourceType::AudioOutput:
        return SourceCapability::Available;
    case SourceType::Image:
    case SourceType::Color:
    case SourceType::Text:
    case SourceType::ImageSlideshow:
    case SourceType::Scene:
    case SourceType::Group:
        return SourceCapability::Available;
    case SourceType::MediaSource:
    case SourceType::Browser:
    case SourceType::AnimatedMedia:
        return SourceCapability::Experimental;
    case SourceType::GameCapture:
    case SourceType::ApplicationCapture:
    case SourceType::ApplicationAudio:
        return SourceCapability::NotImplemented;
    case SourceType::VlcSource:
    case SourceType::Ndi:
    case SourceType::DeckLink:
        return SourceCapability::NotAvailable;
    case SourceType::Plugin:
        return SourceCapability::NotImplemented;
    }
    return SourceCapability::NotImplemented;
}

QString SourceFactory::capabilityNoteFor(SourceType type)
{
    switch (type) {
    case SourceType::DisplayCapture:
        return QStringLiteral("WGC foundation — monitor enumeration live; frame capture TBD");
    case SourceType::WindowCapture:
        return QStringLiteral("WGC foundation — window enumeration live; frame capture TBD");
    case SourceType::GameCapture:
        return QStringLiteral("Game Capture — Experimental / Not Yet Available (no fake display capture)");
    case SourceType::ApplicationCapture:
        return QStringLiteral("Application video capture not implemented");
    case SourceType::Camera:
        return QStringLiteral("Device/format properties ready; Media Foundation capture TBD");
    case SourceType::MediaSource:
        return QStringLiteral("FFmpeg decode path planned; file properties only for now");
    case SourceType::VlcSource:
        return QStringLiteral("VLC SDK not integrated");
    case SourceType::Browser:
        return QStringLiteral("Sandboxed browser planned; URL/size properties only — no native bridge");
    case SourceType::AudioInput:
        return QStringLiteral("Uses WASAPI AudioEngine microphone path");
    case SourceType::AudioOutput:
        return QStringLiteral("Uses WASAPI AudioEngine desktop loopback path");
    case SourceType::ApplicationAudio:
        return QStringLiteral("Process audio capture — Not Available yet (no fake separation)");
    case SourceType::Image:
        return QStringLiteral("PNG/JPG/WebP path properties; compositor draw TBD");
    case SourceType::ImageSlideshow:
        return QStringLiteral("Playlist properties ready; playback TBD");
    case SourceType::Color:
        return QStringLiteral("Solid color fill properties ready");
    case SourceType::Text:
        return QStringLiteral("Font/content properties ready; text render TBD");
    case SourceType::AnimatedMedia:
        return QStringLiteral("GIF/WebP/video overlay — Experimental; alpha not claimed");
    case SourceType::Scene:
        return QStringLiteral("Nested scene reference with cycle detection");
    case SourceType::Group:
        return QStringLiteral("Group of child source IDs");
    case SourceType::Ndi:
        return QStringLiteral("NDI SDK not installed");
    case SourceType::DeckLink:
        return QStringLiteral("Blackmagic DeckLink SDK not installed");
    case SourceType::Plugin:
        return QStringLiteral("Plugin host not implemented");
    }
    return {};
}

std::shared_ptr<ISource> SourceFactory::create(SourceType type, const QString& name)
{
    return concrete_factory::createTyped(type, name);
}

std::shared_ptr<ISource> SourceFactory::fromJson(const QJsonObject& obj)
{
    const SourceType type = sourceTypeFromString(obj.value(QStringLiteral("type")).toString());
    auto src = concrete_factory::createTyped(type, obj.value(QStringLiteral("name")).toString());
    src->applyJson(obj);
    return src;
}

QVector<SourceCatalogEntry> SourceFactory::catalog()
{
    const SourceType order[] = {
        SourceType::DisplayCapture, SourceType::WindowCapture, SourceType::GameCapture,
        SourceType::ApplicationCapture, SourceType::Camera, SourceType::MediaSource,
        SourceType::VlcSource, SourceType::Browser,
        SourceType::AudioInput, SourceType::AudioOutput, SourceType::ApplicationAudio,
        SourceType::Image, SourceType::ImageSlideshow, SourceType::Color, SourceType::Text,
        SourceType::AnimatedMedia,
        SourceType::Scene, SourceType::Group,
        SourceType::Ndi, SourceType::DeckLink, SourceType::Plugin
    };

    const char* icons[] = {
        "🖥", "🪟", "🎮", "📱", "📷", "🎞", "▶", "🌐",
        "🎙", "🔊", "🎧",
        "🖼", "🗂", "🎨", "✍", "✨",
        "🎬", "📁",
        "📡", "🎥", "🔌"
    };

    QVector<SourceCatalogEntry> entries;
    for (int i = 0; i < static_cast<int>(sizeof(order) / sizeof(order[0])); ++i) {
        SourceCatalogEntry e;
        e.type = order[i];
        e.category = sourceCategoryFor(order[i]);
        e.displayName = sourceTypeDisplayName(order[i]);
        e.icon = QString::fromUtf8(icons[i]);
        e.capability = capabilityFor(order[i]);
        e.note = capabilityNoteFor(order[i]);
        entries.push_back(e);
    }
    return entries;
}

SourceCatalogModel::SourceCatalogModel(QObject* parent)
    : QAbstractListModel(parent)
    , entries_(SourceFactory::catalog())
{
}

int SourceCatalogModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : entries_.size();
}

QVariant SourceCatalogModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= entries_.size()) {
        return {};
    }
    const auto& e = entries_.at(index.row());
    switch (role) {
    case TypeRole: return static_cast<int>(e.type);
    case TypeKeyRole: return sourceTypeToString(e.type);
    case CategoryRole: return static_cast<int>(e.category);
    case CategoryNameRole: return sourceCategoryName(e.category);
    case DisplayNameRole: return e.displayName;
    case IconRole: return e.icon;
    case CapabilityRole: return sourceCapabilityToString(e.capability);
    case CapabilityLabelRole: return sourceCapabilityToString(e.capability);
    case NoteRole: return e.note;
    case CanAddRole:
        // Allow adding Experimental & Available. NotImplemented/NotAvailable still addable
        // as design placeholders but UI marks them disabled by default.
        return e.capability == SourceCapability::Available
            || e.capability == SourceCapability::Experimental
            || e.capability == SourceCapability::NotImplemented
            || e.capability == SourceCapability::NotAvailable;
    default:
        return {};
    }
}

QHash<int, QByteArray> SourceCatalogModel::roleNames() const
{
    return {
        {TypeRole, "sourceType"},
        {TypeKeyRole, "typeKey"},
        {CategoryRole, "category"},
        {CategoryNameRole, "categoryName"},
        {DisplayNameRole, "displayName"},
        {IconRole, "icon"},
        {CapabilityRole, "capability"},
        {CapabilityLabelRole, "capabilityLabel"},
        {NoteRole, "note"},
        {CanAddRole, "canAdd"}
    };
}

QStringList SourceCatalogModel::categoryNames() const
{
    return {
        sourceCategoryName(SourceCategory::Video),
        sourceCategoryName(SourceCategory::Audio),
        sourceCategoryName(SourceCategory::Graphics),
        sourceCategoryName(SourceCategory::Composition),
        sourceCategoryName(SourceCategory::Advanced)
    };
}

int SourceCatalogModel::indexOfType(const QString& typeKey) const
{
    for (int i = 0; i < entries_.size(); ++i) {
        if (sourceTypeToString(entries_[i].type) == typeKey) {
            return i;
        }
    }
    return -1;
}

} // namespace nitro
