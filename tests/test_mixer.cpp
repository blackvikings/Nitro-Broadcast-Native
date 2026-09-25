#include "nitro/audio/AudioMixer.hpp"
#include "nitro/audio/AudioMeter.hpp"
#include "nitro/audio/AudioRingBuffer.hpp"

#include <QtTest/QtTest>
#include <vector>

class TestMixer : public QObject {
    Q_OBJECT
private slots:
    void defaults()
    {
        nitro::AudioMixer mixer;
        QCOMPARE(mixer.rowCount(), 5);
        QVERIFY(mixer.channelAt(0) != nullptr);
        QCOMPARE(mixer.channelAt(0)->name(), QStringLiteral("MIC"));
        QVERIFY(!mixer.channelAt(0)->levelSimulated());
        QVERIFY(mixer.channelAt(0)->hasCapture());
        QVERIFY(!mixer.channelAt(1)->hasCapture()); // GAME unassigned
        QVERIFY(mixer.channelAt(2)->hasCapture());  // DESKTOP
    }

    void volumeAndMute()
    {
        nitro::AudioMixer mixer;
        mixer.setVolume(0, 0.5);
        QCOMPARE(mixer.channelAt(0)->volume(), 0.5);
        QVERIFY(mixer.channelAt(0)->gainDb() < 0.0);
        mixer.setMuted(0, true);
        QVERIFY(mixer.channelAt(0)->muted());
        mixer.setMonitoring(1, true);
        QVERIFY(mixer.channelAt(1)->monitoring());
    }

    void meterFromRealSignal()
    {
        nitro::AudioMeter meter;
        meter.setSampleRate(48000);
        std::vector<float> silence(480 * 2, 0.0f);
        meter.process(silence.data(), 480, 2);
        QCOMPARE(meter.uiLevel(), 0.0f);

        std::vector<float> loud(480 * 2, 0.5f);
        for (int i = 0; i < 10; ++i) {
            meter.process(loud.data(), 480, 2);
        }
        QVERIFY(meter.uiLevel() > 0.4f);
        const auto snap = meter.snapshot();
        QVERIFY(snap.peakHoldLinear >= 0.45f);
        QVERIFY(snap.rmsDb > -20.0f);
    }

    void ringBufferRoundTrip()
    {
        nitro::AudioRingBuffer ring(1024, 2);
        std::vector<float> in(200, 0.25f);
        QCOMPARE(ring.write(in.data(), 100), std::size_t{100});
        std::vector<float> out(200, 0.0f);
        QCOMPARE(ring.read(out.data(), 100), std::size_t{100});
        QCOMPARE(out[0], 0.25f);
    }
};

int run_mixer_tests(int argc, char** argv)
{
    TestMixer tc;
    return QTest::qExec(&tc, argc, argv);
}

#include "test_mixer.moc"
