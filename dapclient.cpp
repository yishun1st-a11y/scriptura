#include "dapclient.h"

#include <QJsonParseError>
#include <QRegularExpression>
#include <QDebug>
#include <QApplication>

DapClient::DapClient(QObject *parent)
    : QObject(parent)
    , m_process(new QProcess(this))
    , m_requestId(1)
    , m_seq(1)
    , m_timeoutTimer(new QTimer(this))
    , m_currentThreadId(1)
{
    m_timeoutTimer->setSingleShot(true);
    m_timeoutTimer->setInterval(30000);

    connect(m_process, &QProcess::readyReadStandardOutput, this, &DapClient::onProcessReadyRead);
    connect(m_process, QOverload<QProcess::ProcessError>::of(&QProcess::errorOccurred),
            this, &DapClient::onProcessError);
    connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &DapClient::onProcessFinished);
    connect(m_timeoutTimer, &QTimer::timeout, this, &DapClient::onTimeout);
}

DapClient::~DapClient()
{
    stopServer();
}

bool DapClient::startServer(const QString &command, const QStringList &args)
{
    if (isRunning())
        return false;

    m_process->start(command, args);
    if (!m_process->waitForStarted(5000)) {
        emit serverFailed(tr("Failed to start debugger: %1").arg(command));
        return false;
    }
    m_timeoutTimer->start();
    emit serverStarted();
    emit logMessage(tr("Debugger started: %1").arg(command));
    return true;
}

void DapClient::stopServer()
{
    if (isRunning()) {
        disconnect();
        m_process->terminate();
        if (!m_process->waitForFinished(3000))
            m_process->kill();
    }
}

void DapClient::initialize(const QString &program, const QStringList &args, const QString &cwd)
{
    m_program = program;
    m_args = args;
    m_cwd = cwd;

    QJsonObject params;
    params["clientID"] = "scriptura";
    params["adapterID"] = "scriptura";
    params["pathFormat"] = "path";
    params["linesStartAt1"] = true;
    params["columnsStartAt1"] = true;
    params["supportsVariableType"] = true;
    params["supportsVariablePaging"] = true;
    params["supportsRunInTerminalRequest"] = false;

    sendRequest("initialize", params, m_requestId);
    m_pendingRequests[m_requestId] = "initialize";
    m_requestId++;
}

void DapClient::launch()
{
    QJsonObject params;
    params["name"] = "Launch";
    params["type"] = "scriptura";
    params["request"] = "launch";
    params["program"] = m_program;
    params["args"] = QJsonArray::fromStringList(m_args);
    params["cwd"] = m_cwd;
    params["stopOnEntry"] = false;

    sendRequest("launch", params, m_requestId);
    m_pendingRequests[m_requestId] = "launch";
    m_requestId++;
}

void DapClient::configurationDone()
{
    sendNotification("configurationDone", QJsonObject{});
}

void DapClient::setBreakpoints(const QString &sourcePath, const QList<int> &lines)
{
    QList<Breakpoint> bps;
    for (int line : lines) {
        Breakpoint bp;
        bp.line = line;
        bps.append(bp);
    }
    setBreakpointsWithConditions(sourcePath, bps);
}

void DapClient::setBreakpointsWithConditions(const QString &sourcePath, const QList<Breakpoint> &breakpoints)
{
    QJsonArray bpArray;
    for (const Breakpoint &bp : breakpoints) {
        QJsonObject bpObj;
        bpObj["line"] = bp.line;
        if (!bp.condition.isEmpty())
            bpObj["condition"] = bp.condition;
        if (!bp.hitCondition.isEmpty())
            bpObj["hitCondition"] = bp.hitCondition;
        if (!bp.logMessage.isEmpty())
            bpObj["logMessage"] = bp.logMessage;
        bpArray.append(bpObj);
    }

    QJsonObject params;
    params["source"] = QJsonObject{{"path", sourcePath}};
    params["breakpoints"] = bpArray;

    int id = m_requestId;
    sendRequest("setBreakpoints", params, id);
    m_pendingRequests[id] = "setBreakpoints";
    m_requestId++;
}

void DapClient::continueDebug()
{
    QJsonObject params;
    params["threadId"] = m_currentThreadId;
    sendRequest("continue", params, m_requestId);
    m_pendingRequests[m_requestId] = "continue";
    m_requestId++;
}

void DapClient::next()
{
    QJsonObject params;
    params["threadId"] = m_currentThreadId;
    sendRequest("next", params, m_requestId);
    m_pendingRequests[m_requestId] = "next";
    m_requestId++;
}

void DapClient::stepIn()
{
    QJsonObject params;
    params["threadId"] = m_currentThreadId;
    sendRequest("stepIn", params, m_requestId);
    m_pendingRequests[m_requestId] = "stepIn";
    m_requestId++;
}

void DapClient::stepOut()
{
    QJsonObject params;
    params["threadId"] = m_currentThreadId;
    sendRequest("stepOut", params, m_requestId);
    m_pendingRequests[m_requestId] = "stepOut";
    m_requestId++;
}

