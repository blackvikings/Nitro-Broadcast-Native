#include "nitro/audio/WasapiCapture.hpp"

#include "nitro/audio/AudioFormat.hpp"
#include "nitro/diagnostics/NitroLogger.hpp"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <vector>

#ifdef _WIN32
#  define WIN32_LEAN_AND_MEAN
#  define NOMINMAX
#  include <windows.h>
#  include <mmdeviceapi.h>
#  include <audioclient.h>
#  include <mmreg.h>
#  include <ks.h>
#  include <ksmedia.h>
#  include <avrt.h>
#  include <wrl/client.h>
#  pragma comment(lib, "Ole32.lib")
#  pragma comment(lib, "Avrt.lib")
#endif

namespace nitro {

namespace {

constexpr int kEngineRate = 48000;
constexpr int kEngineChannels = 2;
constexpr std::size_t kRingFrames = 48000; // 1 second

#ifdef _WIN32

bool isFloatFormat(const WAVEFORMATEX* fmt)
{
    if (!fmt) {
        return false;
    }
    if (fmt->wFormatTag == WAVE_FORMAT_IEEE_FLOAT) {
        return true;
    }
    if (fmt->wFormatTag == WAVE_FORMAT_EXTENSIBLE) {
        const auto* ext = reinterpret_cast<const WAVEFORMATEXTENSIBLE*>(fmt);
        return IsEqualGUID(ext->SubFormat, KSDATAFORMAT_SUBTYPE_IEEE_FLOAT);
    }
    return false;
}

bool isPcmFormat(const WAVEFORMATEX* fmt)
{
    if (!fmt) {
        return false;
    }
    if (fmt->wFormatTag == WAVE_FORMAT_PCM) {
        return true;
    }
    if (fmt->wFormatTag == WAVE_FORMAT_EXTENSIBLE) {
        const auto* ext = reinterpret_cast<const WAVEFORMATEXTENSIBLE*>(fmt);
        return IsEqualGUID(ext->SubFormat, KSDATAFORMAT_SUBTYPE_PCM);
    }
    return false;
}

void convertToFloatStereo(
    const BYTE* src,
    UINT32 frames,
    const WAVEFORMATEX* fmt,
    std::vector<float>& outStereo)
{
    outStereo.resize(static_cast<std::size_t>(frames) * 2u);
    const int ch = fmt->nChannels;
    const int bits = fmt->wBitsPerSample;
    const bool isFloat = isFloatFormat(fmt);
    const bool isPcm = isPcmFormat(fmt);

    for (UINT32 f = 0; f < frames; ++f) {
        float L = 0.0f;
        float R = 0.0f;
        if (isFloat && bits == 32) {
            const auto* s = reinterpret_cast<const float*>(src) + f * ch;
            L = s[0];
            R = ch > 1 ? s[1] : s[0];
        } else if (isPcm && bits == 16) {
            const auto* s = reinterpret_cast<const int16_t*>(src) + f * ch;
            L = s[0] / 32768.0f;
            R = ch > 1 ? s[1] / 32768.0f : L;
        } else if (isPcm && bits == 32) {
            const auto* s = reinterpret_cast<const int32_t*>(src) + f * ch;
            L = s[0] / 2147483648.0f;
            R = ch > 1 ? s[1] / 2147483648.0f : L;
        } else if (isPcm && bits == 24) {
            const BYTE* p = src + f * fmt->nBlockAlign;
            auto read24 = [](const BYTE* b) -> float {
                int32_t v = (static_cast<int32_t>(b[2]) << 16) | (static_cast<int32_t>(b[1]) << 8) | b[0];
                if (v & 0x800000) {
                    v |= ~0xFFFFFF;
                }
                return v / 8388608.0f;
            };
            L = read24(p);
            R = ch > 1 ? read24(p + 3) : L;
        } else {
            // Unsupported — silence
        }
        outStereo[static_cast<std::size_t>(f) * 2u] = L;
        outStereo[static_cast<std::size_t>(f) * 2u + 1u] = R;
    }
}

void resampleLinearStereo(
    const std::vector<float>& in,
    int inRate,
    std::vector<float>& out,
    int outRate)
{
    if (inRate <= 0 || outRate <= 0 || in.empty()) {
        out.clear();
        return;
    }
    if (inRate == outRate) {
        out = in;
        return;
    }
    const std::size_t inFrames = in.size() / 2;
    const auto outFrames = static_cast<std::size_t>(
        (std::max)(std::int64_t{1}, (static_cast<std::int64_t>(inFrames) * outRate) / inRate));
    out.resize(outFrames * 2);
    for (std::size_t i = 0; i < outFrames; ++i) {
        const double srcPos = static_cast<double>(i) * static_cast<double>(inRate) / static_cast<double>(outRate);
        const std::size_t i0 = static_cast<std::size_t>(srcPos);
        const std::size_t i1 = std::min(i0 + 1, inFrames > 0 ? inFrames - 1 : 0);
        const float t = static_cast<float>(srcPos - static_cast<double>(i0));
        out[i * 2] = in[i0 * 2] * (1.0f - t) + in[i1 * 2] * t;
        out[i * 2 + 1] = in[i0 * 2 + 1] * (1.0f - t) + in[i1 * 2 + 1] * t;
    }
}

#endif

} // namespace

WasapiCapture::WasapiCapture()
{
    ring_.reset(kRingFrames, kEngineChannels);
    meter_.setSampleRate(kEngineRate);
}

WasapiCapture::~WasapiCapture()
{
    stop();
}

QString WasapiCapture::lastError() const
{
    std::lock_guard lock(errorMutex_);
    return lastError_;
}

bool WasapiCapture::start(const QString& deviceId, WasapiCaptureMode mode, AudioClock* clock)
{
    stop();
    clock_ = clock;
    stopRequested_.store(false, std::memory_order_release);
    ring_.clear();
    meter_.reset();
    framesCaptured_.store(0, std::memory_order_relaxed);
    {
        std::lock_guard lock(errorMutex_);
        lastError_.clear();
    }
    running_.store(true, std::memory_order_release);
    thread_ = std::thread(&WasapiCapture::threadMain, this, deviceId, mode);
    return true;
}

void WasapiCapture::stop()
{
    stopRequested_.store(true, std::memory_order_release);
    if (thread_.joinable()) {
        thread_.join();
    }
    running_.store(false, std::memory_order_release);
}

std::size_t WasapiCapture::pull(float* interleavedStereo, std::size_t frames)
{
    return ring_.read(interleavedStereo, frames);
}

void WasapiCapture::threadMain(QString deviceId, WasapiCaptureMode mode)
{
#ifdef _WIN32
    HRESULT hrInit = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    const bool needUninit = (hrInit == S_OK || hrInit == S_FALSE);

    DWORD taskIndex = 0;
    HANDLE mmcss = AvSetMmThreadCharacteristicsW(L"Pro Audio", &taskIndex);

    auto fail = [&](const QString& msg) {
        {
            std::lock_guard lock(errorMutex_);
            lastError_ = msg;
        }
        NITRO_LOG_ERROR(QStringLiteral("WASAPI"), msg);
        running_.store(false, std::memory_order_release);
        if (mmcss) {
            AvRevertMmThreadCharacteristics(mmcss);
        }
        if (needUninit) {
            CoUninitialize();
        }
    };

    Microsoft::WRL::ComPtr<IMMDeviceEnumerator> enumerator;
    HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, IID_PPV_ARGS(&enumerator));
    if (FAILED(hr)) {
        fail(QStringLiteral("Failed to create device enumerator"));
        return;
    }

