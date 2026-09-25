#include "nitro/config/NitroSettings.hpp"

#include <QJsonObject>
#include <QTemporaryDir>
#include <QtTest/QtTest>

class TestSettings : public QObject {
    Q_OBJECT
private slots:
    void defaults()
    {
        nitro::NitroSettings s;
        QCOMPARE(s.videoWidth(), 1920);
        QCOMPARE(s.fps(), 60);
        QCOMPARE(s.theme(), QStringLiteral("dark"));
    }

    void jsonRoundTrip()
    {
        nitro::NitroSettings s;
        s.setVideoWidth(1280);
        s.setVideoHeight(720);
        s.setFps(30);
        s.setEncoder(QStringLiteral("nvenc"));
        const QJsonObject obj = s.toJson();

        nitro::NitroSettings other;
        other.fromJson(obj);
        QCOMPARE(other.videoWidth(), 1280);
        QCOMPARE(other.videoHeight(), 720);
        QCOMPARE(other.fps(), 30);
        QCOMPARE(other.encoder(), QStringLiteral("nvenc"));
    }

    void fileRoundTrip()
    {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        const QString path = dir.filePath(QStringLiteral("settings.json"));

        nitro::NitroSettings s;
        s.setCurrentProfile(QStringLiteral("Vertical"));
        QVERIFY(s.saveToFile(path));

        nitro::NitroSettings loaded;
        QVERIFY(loaded.loadFromFile(path));
        QCOMPARE(loaded.currentProfile(), QStringLiteral("Vertical"));
    }
};

int run_settings_tests(int argc, char** argv)
{
    TestSettings tc;
    return QTest::qExec(&tc, argc, argv);
}

#include "test_settings.moc"
