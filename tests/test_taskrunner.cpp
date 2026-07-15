#include <QTest>
#include <QTemporaryDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "taskrunner.h"
#include "test_taskrunner.h"

void TestTaskRunner::testLoadTasksValidJson()
{
    TaskRunner runner;
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    QJsonObject root;
    QJsonArray tasks;
    QJsonObject task;
    task["label"] = "build";
    task["type"] = "shell";
    task["command"] = "make";
    task["args"] = QJsonArray{"-j4"};
    QJsonObject options;
    options["cwd"] = "/tmp";
    task["options"] = options;
    tasks.append(task);
    root["tasks"] = tasks;

    QString path = dir.filePath("tasks.json");
    QFile f(path);
    QVERIFY(f.open(QIODevice::WriteOnly));
    f.write(QJsonDocument(root).toJson());
    f.close();

    QVERIFY(runner.loadTasks(path));
    QStringList available = runner.availableTasks();
    QVERIFY(available.contains("build"));
}

void TestTaskRunner::testLoadTasksInvalidJson()
{
    TaskRunner runner;
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    QString path = dir.filePath("bad.json");
    QFile f(path);
    QVERIFY(f.open(QIODevice::WriteOnly));
    f.write("not json");
    f.close();

    QVERIFY(!runner.loadTasks(path));
}

void TestTaskRunner::testLoadTasksMissingFile()
{
    TaskRunner runner;
    QVERIFY(!runner.loadTasks("/nonexistent/path/tasks.json"));
}

void TestTaskRunner::testAvailableTasksAfterLoad()
{
    TaskRunner runner;
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    QJsonObject root;
    QJsonArray tasks;
    QJsonObject t1, t2;
    t1["label"] = "task1";
    t1["type"] = "shell";
    t1["command"] = "echo";
    t1["args"] = QJsonArray{};
    t1["options"] = QJsonObject{};
    t2["label"] = "task2";
    t2["type"] = "shell";
    t2["command"] = "pwd";
    t2["args"] = QJsonArray{};
    t2["options"] = QJsonObject{};
    tasks.append(t1);
    tasks.append(t2);
    root["tasks"] = tasks;

    QString path = dir.filePath("tasks.json");
    QFile f(path);
    QVERIFY(f.open(QIODevice::WriteOnly));
    f.write(QJsonDocument(root).toJson());
    f.close();

    QVERIFY(runner.loadTasks(path));
    QStringList available = runner.availableTasks();
    QCOMPARE(available.size(), 2);
    QVERIFY(available.contains("task1"));
    QVERIFY(available.contains("task2"));
}

void TestTaskRunner::testRunTaskNotFound()
{
    TaskRunner runner;
    bool taskErrorEmitted = false;
    QObject::connect(&runner, &TaskRunner::taskError, [&taskErrorEmitted](const QString&, const QString&) {
        taskErrorEmitted = true;
    });

    runner.runTask("nonexistent");
    QVERIFY(taskErrorEmitted);
}
