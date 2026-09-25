#pragma once

#include <QObject>
#include <QString>

namespace nitro {

/// Runtime UI / session state separate from media engine state.
class NitroState : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool streamingUiActive READ streamingUiActive NOTIFY streamingUiChanged)
    Q_PROPERTY(bool recordingUiActive READ recordingUiActive NOTIFY recordingUiChanged)
    Q_PROPERTY(QString streamStatusText READ streamStatusText NOTIFY streamingUiChanged)
    Q_PROPERTY(QString recordStatusText READ recordStatusText NOTIFY recordingUiChanged)
    Q_PROPERTY(double cpuUsage READ cpuUsage NOTIFY metricsChanged)
    Q_PROPERTY(double gpuUsage READ gpuUsage NOTIFY metricsChanged)
    Q_PROPERTY(int droppedFrames READ droppedFrames NOTIFY metricsChanged)
    Q_PROPERTY(int bitrateKbps READ bitrateKbps NOTIFY metricsChanged)
    Q_PROPERTY(QString networkStatus READ networkStatus NOTIFY metricsChanged)
    Q_PROPERTY(QString encoderName READ encoderName NOTIFY metricsChanged)
    Q_PROPERTY(bool metricsSimulated READ metricsSimulated NOTIFY metricsChanged)
    Q_PROPERTY(bool settingsOpen READ settingsOpen WRITE setSettingsOpen NOTIFY settingsOpenChanged)

public:
    explicit NitroState(QObject* parent = nullptr);

    bool streamingUiActive() const { return streamingUiActive_; }
    bool recordingUiActive() const { return recordingUiActive_; }
    QString streamStatusText() const;
    QString recordStatusText() const;

    double cpuUsage() const { return cpuUsage_; }
    double gpuUsage() const { return gpuUsage_; }
    int droppedFrames() const { return droppedFrames_; }
    int bitrateKbps() const { return bitrateKbps_; }
    QString networkStatus() const { return networkStatus_; }
    QString encoderName() const { return encoderName_; }
    /// True until real performance counters are wired.
    bool metricsSimulated() const { return metricsSimulated_; }

    bool settingsOpen() const { return settingsOpen_; }
    void setSettingsOpen(bool open);

    /// UI-only toggles. Does NOT start real streaming/recording.
    Q_INVOKABLE void toggleStreamingSimulation();
    Q_INVOKABLE void toggleRecordingSimulation();
    Q_INVOKABLE void tickSimulatedMetrics();

signals:
    void streamingUiChanged();
    void recordingUiChanged();
    void metricsChanged();
    void settingsOpenChanged();

private:
    bool streamingUiActive_ = false;
    bool recordingUiActive_ = false;
    bool settingsOpen_ = false;
    bool metricsSimulated_ = true;
    double cpuUsage_ = 8.0;
    double gpuUsage_ = 12.0;
    int droppedFrames_ = 0;
    int bitrateKbps_ = 0;
    QString networkStatus_ = QStringLiteral("Idle");
    QString encoderName_ = QStringLiteral("—");
};

} // namespace nitro
