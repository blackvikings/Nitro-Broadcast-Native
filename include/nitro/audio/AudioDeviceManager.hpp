#pragma once

#include "nitro/audio/AudioDeviceInfo.hpp"

#include <QAbstractListModel>
#include <QVector>
#include <mutex>

namespace nitro {

class AudioDeviceNotificationClient;

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
        ChannelsRole,
        ConnectedRole
    };

    explicit AudioDeviceManager(QObject* parent = nullptr);
    ~AudioDeviceManager() override;

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
    Q_INVOKABLE bool isDeviceConnected(const QString& id) const;

    QVector<AudioDeviceInfo> devices() const;
    QVector<AudioDeviceInfo> inputs() const;
    QVector<AudioDeviceInfo> outputs() const;

    /// Called from IMMNotificationClient (may be any thread) — marshals to Qt.
    void handleEndpointAdded(const QString& id);
    void handleEndpointRemoved(const QString& id);
    void handleDefaultChanged();
    void handleEndpointStateChanged(const QString& id, bool active);

signals:
    void devicesChanged();
    void deviceAdded(const QString& id);
    void deviceRemoved(const QString& id);
    void defaultDeviceChanged();
    void errorOccurred(const QString& message);

private:
    bool enumerateWasapi();
    void startNotifications();
    void stopNotifications();

    mutable std::mutex mutex_;
    QVector<AudioDeviceInfo> devices_;
    AudioDeviceNotificationClient* notifier_ = nullptr; // COM-owned; Release in stopNotifications
};

} // namespace nitro
