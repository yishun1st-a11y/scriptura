#ifndef AIINLINECOMPLETION_H
#define AIINLINECOMPLETION_H

#include <QObject>
#include <QTimer>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QPlainTextEdit>
#include <QTextCursor>

class AiInlineCompletion : public QObject
{
    Q_OBJECT
public:
    explicit AiInlineCompletion(QObject *parent = nullptr);
    ~AiInlineCompletion() override;

    void setEditor(QPlainTextEdit *editor);
     void setSettings(const QString &provider, const QString &endpoint, const QString &model, bool enabled, int debounceMs, const QString &apiKey = {});
     bool isEnabled() const { return m_enabled; }

    void setGhostText(const QString &text) { m_pendingGhostText = text; }
    QString ghostText() const { return m_pendingGhostText; }
    bool hasGhostText() const { return m_hasPendingCompletion; }

signals:
    void completionInserted(const QString &text);

private slots:
    void onEditorTextChanged();
    void onCursorPositionChanged();
    void onReplyFinished();

private:
    QString extractSuffixContext(const QString &text, int maxChars = 4000) const;
    QString currentFilePath() const;
    void requestCompletionInternal(const QString &prompt);

    QPlainTextEdit *m_editor;
    QTimer *m_debounceTimer;
    bool m_enabled;
    QString m_provider;
    QString m_endpoint;
    QString m_model;
    int m_debounceMs;
    QString m_pendingGhostText;
    bool m_hasPendingCompletion;
    int m_currentRequestId;
    QNetworkAccessManager *m_network;
    QString m_lastPrompt;
    QTextCursor m_lastCursor;
    QString m_apiKey;
 };

#endif // AIINLINECOMPLETION_H
