#include "nitro/scenes/SceneManager.hpp"

#include "nitro/diagnostics/NitroLogger.hpp"
#include "nitro/scenes/TransitionEngine.hpp"
#include "nitro/sources/SourceFactory.hpp"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QSaveFile>
#include <QUuid>
#include <algorithm>

namespace nitro {

SceneManager::SceneManager(QObject* parent)
    : QAbstractListModel(parent)
{
    loadDefaults();
}

void SceneManager::setTransitionEngine(TransitionEngine* transitions)
{
    if (transitions_ == transitions) {
        return;
    }
    if (transitions_) {
        disconnect(transitions_, nullptr, this, nullptr);
    }
    transitions_ = transitions;
    if (transitions_) {
        connect(transitions_, &TransitionEngine::completed, this, &SceneManager::onTransitionCompleted);
    }
}

void SceneManager::onTransitionCompleted(int toIndex)
{
    setProgramIndex(toIndex);
}

int SceneManager::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return scenes_.size();
}

QVariant SceneManager::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= scenes_.size()) {
        return {};
    }
    const auto& scene = scenes_.at(index.row());
    switch (role) {
    case IdRole: return scene.id;
    case NameRole: return scene.name;
    case IsPreviewRole: return index.row() == previewIndex_;
    case IsProgramRole: return index.row() == programIndex_;
    default: return {};
    }
}

QHash<int, QByteArray> SceneManager::roleNames() const
{
    return {
        {IdRole, "sceneId"},
        {NameRole, "name"},
        {IsPreviewRole, "isPreview"},
        {IsProgramRole, "isProgram"}
    };
}

void SceneManager::setPreviewIndex(int index)
{
    if (!validSceneIndex(index) || previewIndex_ == index) {
        return;
    }
    const int old = previewIndex_;
    previewIndex_ = index;
    selectedSourceIndex_ = scenes_[index].sources.isEmpty() ? -1 : 0;
    if (old >= 0) {
        emit dataChanged(this->index(old), this->index(old));
    }
    emit dataChanged(this->index(previewIndex_), this->index(previewIndex_));
    emit previewIndexChanged();
    emit sourcesChanged();
    emit selectedSourceChanged();
}

void SceneManager::setProgramIndex(int index)
{
    if (!validSceneIndex(index) || programIndex_ == index) {
        return;
    }
    const int old = programIndex_;
    programIndex_ = index;
    if (old >= 0) {
        emit dataChanged(this->index(old), this->index(old));
    }
    emit dataChanged(this->index(programIndex_), this->index(programIndex_));
    emit programIndexChanged();
}

QString SceneManager::previewSceneName() const
{
    if (!validSceneIndex(previewIndex_)) {
        return {};
    }
    return scenes_.at(previewIndex_).name;
}

QString SceneManager::programSceneName() const
{
    if (!validSceneIndex(programIndex_)) {
        return {};
    }
    return scenes_.at(programIndex_).name;
}

QString SceneManager::createScene(const QString& name)
{
    QString finalName = name.trimmed();
    if (finalName.isEmpty()) {
        int n = 1;
        do {
            finalName = QStringLiteral("Scene %1").arg(n++);
        } while (std::any_of(scenes_.cbegin(), scenes_.cend(),
                             [&](const Scene& s) { return s.name == finalName; }));
    }

    beginInsertRows(QModelIndex(), scenes_.size(), scenes_.size());
    scenes_.push_back(Scene::create(finalName));
    endInsertRows();
    emit scenesChanged();
    NITRO_LOG_INFO(QStringLiteral("Scenes"), QStringLiteral("Created scene %1").arg(finalName));
    setPreviewIndex(scenes_.size() - 1);
    return scenes_.back().id;
}

bool SceneManager::deleteScene(int index)
{
    if (!validSceneIndex(index) || scenes_.size() <= 1) {
        return false;
    }
    beginRemoveRows(QModelIndex(), index, index);
    scenes_.removeAt(index);
    endRemoveRows();
    ensureValidIndices();
    emit scenesChanged();
    emit sourcesChanged();
    return true;
}

bool SceneManager::renameScene(int index, const QString& name)
{
    if (!validSceneIndex(index) || name.trimmed().isEmpty()) {
        return false;
    }
    scenes_[index].name = name.trimmed();
    emit dataChanged(this->index(index), this->index(index), {NameRole});
    emit scenesChanged();
    if (index == previewIndex_) {
        emit previewIndexChanged();
    }
    if (index == programIndex_) {
        emit programIndexChanged();
    }
    return true;
}

