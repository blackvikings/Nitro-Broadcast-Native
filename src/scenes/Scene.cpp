#include "nitro/scenes/Scene.hpp"

#include "nitro/sources/SourceFactory.hpp"

#include <QJsonArray>
#include <QSet>
#include <functional>

namespace nitro {

Scene Scene::create(const QString& name)
{
    Scene scene;
    scene.id = QUuid::createUuid().toString(QUuid::WithoutBraces);
    scene.name = name;
    return scene;
}

QJsonObject Scene::toJson() const
{
    QJsonObject obj;
    obj.insert(QStringLiteral("id"), id);
    obj.insert(QStringLiteral("name"), name);
    obj.insert(QStringLiteral("visible"), visible);
    QJsonArray sourcesArr;
    for (const auto& source : sources) {
        if (source) {
            sourcesArr.append(source->toJson());
        }
    }
    obj.insert(QStringLiteral("sources"), sourcesArr);
    return obj;
}

Scene Scene::fromJson(const QJsonObject& obj)
{
    Scene scene;
    scene.id = obj.value(QStringLiteral("id")).toString();
    if (scene.id.isEmpty()) {
        scene.id = QUuid::createUuid().toString(QUuid::WithoutBraces);
    }
    scene.name = obj.value(QStringLiteral("name")).toString(QStringLiteral("Scene"));
    scene.visible = obj.value(QStringLiteral("visible")).toBool(true);
    const auto arr = obj.value(QStringLiteral("sources")).toArray();
    for (const auto& value : arr) {
        scene.sources.push_back(SourceFactory::fromJson(value.toObject()));
    }
    return scene;
}

bool Scene::wouldCreateSceneCycle(const QVector<Scene>& allScenes,
                                  const QString& parentSceneId,
                                  const QString& nestedSceneId)
{
    if (nestedSceneId.isEmpty() || parentSceneId == nestedSceneId) {
        return true;
    }

    std::function<bool(const QString&, QSet<QString>&)> dfs =
        [&](const QString& sceneId, QSet<QString>& visiting) -> bool {
        if (sceneId == parentSceneId) {
            return true;
        }
        if (visiting.contains(sceneId)) {
            return true;
        }
        visiting.insert(sceneId);
        for (const auto& sc : allScenes) {
            if (sc.id != sceneId) {
                continue;
            }
            for (const auto& src : sc.sources) {
                if (!src || src->type() != SourceType::Scene) {
                    continue;
                }
                const QString child = src->property(QStringLiteral("sceneId")).toString();
                if (!child.isEmpty() && dfs(child, visiting)) {
                    return true;
                }
            }
        }
        visiting.remove(sceneId);
        return false;
    };

    QSet<QString> visiting;
    return dfs(nestedSceneId, visiting);
}

} // namespace nitro
