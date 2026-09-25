#include "nitro/audio/AudioRingBuffer.hpp"
#include "nitro/audio/AudioMeter.hpp"
#include "nitro/audio/AudioResampler.hpp"
#include "nitro/core/NitroEngine.hpp"
#include "nitro/scenes/TransitionEngine.hpp"
#include "nitro/sources/SourceConfigs.hpp"
#include "nitro/sources/SourceFactory.hpp"

#include <QCoreApplication>
#include <QSignalSpy>
#include <QtTest/QtTest>
#include <atomic>
#include <thread>
#include <vector>

class TestHardening : public QObject {
    Q_OBJECT
private slots:
    void engineOwnsSingleAudioEngine()
    {
        nitro::NitroEngine engine;
        QVERIFY(engine.initialize());
        QVERIFY(engine.audioEngine() != nullptr);
        QVERIFY(engine.audioDevices() != nullptr);
        QCOMPARE(engine.audioEngine()->deviceManager(), engine.audioDevices());
        engine.shutdown();
    }

    void ringBufferWriteRead()
    {
        nitro::AudioRingBuffer ring(128, 2);
        std::vector<float> in(20, 0.5f);
        QCOMPARE(ring.write(in.data(), 10), std::size_t{10});
        QCOMPARE(ring.availableFrames(), std::size_t{10});
        std::vector<float> out(20, 0.0f);
        QCOMPARE(ring.read(out.data(), 10), std::size_t{10});
        QCOMPARE(out[0], 0.5f);
        QCOMPARE(ring.underrunCount(), quint64{0});
        QCOMPARE(ring.overrunCount(), quint64{0});
    }

    void ringBufferUnderrun()
    {
        nitro::AudioRingBuffer ring(64, 2);
        std::vector<float> out(40, 1.0f);
        QCOMPARE(ring.read(out.data(), 8), std::size_t{0});
        QVERIFY(ring.underrunCount() >= 8);
    }

    void ringBufferOverrunDoesNotAdvanceRead()
    {
        nitro::AudioRingBuffer ring(16, 2);
        std::vector<float> a(32, 0.25f);
        // Fill completely
        QCOMPARE(ring.write(a.data(), 16), std::size_t{16});
        // Extra write must drop and count overrun — readPos untouched by producer
        const auto before = ring.availableFrames();
        QCOMPARE(before, std::size_t{16});
        QCOMPARE(ring.write(a.data(), 8), std::size_t{0});
        QVERIFY(ring.overrunCount() >= 8);
        QCOMPARE(ring.availableFrames(), std::size_t{16});
    }

    void ringBufferWraparound()
    {
        nitro::AudioRingBuffer ring(8, 2);
        std::vector<float> chunk(4, 0.0f);
        for (int i = 0; i < 20; ++i) {
            chunk[0] = static_cast<float>(i);
            chunk[1] = static_cast<float>(i);
            ring.read(chunk.data(), 2); // drain some to make room after first fills
            std::fill(chunk.begin(), chunk.end(), static_cast<float>(i));
            ring.write(chunk.data(), 2);
        }
        QVERIFY(ring.availableFrames() <= 8);
    }

    void ringBufferConcurrentStress()
    {
        nitro::AudioRingBuffer ring(1024, 2);
        std::atomic<bool> stop{false};
        std::thread prod([&]() {
            std::vector<float> buf(64, 0.1f);
            while (!stop.load()) {
                ring.write(buf.data(), 32);
            }
        });
        std::thread cons([&]() {
            std::vector<float> buf(64, 0.0f);
            for (int i = 0; i < 2000; ++i) {
                ring.read(buf.data(), 32);
            }
            stop.store(true);
        });
        cons.join();
        prod.join();
        // No crash is success; counters may be non-zero
        QVERIFY(true);
    }

    void meterClipAndSnapshot()
    {
        nitro::AudioMeter meter;
        meter.setSampleRate(48000);
        std::vector<float> hot(128, 1.5f);
        // Multiple blocks so attack envelope rises
        for (int i = 0; i < 8; ++i) {
            meter.process(hot.data(), 64, 2);
        }
        const auto snap = meter.snapshot();
        QVERIFY(snap.clipped);
        QVERIFY(snap.peakHoldLinear >= 1.0f);
        QVERIFY(meter.clipped());
    }

    void resamplerContinuitySameRate()
    {
        nitro::AudioResampler rs;
        rs.configure(48000, 48000, 2);
        std::vector<float> in(20, 0.2f);
        std::vector<float> out;
        rs.process(in.data(), 10, out);
        QCOMPARE(out.size(), std::size_t{20});
    }

    void sourceConfigValidation()
    {
        nitro::DisplayCaptureConfig d;
        d.fps = 60;
        QVERIFY(d.validate());
        d.fps = 0;
        QVERIFY(!d.validate());

        nitro::BrowserSourceConfig b;
        b.allowNativeBridge = true;
        QVERIFY(!b.validate());
        b.allowNativeBridge = false;
        QVERIFY(b.validate());

        auto src = nitro::SourceFactory::create(nitro::SourceType::Text);
        auto cfg = nitro::TextConfig::fromMap(src->properties());
        QVERIFY(cfg.validate());
        const auto map = cfg.toMap();
        src->setProperty(QStringLiteral("content"), map.value(QStringLiteral("content")));
        QVERIFY(!src->toJson().isEmpty());
    }

    void transitionFadeProgress()
    {
        nitro::TransitionEngine te;
        te.fade(0, 1, 50);
        QVERIFY(te.isActive());
        QTest::qWait(20);
        te.tick();
        QVERIFY(te.progress() > 0.0);
        QTest::qWait(80);
        te.tick();
        QVERIFY(!te.isActive());
        QCOMPARE(te.progress(), 1.0);
    }

    void transitionCutImmediate()
    {
        nitro::TransitionEngine te;
        QSignalSpy spy(&te, &nitro::TransitionEngine::completed);
        te.cut(0, 2);
        QCOMPARE(spy.count(), 1);
        QCOMPARE(spy.at(0).at(0).toInt(), 2);
    }
};

int run_hardening_tests(int argc, char** argv)
{
    TestHardening tc;
    return QTest::qExec(&tc, argc, argv);
}

#include "test_hardening.moc"