QString SceneManager::duplicateScene(int index)
{
    if (!validSceneIndex(index)) {
        return {};
    }
    Scene copy = scenes_.at(index);
    copy.id = QUuid::createUuid().toString(QUuid::WithoutBraces);
    copy.name = copy.name + QStringLiteral(" Copy");
    QVector<std::shared_ptr<ISource>> newSources;
    for (const auto& src : copy.sources) {
        if (!src) {
            continue;
        }
        auto cloned = SourceFactory::fromJson(src->toJson());
        // force new id
        auto json = cloned->toJson();
        json.insert(QStringLiteral("id"), QUuid::createUuid().toString(QUuid::WithoutBraces));
        cloned = SourceFactory::fromJson(json);
        newSources.push_back(cloned);
    }
    copy.sources = newSources;
    beginInsertRows(QModelIndex(), index + 1, index + 1);
    scenes_.insert(index + 1, copy);
    endInsertRows();
    emit scenesChanged();
    setPreviewIndex(index + 1);
    return copy.id;
}

bool SceneManager::selectPreview(int index)
{
    if (!validSceneIndex(index)) {
        return false;
    }
    setPreviewIndex(index);
    return true;
}

void SceneManager::cutToProgram()
{
    if (transitions_) {
        transitions_->cut(programIndex_, previewIndex_);
    } else {
        setProgramIndex(previewIndex_);
    }
    emit transitionPerformed(QStringLiteral("cut"));
}

void SceneManager::fadeToProgram(int durationMs)
{
    if (transitions_) {
        transitions_->fade(programIndex_, previewIndex_, durationMs);
    } else {
        setProgramIndex(previewIndex_);
    }
    emit transitionPerformed(QStringLiteral("fade"));
}

int SceneManager::sourceCount() const
{
    const Scene* scene = previewScene();
    return scene ? scene->sources.size() : 0;
}

std::shared_ptr<ISource> SceneManager::sourcePtrAt(int index) const
{
    const Scene* scene = previewScene();
    if (!scene || index < 0 || index >= scene->sources.size()) {
        return nullptr;
    }
    return scene->sources.at(index);
}

QVariantMap SceneManager::sourceAt(int index) const
{
    auto s = sourcePtrAt(index);
    if (!s) {
        return {};
    }
    const auto& t = s->transform();
    return {
        {QStringLiteral("id"), s->id()},
        {QStringLiteral("name"), s->name()},
        {QStringLiteral("type"), sourceTypeToString(s->type())},
        {QStringLiteral("typeName"), sourceTypeDisplayName(s->type())},
        {QStringLiteral("capability"), sourceCapabilityToString(s->capability())},
        {QStringLiteral("capabilityNote"), s->capabilityNote()},
        {QStringLiteral("enabled"), s->enabled()},
        {QStringLiteral("visible"), s->visible()},
        {QStringLiteral("locked"), s->locked()},
        {QStringLiteral("audioEnabled"), s->audioEnabled()},
        {QStringLiteral("zOrder"), s->zOrder()},
        {QStringLiteral("x"), t.x},
        {QStringLiteral("y"), t.y},
        {QStringLiteral("width"), t.width},
        {QStringLiteral("height"), t.height},
        {QStringLiteral("rotation"), t.rotation},
        {QStringLiteral("opacity"), t.opacity},
        {QStringLiteral("cropLeft"), t.crop.left},
        {QStringLiteral("cropRight"), t.crop.right},
        {QStringLiteral("cropTop"), t.crop.top},
        {QStringLiteral("cropBottom"), t.crop.bottom},
        {QStringLiteral("isVisual"), s->isVisual()},
        {QStringLiteral("isAudio"), s->isAudio()},
        {QStringLiteral("properties"), s->properties()},
        {QStringLiteral("selected"), index == selectedSourceIndex_}
    };
}

QString SceneManager::addSource(const QString& typeKey, const QString& name)
{
    Scene* scene = previewScene();
    if (!scene) {
        return {};
    }

    const SourceType type = sourceTypeFromString(typeKey);
    const auto capability = SourceFactory::capabilityFor(type);
    if (capability == SourceCapability::NotAvailable) {
        emit sourceError(SourceFactory::capabilityNoteFor(type));
        NITRO_LOG_WARN(QStringLiteral("Sources"), SourceFactory::capabilityNoteFor(type));
        // Still allow adding as a placeholder strip so scenes can be designed.
    }

    auto source = SourceFactory::create(type, name);
    source->setZOrder(scene->sources.size());

    if (type == SourceType::Scene) {
        // Default nested scene: first other scene if any
        for (const auto& sc : scenes_) {
            if (sc.id != scene->id) {
                source->setProperty(QStringLiteral("sceneId"), sc.id);
                break;
            }
        }
        const QString nested = source->property(QStringLiteral("sceneId")).toString();
        if (!nested.isEmpty()
            && Scene::wouldCreateSceneCycle(scenes_, scene->id, nested)) {
            emit sourceError(QStringLiteral("Rejected recursive scene reference"));
            return {};
        }
    }

    scene->sources.push_back(source);
    selectedSourceIndex_ = scene->sources.size() - 1;
    emit sourcesChanged();
    emit selectedSourceChanged();
    NITRO_LOG_INFO(QStringLiteral("Sources"),
                   QStringLiteral("Added %1 [%2]")
                       .arg(source->name(), sourceCapabilityToString(source->capability())));
    return source->id();
}

