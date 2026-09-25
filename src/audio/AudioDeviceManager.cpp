#include "nitro/audio/AudioDeviceManager.hpp"

#include "nitro/diagnostics/NitroLogger.hpp"

#include <QMetaObject>
#include <QVector>
#include <mutex>

#ifdef _WIN32
#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#  endif
#  ifndef NOMINMAX
#    define NOMINMAX
#  endif
#  include <windows.h>
#  include <mmdeviceapi.h>
#  include <audioclient.h>
#  include <functiondiscoverykeys_devpkey.h>
#  include <wrl/client.h>
#  pragma comment(lib, "Ole32.lib")
#endif

namespace nitro {

#ifdef _WIN32
class AudioDeviceNotificationClient final : public IMMNotificationClient {
public:
    explicit AudioDeviceNotificationClient(AudioDeviceManager* owner)
        : owner_(owner)
    {
    }

    // IUnknown
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv) override
    {
        if (!ppv) {
            return E_POINTER;
        }
        if (riid == __uuidof(IUnknown) || riid == __uuidof(IMMNotificationClient)) {
            *ppv = static_cast<IMMNotificationClient*>(this);
            AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }
    ULONG STDMETHODCALLTYPE AddRef() override { return InterlockedIncrement(&ref_); }
    ULONG STDMETHODCALLTYPE Release() override
    {
        const ULONG r = InterlockedDecrement(&ref_);
        if (r == 0) {
            delete this;
        }
        return r;
    }

    HRESULT STDMETHODCALLTYPE OnDeviceStateChanged(LPCWSTR deviceId, DWORD newState) override
    {
        if (!owner_ || !deviceId) {
            return S_OK;
        }
        const QString id = QString::fromWCharArray(deviceId);
        const bool active = (newState == DEVICE_STATE_ACTIVE);
        QMetaObject::invokeMethod(owner_, [owner = owner_, id, active]() {
            owner->handleEndpointStateChanged(id, active);
        }, Qt::QueuedConnection);
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE OnDeviceAdded(LPCWSTR deviceId) override
    {
        if (!owner_ || !deviceId) {
            return S_OK;
        }
        const QString id = QString::fromWCharArray(deviceId);
        QMetaObject::invokeMethod(owner_, [owner = owner_, id]() {
            owner->handleEndpointAdded(id);
        }, Qt::QueuedConnection);
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE OnDeviceRemoved(LPCWSTR deviceId) override
    {
        if (!owner_ || !deviceId) {
            return S_OK;
        }
        const QString id = QString::fromWCharArray(deviceId);
        QMetaObject::invokeMethod(owner_, [owner = owner_, id]() {
            owner->handleEndpointRemoved(id);
        }, Qt::QueuedConnection);
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE OnDefaultDeviceChanged(EDataFlow, ERole, LPCWSTR) override
    {
        if (!owner_) {
            return S_OK;
        }
        QMetaObject::invokeMethod(owner_, [owner = owner_]() {
            owner->handleDefaultChanged();
        }, Qt::QueuedConnection);
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE OnPropertyValueChanged(LPCWSTR, const PROPERTYKEY) override
    {
        return S_OK;
    }

    void setEnumerator(IMMDeviceEnumerator* e) { enumerator_ = e; }
    IMMDeviceEnumerator* enumerator() const { return enumerator_; }

private:
    AudioDeviceManager* owner_ = nullptr;
    IMMDeviceEnumerator* enumerator_ = nullptr;
    LONG ref_ = 1;
};
#else
class AudioDeviceNotificationClient {
public:
    explicit AudioDeviceNotificationClient(AudioDeviceManager*) {}
};
#endif

namespace {

#ifdef _WIN32
QString hresultToString(HRESULT hr)
{
    return QStringLiteral("HRESULT 0x%1").arg(static_cast<quint32>(hr), 8, 16, QLatin1Char('0'));
}

bool getDeviceName(IMMDevice* device, QString& outName)
{
    Microsoft::WRL::ComPtr<IPropertyStore> props;
    if (FAILED(device->OpenPropertyStore(STGM_READ, &props))) {
        return false;
    }
    PROPVARIANT varName;
    PropVariantInit(&varName);
    const HRESULT hr = props->GetValue(PKEY_Device_FriendlyName, &varName);
    if (SUCCEEDED(hr) && varName.vt == VT_LPWSTR) {
        outName = QString::fromWCharArray(varName.pwszVal);
    }
    PropVariantClear(&varName);
    return SUCCEEDED(hr);
}

void collectEndpoints(IMMDeviceEnumerator* enumerator, EDataFlow flow, AudioDeviceFlow nitroFlow,
                      QVector<AudioDeviceInfo>& out)
{
    Microsoft::WRL::ComPtr<IMMDeviceCollection> collection;
    if (FAILED(enumerator->EnumAudioEndpoints(flow, DEVICE_STATE_ACTIVE, &collection))) {
        return;
    }

    Microsoft::WRL::ComPtr<IMMDevice> defaultDev;
    enumerator->GetDefaultAudioEndpoint(flow, eConsole, &defaultDev);
    LPWSTR defaultId = nullptr;
    QString defaultIdStr;
    if (defaultDev) {
        defaultDev->GetId(&defaultId);
        if (defaultId) {
            defaultIdStr = QString::fromWCharArray(defaultId);
            CoTaskMemFree(defaultId);
        }
    }

    UINT count = 0;
    collection->GetCount(&count);
    for (UINT i = 0; i < count; ++i) {
        Microsoft::WRL::ComPtr<IMMDevice> device;
        if (FAILED(collection->Item(i, &device))) {
            continue;
        }
        LPWSTR id = nullptr;
        if (FAILED(device->GetId(&id)) || !id) {
            continue;
        }
        AudioDeviceInfo info;
        info.id = QString::fromWCharArray(id);
        CoTaskMemFree(id);
        getDeviceName(device.Get(), info.name);
        if (info.name.isEmpty()) {
            info.name = info.id;
        }
        info.flow = nitroFlow;
        info.isDefault = (info.id == defaultIdStr);
        info.isConnected = true;

        Microsoft::WRL::ComPtr<IAudioClient> client;
        if (SUCCEEDED(device->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, &client))) {
            WAVEFORMATEX* mix = nullptr;
            if (SUCCEEDED(client->GetMixFormat(&mix)) && mix) {
                info.defaultSampleRate = static_cast<int>(mix->nSamplesPerSec);
                info.maxChannels = static_cast<int>(mix->nChannels);
                CoTaskMemFree(mix);
            }
        }
        out.push_back(info);
    }
}
#endif

} // namespace

AudioDeviceManager::AudioDeviceManager(QObject* parent)
    : QAbstractListModel(parent)
{
    refresh();
    startNotifications();
}

AudioDeviceManager::~AudioDeviceManager()
{
    stopNotifications();
}

void AudioDeviceManager::startNotifications()
{
#ifdef _WIN32
    stopNotifications();
    HRESULT hrInit = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    Q_UNUSED(hrInit);

    Microsoft::WRL::ComPtr<IMMDeviceEnumerator> enumerator;
    if (FAILED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
                                 IID_PPV_ARGS(&enumerator)))) {
        return;
    }
    auto* client = new AudioDeviceNotificationClient(this);
    IMMDeviceEnumerator* rawEnum = enumerator.Detach();
    client->setEnumerator(rawEnum);
    if (FAILED(rawEnum->RegisterEndpointNotificationCallback(client))) {
        rawEnum->Release();
        client->Release();
        return;
    }
    notifier_ = client;
    NITRO_LOG_INFO(QStringLiteral("Audio"), QStringLiteral("WASAPI device notifications registered"));
#endif
}

void AudioDeviceManager::stopNotifications()
{
#ifdef _WIN32
    if (!notifier_) {
        return;
    }
    if (notifier_->enumerator()) {
        notifier_->enumerator()->UnregisterEndpointNotificationCallback(notifier_);
        notifier_->enumerator()->Release();
        notifier_->setEnumerator(nullptr);
    }
    notifier_->Release();
    notifier_ = nullptr;
#endif
}

int AudioDeviceManager::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    std::lock_guard lock(mutex_);
    return devices_.size();
}

QVariant AudioDeviceManager::data(const QModelIndex& index, int role) const
{
    std::lock_guard lock(mutex_);
    if (!index.isValid() || index.row() < 0 || index.row() >= devices_.size()) {
        return {};
    }
    const auto& d = devices_.at(index.row());
    switch (role) {
    case IdRole: return d.id;
    case NameRole: return d.name;
    case FlowRole: return d.flow == AudioDeviceFlow::Input ? QStringLiteral("input") : QStringLiteral("output");
    case IsDefaultRole: return d.isDefault;
    case SampleRateRole: return d.defaultSampleRate;
    case ChannelsRole: return d.maxChannels;
    case ConnectedRole: return d.isConnected;
    default: return {};
    }
}

QHash<int, QByteArray> AudioDeviceManager::roleNames() const
{
    return {
        {IdRole, "deviceId"},
        {NameRole, "name"},
        {FlowRole, "flow"},
        {IsDefaultRole, "isDefault"},
        {SampleRateRole, "sampleRate"},
        {ChannelsRole, "channels"},
        {ConnectedRole, "connected"}
    };
}

void AudioDeviceManager::refresh()
{
    beginResetModel();
    {
        std::lock_guard lock(mutex_);
        devices_.clear();
        if (!enumerateWasapi()) {
            NITRO_LOG_ERROR(QStringLiteral("Audio"), QStringLiteral("Device enumeration failed"));
        }
    }
    endResetModel();
    emit devicesChanged();
    NITRO_LOG_INFO(QStringLiteral("Audio"), QStringLiteral("Enumerated %1 audio devices").arg(rowCount()));
}

bool AudioDeviceManager::enumerateWasapi()
{
#ifdef _WIN32
    HRESULT hrInit = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    const bool needUninit = (hrInit == S_OK || hrInit == S_FALSE);
    if (FAILED(hrInit) && hrInit != RPC_E_CHANGED_MODE) {
        emit errorOccurred(QStringLiteral("COM init failed: %1").arg(hresultToString(hrInit)));
        return false;
    }

    Microsoft::WRL::ComPtr<IMMDeviceEnumerator> enumerator;
    const HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
                                        IID_PPV_ARGS(&enumerator));
    if (FAILED(hr)) {
        emit errorOccurred(QStringLiteral("MMDeviceEnumerator create failed: %1").arg(hresultToString(hr)));
        if (needUninit) {
            CoUninitialize();
        }
        return false;
    }

