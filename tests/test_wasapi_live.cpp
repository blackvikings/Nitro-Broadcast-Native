#include "nitro/audio/AudioClock.hpp"
#include "nitro/audio/AudioDeviceManager.hpp"
#include "nitro/audio/AudioMeter.hpp"
#include "nitro/audio/WasapiCapture.hpp"

#include <QCoreApplication>
#include <QtTest/QtTest>
#include <thread>
#include <chrono>

class TestWasapiLive : public QObject {
    Q_OBJECT
private slots:
    void enumerateDevices()
    {
        nitro::AudioDeviceManager mgr;
        QVERIFY(mgr.rowCount() > 0);
        QVERIFY(!mgr.defaultInputId().isEmpty() || !mgr.inputDeviceIds().isEmpty());
        QVERIFY(!mgr.defaultOutputId().isEmpty() || !mgr.outputDeviceIds().isEmpty());
    }

    void micCaptureStarts()
    {
        nitro::AudioDeviceManager mgr;
        const QString id = mgr.defaultInputId();
        if (id.isEmpty()) {
            QSKIP("No default input device");
        }

        nitro::AudioClock clock;
        nitro::WasapiCapture capture;
        QVERIFY(capture.start(id, nitro::WasapiCaptureMode::Microphone, &clock));
        std::this_thread::sleep_for(std::chrono::milliseconds(400));
        QVERIFY2(capture.isRunning(), qPrintable(capture.lastError()));

        // Pull some frames — silence is OK; proves pipeline delivers.
        std::vector<float> buf(480 * 2, 0.0f);
        const auto got = capture.pull(buf.data(), 480);
        QVERIFY(got <= 480);

        capture.stop();
        QVERIFY(!capture.isRunning());
    }

    void loopbackCaptureStarts()
    {
        nitro::AudioDeviceManager mgr;
        const QString id = mgr.defaultOutputId();
        if (id.isEmpty()) {
            QSKIP("No default output device");
        }

        nitro::AudioClock clock;
        nitro::WasapiCapture capture;
        QVERIFY(capture.start(id, nitro::WasapiCaptureMode::Loopback, &clock));
        std::this_thread::sleep_for(std::chrono::milliseconds(400));
        QVERIFY2(capture.isRunning(), qPrintable(capture.lastError()));
        capture.stop();
    }
};

int run_wasapi_tests(int argc, char** argv)
{
    TestWasapiLive tc;
    return QTest::qExec(&tc, argc, argv);
}

#include "test_wasapi_live.moc"
