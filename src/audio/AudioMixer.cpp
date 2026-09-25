#include "nitro/audio/AudioMixer.hpp"

#include "nitro/audio/AudioEngine.hpp"
#include "nitro/audio/AudioMeter.hpp"
#include "nitro/diagnostics/NitroLogger.hpp"

#include <QtMath>
#include <algorithm>

namespace nitro {

AudioChannel::AudioChannel(QString id, QString name, AudioSourceKind kind, QObject* parent)
    : QObject(parent)
    , id_(std::move(id))
    , name_(std::move(name))
    , kind_(kind)
{
    gainDb_ = volumeToGainDb(volume_);
    if (!hasCapture()) {
        sourceName_ = QStringLiteral("No source yet");
    }
}

QString AudioChannel::sourceKind() const
{
    return QString::fromUtf8(audioSourceKindName(kind_));
}

void AudioChannel::setSourceName(const QString& name)
{
    if (sourceName_ == name) {
        return;
    }
    sourceName_ = name;
    emit sourceNameChanged();
}

double AudioChannel::volumeToGainDb(double volume)
{
    volume = std::clamp(volume, 0.0, 1.0);
    // Map 0..1 → -60..+12 dB (0.8 ≈ 0 dB)
    // Using: db = -60 + volume * 72
    return -60.0 + volume * 72.0;
}

float AudioChannel::gainDbToLinear(double db)
{
    return dbToLinear(static_cast<float>(db));
}

void AudioChannel::setVolume(double volume)
{
    volume = std::clamp(volume, 0.0, 1.0);
    if (qFuzzyCompare(volume_, volume)) {
        return;
    }
    volume_ = volume;
    gainDb_ = volumeToGainDb(volume_);
    if (engine_) {
        engine_->setChannelGainDb(id_, gainDb_);
    }
    emit volumeChanged();
}

void AudioChannel::setMuted(bool muted)
{
    if (muted_ == muted) {
        return;
    }
    muted_ = muted;
    if (engine_) {
        engine_->setChannelMuted(id_, muted_);
    }
    emit mutedChanged();
}

void AudioChannel::setMonitoring(bool monitoring)
{
    if (monitoring_ == monitoring) {
        return;
    }
    monitoring_ = monitoring;
    if (engine_) {
        engine_->setChannelMonitoring(id_, monitoring_);
    }
    emit monitoringChanged();
}

void AudioChannel::updateMeter(float uiLevel, float peakDb, float peakHoldDb)
{
    level_ = uiLevel;
    peakDb_ = peakDb;
    peakHoldDb_ = peakHoldDb;
    emit levelChanged();
}

AudioMixer::AudioMixer(QObject* parent)
    : QAbstractListModel(parent)
{
    loadDefaults();
}

void AudioMixer::setEngine(AudioEngine* engine)
{
    engine_ = engine;
    for (auto* ch : channels_) {
        ch->setEngine(engine_);
        if (engine_) {
            engine_->setChannelGainDb(ch->id(), ch->gainDb());
            engine_->setChannelMuted(ch->id(), ch->muted());
            engine_->setChannelMonitoring(ch->id(), ch->monitoring());
        }
    }
}

int AudioMixer::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return channels_.size();
}

QVariant AudioMixer::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= channels_.size()) {
        return {};
    }
    AudioChannel* ch = channels_.at(index.row());
    switch (role) {
    case ChannelRole: return QVariant::fromValue(ch);
    case NameRole: return ch->name();
    case SourceNameRole: return ch->sourceName();
    case VolumeRole: return ch->volume();
    case MutedRole: return ch->muted();
    case MonitoringRole: return ch->monitoring();
    case LevelRole: return ch->level();
    case PeakDbRole: return ch->peakDb();
    case PeakHoldDbRole: return ch->peakHoldDb();
    case HasCaptureRole: return ch->hasCapture();
    default: return {};
    }
}

QHash<int, QByteArray> AudioMixer::roleNames() const
{
    return {
        {ChannelRole, "channel"},
        {NameRole, "name"},
        {SourceNameRole, "sourceName"},
        {VolumeRole, "volume"},
        {MutedRole, "muted"},
        {MonitoringRole, "monitoring"},
        {LevelRole, "level"},
        {PeakDbRole, "peakDb"},
        {PeakHoldDbRole, "peakHoldDb"},
        {HasCaptureRole, "hasCapture"}
    };
}

AudioChannel* AudioMixer::channelAt(int index) const
{
    if (index < 0 || index >= channels_.size()) {
        return nullptr;
    }
    return channels_.at(index);
}

void AudioMixer::setVolume(int index, double volume)
{
    if (auto* ch = channelAt(index)) {
        ch->setVolume(volume);
        emit dataChanged(this->index(index), this->index(index), {VolumeRole});
    }
}

void AudioMixer::setMuted(int index, bool muted)
{
    if (auto* ch = channelAt(index)) {
        ch->setMuted(muted);
        emit dataChanged(this->index(index), this->index(index), {MutedRole});
    }
}

void AudioMixer::setMonitoring(int index, bool monitoring)
{
    if (auto* ch = channelAt(index)) {
        ch->setMonitoring(monitoring);
        emit dataChanged(this->index(index), this->index(index), {MonitoringRole});
    }
}

void AudioMixer::setMasterGainDb(double db)
{
    db = std::clamp(db, -60.0, 12.0);
    if (qFuzzyCompare(masterGainDb_, db)) {
        return;
    }
    masterGainDb_ = db;
    if (engine_) {
        engine_->setMasterGainDb(db);
    }
    emit masterChanged();
}

void AudioMixer::setMasterMuted(bool muted)
{
    if (masterMuted_ == muted) {
        return;
    }
    masterMuted_ = muted;
    if (engine_) {
        engine_->setMasterMuted(muted);
    }
    emit masterChanged();
}

void AudioMixer::refreshFromEngine()
{
    if (!engine_) {
        return;
    }
    for (int i = 0; i < channels_.size(); ++i) {
        AudioChannel* ch = channels_[i];
        const auto levels = engine_->channelMeter(ch->id());
        ch->updateMeter(engine_->channelUiLevel(ch->id()), levels.peakDb, levels.peakHoldDb);
        emit dataChanged(this->index(i), this->index(i),
                         {LevelRole, PeakDbRole, PeakHoldDbRole, SourceNameRole});
    }
    masterLevel_ = engine_->masterLevel();
    masterPeakDb_ = engine_->masterPeakDb();
    emit masterMeterChanged();
}

void AudioMixer::loadDefaults()
{
    beginResetModel();
    qDeleteAll(channels_);
    channels_.clear();

    struct Def {
        const char* id;
        const char* name;
        AudioSourceKind kind;
    };
    const Def defs[] = {
        {"mic", "MIC", AudioSourceKind::Microphone},
        {"game", "GAME", AudioSourceKind::Application},
        {"desktop", "DESKTOP", AudioSourceKind::DesktopLoopback},
        {"music", "MUSIC", AudioSourceKind::MediaFile},
        {"browser", "BROWSER", AudioSourceKind::Browser},
    };
    for (const auto& d : defs) {
        channels_.push_back(new AudioChannel(QString::fromUtf8(d.id), QString::fromUtf8(d.name), d.kind, this));
    }
    endResetModel();
    emit channelsChanged();
    NITRO_LOG_INFO(QStringLiteral("Audio"),
                   QStringLiteral("Mixer channels ready (MIC+DESKTOP capture; others unassigned)"));
}

} // namespace nitro