    Microsoft::WRL::ComPtr<IMMDevice> device;
    if (deviceId.isEmpty()) {
        const EDataFlow flow = (mode == WasapiCaptureMode::Loopback) ? eRender : eCapture;
        hr = enumerator->GetDefaultAudioEndpoint(flow, eConsole, &device);
    } else {
        hr = enumerator->GetDevice(reinterpret_cast<LPCWSTR>(deviceId.utf16()), &device);
    }
    if (FAILED(hr) || !device) {
        fail(QStringLiteral("Failed to open audio device"));
        return;
    }

    Microsoft::WRL::ComPtr<IAudioClient> audioClient;
    hr = device->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, &audioClient);
    if (FAILED(hr)) {
        fail(QStringLiteral("Failed to activate IAudioClient"));
        return;
    }

    WAVEFORMATEX* mixFormat = nullptr;
    hr = audioClient->GetMixFormat(&mixFormat);
    if (FAILED(hr) || !mixFormat) {
        fail(QStringLiteral("GetMixFormat failed"));
        return;
    }

    DWORD streamFlags = AUDCLNT_STREAMFLAGS_EVENTCALLBACK;
    if (mode == WasapiCaptureMode::Loopback) {
        streamFlags |= AUDCLNT_STREAMFLAGS_LOOPBACK;
    }

    REFERENCE_TIME bufferDuration = 20'0000; // 20 ms in 100-ns units
    hr = audioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, streamFlags, bufferDuration, 0, mixFormat, nullptr);
    if (FAILED(hr)) {
        // Retry without event callback
        streamFlags = (mode == WasapiCaptureMode::Loopback) ? AUDCLNT_STREAMFLAGS_LOOPBACK : 0;
        hr = audioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, streamFlags, bufferDuration, 0, mixFormat, nullptr);
    }
    if (FAILED(hr)) {
        CoTaskMemFree(mixFormat);
        fail(QStringLiteral("IAudioClient::Initialize failed (0x%1)")
                 .arg(static_cast<quint32>(hr), 8, 16, QLatin1Char('0')));
        return;
    }

    HANDLE eventHandle = nullptr;
    const bool useEvent = (streamFlags & AUDCLNT_STREAMFLAGS_EVENTCALLBACK) != 0;
    if (useEvent) {
        eventHandle = CreateEventW(nullptr, FALSE, FALSE, nullptr);
        audioClient->SetEventHandle(eventHandle);
    }

    Microsoft::WRL::ComPtr<IAudioCaptureClient> capture;
    hr = audioClient->GetService(IID_PPV_ARGS(&capture));
    if (FAILED(hr)) {
        if (eventHandle) {
            CloseHandle(eventHandle);
        }
        CoTaskMemFree(mixFormat);
        fail(QStringLiteral("GetService IAudioCaptureClient failed"));
        return;
    }

    const int srcRate = static_cast<int>(mixFormat->nSamplesPerSec);
    meter_.setSampleRate(kEngineRate);

    hr = audioClient->Start();
    if (FAILED(hr)) {
        if (eventHandle) {
            CloseHandle(eventHandle);
        }
        CoTaskMemFree(mixFormat);
        fail(QStringLiteral("IAudioClient::Start failed"));
        return;
    }

    NITRO_LOG_INFO(QStringLiteral("WASAPI"),
                   QStringLiteral("Capture started (%1) %2 Hz %3 ch")
                       .arg(mode == WasapiCaptureMode::Loopback ? QStringLiteral("loopback")
                                                                : QStringLiteral("mic"))
                       .arg(srcRate)
                       .arg(mixFormat->nChannels));

    std::vector<float> converted;
    std::vector<float> resampled;

    while (!stopRequested_.load(std::memory_order_acquire)) {
        if (useEvent && eventHandle) {
            WaitForSingleObject(eventHandle, 50);
        } else {
            Sleep(5);
        }

        UINT32 packetFrames = 0;
        hr = capture->GetNextPacketSize(&packetFrames);
        if (FAILED(hr)) {
            break;
        }

        while (packetFrames > 0) {
            BYTE* data = nullptr;
            UINT32 numFrames = 0;
            DWORD flags = 0;
            hr = capture->GetBuffer(&data, &numFrames, &flags, nullptr, nullptr);
            if (FAILED(hr)) {
                break;
            }

            if (numFrames > 0) {
                if (flags & AUDCLNT_BUFFERFLAGS_SILENT) {
                    converted.assign(static_cast<std::size_t>(numFrames) * 2u, 0.0f);
                } else {
                    convertToFloatStereo(data, numFrames, mixFormat, converted);
                }
                resampleLinearStereo(converted, srcRate, resampled, kEngineRate);
                if (!resampled.empty()) {
                    const std::size_t frames = resampled.size() / 2;
                    // Meter on engine-format signal (actual audio amplitude)
                    meter_.process(resampled.data(), frames, kEngineChannels);
                    ring_.writeOverwrite(resampled.data(), frames);
                    framesCaptured_.fetch_add(static_cast<std::int64_t>(frames), std::memory_order_relaxed);
                }
            }

            capture->ReleaseBuffer(numFrames);
            capture->GetNextPacketSize(&packetFrames);
        }
    }

    audioClient->Stop();
    if (eventHandle) {
        CloseHandle(eventHandle);
    }
    CoTaskMemFree(mixFormat);

    if (mmcss) {
        AvRevertMmThreadCharacteristics(mmcss);
    }
    if (needUninit) {
        CoUninitialize();
    }
    running_.store(false, std::memory_order_release);
    NITRO_LOG_INFO(QStringLiteral("WASAPI"), QStringLiteral("Capture stopped"));
#else
    Q_UNUSED(deviceId);
    Q_UNUSED(mode);
    {
        std::lock_guard lock(errorMutex_);
        lastError_ = QStringLiteral("WASAPI capture requires Windows");
    }
    running_.store(false, std::memory_order_release);
#endif
}

} // namespace nitro
