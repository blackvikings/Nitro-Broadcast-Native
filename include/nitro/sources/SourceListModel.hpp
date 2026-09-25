#pragma once

#include <QAbstractListModel>
#include <QPointer>

namespace nitro {

class SceneManager;

class SourceListModel : public QAbstractListModel {
    Q_OBJECT
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)

public:
    enum Roles {
        IdRole = Qt::UserRole + 1,
        NameRole,
        TypeRole,
        TypeNameRole,
        CapabilityRole,
        EnabledRole,
        VisibleRole,
        LockedRole,
        SelectedRole,
        IsAudioRole,
        IsVisualRole
    };

    explicit SourceListModel(SceneManager* scenes, QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

public slots:
    void refresh();

signals:
    void countChanged();

private:
    QPointer<SceneManager> scenes_;
};

} // namespace nitro
