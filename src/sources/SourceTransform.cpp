#include "nitro/sources/SourceTransform.hpp"

#include <algorithm>

namespace nitro {

QJsonObject SourceCrop::toJson() const
{
    QJsonObject o;
    o.insert(QStringLiteral("left"), left);
    o.insert(QStringLiteral("right"), right);
    o.insert(QStringLiteral("top"), top);
    o.insert(QStringLiteral("bottom"), bottom);
    return o;
}

SourceCrop SourceCrop::fromJson(const QJsonObject& obj)
{
    SourceCrop c;
    c.left = obj.value(QStringLiteral("left")).toDouble();
    c.right = obj.value(QStringLiteral("right")).toDouble();
    c.top = obj.value(QStringLiteral("top")).toDouble();
    c.bottom = obj.value(QStringLiteral("bottom")).toDouble();
    return c;
}

void SourceTransform::reset(double canvasW, double canvasH)
{
    x = 0.0;
    y = 0.0;
    width = canvasW;
    height = canvasH;
    rotation = 0.0;
    opacity = 1.0;
    crop = {};
}

void SourceTransform::fitToCanvas(double canvasW, double canvasH, double contentW, double contentH)
{
    if (contentW <= 0.0 || contentH <= 0.0) {
        stretchToCanvas(canvasW, canvasH);
        return;
    }
    const double scale = std::min(canvasW / contentW, canvasH / contentH);
    width = contentW * scale;
    height = contentH * scale;
    centerOnCanvas(canvasW, canvasH);
}

void SourceTransform::centerOnCanvas(double canvasW, double canvasH)
{
    x = (canvasW - width) * 0.5;
    y = (canvasH - height) * 0.5;
}

void SourceTransform::stretchToCanvas(double canvasW, double canvasH)
{
    x = 0.0;
    y = 0.0;
    width = canvasW;
    height = canvasH;
}

QJsonObject SourceTransform::toJson() const
{
    QJsonObject o;
    o.insert(QStringLiteral("x"), x);
    o.insert(QStringLiteral("y"), y);
    o.insert(QStringLiteral("width"), width);
    o.insert(QStringLiteral("height"), height);
    o.insert(QStringLiteral("rotation"), rotation);
    o.insert(QStringLiteral("opacity"), opacity);
    o.insert(QStringLiteral("crop"), crop.toJson());
    return o;
}

SourceTransform SourceTransform::fromJson(const QJsonObject& obj)
{
    SourceTransform t;
    t.x = obj.value(QStringLiteral("x")).toDouble();
    t.y = obj.value(QStringLiteral("y")).toDouble();
    t.width = obj.value(QStringLiteral("width")).toDouble(1920.0);
    t.height = obj.value(QStringLiteral("height")).toDouble(1080.0);
    t.rotation = obj.value(QStringLiteral("rotation")).toDouble();
    t.opacity = obj.value(QStringLiteral("opacity")).toDouble(1.0);
    t.crop = SourceCrop::fromJson(obj.value(QStringLiteral("crop")).toObject());
    // Legacy flat fields
    if (obj.contains(QStringLiteral("x")) && !obj.contains(QStringLiteral("crop"))) {
        // already loaded
    }
    return t;
}

} // namespace nitro
