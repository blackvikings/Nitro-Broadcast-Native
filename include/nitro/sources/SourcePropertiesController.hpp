#pragma once

#include <QObject>
#include <QVariantMap>
#include <QPointer>

namespace nitro {

class SceneManager;

/// Exposes the selected source's properties/transforms to QML.
class SourcePropertiesController : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool hasSelection READ hasSelection NOTIFY changed)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY changed)
    Q_PROPERTY(QString typeName READ typeName NOTIFY changed)
    Q_PROPERTY(QString capability READ capability NOTIFY changed)
    Q_PROPERTY(QString capabilityNote READ capabilityNote NOTIFY changed)
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY changed)
    Q_PROPERTY(bool visible READ visible WRITE setVisible NOTIFY changed)
    Q_PROPERTY(bool locked READ locked WRITE setLocked NOTIFY changed)
    Q_PROPERTY(double x READ x WRITE setX NOTIFY changed)
    Q_PROPERTY(double y READ y WRITE setY NOTIFY changed)
    Q_PROPERTY(double width READ width WRITE setWidth NOTIFY changed)
    Q_PROPERTY(double height READ height WRITE setHeight NOTIFY changed)
    Q_PROPERTY(double rotation READ rotation WRITE setRotation NOTIFY changed)
    Q_PROPERTY(double opacity READ opacity WRITE setOpacity NOTIFY changed)
    Q_PROPERTY(QVariantMap properties READ properties NOTIFY changed)

public:
    explicit SourcePropertiesController(SceneManager* scenes, QObject* parent = nullptr);

    bool hasSelection() const;
    QString name() const;
    void setName(const QString& n);
    QString typeName() const;
    QString capability() const;
    QString capabilityNote() const;
    bool enabled() const;
    void setEnabled(bool v);
    bool visible() const;
    void setVisible(bool v);
    bool locked() const;
    void setLocked(bool v);

    double x() const;
    void setX(double v);
    double y() const;
    void setY(double v);
    double width() const;
    void setWidth(double v);
    double height() const;
    void setHeight(double v);
    double rotation() const;
    void setRotation(double v);
    double opacity() const;
    void setOpacity(double v);

    QVariantMap properties() const;
    Q_INVOKABLE void setPropertyValue(const QString& key, const QVariant& value);
    Q_INVOKABLE void resetTransform();
    Q_INVOKABLE void fitToCanvas();
    Q_INVOKABLE void center();
    Q_INVOKABLE void stretch();

signals:
    void changed();

private:
    QVariantMap current() const;
    QPointer<SceneManager> scenes_;
};

} // namespace nitro
