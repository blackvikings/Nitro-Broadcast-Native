#include "nitro/app/NitroApplication.hpp"

#include "nitro/diagnostics/NitroLogger.hpp"

#include <QQmlApplicationEngine>
#include <QQmlContext>

namespace nitro {

NitroApplication::NitroApplication(QObject* parent)
    : QObject(parent)
{
}

NitroApplication::~NitroApplication()
{
    shutdown();
}

bool NitroApplication::initialize()
{
    NITRO_LOG_INFO(QStringLiteral("App"), QStringLiteral("Nitro Broadcast %1 starting").arg(version()));

    settings_ = std::make_unique<NitroSettings>(this);
    state_ = std::make_unique<NitroState>(this);
    engine_ = std::make_unique<NitroEngine>();
    if (!engine_->initialize()) {
        NITRO_LOG_ERROR(QStringLiteral("App"), QStringLiteral("NitroEngine failed to initialize"));
        return false;
    }

    // Restore device selections into the engine-owned AudioEngine
    if (auto* audio = engine_->audioEngine()) {
        if (!settings_->microphoneDeviceId().isEmpty()) {
            audio->setMicDeviceId(settings_->microphoneDeviceId());
        } else {
            settings_->setMicrophoneDeviceId(audio->micDeviceId());
        }
        if (!settings_->desktopDeviceId().isEmpty()) {
            audio->setDesktopDeviceId(settings_->desktopDeviceId());
        } else {
            settings_->setDesktopDeviceId(audio->desktopDeviceId());
        }
        QObject::connect(audio, &AudioEngine::devicesBoundChanged, this, [this, audio]() {
            settings_->setMicrophoneDeviceId(audio->micDeviceId());
            settings_->setDesktopDeviceId(audio->desktopDeviceId());
        });
    }

    scenes_ = std::make_unique<SceneManager>(this);
    if (engine_->transitions()) {
        scenes_->setTransitionEngine(engine_->transitions());
    }
    sources_ = std::make_unique<SourceListModel>(scenes_.get(), this);
    sourceCatalog_ = std::make_unique<SourceCatalogModel>(this);
    sourceProperties_ = std::make_unique<SourcePropertiesController>(scenes_.get(), this);
    captureDevices_ = std::make_unique<CaptureDeviceHelper>(this);

    QObject::connect(&metricsTimer_, &QTimer::timeout, this, [this]() {
        state_->tickSimulatedMetrics();
    });
    metricsTimer_.start(200);

    QObject::connect(&transitionTimer_, &QTimer::timeout, this, [this]() {
        if (engine_ && engine_->transitions()) {
            engine_->transitions()->tick();
        }
    });
    transitionTimer_.start(16);

    NITRO_LOG_INFO(QStringLiteral("App"),
                   QStringLiteral("Source catalog: %1 types").arg(sourceCatalog_->rowCount()));
    NITRO_LOG_INFO(QStringLiteral("App"), QStringLiteral("Application core ready"));
    return true;
}

void NitroApplication::shutdown()
{
    metricsTimer_.stop();
    transitionTimer_.stop();
    if (engine_) {
        engine_->shutdown();
    }
}

QString NitroApplication::ffmpegStatus() const
{
    return engine_ && engine_->ffmpeg() ? engine_->ffmpeg()->statusMessage()
                                        : QStringLiteral("Unavailable");
}

void NitroApplication::registerQmlTypes()
{
}

void NitroApplication::exposeToQml(QQmlApplicationEngine& qmlEngine)
{
    auto* ctx = qmlEngine.rootContext();
    ctx->setContextProperty(QStringLiteral("NitroApp"), this);
    ctx->setContextProperty(QStringLiteral("NitroSettings"), settings_.get());
    ctx->setContextProperty(QStringLiteral("NitroState"), state_.get());
    ctx->setContextProperty(QStringLiteral("NitroScenes"), scenes_.get());
    ctx->setContextProperty(QStringLiteral("NitroSources"), sources_.get());
    ctx->setContextProperty(QStringLiteral("NitroSourceCatalog"), sourceCatalog_.get());
    ctx->setContextProperty(QStringLiteral("NitroSourceProps"), sourceProperties_.get());
    ctx->setContextProperty(QStringLiteral("NitroCapture"), captureDevices_.get());
    ctx->setContextProperty(QStringLiteral("NitroMixer"), mixer());
    ctx->setContextProperty(QStringLiteral("NitroAudio"), audioEngine());
    ctx->setContextProperty(QStringLiteral("NitroAudioDevices"), audioDevices());
    ctx->setContextProperty(QStringLiteral("NitroTransitions"), transitions());
}

} // namespace nitro
