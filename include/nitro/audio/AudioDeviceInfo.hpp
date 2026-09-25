#pragma once

#include <QString>
#include <QMetaType>

namespace nitro {

enum class AudioDeviceFlow {
    Input,
    Output
};

struct AudioDeviceInfo {
    QString id;          // WASAPI endpoint ID
    QString name;
    AudioDeviceFlow flow = AudioDeviceFlow::Input;
    int maxChannels = 0;
    int defaultSampleRate = 0;
    bool isDefault = false;
    bool isConnected = true;
};

} // namespace nitro

Q_DECLARE_METATYPE(nitro::AudioDeviceInfo)
