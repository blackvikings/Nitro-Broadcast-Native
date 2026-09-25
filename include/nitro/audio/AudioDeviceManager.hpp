#pragma once

#include "nitro/audio/AudioDeviceInfo.hpp"

#include <QAbstractListModel>
#include <QVector>
#include <mutex>

namespace nitro {

class AudioDeviceManager : public QAbstractListModel {
    Q_OBJECT
    Q_PROPERTY(int count READ rowCount NOTIFY devicesChanged)

public:
    enum Roles {
        IdRole = Qt::UserRole + 1,
        NameRole,
        FlowRole,
        IsDefaultRole,
        SampleRateRole,
        ChannelsRole
    };

    explicit AudioDeviceManager(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void refresh();
    Q_INVOKABLE QStringList inputDeviceNames() const;
    Q_INVOKABLE QStringList outputDeviceNames() const;
    Q_INVOKABLE QStringList inputDeviceIds() const;
    Q_INVOKABLE QStringList outputDeviceIds() const;
    Q_INVOKABLE QString defaultInputId() const;
    Q_INVOKABLE QString defaultOutputId() const;
    Q_INVOKABLE QString deviceNameForId(const QString& id) const;

    QVector<AudioDeviceInfo> devices() const;
    QVector<AudioDeviceInfo> inputs() const;
    QVector<AudioDeviceInfo> outputs() const;

signals:
    void devicesChanged();
    void errorOccurred(const QString& message);

private:
    bool enumerateWasapi();

    mutable std::mutex mutex_;
    QVector<AudioDeviceInfo> devices_;
};

} // namespace nitro
