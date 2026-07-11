#include "aiinlinecompletion.h"

#include <QPainter>
#include <QKeyEvent>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTabWidget>
#include <QTabBar>
#include "codeeditor.h"

AiInlineCompletion::AiInlineCompletion(QObject *parent)
    : QObject(parent)
    , m_editor(nullptr)
    , m_debounceTimer(new QTimer(this))
    , m_enabled(false)
    , m_debounceMs(400)
    , m_hasPendingCompletion(false)
    , m_currentRequestId(0)
    , m_network(new QNetworkAccessManager(this))
{
    m_debounceTimer->setSingleShot(true);
    connect(m_debounceTimer, &QTimer::timeout, this, [this]() {
        if (!m_enabled || !m_editor)
            return;
        QTextCursor cursor = m_editor->textCursor();
        QString text = m_editor->toPlainText();
        int pos = cursor.position();
        QString prompt = text.left(pos);
        if (prompt.trimmed().isEmpty())
            return;
        m_lastCursor = cursor;
        requestCompletionInternal(prompt);
    });
}

AiInlineCompletion::~AiInlineCompletion()
{
    m_debounceTimer->stop();
}

void AiInlineCompletion::setEditor(QPlainTextEdit *editor)
{
    if (m_editor) {
        disconnect(m_editor, nullptr, this, nullptr);
    }
    m_editor = editor;
    if (m_editor) {
        if (CodeEditor *ce = qobject_cast<CodeEditor*>(m_editor)) {
            connect(ce, &CodeEditor::ghostTextAccepted, this, &AiInlineCompletion::completionInserted);
        }
        connect(m_editor, &QPlainTextEdit::textChanged, this, &AiInlineCompletion::onEditorTextChanged);
        connect(m_editor, &QPlainTextEdit::cursorPositionChanged, this, &AiInlineCompletion::onCursorPositionChanged);
    }
    m_pendingGhostText.clear();
    m_hasPendingCompletion = false;
}

void AiInlineCompletion::setSettings(const QString &provider, const QString &endpoint, const QString &model, bool enabled, int debounceMs, const QString &apiKey)
{
    m_provider = provider;
    m_endpoint = endpoint;
    m_model = model;
    m_enabled = enabled;
    m_debounceMs = debounceMs;
    m_apiKey = apiKey;
    m_debounceTimer->setInterval(debounceMs);
    if (!enabled) {
        m_pendingGhostText.clear();
        m_hasPendingCompletion = false;
        if (CodeEditor *ce = qobject_cast<CodeEditor*>(m_editor))
            ce->clearGhostText();
    }
}

QString AiInlineCompletion::extractSuffixContext(const QString &text, int maxChars) const
{
    if (text.size() <= maxChars)
        return text;
    return text.right(maxChars);
}

QString AiInlineCompletion::currentFilePath() const
{
    if (!m_editor)
        return {};
    if (QPlainTextEdit *te = qobject_cast<QPlainTextEdit*>(m_editor)) {
        if (QWidget *w = te->parentWidget()) {
            if (QTabWidget *tabs = qobject_cast<QTabWidget*>(w->parentWidget())) {
                int idx = tabs->indexOf(te);
                if (idx >= 0 && idx < tabs->tabBar()->count()) {
                    return tabs->tabToolTip(idx);
                }
            }
        }
    }
    return {};
}

void AiInlineCompletion::requestCompletionInternal(const QString &prompt)
{
    if (!m_enabled || m_endpoint.isEmpty() || m_model.isEmpty() || !m_editor)
        return;

    m_lastPrompt = prompt;
    m_currentRequestId++;
    int requestId = m_currentRequestId;

    QJsonObject root;
    root["model"] = m_model;
    root["stream"] = false;
    QJsonArray messages;
    QJsonObject sys;
    sys["role"] = "system";
    sys["content"] = "You are an inline code completion engine. Return only the completion text, no explanation, no markdown fences.";
    messages.append(sys);
    QJsonObject usr;
    usr["role"] = "user";
    usr["content"] = extractSuffixContext(prompt);
    messages.append(usr);
    root["messages"] = messages;
    root["max_tokens"] = 64;
    root["temperature"] = 0.2;

    QByteArray payload = QJsonDocument(root).toJson(QJsonDocument::Compact);

    QNetworkRequest req = QNetworkRequest(QUrl(m_endpoint));
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    if (!m_apiKey.isEmpty()) {
        req.setRawHeader("Authorization", "Bearer " + m_apiKey.toUtf8());
    }

    QNetworkReply *reply = m_network->post(req, payload);
    reply->setProperty("requestId", requestId);
    connect(reply, &QNetworkReply::finished, this, &AiInlineCompletion::onReplyFinished);
}

void AiInlineCompletion::onReplyFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply)
        return;

    int requestId = reply->property("requestId").toInt();
    if (requestId != m_currentRequestId) {
        reply->deleteLater();
        return;
    }

    if (reply->error() == QNetworkReply::NoError) {
        QByteArray data = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        QJsonObject root = doc.object();
        QString text;

        if (root.contains("choices")) {
            QJsonArray choices = root["choices"].toArray();
            if (!choices.isEmpty()) {
                QJsonObject choice = choices.first().toObject();
                QJsonObject msg = choice["message"].toObject();
                text = msg["content"].toString();
            }
        } else if (root.contains("message")) {
            // Ollama /api/chat returns { message: { content: "..." } }
            QJsonObject msg = root["message"].toObject();
            text = msg["content"].toString();
        } else if (root.contains("response")) {
            text = root["response"].toString();
        }

        text = text.trimmed();
        if (!text.isEmpty() && !text.startsWith("```") && !text.startsWith("<")) {
            m_pendingGhostText = text;
            m_hasPendingCompletion = true;
            if (CodeEditor *ce = qobject_cast<CodeEditor*>(m_editor)) {
                ce->setGhostText(text);
            }
        }
    }

    reply->deleteLater();
}

void AiInlineCompletion::onEditorTextChanged()
{
    if (CodeEditor *ce = qobject_cast<CodeEditor*>(m_editor)) {
        ce->clearGhostText();
    }
    m_pendingGhostText.clear();
    m_hasPendingCompletion = false;

    if (!m_enabled || !m_editor)
        return;

    m_debounceTimer->start(m_debounceMs);
}

void AiInlineCompletion::onCursorPositionChanged()
{
    if (m_hasPendingCompletion) {
        if (CodeEditor *ce = qobject_cast<CodeEditor*>(m_editor)) {
            ce->clearGhostText();
        }
        m_pendingGhostText.clear();
        m_hasPendingCompletion = false;
        if (m_enabled)
            m_debounceTimer->start(80);
    }
}
