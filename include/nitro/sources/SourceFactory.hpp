#pragma once

#include "nitro/sources/ISource.hpp"
#include "nitro/sources/SourceType.hpp"

#include <QAbstractListModel>
#include <QVector>
#include <memory>

namespace nitro {

struct SourceCatalogEntry {
    SourceType type = SourceType::DisplayCapture;
    SourceCategory category = SourceCategory::Video;
    QString displayName;
    QString icon; // short glyph / emoji for UI
    SourceCapability capability = SourceCapability::NotImplemented;
    QString note;
};

class SourceFactory {
public:
    static std::shared_ptr<ISource> create(SourceType type, const QString& name = {});
    static std::shared_ptr<ISource> fromJson(const QJsonObject& obj);
    static QString defaultName(SourceType type);
    static SourceCapability capabilityFor(SourceType type);
    static QString capabilityNoteFor(SourceType type);
    static QVector<SourceCatalogEntry> catalog();
};

/// QML model for categorized Add Source menu.
class SourceCatalogModel : public QAbstractListModel {
    Q_OBJECT
public:
    enum Roles {
        TypeRole = Qt::UserRole + 1,
        TypeKeyRole,
        CategoryRole,
        CategoryNameRole,
        DisplayNameRole,
        IconRole,
        CapabilityRole,
        CapabilityLabelRole,
        NoteRole,
        CanAddRole
    };

    explicit SourceCatalogModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE QStringList categoryNames() const;
    Q_INVOKABLE int indexOfType(const QString& typeKey) const;

private:
    QVector<SourceCatalogEntry> entries_;
};

} // namespace nitro