    collectEndpoints(enumerator.Get(), eCapture, AudioDeviceFlow::Input, devices_);
    collectEndpoints(enumerator.Get(), eRender, AudioDeviceFlow::Output, devices_);

    if (needUninit) {
        CoUninitialize();
    }
    return true;
#else
    emit errorOccurred(QStringLiteral("WASAPI enumeration is Windows-only"));
    return false;
#endif
}

void AudioDeviceManager::handleEndpointAdded(const QString& id)
{
    refresh();
    emit deviceAdded(id);
}

void AudioDeviceManager::handleEndpointRemoved(const QString& id)
{
    emit deviceRemoved(id);
    refresh();
}

void AudioDeviceManager::handleDefaultChanged()
{
    refresh();
    emit defaultDeviceChanged();
}

void AudioDeviceManager::handleEndpointStateChanged(const QString& id, bool active)
{
    if (!active) {
        emit deviceRemoved(id);
    }
    refresh();
}

bool AudioDeviceManager::isDeviceConnected(const QString& id) const
{
    std::lock_guard lock(mutex_);
    for (const auto& d : devices_) {
        if (d.id == id) {
            return d.isConnected;
        }
    }
    return false;
}

QStringList AudioDeviceManager::inputDeviceNames() const
{
    QStringList names;
    for (const auto& d : inputs()) {
        names.push_back(d.isDefault ? d.name + QStringLiteral(" (Default)") : d.name);
    }
    return names;
}