void DapClient::pause()
{
    QJsonObject params;
    params["threadId"] = m_currentThreadId;
    sendRequest("pause", params, m_requestId);
    m_pendingRequests[m_requestId] = "pause";
    m_requestId++;
}

void DapClient::disconnect()
{
    sendRequest("disconnect", QJsonObject{}, m_requestId);
    m_pendingRequests[m_requestId] = "disconnect";
    m_requestId++;
}

void DapClient::stackTrace(int threadId)
{
    QJsonObject params;
    params["threadId"] = threadId;
    params["startFrame"] = 0;
    params["levels"] = 100;
    int id = m_requestId;
    sendRequest("stackTrace", params, id);
    m_pendingRequests[id] = "stackTrace";
    m_requestId++;
}

void DapClient::scopes(int frameId)
{
    QJsonObject params;
    params["frameId"] = frameId;
    int id = m_requestId;
    sendRequest("scopes", params, id);
    m_pendingRequests[id] = "scopes";
    m_requestId++;
}

void DapClient::variables(int variablesReference)
{
    QJsonObject params;
    params["variablesReference"] = variablesReference;
    int id = m_requestId;
    sendRequest("variables", params, id);
    m_pendingRequests[id] = "variables";
    m_requestId++;
}

void DapClient::onProcessReadyRead()
{
    QMutexLocker locker(&m_mutex);
    m_buffer.append(m_process->readAllStandardOutput());
    locker.unlock();

    m_timeoutTimer->start();

    while (true) {
        QByteArray msg = readMessage();
        if (msg.isEmpty())
            break;
        processMessage(msg);
    }
}

void DapClient::onProcessError(QProcess::ProcessError error)
{
    Q_UNUSED(error)
    emit serverFailed(tr("Debugger error: %1").arg(m_process->errorString()));
    emit logMessage(tr("DAP Error: %1").arg(m_process->errorString()));
}

void DapClient::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitCode)
    Q_UNUSED(exitStatus)
    m_timeoutTimer->stop();
    emit serverStopped();
    emit logMessage(tr("Debugger stopped"));
}

void DapClient::onTimeout()
{
    emit logMessage(tr("Debugger timeout - no response"));
}

void DapClient::sendMessage(const QJsonObject &msg)
{
    QByteArray data = QJsonDocument(msg).toJson(QJsonDocument::Compact);
    int contentLength = data.size();
    QByteArray header = QString("Content-Length: %1\r\n"
                                "Content-Type: application/vscode-jsonrpc; charset=utf-8\r\n"
                                "\r\n").arg(contentLength).toUtf8();
    m_process->write(header + data);
}

void DapClient::sendRequest(const QString &method, const QJsonObject &params, int id)
{
    QJsonObject obj;
    obj["seq"] = m_seq++;
    obj["type"] = "request";
    obj["command"] = method;
    obj["arguments"] = params;
    sendMessage(obj);
}

void DapClient::sendNotification(const QString &method, const QJsonObject &params)
{
    QJsonObject obj;
    obj["seq"] = m_seq++;
    obj["type"] = "request";
    obj["command"] = method;
    obj["arguments"] = params;
    sendMessage(obj);
}

void DapClient::processMessage(const QByteArray &data)
{
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);
    if (error.error != QJsonParseError::NoError) {
        emit logMessage(tr("DAP JSON parse error: %1").arg(error.errorString()));
        return;
    }
    if (doc.isObject()) {
        QJsonObject obj = doc.object();
        if (obj.contains("seq")) {
            int seq = obj["seq"].toInt();
            if (obj["type"].toString() == "response") {
                handleResponse(obj);
            } else if (obj["type"].toString() == "event") {
                processEvent(obj);
            } else if (obj["type"].toString() == "request") {
                handleRequest(obj);
            }
        }
    }
}

void DapClient::handleResponse(const QJsonObject &obj)
{
    int seq = obj["seq"].toInt();
    Q_UNUSED(seq)
    QString command = obj["command"].toString();
    bool success = !obj.contains("success") || obj["success"].toBool();

    if (!success) {
        QJsonObject err = obj["body"].toObject();
        emit logMessage(tr("DAP error for %1: %2").arg(command, err["error"].toString()));
    }

    if (command == "initialize") {
        emit initialized();
    } else if (command == "launch") {
        configurationDone();
    } else if (command == "stackTrace") {
        QList<StackFrame> frames;
        QJsonArray arr = obj["body"].toObject()["stackFrames"].toArray();
        for (const QJsonValue &val : arr)
            frames.append(parseStackFrame(val.toObject()));
        if (!frames.isEmpty()) {
            m_currentThreadId = 1;
            emit stackTraceReceived(m_currentThreadId, frames);
        }
    } else if (command == "scopes") {
        QJsonArray arr = obj["body"].toObject()["scopes"].toArray();
        QList<Scope> scopes;
        for (const QJsonValue &val : arr)
            scopes.append(parseScope(val.toObject()));
        emit scopesReceived(obj["arguments"].toObject()["frameId"].toInt(), scopes);
    } else if (command == "variables") {
        QJsonArray arr = obj["body"].toObject()["variables"].toArray();
        QList<Variable> vars;
        for (const QJsonValue &val : arr)
            vars.append(parseVariable(val.toObject()));
        emit variablesReceived(obj["arguments"].toObject()["variablesReference"].toInt(), vars);
    }
}

