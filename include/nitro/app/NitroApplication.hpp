#pragma once

#include "nitro/audio/AudioMixer.hpp"
#include "nitro/audio/AudioDeviceManager.hpp"
#include "nitro/audio/AudioEngine.hpp"
#include "nitro/config/NitroSettings.hpp"
#include "nitro/core/NitroEngine.hpp"
#include "nitro/core/NitroState.hpp"
#include "nitro/encoder/FFmpegManager.hpp"
#include "nitro/scenes/SceneManager.hpp"
#include "nitro/sources/CaptureDeviceHelper.hpp"
#include "nitro/sources/SourceFactory.hpp"
#include "nitro/sources/SourceListModel.hpp"
#include "nitro/sources/SourcePropertiesController.hpp"

#include <QObject>
#include <QTimer>
#include <memory>

class QQmlApplicationEngine;

namespace nitro {

class NitroApplication : public QObject {
    Q_OBJECT
    Q_PROPERTY(NitroSettings* settings READ settings CONSTANT)
    Q_PROPERTY(NitroState* state READ state CONSTANT)
    Q_PROPERTY(SceneManager* scenes READ scenes CONSTANT)
    Q_PROPERTY(SourceListModel* sources READ sources CONSTANT)
    Q_PROPERTY(SourceCatalogModel* sourceCatalog READ sourceCatalog CONSTANT)
    Q_PROPERTY(SourcePropertiesController* sourceProperties READ sourceProperties CONSTANT)
    Q_PROPERTY(CaptureDeviceHelper* captureDevices READ captureDevices CONSTANT)
    Q_PROPERTY(AudioMixer* mixer READ mixer CONSTANT)
    Q_PROPERTY(AudioEngine* audioEngine READ audioEngine CONSTANT)
    Q_PROPERTY(AudioDeviceManager* audioDevices READ audioDevices CONSTANT)
    Q_PROPERTY(QString version READ version CONSTANT)
    Q_PROPERTY(QString ffmpegStatus READ ffmpegStatus CONSTANT)

public:
    explicit NitroApplication(QObject* parent = nullptr);
    ~NitroApplication() override;

    bool initialize();
    void shutdown();

    NitroSettings* settings() const { return settings_.get(); }
    NitroState* state() const { return state_.get(); }
    SceneManager* scenes() const { return scenes_.get(); }
    SourceListModel* sources() const { return sources_.get(); }
    SourceCatalogModel* sourceCatalog() const { return sourceCatalog_.get(); }
    SourcePropertiesController* sourceProperties() const { return sourceProperties_.get(); }
    CaptureDeviceHelper* captureDevices() const { return captureDevices_.get(); }
    AudioMixer* mixer() const { return mixer_.get(); }
    AudioEngine* audioEngine() const { return audioEngine_.get(); }
    AudioDeviceManager* audioDevices() const { return audioDevices_.get(); }
    NitroEngine* engine() const { return engine_.get(); }

    QString version() const { return QStringLiteral("0.1.0"); }
    QString ffmpegStatus() const;

    void registerQmlTypes();
    void exposeToQml(QQmlApplicationEngine& qmlEngine);

private:
    std::unique_ptr<NitroSettings> settings_;
    std::unique_ptr<NitroState> state_;
    std::unique_ptr<SceneManager> scenes_;
    std::unique_ptr<SourceListModel> sources_;
    std::unique_ptr<SourceCatalogModel> sourceCatalog_;
    std::unique_ptr<SourcePropertiesController> sourceProperties_;
    std::unique_ptr<CaptureDeviceHelper> captureDevices_;
    std::unique_ptr<AudioDeviceManager> audioDevices_;
    std::unique_ptr<AudioMixer> mixer_;
    std::unique_ptr<AudioEngine> audioEngine_;
    std::unique_ptr<NitroEngine> engine_;
    std::unique_ptr<FFmpegManager> ffmpeg_;
    QTimer metricsTimer_;
};

} // namespace nitro
