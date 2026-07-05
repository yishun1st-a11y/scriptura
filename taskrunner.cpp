#include "taskrunner.h"
#include <QJsonDocument>
#include <QFile>
#include <QJsonArray>
#include <QFileInfo>

TaskRunner::TaskRunner(QObject *parent)
    : QObject(parent)
    , m_currentProcess(nullptr)
{
}

bool TaskRunner::loadTasks(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);
    if (error.error != QJsonParseError::NoError) {
        return false;
    }

    QJsonObject root = doc.object();
    QJsonArray tasksArray = root["tasks"].toArray();
    
    m_tasks.clear();
    for (const QJsonValue &value : tasksArray) {
        QJsonObject taskObj = value.toObject();
        Task task;
        task.name = taskObj["label"].toString();
        task.type = taskObj["type"].toString();
        task.command = taskObj["command"].toString();
        
        QJsonArray argsArray = taskObj["args"].toArray();
        for (const QJsonValue &arg : argsArray) {
            task.args.append(arg.toString());
        }
        
        task.cwd = taskObj["options"].toObject()["cwd"].toString();
        
        QJsonObject envObj = taskObj["options"].toObject()["env"].toObject();
        for (const QString &key : envObj.keys()) {
            task.env[key] = envObj[key].toString();
        }
        
        task.dependsOn = taskObj.contains("dependsOn");
        
        m_tasks[task.name] = task;
    }
    
    return true;
}

void TaskRunner::runTask(const QString &name)
{
    if (!m_tasks.contains(name)) {
        emit taskError(name, "Task not found");
        return;
    }
    
    if (m_currentProcess) {
        stopTask();
    }
    
    Task task = m_tasks[name];
    m_currentTaskName = name;
    
    m_currentProcess = new QProcess(this);
    m_currentProcess->setProgram(task.command);
    m_currentProcess->setArguments(task.args);
    
    if (!task.cwd.isEmpty()) {
        m_currentProcess->setWorkingDirectory(task.cwd);
    }
    
    QProcessEnvironment env = m_currentProcess->processEnvironment();
    for (const QString &key : task.env.keys()) {
        env.insert(key, task.env[key]);
    }
    m_currentProcess->setProcessEnvironment(env);
    
    connect(m_currentProcess, &QProcess::readyReadStandardOutput, this, &TaskRunner::onProcessReadyRead);
    connect(m_currentProcess, &QProcess::readyReadStandardError, this, &TaskRunner::onProcessReadyRead);
    connect(m_currentProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &TaskRunner::onProcessFinished);
    connect(m_currentProcess, &QProcess::errorOccurred, this, &TaskRunner::onProcessError);
    
    m_currentProcess->start();
    emit taskStarted(name);
}

void TaskRunner::stopTask()
{
    if (m_currentProcess) {
        m_currentProcess->terminate();
        if (!m_currentProcess->waitForFinished(3000)) {
            m_currentProcess->kill();
        }
        m_currentProcess->deleteLater();
        m_currentProcess = nullptr;
        m_currentTaskName.clear();
    }
}

QStringList TaskRunner::availableTasks() const
{
    return m_tasks.keys();
}

void TaskRunner::onProcessReadyRead()
{
    if (!m_currentProcess) return;
    
    QByteArray stdoutData = m_currentProcess->readAllStandardOutput();
    QByteArray stderrData = m_currentProcess->readAllStandardError();
    
    if (!stdoutData.isEmpty()) {
        emit taskOutput(m_currentTaskName, QString::fromUtf8(stdoutData));
    }
    if (!stderrData.isEmpty()) {
        emit taskOutput(m_currentTaskName, QString::fromUtf8(stderrData));
    }
}

void TaskRunner::onProcessFinished(int exitCode, QProcess::ExitStatus status)
{
    emit taskFinished(m_currentTaskName, exitCode);
    
    if (m_currentProcess) {
        m_currentProcess->deleteLater();
        m_currentProcess = nullptr;
        m_currentTaskName.clear();
    }
}

void TaskRunner::onProcessError(QProcess::ProcessError error)
{
    QString errorString;
    switch (error) {
        case QProcess::FailedToStart:
            errorString = "Failed to start process";
            break;
        case QProcess::Crashed:
            errorString = "Process crashed";
            break;
        case QProcess::Timedout:
            errorString = "Process timed out";
            break;
        case QProcess::WriteError:
            errorString = "Write error";
            break;
        case QProcess::ReadError:
            errorString = "Read error";
            break;
        default:
            errorString = "Unknown error";
    }
    
    emit taskError(m_currentTaskName, errorString);
}
