#pragma once

#include "nitro/audio/AudioFormat.hpp"

#include <QAbstractListModel>
#include <QObject>
#include <QString>
#include <QVector>

namespace nitro {

class AudioEngine;

/**
 * UI-facing channel strip.
 *
 * Gain ordering: Capture → Gain → Meter (UI) → Mute → Mixer
 * Mute: meter continues to show post-gain pre-mute signal; mix receives silence.
 */
class AudioChannel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString id READ id CONSTANT)
    Q_PROPERTY(QString name READ name CONSTANT)
    Q_PROPERTY(QString sourceName READ sourceName NOTIFY sourceNameChanged)
    Q_PROPERTY(QString sourceKind READ sourceKind NOTIFY sourceNameChanged)
    Q_PROPERTY(double volume READ volume WRITE setVolume NOTIFY volumeChanged)
    Q_PROPERTY(double gainDb READ gainDb NOTIFY volumeChanged)
    Q_PROPERTY(bool muted READ muted WRITE setMuted NOTIFY mutedChanged)
    Q_PROPERTY(bool monitoring READ monitoring WRITE setMonitoring NOTIFY monitoringChanged)
    Q_PROPERTY(double level READ level NOTIFY levelChanged)
    Q_PROPERTY(double peakDb READ peakDb NOTIFY levelChanged)
    Q_PROPERTY(double peakHoldDb READ peakHoldDb NOTIFY levelChanged)
    Q_PROPERTY(bool levelSimulated READ levelSimulated CONSTANT)
    Q_PROPERTY(bool hasCapture READ hasCapture NOTIFY sourceNameChanged)

public:
    explicit AudioChannel(QString id, QString name, AudioSourceKind kind, QObject* parent = nullptr);

    QString id() const { return id_; }
    QString name() const { return name_; }
    QString sourceName() const { return sourceName_; }
    QString sourceKind() const;
    void setSourceName(const QString& name);

    AudioSourceKind kind() const { return kind_; }
    bool hasCapture() const
    {
        return kind_ == AudioSourceKind::Microphone || kind_ == AudioSourceKind::DesktopLoopback;
    }

    double volume() const { return volume_; }
    void setVolume(double volume); // 0..1 UI fader → -60..+12 dB

    double gainDb() const { return gainDb_; }

    bool muted() const { return muted_; }
    void setMuted(bool muted);

    bool monitoring() const { return monitoring_; }
    void setMonitoring(bool monitoring);

    double level() const { return level_; }
    double peakDb() const { return peakDb_; }
    double peakHoldDb() const { return peakHoldDb_; }
    bool levelSimulated() const { return false; }

    void setEngine(AudioEngine* engine) { engine_ = engine; }
    void updateMeter(float uiLevel, float peakDb, float peakHoldDb);

signals:
    void volumeChanged();
    void mutedChanged();
    void monitoringChanged();
    void levelChanged();
    void sourceNameChanged();

private:
    static double volumeToGainDb(double volume);
    static float gainDbToLinear(double db);

    QString id_;
    QString name_;
    QString sourceName_ = QStringLiteral("Unassigned");
    AudioSourceKind kind_ = AudioSourceKind::None;
    double volume_ = 0.8;   // UI 0..1
    double gainDb_ = 0.0;
    bool muted_ = false;
    bool monitoring_ = false;
    double level_ = 0.0;
    double peakDb_ = -120.0;
    double peakHoldDb_ = -120.0;
    AudioEngine* engine_ = nullptr;
};

class AudioMixer : public QAbstractListModel {
    Q_OBJECT
    Q_PROPERTY(int count READ rowCount NOTIFY channelsChanged)
    Q_PROPERTY(double masterLevel READ masterLevel NOTIFY masterMeterChanged)
    Q_PROPERTY(double masterPeakDb READ masterPeakDb NOTIFY masterMeterChanged)
    Q_PROPERTY(double masterGainDb READ masterGainDb WRITE setMasterGainDb NOTIFY masterChanged)
    Q_PROPERTY(bool masterMuted READ masterMuted WRITE setMasterMuted NOTIFY masterChanged)

public:
    enum Roles {
        ChannelRole = Qt::UserRole + 1,
        NameRole,
        SourceNameRole,
        VolumeRole,
        MutedRole,
        MonitoringRole,
        LevelRole,
        PeakDbRole,
        PeakHoldDbRole,
        HasCaptureRole
    };

    explicit AudioMixer(QObject* parent = nullptr);

    void setEngine(AudioEngine* engine);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE AudioChannel* channelAt(int index) const;
    Q_INVOKABLE void setVolume(int index, double volume);
    Q_INVOKABLE void setMuted(int index, bool muted);
    Q_INVOKABLE void setMonitoring(int index, bool monitoring);

    double masterLevel() const { return masterLevel_; }
    double masterPeakDb() const { return masterPeakDb_; }
    double masterGainDb() const { return masterGainDb_; }
    void setMasterGainDb(double db);
    bool masterMuted() const { return masterMuted_; }
    void setMasterMuted(bool muted);

    void refreshFromEngine();
    void loadDefaults();

signals:
    void channelsChanged();
    void masterMeterChanged();
    void masterChanged();

private:
    AudioEngine* engine_ = nullptr;
    QVector<AudioChannel*> channels_;
    double masterLevel_ = 0.0;
    double masterPeakDb_ = -120.0;
    double masterGainDb_ = 0.0;
    bool masterMuted_ = false;
};

} // namespace nitro
