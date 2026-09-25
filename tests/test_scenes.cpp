#include "nitro/scenes/SceneManager.hpp"
#include "nitro/sources/SourceFactory.hpp"
#include "nitro/sources/WinCaptureEnum.hpp"

#include <QCoreApplication>
#include <QTemporaryDir>
#include <QtTest/QtTest>

class TestScenes : public QObject {
    Q_OBJECT
private slots:
    void defaultsLoaded()
    {
        nitro::SceneManager mgr;
        QCOMPARE(mgr.rowCount(), 5);
        QCOMPARE(mgr.previewSceneName(), QStringLiteral("Gameplay"));
    }

    void createRenameDelete()
    {
        nitro::SceneManager mgr;
        const QString id = mgr.createScene(QStringLiteral("Test Scene"));
        QVERIFY(!id.isEmpty());
        QCOMPARE(mgr.previewSceneName(), QStringLiteral("Test Scene"));
        QVERIFY(mgr.renameScene(mgr.previewIndex(), QStringLiteral("Renamed")));
        QCOMPARE(mgr.previewSceneName(), QStringLiteral("Renamed"));
        const int count = mgr.rowCount();
        QVERIFY(mgr.deleteScene(mgr.previewIndex()));
        QCOMPARE(mgr.rowCount(), count - 1);
    }

    void duplicateAndSources()
    {
        nitro::SceneManager mgr;
        mgr.selectPreview(1);
        const QString dupId = mgr.duplicateScene(1);
        QVERIFY(!dupId.isEmpty());
        QVERIFY(mgr.sourceCount() >= 1);
        const QString sid = mgr.addSource(QStringLiteral("image"), QStringLiteral("Logo"));
        QVERIFY(!sid.isEmpty());
        QCOMPARE(mgr.sourceAt(mgr.sourceCount() - 1).value("name").toString(), QStringLiteral("Logo"));
        QVERIFY(mgr.removeSource(mgr.sourceCount() - 1));
    }

    void serializationRoundTrip()
    {
        nitro::SceneManager mgr;
        mgr.createScene(QStringLiteral("Persist"));
        const auto json = mgr.toJson();
        nitro::SceneManager other;
        other.fromJson(json);
        QCOMPARE(other.rowCount(), mgr.rowCount());
    }

    void cutToProgram()
    {
        nitro::SceneManager mgr;
        mgr.selectPreview(0);
        mgr.cutToProgram();
        QCOMPARE(mgr.programIndex(), 0);
    }

    void sourceFactoryCatalog()
    {
        const auto catalog = nitro::SourceFactory::catalog();
        QVERIFY(catalog.size() >= 20);
        QCOMPARE(nitro::SourceFactory::capabilityFor(nitro::SourceType::AudioInput),
                 nitro::SourceCapability::Available);
        QCOMPARE(nitro::SourceFactory::capabilityFor(nitro::SourceType::Ndi),
                 nitro::SourceCapability::NotAvailable);
        QCOMPARE(nitro::SourceFactory::capabilityFor(nitro::SourceType::GameCapture),
                 nitro::SourceCapability::NotImplemented);
    }

    void addRemoveRenameReorderLockHide()
    {
        nitro::SceneManager mgr;
        mgr.selectPreview(1);
        const int base = mgr.sourceCount();
        const QString id = mgr.addSource(QStringLiteral("color"), QStringLiteral("BG"));
        QVERIFY(!id.isEmpty());
        QCOMPARE(mgr.sourceCount(), base + 1);
        const int idx = mgr.sourceCount() - 1;
        QVERIFY(mgr.renameSource(idx, QStringLiteral("Background")));
        QVERIFY(mgr.setSourceVisible(idx, false));
        QVERIFY(mgr.setSourceLocked(idx, true));
        QVERIFY(!mgr.renameSource(idx, QStringLiteral("Nope"))); // locked
        QVERIFY(mgr.setSourceLocked(idx, false));
        QVERIFY(mgr.moveSource(idx, 0));
        QCOMPARE(mgr.selectedSourceIndex(), 0);
        QVERIFY(mgr.removeSource(0));
    }

    void transformHelpers()
    {
        nitro::SceneManager mgr;
        mgr.selectPreview(1);
        mgr.addSource(QStringLiteral("color"), QStringLiteral("Fill"));
        const int idx = mgr.sourceCount() - 1;
        QVERIFY(mgr.setSourceProperty(idx, QStringLiteral("x"), 100));
        QVERIFY(mgr.centerSource(idx));
        QVERIFY(mgr.fitSourceToCanvas(idx));
        QVERIFY(mgr.stretchSourceToCanvas(idx));
        QVERIFY(mgr.resetSourceTransform(idx));
    }

    void nestedSceneCycleRejected()
    {
        nitro::SceneManager mgr;
        // Scene 0 and 1 exist
        mgr.selectPreview(0);
        const QString nested = mgr.addSource(QStringLiteral("scene"), QStringLiteral("Nested"));
        QVERIFY(!nested.isEmpty());
        const int idx = mgr.sourceCount() - 1;
        // Point scene 0's nested source at scene 0 itself → cycle
        QVERIFY(!mgr.setSourceProperty(idx, QStringLiteral("sceneId"),
                                       mgr.scenes().at(0).id));
    }

    void saveLoadFile()
    {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        const QString path = dir.filePath(QStringLiteral("scenes.json"));
        nitro::SceneManager mgr;
        mgr.addSource(QStringLiteral("text"), QStringLiteral("Title"));
        QVERIFY(mgr.saveScenesToFile(path));
        nitro::SceneManager other;
        QVERIFY(other.loadScenesFromFile(path));
        QCOMPARE(other.rowCount(), mgr.rowCount());
    }

    void monitorEnumeration()
    {
        const auto monitors = nitro::win_capture::enumerateMonitors();
        // At least one monitor on a real Windows desktop
        QVERIFY(monitors.size() >= 1);
        QCOMPARE(nitro::win_capture::preferredCaptureApi(), QStringLiteral("WGC"));
    }

    void typedSourceProperties()
    {
        auto img = nitro::SourceFactory::create(nitro::SourceType::Image, QStringLiteral("Logo"));
        QVERIFY(img);
        QCOMPARE(img->type(), nitro::SourceType::Image);
        img->setProperty(QStringLiteral("filePath"), QStringLiteral("C:/logo.png"));
        QCOMPARE(img->property(QStringLiteral("filePath")).toString(), QStringLiteral("C:/logo.png"));
        const auto json = img->toJson();
        auto restored = nitro::SourceFactory::fromJson(json);
        QCOMPARE(restored->name(), QStringLiteral("Logo"));
        QCOMPARE(restored->property(QStringLiteral("filePath")).toString(), QStringLiteral("C:/logo.png"));
    }
};

int run_scene_tests(int argc, char** argv)
{
    TestScenes tc;
    return QTest::qExec(&tc, argc, argv);
}

#include "test_scenes.moc"