bool SceneManager::removeSource(int index)
{
    Scene* scene = previewScene();
    if (!scene || index < 0 || index >= scene->sources.size()) {
        return false;
    }
    scene->sources.removeAt(index);
    if (selectedSourceIndex_ >= scene->sources.size()) {
        selectedSourceIndex_ = scene->sources.size() - 1;
    }
    emit sourcesChanged();
    emit selectedSourceChanged();
    return true;
}

bool SceneManager::setSourceEnabled(int index, bool enabled)
{
    auto s = sourcePtrAt(index);
    if (!s || s->locked()) {
        return false;
    }
    s->setEnabled(enabled);
    emit sourcesChanged();
    return true;
}

bool SceneManager::setSourceVisible(int index, bool visible)
{
    auto s = sourcePtrAt(index);
    if (!s || s->locked()) {
        return false;
    }
    s->setVisible(visible);
    emit sourcesChanged();
    return true;
}

bool SceneManager::setSourceLocked(int index, bool locked)
{
    auto s = sourcePtrAt(index);
    if (!s) {
        return false;
    }
    s->setLocked(locked);
    emit sourcesChanged();
    return true;
}

bool SceneManager::renameSource(int index, const QString& name)
{
    auto s = sourcePtrAt(index);
    if (!s || name.trimmed().isEmpty() || s->locked()) {
        return false;
    }
    s->setName(name.trimmed());
    emit sourcesChanged();
    return true;
}

bool SceneManager::moveSource(int from, int to)
{
    Scene* scene = previewScene();
    if (!scene || from < 0 || from >= scene->sources.size() || to < 0 || to >= scene->sources.size()) {
        return false;
    }
    scene->sources.move(from, to);
    for (int i = 0; i < scene->sources.size(); ++i) {
        scene->sources[i]->setZOrder(i);
    }
    selectedSourceIndex_ = to;
    emit sourcesChanged();
    emit selectedSourceChanged();
    return true;
}

bool SceneManager::setSourceProperty(int index, const QString& key, const QVariant& value)
{
    auto s = sourcePtrAt(index);
    if (!s || s->locked()) {
        return false;
    }

    if (s->type() == SourceType::Scene && key == QLatin1String("sceneId")) {
        const QString nested = value.toString();
        const Scene* parent = previewScene();
        if (parent && Scene::wouldCreateSceneCycle(scenes_, parent->id, nested)) {
            emit sourceError(QStringLiteral("Rejected recursive scene reference"));
            return false;
        }
    }

    // Transform shortcuts
    auto& t = s->transform();
    if (key == QLatin1String("x")) t.x = value.toDouble();
    else if (key == QLatin1String("y")) t.y = value.toDouble();
    else if (key == QLatin1String("width")) t.width = value.toDouble();
    else if (key == QLatin1String("height")) t.height = value.toDouble();
    else if (key == QLatin1String("rotation")) t.rotation = value.toDouble();
    else if (key == QLatin1String("opacity")) t.opacity = value.toDouble();
    else if (key == QLatin1String("cropLeft")) t.crop.left = value.toDouble();
    else if (key == QLatin1String("cropRight")) t.crop.right = value.toDouble();
    else if (key == QLatin1String("cropTop")) t.crop.top = value.toDouble();
    else if (key == QLatin1String("cropBottom")) t.crop.bottom = value.toDouble();
    else {
        s->setProperty(key, value);
    }

    emit sourcesChanged();
    return true;
}

bool SceneManager::resetSourceTransform(int index)
{
    auto s = sourcePtrAt(index);
    if (!s || s->locked()) {
        return false;
    }
    s->transform().reset(canvasWidth(), canvasHeight());
    emit sourcesChanged();
    return true;
}

bool SceneManager::fitSourceToCanvas(int index)
{
    auto s = sourcePtrAt(index);
    if (!s || s->locked()) {
        return false;
    }
    s->transform().fitToCanvas(canvasWidth(), canvasHeight(),
                               s->transform().width, s->transform().height);
    emit sourcesChanged();
    return true;
}

bool SceneManager::centerSource(int index)
{
    auto s = sourcePtrAt(index);
    if (!s || s->locked()) {
        return false;
    }
    s->transform().centerOnCanvas(canvasWidth(), canvasHeight());
    emit sourcesChanged();
    return true;
}

