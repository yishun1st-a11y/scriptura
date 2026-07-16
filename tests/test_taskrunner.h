#ifndef TEST_TASKRUNNER_H
#define TEST_TASKRUNNER_H

#include <QObject>

class TestTaskRunner : public QObject
{
    Q_OBJECT
private slots:
    void testLoadTasksValidJson();
    void testLoadTasksInvalidJson();
    void testLoadTasksMissingFile();
    void testAvailableTasksAfterLoad();
    void testRunTaskNotFound();
};

#endif // TEST_TASKRUNNER_H
