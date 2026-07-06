#ifndef HTTPCLIENTPANEL_H
#define HTTPCLIENTPANEL_H

#include <QWidget>
#include <QPlainTextEdit>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QTreeWidget>
#include <QTableWidget>
#include <QLabel>
#include <QSpinBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>
#include <QList>
#include <QMap>
#include <QRegularExpression>

class HttpClientPanel : public QWidget
{
    Q_OBJECT
public:
    explicit HttpClientPanel(QWidget *parent = nullptr);
    ~HttpClientPanel() override;

    QString currentProjectPath() const { return m_projectPath; }
    void setCurrentProjectPath(const QString &path);
     QList<QJsonObject> recentRequests() const { return m_recentRequests; }
     void setEnvVariables(const QJsonObject &envs);

 signals:
    void requestSent(const QString &url, int statusCode);
    void openFileRequested(const QString &filePath);

private slots:
    void onSendClicked();
    void onMethodChanged(int index);
    void onUrlReturnPressed();
    void onReplyFinished();
    void onAuthChanged(int index);
    void onHeaderDoubleClicked(int row, int column);
    void onBodyTextChanged();

private:
    struct RequestState {
        int statusCode;
        QString statusText;
        int httpMajor;
        int httpMinor;
        qint64 elapsedMs;
        QByteArray body;
        QList<QPair<QString, QString>> headers;
    };

    void saveRequest();
    void loadRecentRequests();
    void renderResponse(const RequestState &state);
    void renderResponseHeaders(const QList<QPair<QString, QString>> &headers);
    void applyBasicAuth(QNetworkRequest &req);
     void addHeader(const QString &key, const QString &value);
     void clearHeaders();
     QString substituteEnvVars(const QString &input) const;

     QComboBox *m_methodCombo;
     QLineEdit *m_urlEdit;
     QPushButton *m_sendButton;
     QComboBox *m_envCombo;
     QComboBox *m_authCombo;
     QLineEdit *m_usernameEdit;
     QLineEdit *m_passwordEdit;
     QPlainTextEdit *m_bodyEdit;
     QTabWidget *m_tabs;
     QTreeWidget *m_responseHeaders;
     QTableWidget *m_responseBody;
     QLabel *m_statusLabel;
     QSpinBox *m_timeoutSpin;
     QWidget *m_loadingWidget;
     QList<QJsonObject> m_recentRequests;
     QString m_projectPath;
     QMap<QString, QString> m_envs;
     QNetworkAccessManager *m_network;
     QNetworkReply *m_currentReply;
 };

#endif // HTTPCLIENTPANEL_H
