#include "dapclient.h"

#include <QJsonParseError>
#include <QRegularExpression>
#include <QDebug>
#include <QTextDocument>
#include <QTextBlock>
#include <QApplication>
#include <QPalette>

DapClient::DapClient(QObject *parent)
    : QObject(parent)
    , m_process(new QProcess(this))
    , m_framer(new LengthPrefixedFramer(this))
    , m_requestId(1)
    , m_seq(1)
    , m_timeoutTimer(new QTimer(this))
    , m_currentThreadId(0)
    , m_configurationDoneSent(false)
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

    m_program = command;
    m_args = args;
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
        m_process->terminate();
        if (!m_process->waitForFinished(3000))
            m_process->kill();
    }
    m_framer->clear();
    m_configurationDoneSent = false;
}

void DapClient::initialize(const QString &program, const QStringList &args, const QString &cwd)
{
    m_program = program;
    m_args = args;
    m_cwd = cwd;
    m_configurationDoneSent = false;

    QJsonObject params;
    params["adapterID"] = "scriptura";
    params["pathFormat"] = "path";
    params["linesStartAt1"] = true;
    params["columnsStartAt1"] = true;
    params["supportsVariableType"] = true;
    params["supportsRunInTerminalRequest"] = true;

    int id = m_requestId;
    sendRequest("initialize", params, id);
}

void DapClient::launch()
{
    QJsonObject params;
    params["name"] = m_program;
    params["type"] = "scriptura";
    params["request"] = "launch";
    params["program"] = m_program;
    params["args"] = QJsonArray::fromStringList(m_args);
    if (!m_cwd.isEmpty())
        params["cwd"] = m_cwd;

    int id = m_requestId;
    sendRequest("launch", params, id);
}

void DapClient::configurationDone()
{
    if (m_configurationDoneSent)
        return;
    m_configurationDoneSent = true;
    sendNotification("configurationDone", QJsonObject{});
}

void DapClient::setBreakpoints(const QString &sourcePath, const QList<int> &lines)
{
    QJsonArray bpLines;
    for (int line : lines)
        bpLines.append(line);

    QJsonObject params;
    params["source"] = QJsonObject{{"path", sourcePath}};
    params["lines"] = bpLines;

    int id = m_requestId;
    sendRequest("setBreakpoints", params, id);
}

void DapClient::setBreakpointsWithConditions(const QString &sourcePath, const QList<Breakpoint> &breakpoints)
{
    QJsonArray bpArray;
    for (const Breakpoint &bp : breakpoints) {
        QJsonObject obj;
        obj["line"] = bp.line;
        if (!bp.condition.isEmpty())
            obj["condition"] = bp.condition;
        if (!bp.hitCondition.isEmpty())
            obj["hitCondition"] = bp.hitCondition;
        if (!bp.logMessage.isEmpty())
            obj["logMessage"] = bp.logMessage;
        bpArray.append(obj);
    }

    QJsonObject params;
    params["source"] = QJsonObject{{"path", sourcePath}};
    params["breakpoints"] = bpArray;

    int id = m_requestId;
    sendRequest("setBreakpoints", params, id);
}

void DapClient::continueDebug()
{
    QJsonObject params;
    params["threadId"] = m_currentThreadId;
    int id = m_requestId;
    sendRequest("continue", params, id);
}

void DapClient::next()
{
    QJsonObject params;
    params["threadId"] = m_currentThreadId;
    int id = m_requestId;
    sendRequest("next", params, id);
}

void DapClient::stepIn()
{
    QJsonObject params;
    params["threadId"] = m_currentThreadId;
    int id = m_requestId;
    sendRequest("stepIn", params, id);
}

void DapClient::stepOut()
{
    QJsonObject params;
    params["threadId"] = m_currentThreadId;
    int id = m_requestId;
    sendRequest("stepOut", params, id);
}

