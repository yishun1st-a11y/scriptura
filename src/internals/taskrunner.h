#ifndef TASKRUNNER_H
#define TASKRUNNER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QProcess>
#include <QJsonObject>
#include <QMap>

class TaskRunner : public QObject
{
    Q_OBJECT
public:
    struct Task {
        QString name;
        QString type;
        QString command;
        QStringList args;
        QString cwd;
        QMap<QString, QString> env;
        bool dependsOn;
    };

    explicit TaskRunner(QObject *parent = nullptr);

    bool loadTasks(const QString &path);
    void runTask(const QString &name);
    void stopTask();
    QStringList availableTasks() const;

signals:
    void taskStarted(const QString &name);
    void taskFinished(const QString &name, int exitCode);
    void taskOutput(const QString &name, const QString &output);
    void taskError(const QString &name, const QString &error);

private slots:
    void onProcessReadyRead();
    void onProcessFinished(int exitCode, QProcess::ExitStatus status);
    void onProcessError(QProcess::ProcessError error);

private:
    QMap<QString, Task> m_tasks;
    QProcess *m_currentProcess;
    QString m_currentTaskName;
};

#endif // TASKRUNNER_H
