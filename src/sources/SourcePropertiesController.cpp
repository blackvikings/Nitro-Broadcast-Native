#include "nitro/sources/SourcePropertiesController.hpp"

#include "nitro/scenes/SceneManager.hpp"

namespace nitro {

SourcePropertiesController::SourcePropertiesController(SceneManager* scenes, QObject* parent)
    : QObject(parent)
    , scenes_(scenes)
{
    if (scenes_) {
        connect(scenes_, &SceneManager::selectedSourceChanged, this, &SourcePropertiesController::changed);
        connect(scenes_, &SceneManager::sourcesChanged, this, &SourcePropertiesController::changed);
    }
}

QVariantMap SourcePropertiesController::current() const
{
    if (!scenes_ || scenes_->selectedSourceIndex() < 0) {
        return {};
    }
    return scenes_->sourceAt(scenes_->selectedSourceIndex());
}

bool SourcePropertiesController::hasSelection() const
{
    return scenes_ && scenes_->selectedSourceIndex() >= 0;
}

QString SourcePropertiesController::name() const { return current().value(QStringLiteral("name")).toString(); }
QString SourcePropertiesController::typeName() const { return current().value(QStringLiteral("typeName")).toString(); }
QString SourcePropertiesController::capability() const { return current().value(QStringLiteral("capability")).toString(); }
QString SourcePropertiesController::capabilityNote() const { return current().value(QStringLiteral("capabilityNote")).toString(); }
bool SourcePropertiesController::enabled() const { return current().value(QStringLiteral("enabled")).toBool(); }
bool SourcePropertiesController::visible() const
{
    const QVariant v = current().value(QStringLiteral("visible"));
    return v.isValid() ? v.toBool() : true;
}
bool SourcePropertiesController::locked() const { return current().value(QStringLiteral("locked")).toBool(); }
double SourcePropertiesController::x() const { return current().value(QStringLiteral("x")).toDouble(); }
double SourcePropertiesController::y() const { return current().value(QStringLiteral("y")).toDouble(); }
double SourcePropertiesController::width() const { return current().value(QStringLiteral("width")).toDouble(); }
double SourcePropertiesController::height() const { return current().value(QStringLiteral("height")).toDouble(); }
double SourcePropertiesController::rotation() const { return current().value(QStringLiteral("rotation")).toDouble(); }
double SourcePropertiesController::opacity() const
{
    const QVariant v = current().value(QStringLiteral("opacity"));
    return v.isValid() ? v.toDouble() : 1.0;
}
QVariantMap SourcePropertiesController::properties() const
{
    return current().value(QStringLiteral("properties")).toMap();
}

void SourcePropertiesController::setName(const QString& n)
{
    if (scenes_) scenes_->renameSource(scenes_->selectedSourceIndex(), n);
}
void SourcePropertiesController::setEnabled(bool v)
{
    if (scenes_) scenes_->setSourceEnabled(scenes_->selectedSourceIndex(), v);
}
void SourcePropertiesController::setVisible(bool v)
{
    if (scenes_) scenes_->setSourceVisible(scenes_->selectedSourceIndex(), v);
}
void SourcePropertiesController::setLocked(bool v)
{
    if (scenes_) scenes_->setSourceLocked(scenes_->selectedSourceIndex(), v);
}
void SourcePropertiesController::setX(double v)
{
    if (scenes_) scenes_->setSourceProperty(scenes_->selectedSourceIndex(), QStringLiteral("x"), v);
}
void SourcePropertiesController::setY(double v)
{
    if (scenes_) scenes_->setSourceProperty(scenes_->selectedSourceIndex(), QStringLiteral("y"), v);
}
void SourcePropertiesController::setWidth(double v)
{
    if (scenes_) scenes_->setSourceProperty(scenes_->selectedSourceIndex(), QStringLiteral("width"), v);
}
void SourcePropertiesController::setHeight(double v)
{
    if (scenes_) scenes_->setSourceProperty(scenes_->selectedSourceIndex(), QStringLiteral("height"), v);
}
void SourcePropertiesController::setRotation(double v)
{
    if (scenes_) scenes_->setSourceProperty(scenes_->selectedSourceIndex(), QStringLiteral("rotation"), v);
}
void SourcePropertiesController::setOpacity(double v)
{
    if (scenes_) scenes_->setSourceProperty(scenes_->selectedSourceIndex(), QStringLiteral("opacity"), v);
}
void SourcePropertiesController::setPropertyValue(const QString& key, const QVariant& value)
{
    if (scenes_) scenes_->setSourceProperty(scenes_->selectedSourceIndex(), key, value);
}
void SourcePropertiesController::resetTransform()
{
    if (scenes_) scenes_->resetSourceTransform(scenes_->selectedSourceIndex());
}
void SourcePropertiesController::fitToCanvas()
{
    if (scenes_) scenes_->fitSourceToCanvas(scenes_->selectedSourceIndex());
}
void SourcePropertiesController::center()
{
    if (scenes_) scenes_->centerSource(scenes_->selectedSourceIndex());
}
void SourcePropertiesController::stretch()
{
    if (scenes_) scenes_->stretchSourceToCanvas(scenes_->selectedSourceIndex());
}

} // namespace nitro
