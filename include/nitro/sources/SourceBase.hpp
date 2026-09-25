#pragma once

#include "nitro/sources/ISource.hpp"

#include <QUuid>
#include <QVariantMap>

namespace nitro {

class SourceBase : public ISource {
public:
    SourceBase(SourceType type, QString name, SourceCapability capability,
               QString capabilityNote = {});

    QString id() const override { return id_; }
    QString name() const override { return name_; }
    void setName(const QString& name) override { name_ = name; }
    SourceType type() const override { return type_; }
    SourceCapability capability() const override { return capability_; }
    QString capabilityNote() const override { return capabilityNote_; }

    bool enabled() const override { return enabled_; }
    void setEnabled(bool enabled) override { enabled_ = enabled; }
    bool visible() const override { return visible_; }
    void setVisible(bool visible) override { visible_ = visible; }
    bool locked() const override { return locked_; }
    void setLocked(bool locked) override { locked_ = locked; }
    bool audioEnabled() const override { return audioEnabled_; }
    void setAudioEnabled(bool enabled) override { audioEnabled_ = enabled; }

    SourceTransform& transform() override { return transform_; }
    const SourceTransform& transform() const override { return transform_; }

    int zOrder() const override { return zOrder_; }
    void setZOrder(int z) override { zOrder_ = z; }

    QVariantMap properties() const override { return properties_; }
    void setProperty(const QString& key, const QVariant& value) override;
    QVariant property(const QString& key) const override;

    std::vector<std::shared_ptr<IFilter>>& filters() override { return filters_; }
    const std::vector<std::shared_ptr<IFilter>>& filters() const override { return filters_; }

    QJsonObject toJson() const override;
    bool applyJson(const QJsonObject& obj) override;

    bool isVisual() const override;
    bool isAudio() const override;

    void setId(const QString& id) { id_ = id; }

protected:
    QString id_;
    QString name_;
    SourceType type_;
    SourceCapability capability_;
    QString capabilityNote_;
    bool enabled_ = true;
    bool visible_ = true;
    bool locked_ = false;
    bool audioEnabled_ = false;
    int zOrder_ = 0;
    SourceTransform transform_;
    QVariantMap properties_;
    std::vector<std::shared_ptr<IFilter>> filters_;
};

} // namespace nitro
