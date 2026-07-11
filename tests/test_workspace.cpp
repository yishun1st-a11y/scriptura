#include <QTest>
#include <QTemporaryDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "workspace.h"
#include "test_workspace.h"

class TestWorkspace : public QObject
{
    Q_OBJECT
private slots:
    void testLoadValidWorkspace();
    void testLoadMissingFile();
    void testLoadInvalidJson();
    void testSaveAndReloadRoundTrip();
    void testAddRecentFileDedup();
    void testAddRecentFileCapacityCap();
    void testIsLoadedBeforeAndAfterSave();
};

void TestWorkspace::testLoadValidWorkspace()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    QJsonObject root;
    QJsonArray folders;
    folders.append(QJsonObject{{"path", "/home/user/project1"}});
    folders.append(QJsonObject{{"path", "/home/user/project2"}});
    root["folders"] = folders;

    QJsonArray recent;
    recent.append("/home/user/file1.cpp");
    recent.append("/home/user/file2.h");
    root["recentFiles"] = recent;

    root["settings"] = QJsonObject{{"editor.font", "Monospace"}};

    QString path = dir.filePath("workspace.code-workspace");
    QFile f(path);
    QVERIFY(f.open(QIODevice::WriteOnly));
    f.write(QJsonDocument(root).toJson());
    f.close();

    Workspace ws;
    QVERIFY(ws.load(path));

    QCOMPARE(ws.folders().size(), 2);
    QCOMPARE(ws.folders().at(0), QString("/home/user/project1"));
    QCOMPARE(ws.folders().at(1), QString("/home/user/project2"));

    QCOMPARE(ws.recentFiles().size(), 2);
    QCOMPARE(ws.recentFiles().at(0), QString("/home/user/file1.cpp"));
    QCOMPARE(ws.recentFiles().at(1), QString("/home/user/file2.h"));

    QVERIFY(ws.settings().contains("editor.font"));
    QCOMPARE(ws.settings()["editor.font"].toString(), QString("Monospace"));
}

void TestWorkspace::testLoadMissingFile()
{
    Workspace ws;
    QVERIFY(!ws.load("/nonexistent/path/workspace.code-workspace"));
    QVERIFY(!ws.isLoaded());
    QVERIFY(ws.folders().isEmpty());
    QVERIFY(ws.recentFiles().isEmpty());
}

void TestWorkspace::testLoadInvalidJson()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    QString path = dir.filePath("bad.json");
    QFile f(path);
    QVERIFY(f.open(QIODevice::WriteOnly));
    f.write("{not valid json!!!");
    f.close();

    Workspace ws;
    QVERIFY(!ws.load(path));
    QVERIFY(!ws.isLoaded());
}

void TestWorkspace::testSaveAndReloadRoundTrip()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    QString path = dir.filePath("workspace.code-workspace");

    Workspace ws;
    ws.setFolders({"/src", "/tests"});
    ws.setRecentFiles({"/src/main.cpp", "/src/widgets/button.h"});
    ws.setSettings(QJsonObject{{"theme", "dark"}});

    QVERIFY(ws.saveAs(path));
    QVERIFY(ws.isLoaded());
    QCOMPARE(ws.path(), path);

    Workspace ws2;
    QVERIFY(ws2.load(path));
    QCOMPARE(ws2.folders(), ws.folders());
    QCOMPARE(ws2.recentFiles(), ws.recentFiles());
    QCOMPARE(ws2.settings()["theme"].toString(), QString("dark"));
}

void TestWorkspace::testAddRecentFileDedup()
{
    Workspace ws;
    ws.setRecentFiles({"a.cpp", "b.cpp", "a.cpp"});
    ws.addRecentFile("c.cpp");

    QVERIFY(ws.recentFiles().contains("c.cpp"));
    int count = 0;
    for (const QString &f : ws.recentFiles())
        if (f == "a.cpp") ++count;
    QCOMPARE(count, 1);
}

void TestWorkspace::testAddRecentFileCapacityCap()
{
    Workspace ws;
    for (int i = 0; i < 25; ++i)
        ws.addRecentFile(QString("file%1.cpp").arg(i));

    QCOMPARE(ws.recentFiles().size(), 20);
    QCOMPARE(ws.recentFiles().first(), QString("file24.cpp"));
    QCOMPARE(ws.recentFiles().last(), QString("file5.cpp"));
}

void TestWorkspace::testIsLoadedBeforeAndAfterSave()
{
    Workspace ws;
    QVERIFY(!ws.isLoaded());
    QVERIFY(!ws.save());

    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    QString path = dir.filePath("ws.code-workspace");

    QJsonObject root;
    root["folders"] = QJsonArray();
    root["recentFiles"] = QJsonArray();
    root["settings"] = QJsonObject();
    QFile f(path);
    if (f.open(QIODevice::WriteOnly)) {
        f.write(QJsonDocument(root).toJson());
        f.close();
    }

    QVERIFY(ws.load(path));
    QVERIFY(ws.isLoaded());
}