QStringList AudioDeviceManager::outputDeviceNames() const
{
    QStringList names;
    for (const auto& d : outputs()) {
        names.push_back(d.isDefault ? d.name + QStringLiteral(" (Default)") : d.name);
    }
    return names;
}

QStringList AudioDeviceManager::inputDeviceIds() const
{
    QStringList ids;
    for (const auto& d : inputs()) {
        ids.push_back(d.id);
    }
    return ids;
}

QStringList AudioDeviceManager::outputDeviceIds() const
{
    QStringList ids;
    for (const auto& d : outputs()) {
        ids.push_back(d.id);
    }
    return ids;
}

QString AudioDeviceManager::defaultInputId() const
{
    for (const auto& d : inputs()) {
        if (d.isDefault) {
            return d.id;
        }
    }
    const auto list = inputs();
    return list.isEmpty() ? QString() : list.first().id;
}

QString AudioDeviceManager::defaultOutputId() const
{
    for (const auto& d : outputs()) {
        if (d.isDefault) {
            return d.id;
        }
    }
    const auto list = outputs();
    return list.isEmpty() ? QString() : list.first().id;
}

QString AudioDeviceManager::deviceNameForId(const QString& id) const
{
    std::lock_guard lock(mutex_);
    for (const auto& d : devices_) {
        if (d.id == id) {
            return d.name;
        }
    }
    return QStringLiteral("No device");
}

QVector<AudioDeviceInfo> AudioDeviceManager::devices() const
{
    std::lock_guard lock(mutex_);
    return devices_;
}

QVector<AudioDeviceInfo> AudioDeviceManager::inputs() const
{
    QVector<AudioDeviceInfo> out;
    std::lock_guard lock(mutex_);
    for (const auto& d : devices_) {
        if (d.flow == AudioDeviceFlow::Input) {
            out.push_back(d);
        }
    }
    return out;
}

QVector<AudioDeviceInfo> AudioDeviceManager::outputs() const
{
    QVector<AudioDeviceInfo> out;
    std::lock_guard lock(mutex_);
    for (const auto& d : devices_) {
        if (d.flow == AudioDeviceFlow::Output) {
            out.push_back(d);
        }
    }
    return out;
}

} // namespace nitro