bool SceneManager::stretchSourceToCanvas(int index)
{
    auto s = sourcePtrAt(index);
    if (!s || s->locked()) {
        return false;
    }
    s->transform().stretchToCanvas(canvasWidth(), canvasHeight());
    emit sourcesChanged();
    return true;
}

void SceneManager::selectSource(int index)
{
    Scene* scene = previewScene();
    if (!scene) {
        selectedSourceIndex_ = -1;
        emit selectedSourceChanged();
        return;
    }
    if (index < -1 || index >= scene->sources.size()) {
        return;
    }
    selectedSourceIndex_ = index;
    emit selectedSourceChanged();
    emit sourcesChanged();
}

QJsonObject SceneManager::toJson() const
{
    QJsonObject root;
    QJsonArray scenes;
    for (const auto& scene : scenes_) {
        scenes.append(scene.toJson());
    }
    root.insert(QStringLiteral("scenes"), scenes);
    root.insert(QStringLiteral("previewIndex"), previewIndex_);
    root.insert(QStringLiteral("programIndex"), programIndex_);
    return root;
}

void SceneManager::fromJson(const QJsonObject& obj)
{
    beginResetModel();
    scenes_.clear();
    const auto arr = obj.value(QStringLiteral("scenes")).toArray();
    for (const auto& value : arr) {
        scenes_.push_back(Scene::fromJson(value.toObject()));
    }
    if (scenes_.isEmpty()) {
        loadDefaults();
    } else {
        previewIndex_ = obj.value(QStringLiteral("previewIndex")).toInt(0);
        programIndex_ = obj.value(QStringLiteral("programIndex")).toInt(0);
        ensureValidIndices();
    }
    endResetModel();
    emit scenesChanged();
    emit previewIndexChanged();
    emit programIndexChanged();
    emit sourcesChanged();
}

bool SceneManager::saveScenesToFile(const QString& path) const
{
    QSaveFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }
    file.write(QJsonDocument(toJson()).toJson(QJsonDocument::Indented));
    return file.commit();
}

bool SceneManager::loadScenesFromFile(const QString& path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }
    const auto doc = QJsonDocument::fromJson(file.readAll());
    if (!doc.isObject()) {
        return false;
    }
    fromJson(doc.object());
    return true;
}

void SceneManager::loadDefaults()
{
    const bool resetting = !scenes_.isEmpty();
    if (resetting) {
        beginResetModel();
    }
    scenes_.clear();
    const QStringList names = {
        QStringLiteral("Starting Soon"),
        QStringLiteral("Gameplay"),
        QStringLiteral("Webcam"),
        QStringLiteral("BRB"),
        QStringLiteral("Ending")
    };
    for (const auto& name : names) {
        scenes_.push_back(Scene::create(name));
    }
    // Seed Gameplay
    scenes_[1].sources.push_back(SourceFactory::create(SourceType::DisplayCapture));
    scenes_[1].sources.push_back(SourceFactory::create(SourceType::Camera));
    scenes_[1].sources.push_back(SourceFactory::create(SourceType::AudioInput, QStringLiteral("Microphone")));
    for (int i = 0; i < scenes_[1].sources.size(); ++i) {
        scenes_[1].sources[i]->setZOrder(i);
    }
    previewIndex_ = 1;
    programIndex_ = 1;
    selectedSourceIndex_ = 0;
    if (resetting) {
        endResetModel();
    }
    emit scenesChanged();
    emit previewIndexChanged();
    emit programIndexChanged();
    emit sourcesChanged();
}

bool SceneManager::validSceneIndex(int index) const
{
    return index >= 0 && index < scenes_.size();
}

Scene* SceneManager::previewScene()
{
    return validSceneIndex(previewIndex_) ? &scenes_[previewIndex_] : nullptr;
}

const Scene* SceneManager::previewScene() const
{
    return validSceneIndex(previewIndex_) ? &scenes_[previewIndex_] : nullptr;
}

void SceneManager::ensureValidIndices()
{
    if (scenes_.isEmpty()) {
        previewIndex_ = -1;
        programIndex_ = -1;
        selectedSourceIndex_ = -1;
        return;
    }
    previewIndex_ = std::clamp(previewIndex_, 0, static_cast<int>(scenes_.size() - 1));
    programIndex_ = std::clamp(programIndex_, 0, static_cast<int>(scenes_.size() - 1));
    selectedSourceIndex_ = previewScene() && !previewScene()->sources.isEmpty() ? 0 : -1;
    emit previewIndexChanged();
    emit programIndexChanged();
    emit selectedSourceChanged();
}

} // namespace nitro
