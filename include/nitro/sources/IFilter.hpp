#pragma once

#include <QJsonObject>
#include <QString>
#include <QUuid>
#include <memory>
#include <vector>

namespace nitro {

enum class FilterType {
    Crop,
    ColorCorrection,
    Blur,
    Sharpen,
    ChromaKey,
    LumaKey,
    Mask,
    Lut,
    NoiseReduction
};

class IFilter {
public:
    virtual ~IFilter() = default;
    virtual QString id() const = 0;
    virtual FilterType type() const = 0;
    virtual QString name() const = 0;
    virtual bool enabled() const = 0;
    virtual void setEnabled(bool enabled) = 0;
    virtual QJsonObject toJson() const = 0;
};

/// Placeholder filter entry until real GPU/CPU filter graph exists.
class StubFilter final : public IFilter {
public:
    explicit StubFilter(FilterType type, QString name)
        : id_(QUuid::createUuid().toString(QUuid::WithoutBraces))
        , type_(type)
        , name_(std::move(name))
    {
    }

    QString id() const override { return id_; }
    FilterType type() const override { return type_; }
    QString name() const override { return name_; }
    bool enabled() const override { return enabled_; }
    void setEnabled(bool enabled) override { enabled_ = enabled; }

    QJsonObject toJson() const override
    {
        QJsonObject o;
        o.insert(QStringLiteral("id"), id_);
        o.insert(QStringLiteral("type"), static_cast<int>(type_));
        o.insert(QStringLiteral("name"), name_);
        o.insert(QStringLiteral("enabled"), enabled_);
        return o;
    }

private:
    QString id_;
    FilterType type_;
    QString name_;
    bool enabled_ = true;
};

} // namespace nitro
