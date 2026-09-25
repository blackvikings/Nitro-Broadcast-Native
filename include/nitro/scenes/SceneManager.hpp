#pragma once

#include "nitro/scenes/Scene.hpp"
#include "nitro/sources/ISource.hpp"

#include <QAbstractListModel>
#include <QJsonObject>

namespace nitro {

class TransitionEngine;

class SceneManager : public QAbstractListModel {
    Q_OBJECT
    Q_PROPERTY(int count READ rowCount NOTIFY scenesChanged)
    Q_PROPERTY(int previewIndex READ previewIndex WRITE setPreviewIndex NOTIFY previewIndexChanged)
    Q_PROPERTY(int programIndex READ programIndex WRITE setProgramIndex NOTIFY programIndexChanged)
    Q_PROPERTY(QString previewSceneName READ previewSceneName NOTIFY previewIndexChanged)
    Q_PROPERTY(QString programSceneName READ programSceneName NOTIFY programIndexChanged)
    Q_PROPERTY(int selectedSourceIndex READ selectedSourceIndex NOTIFY selectedSourceChanged)

public:
    enum Roles {
        IdRole = Qt::UserRole + 1,
        NameRole,
        IsPreviewRole,
        IsProgramRole
    };

    explicit SceneManager(QObject* parent = nullptr);

    void setTransitionEngine(TransitionEngine* transitions);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    int previewIndex() const { return previewIndex_; }
    void setPreviewIndex(int index);

    int programIndex() const { return programIndex_; }
    void setProgramIndex(int index);

    QString previewSceneName() const;
    QString programSceneName() const;

    Q_INVOKABLE QString createScene(const QString& name = QString());
    Q_INVOKABLE bool deleteScene(int index);
    Q_INVOKABLE bool renameScene(int index, const QString& name);
    Q_INVOKABLE QString duplicateScene(int index);
    Q_INVOKABLE bool selectPreview(int index);
    Q_INVOKABLE void cutToProgram();
    Q_INVOKABLE void fadeToProgram(int durationMs = 300);

    Q_INVOKABLE int sourceCount() const;
    Q_INVOKABLE QVariantMap sourceAt(int index) const;
    Q_INVOKABLE QString addSource(const QString& typeKey, const QString& name = QString());
    Q_INVOKABLE bool removeSource(int index);
    Q_INVOKABLE bool setSourceEnabled(int index, bool enabled);
    Q_INVOKABLE bool setSourceVisible(int index, bool visible);
    Q_INVOKABLE bool setSourceLocked(int index, bool locked);
    Q_INVOKABLE bool renameSource(int index, const QString& name);
    Q_INVOKABLE bool moveSource(int from, int to);
    Q_INVOKABLE bool setSourceProperty(int index, const QString& key, const QVariant& value);
    Q_INVOKABLE bool resetSourceTransform(int index);
    Q_INVOKABLE bool fitSourceToCanvas(int index);
    Q_INVOKABLE bool centerSource(int index);
    Q_INVOKABLE bool stretchSourceToCanvas(int index);
    Q_INVOKABLE int selectedSourceIndex() const { return selectedSourceIndex_; }
    Q_INVOKABLE void selectSource(int index);

    Q_INVOKABLE bool saveScenesToFile(const QString& path) const;
    Q_INVOKABLE bool loadScenesFromFile(const QString& path);

    std::shared_ptr<ISource> sourcePtrAt(int index) const;
    const QVector<Scene>& scenes() const { return scenes_; }

    QJsonObject toJson() const;
    void fromJson(const QJsonObject& obj);
    void loadDefaults();

signals:
    void scenesChanged();
    void previewIndexChanged();
    void programIndexChanged();
    void sourcesChanged();
    void selectedSourceChanged();
    void transitionPerformed(const QString& type);
    void sourceError(const QString& message);

private:
    bool validSceneIndex(int index) const;
    Scene* previewScene();
    const Scene* previewScene() const;
    void ensureValidIndices();
    double canvasWidth() const { return 1920.0; }
    double canvasHeight() const { return 1080.0; }
    void onTransitionCompleted(int toIndex);

    QVector<Scene> scenes_;
    int previewIndex_ = 0;
    int programIndex_ = 0;
    int selectedSourceIndex_ = -1;
    TransitionEngine* transitions_ = nullptr;
};

} // namespace nitro