void DapClient::pause()
{
    QJsonObject params;
    params["threadId"] = m_currentThreadId;
    int id = m_requestId;
    sendRequest("pause", params, id);
}

void DapClient::disconnect()
{
    QJsonObject params;
    params["terminateDebuggee"] = true;
    int id = m_requestId;
    sendRequest("disconnect", params, id);
}

void DapClient::stackTrace(int threadId)
{
    QJsonObject params;
    params["threadId"] = threadId;
    params["startFrame"] = 0;
    params["levels"] = 100;
    int id = m_requestId;
    sendRequest("stackTrace", params, id);
}

void DapClient::scopes(int frameId)
{
    QJsonObject params;
    params["frameId"] = frameId;
    int id = m_requestId;
    sendRequest("scopes", params, id);
}

void DapClient::variables(int variablesReference)
{
    QJsonObject params;
    params["variablesReference"] = variablesReference;
    int id = m_requestId;
    sendRequest("variables", params, id);
}

void DapClient::evaluate(const QString &expression, int frameId, const QString &context)
{
    QJsonObject params;
    params["expression"] = expression;
    params["frameId"] = frameId;
    params["context"] = context;
    int id = m_requestId;
    sendRequest("evaluate", params, id);
}

void DapClient::onProcessReadyRead()
{
    m_framer->append(m_process->readAllStandardOutput());

    m_timeoutTimer->start();

    while (m_framer->canReadMessage()) {
        QByteArray msg = m_framer->readMessage();
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
    m_process->write(LengthPrefixedFramer::frame(data));
}

void DapClient::sendRequest(const QString &method, const QJsonObject &params, int id)
{
    Q_UNUSED(id)
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
    obj["type"] = "notification";
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
            QString type = obj["type"].toString();
            if (type == "response") {
                handleResponse(obj);
            } else if (type == "event") {
                processEvent(obj);
            } else if (type == "request") {
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
        // configurationDone is now sent on the "initialized" event per DAP spec
    } else if (command == "stackTrace") {
        QList<StackFrame> frames;
        QJsonArray arr = obj["body"].toObject()["stackFrames"].toArray();
        for (const QJsonValue &val : arr)
            frames.append(parseStackFrame(val.toObject()));
        if (!frames.isEmpty()) {
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
    } else if (command == "evaluate") {
        QJsonObject body = obj["body"].toObject();
        QString result = body["result"].toString();
        QJsonObject args = obj["arguments"].toObject();
        QString expr = args["expression"].toString();
        QString ctx = args["context"].toString();
        emit evaluationReceived(expr, result, ctx);
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
        configurationDone();
        emit initialized();
    } else if (eventName == "stopped") {
        QJsonObject body = event["body"].toObject();
        QString reason = body["reason"].toString();
        int tid = body.value("threadId").toInt(m_currentThreadId);
        if (tid != 0)
            m_currentThreadId = tid;
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
    obj["type"] = "notification";
    obj["command"] = method;
    obj["arguments"] = params;
    return obj;
}

QJsonObject DapClient::createResponse(int id, const QJsonValue &result)
{
    QJsonObject obj;
    obj["seq"] = m_seq++;
    obj["type"] = "response";
    obj["command"] = "";
    obj["success"] = true;
    obj["body"] = QJsonObject{{"result", result}};
    return obj;
}

QJsonObject DapClient::createErrorResponse(int id, int code, const QString &message)
{
    QJsonObject obj;
    obj["seq"] = m_seq++;
    obj["type"] = "response";
    obj["command"] = "";
    obj["success"] = false;
    obj["body"] = QJsonObject{{"error", QJsonObject{{"code", code}, {"message", message}}}};
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
    if (obj.contains("source")) {
        QJsonObject src = obj["source"].toObject();
        frame.source.name = src["name"].toString();
        frame.source.path = src["path"].toString();
        frame.source.sourceReference = src["sourceReference"].toInt();
    }
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
