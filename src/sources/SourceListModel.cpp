#include "nitro/sources/SourceListModel.hpp"

#include "nitro/scenes/SceneManager.hpp"

namespace nitro {

SourceListModel::SourceListModel(SceneManager* scenes, QObject* parent)
    : QAbstractListModel(parent)
    , scenes_(scenes)
{
    if (scenes_) {
        connect(scenes_, &SceneManager::sourcesChanged, this, &SourceListModel::refresh);
        connect(scenes_, &SceneManager::selectedSourceChanged, this, &SourceListModel::refresh);
        connect(scenes_, &SceneManager::previewIndexChanged, this, &SourceListModel::refresh);
    }
}

int SourceListModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid() || !scenes_) {
        return 0;
    }
    return scenes_->sourceCount();
}

QVariant SourceListModel::data(const QModelIndex& index, int role) const
{
    if (!scenes_ || !index.isValid()) {
        return {};
    }
    const auto map = scenes_->sourceAt(index.row());
    switch (role) {
    case IdRole: return map.value(QStringLiteral("id"));
    case NameRole: return map.value(QStringLiteral("name"));
    case TypeRole: return map.value(QStringLiteral("type"));
    case TypeNameRole: return map.value(QStringLiteral("typeName"));
    case CapabilityRole: return map.value(QStringLiteral("capability"));
    case EnabledRole: return map.value(QStringLiteral("enabled"));
    case VisibleRole: return map.value(QStringLiteral("visible"));
    case LockedRole: return map.value(QStringLiteral("locked"));
    case SelectedRole: return map.value(QStringLiteral("selected"));
    case IsAudioRole: return map.value(QStringLiteral("isAudio"));
    case IsVisualRole: return map.value(QStringLiteral("isVisual"));
    default: return {};
    }
}

QHash<int, QByteArray> SourceListModel::roleNames() const
{
    return {
        {IdRole, "sourceId"},
        {NameRole, "name"},
        {TypeRole, "type"},
        {TypeNameRole, "typeName"},
        {CapabilityRole, "capability"},
        {EnabledRole, "enabled"},
        {VisibleRole, "visible"},
        {LockedRole, "locked"},
        {SelectedRole, "selected"},
        {IsAudioRole, "isAudio"},
        {IsVisualRole, "isVisual"}
    };
}

void SourceListModel::refresh()
{
    beginResetModel();
    endResetModel();
    emit countChanged();
}

} // namespace nitro
