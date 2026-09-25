#include "nitro/core/NitroState.hpp"

#include "nitro/diagnostics/NitroLogger.hpp"

#include <cstdlib>

namespace nitro {

NitroState::NitroState(QObject* parent)
    : QObject(parent)
{
}

QString NitroState::streamStatusText() const
{
    return streamingUiActive_ ? QStringLiteral("LIVE (UI sim)") : QStringLiteral("Idle");
}

QString NitroState::recordStatusText() const
{
    return recordingUiActive_ ? QStringLiteral("REC (UI sim)") : QStringLiteral("Idle");
}

void NitroState::setSettingsOpen(bool open)
{
    if (settingsOpen_ == open) {
        return;
    }
    settingsOpen_ = open;
    emit settingsOpenChanged();
}

void NitroState::toggleStreamingSimulation()
{
    streamingUiActive_ = !streamingUiActive_;
    if (streamingUiActive_) {
        bitrateKbps_ = 6000;
        networkStatus_ = QStringLiteral("Simulated");
        encoderName_ = QStringLiteral("Not connected");
        NITRO_LOG_WARN(QStringLiteral("Stream"),
                       QStringLiteral("Streaming UI toggled ON — no real RTMP output yet"));
    } else {
        bitrateKbps_ = 0;
        networkStatus_ = QStringLiteral("Idle");
        encoderName_ = QStringLiteral("—");
        NITRO_LOG_INFO(QStringLiteral("Stream"), QStringLiteral("Streaming UI toggled OFF"));
    }
    emit streamingUiChanged();
    emit metricsChanged();
}

void NitroState::toggleRecordingSimulation()
{
    recordingUiActive_ = !recordingUiActive_;
    NITRO_LOG_WARN(QStringLiteral("Record"),
                   recordingUiActive_
                       ? QStringLiteral("Recording UI toggled ON — no real file output yet")
                       : QStringLiteral("Recording UI toggled OFF"));
    emit recordingUiChanged();
}

void NitroState::tickSimulatedMetrics()
{
    cpuUsage_ = 6.0 + (std::rand() % 20);
    gpuUsage_ = 10.0 + (std::rand() % 25);
    if (streamingUiActive_) {
        droppedFrames_ = (std::rand() % 100 == 0) ? droppedFrames_ + 1 : droppedFrames_;
        bitrateKbps_ = 5800 + (std::rand() % 400);
    }
    emit metricsChanged();
}

} // namespace nitro
