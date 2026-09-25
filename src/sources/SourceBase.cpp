#include "nitro/sources/SourceBase.hpp"

#include "nitro/sources/SourceType.hpp"

#include <QJsonArray>

namespace nitro {

SourceBase::SourceBase(SourceType type, QString name, SourceCapability capability, QString capabilityNote)
    : id_(QUuid::createUuid().toString(QUuid::WithoutBraces))
    , name_(std::move(name))
    , type_(type)
    , capability_(capability)
    , capabilityNote_(std::move(capabilityNote))
{
    audioEnabled_ = isAudio();
}

void SourceBase::setProperty(const QString& key, const QVariant& value)
{
    properties_.insert(key, value);
}

QVariant SourceBase::property(const QString& key) const
{
    return properties_.value(key);
}

bool SourceBase::isVisual() const
{
    switch (type_) {
    case SourceType::AudioInput:
    case SourceType::AudioOutput:
    case SourceType::ApplicationAudio:
        return false;
    default:
        return true;
    }
}

bool SourceBase::isAudio() const
{
    switch (type_) {
    case SourceType::AudioInput:
    case SourceType::AudioOutput:
    case SourceType::ApplicationAudio:
    case SourceType::MediaSource:
    case SourceType::VlcSource:
    case SourceType::Camera: // may include mic track later
        return true;
    default:
        return false;
    }
}

QJsonObject SourceBase::toJson() const
{
    QJsonObject obj;
    obj.insert(QStringLiteral("id"), id_);
    obj.insert(QStringLiteral("name"), name_);
    obj.insert(QStringLiteral("type"), sourceTypeToString(type_));
    obj.insert(QStringLiteral("capability"), sourceCapabilityToString(capability_));
    obj.insert(QStringLiteral("capabilityNote"), capabilityNote_);
    obj.insert(QStringLiteral("enabled"), enabled_);
    obj.insert(QStringLiteral("visible"), visible_);
    obj.insert(QStringLiteral("locked"), locked_);
    obj.insert(QStringLiteral("audioEnabled"), audioEnabled_);
    obj.insert(QStringLiteral("zOrder"), zOrder_);
    obj.insert(QStringLiteral("transform"), transform_.toJson());

    // Legacy flat transform fields for older readers
    obj.insert(QStringLiteral("x"), transform_.x);
    obj.insert(QStringLiteral("y"), transform_.y);
    obj.insert(QStringLiteral("width"), transform_.width);
    obj.insert(QStringLiteral("height"), transform_.height);
    obj.insert(QStringLiteral("opacity"), transform_.opacity);

    QJsonObject props;
    for (auto it = properties_.cbegin(); it != properties_.cend(); ++it) {
        props.insert(it.key(), QJsonValue::fromVariant(it.value()));
    }
    obj.insert(QStringLiteral("properties"), props);

    QJsonArray filt;
    for (const auto& f : filters_) {
        if (f) {
            filt.append(f->toJson());
        }
    }
    obj.insert(QStringLiteral("filters"), filt);
    return obj;
}

bool SourceBase::applyJson(const QJsonObject& obj)
{
    id_ = obj.value(QStringLiteral("id")).toString(id_);
    if (id_.isEmpty()) {
        id_ = QUuid::createUuid().toString(QUuid::WithoutBraces);
    }
    name_ = obj.value(QStringLiteral("name")).toString(name_);
    enabled_ = obj.value(QStringLiteral("enabled")).toBool(true);
    visible_ = obj.value(QStringLiteral("visible")).toBool(true);
    locked_ = obj.value(QStringLiteral("locked")).toBool(false);
    audioEnabled_ = obj.value(QStringLiteral("audioEnabled")).toBool(audioEnabled_);
    zOrder_ = obj.value(QStringLiteral("zOrder")).toInt(0);

    if (obj.contains(QStringLiteral("transform"))) {
        transform_ = SourceTransform::fromJson(obj.value(QStringLiteral("transform")).toObject());
    } else {
        transform_.x = obj.value(QStringLiteral("x")).toDouble();
        transform_.y = obj.value(QStringLiteral("y")).toDouble();
        transform_.width = obj.value(QStringLiteral("width")).toDouble(1920.0);
        transform_.height = obj.value(QStringLiteral("height")).toDouble(1080.0);
        transform_.opacity = obj.value(QStringLiteral("opacity")).toDouble(1.0);
    }

    properties_.clear();
    const QJsonObject props = obj.value(QStringLiteral("properties")).toObject();
    for (auto it = props.begin(); it != props.end(); ++it) {
        properties_.insert(it.key(), it.value().toVariant());
    }
    return true;
}

} // namespace nitro
