#pragma once

#include "nitro/sources/ISource.hpp"

#include <QJsonObject>
#include <QString>
#include <QVector>
#include <QUuid>
#include <memory>

namespace nitro {

struct Scene {
    QString id;
    QString name;
    QVector<std::shared_ptr<ISource>> sources;
    bool visible = true;

    static Scene create(const QString& name);
    QJsonObject toJson() const;
    static Scene fromJson(const QJsonObject& obj);

    /// Returns false if adding nestedSceneId would create a cycle within this collection helper.
    static bool wouldCreateSceneCycle(const QVector<Scene>& allScenes,
                                      const QString& parentSceneId,
                                      const QString& nestedSceneId);
};

} // namespace nitro
