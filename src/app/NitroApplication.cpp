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
    scenes_ = std::make_unique<SceneManager>(this);
    sources_ = std::make_unique<SourceListModel>(scenes_.get(), this);
    sourceCatalog_ = std::make_unique<SourceCatalogModel>(this);
    sourceProperties_ = std::make_unique<SourcePropertiesController>(scenes_.get(), this);
    captureDevices_ = std::make_unique<CaptureDeviceHelper>(this);
    audioDevices_ = std::make_unique<AudioDeviceManager>(this);
    mixer_ = std::make_unique<AudioMixer>(this);
    audioEngine_ = std::make_unique<AudioEngine>(audioDevices_.get(), this);
    audioEngine_->attachMixer(mixer_.get());

    if (!settings_->microphoneDeviceId().isEmpty()) {
        audioEngine_->setMicDeviceId(settings_->microphoneDeviceId());
    } else {
        settings_->setMicrophoneDeviceId(audioEngine_->micDeviceId());
    }
    if (!settings_->desktopDeviceId().isEmpty()) {
        audioEngine_->setDesktopDeviceId(settings_->desktopDeviceId());
    } else {
        settings_->setDesktopDeviceId(audioEngine_->desktopDeviceId());
    }

    QObject::connect(audioEngine_.get(), &AudioEngine::devicesBoundChanged, this, [this]() {
        settings_->setMicrophoneDeviceId(audioEngine_->micDeviceId());
        settings_->setDesktopDeviceId(audioEngine_->desktopDeviceId());
    });

    engine_ = std::make_unique<NitroEngine>();
    ffmpeg_ = std::make_unique<FFmpegManager>();

    if (!engine_->initialize()) {
        NITRO_LOG_ERROR(QStringLiteral("App"), QStringLiteral("Engine failed to initialize"));
        return false;
    }

    if (!audioEngine_->start()) {
        NITRO_LOG_WARN(QStringLiteral("App"), QStringLiteral("Audio engine failed to start — continuing without capture"));
    }

    QObject::connect(&metricsTimer_, &QTimer::timeout, this, [this]() {
        state_->tickSimulatedMetrics();
    });
    metricsTimer_.start(200);

    NITRO_LOG_INFO(QStringLiteral("App"),
                   QStringLiteral("Source catalog: %1 types").arg(sourceCatalog_->rowCount()));
    NITRO_LOG_INFO(QStringLiteral("App"), QStringLiteral("Application core ready"));
    return true;
}

void NitroApplication::shutdown()
{
    metricsTimer_.stop();
    if (audioEngine_) {
        audioEngine_->stop();
    }
    if (engine_) {
        engine_->shutdown();
    }
}

QString NitroApplication::ffmpegStatus() const
{
    return ffmpeg_ ? ffmpeg_->statusMessage() : QStringLiteral("Unavailable");
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
    ctx->setContextProperty(QStringLiteral("NitroMixer"), mixer_.get());
    ctx->setContextProperty(QStringLiteral("NitroAudio"), audioEngine_.get());
    ctx->setContextProperty(QStringLiteral("NitroAudioDevices"), audioDevices_.get());
}

} // namespace nitro