void DapClient::handleNotification(const QJsonObject &obj)
{
    Q_UNUSED(obj)
}

void DapClient::handleRequest(const QJsonObject &obj)
{
    Q_UNUSED(obj)
}

void DapClient::processEvent(const QJsonObject &event)
{
    QString eventName = event["event"].toString();
    if (eventName == "initialized") {
        emit initialized();
    } else if (eventName == "stopped") {
        QString reason = event["body"].toObject()["reason"].toString();
        emit stopped(reason);
    } else if (eventName == "continued") {
        emit continued();
    } else if (eventName == "breakpoint") {
        QJsonObject body = event["body"].toObject();
        QString path = body["source"].toObject()["path"].toString();
        QList<Breakpoint> bps;
        Breakpoint bp;
        bp.verified = body["verified"].toBool();
        bp.line = body["line"].toInt();
        bp.message = body["message"].toString();
        bps.append(bp);
        emit breakpointUpdated(path, bps);
    } else if (eventName == "output") {
        QJsonObject body = event["body"].toObject();
        emit logMessage(body["output"].toString());
    }
}

QByteArray DapClient::readMessage()
{
    QMutexLocker locker(&m_mutex);
    int headerEnd = m_buffer.indexOf("\r\n\r\n");
    if (headerEnd == -1)
        return QByteArray();

    QByteArray header = m_buffer.left(headerEnd);
    m_buffer.remove(0, headerEnd + 4);

    QRegularExpression re("Content-Length: (\\d+)", QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatch match = re.match(header);
    if (!match.hasMatch())
        return QByteArray();

    int contentLength = match.captured(1).toInt();
    if (m_buffer.size() < contentLength)
        return QByteArray();

    QByteArray msg = m_buffer.left(contentLength);
    m_buffer.remove(0, contentLength);
    return msg;
}

QJsonObject DapClient::createRequest(const QString &method, const QJsonObject &params, int id)
{
    QJsonObject obj;
    obj["seq"] = m_seq++;
    obj["type"] = "request";
    obj["command"] = method;
    obj["arguments"] = params;
    return obj;
}

QJsonObject DapClient::createNotification(const QString &method, const QJsonObject &params)
{
    QJsonObject obj;
    obj["seq"] = m_seq++;
    obj["type"] = "request";
    obj["command"] = method;
    obj["arguments"] = params;
    return obj;
}

QJsonObject DapClient::createResponse(int id, const QJsonValue &result)
{
    QJsonObject obj;
    obj["seq"] = m_seq++;
    obj["type"] = "response";
    obj["request_seq"] = id;
    obj["success"] = true;
    obj["command"] = "";
    obj["body"] = result;
    return obj;
}

QJsonObject DapClient::createErrorResponse(int id, int code, const QString &message)
{
    QJsonObject body;
    body["error"] = QJsonObject{{"id", code}, {"message", message}};
    QJsonObject obj;
    obj["seq"] = m_seq++;
    obj["type"] = "response";
    obj["request_seq"] = id;
    obj["success"] = false;
    obj["command"] = "";
    obj["body"] = body;
    return obj;
}

DapClient::Breakpoint DapClient::parseBreakpoint(const QJsonObject &obj) const
{
    Breakpoint bp;
    bp.id = obj["id"].toInt();
    bp.verified = obj["verified"].toBool();
    bp.line = obj["line"].toInt();
    bp.message = obj["message"].toString();
    return bp;
}

DapClient::StackFrame DapClient::parseStackFrame(const QJsonObject &obj) const
{
    StackFrame frame;
    frame.id = obj["id"].toInt();
    frame.name = obj["name"].toString();
    frame.line = obj["line"].toInt();
    frame.source = {obj["source"].toObject()["name"].toString(),
                    obj["source"].toObject()["path"].toString(),
                    obj["source"].toObject()["sourceReference"].toInt()};
    return frame;
}

DapClient::Scope DapClient::parseScope(const QJsonObject &obj) const
{
    Scope scope;
    scope.name = obj["name"].toString();
    scope.variablesReference = obj["variablesReference"].toInt();
    if (obj.contains("range")) {
        QJsonObject r = obj["range"].toObject();
        scope.variables.start.line = r["start"].toObject()["line"].toInt();
        scope.variables.start.character = r["start"].toObject()["character"].toInt();
        scope.variables.end.line = r["end"].toObject()["line"].toInt();
        scope.variables.end.character = r["end"].toObject()["character"].toInt();
    }
    return scope;
}

DapClient::Variable DapClient::parseVariable(const QJsonObject &obj) const
{
    Variable var;
    var.name = obj["name"].toString();
    var.value = obj["value"].toString();
    var.type = obj["type"].toString();
    var.variablesReference = obj["variablesReference"].toInt();
    return var;
}
