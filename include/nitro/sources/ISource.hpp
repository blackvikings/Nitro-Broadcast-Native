#pragma once

#include "nitro/sources/IFilter.hpp"
#include "nitro/sources/SourceTransform.hpp"
#include "nitro/sources/SourceType.hpp"

#include <QJsonObject>
#include <QString>
#include <QVariantMap>
#include <memory>
#include <vector>

namespace nitro {

class ISource {
public:
    virtual ~ISource() = default;

    virtual QString id() const = 0;
    virtual QString name() const = 0;
    virtual void setName(const QString& name) = 0;
    virtual SourceType type() const = 0;
    virtual SourceCapability capability() const = 0;
    virtual QString capabilityNote() const = 0;

    virtual bool enabled() const = 0;
    virtual void setEnabled(bool enabled) = 0;
    virtual bool visible() const = 0;
    virtual void setVisible(bool visible) = 0;
    virtual bool locked() const = 0;
    virtual void setLocked(bool locked) = 0;
    virtual bool audioEnabled() const = 0;
    virtual void setAudioEnabled(bool enabled) = 0;

    virtual SourceTransform& transform() = 0;
    virtual const SourceTransform& transform() const = 0;

    virtual int zOrder() const = 0;
    virtual void setZOrder(int z) = 0;

    /// Type-specific settings (device id, file path, text, color, …).
    virtual QVariantMap properties() const = 0;
    virtual void setProperty(const QString& key, const QVariant& value) = 0;
    virtual QVariant property(const QString& key) const = 0;

    virtual std::vector<std::shared_ptr<IFilter>>& filters() = 0;
    virtual const std::vector<std::shared_ptr<IFilter>>& filters() const = 0;

    virtual QJsonObject toJson() const = 0;
    virtual bool applyJson(const QJsonObject& obj) = 0;

    /// True if this source type produces visual frames (vs audio-only).
    virtual bool isVisual() const = 0;
    virtual bool isAudio() const = 0;
};

} // namespace nitro
