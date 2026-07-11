#include "httpclientpanel.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QNetworkRequest>
#include <QUrlQuery>
#include <QMessageBox>
#include <QApplication>
#include <QClipboard>
#include <QTabBar>
#include <QHeaderView>
#include <QStandardPaths>
#include <QTextStream>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSplitter>
#include <QRegularExpression>
#include <QMapIterator>

HttpClientPanel::HttpClientPanel(QWidget *parent)
    : QWidget(parent)
    , m_methodCombo(new QComboBox(this))
    , m_urlEdit(new QLineEdit(this))
    , m_sendButton(new QPushButton(tr("Send"), this))
    , m_envCombo(new QComboBox(this))
    , m_authCombo(new QComboBox(this))
    , m_usernameEdit(new QLineEdit(this))
    , m_passwordEdit(new QLineEdit(this))
    , m_bodyEdit(new QPlainTextEdit(this))
    , m_tabs(new QTabWidget(this))
    , m_responseHeaders(new QTreeWidget(this))
    , m_responseBody(new QTableWidget(this))
    , m_statusLabel(new QLabel(tr("Ready"), this))
    , m_timeoutSpin(new QSpinBox(this))
    , m_loadingWidget(new QProgressBar(this))
    , m_network(new QNetworkAccessManager(this))
    , m_currentReply(nullptr)
{
    m_methodCombo->addItems({"GET", "POST", "PUT", "DELETE", "PATCH", "HEAD", "OPTIONS"});
    m_methodCombo->setCurrentIndex(0);
    m_envCombo->addItems({tr("No Environment"), tr("Localhost"), tr("Dev"), tr("Staging"), tr("Prod")});
    m_environments = {
        {tr("Localhost"), {{"BASE_URL", "http://localhost:3000"}, {"API_KEY", ""}, {"USER_ID", "1"}}},
        {tr("Dev"),       {{"BASE_URL", "https://dev.example.com"}, {"API_KEY", ""}, {"USER_ID", "1"}}},
        {tr("Staging"),   {{"BASE_URL", "https://staging.example.com"}, {"API_KEY", ""}, {"USER_ID", "1"}}},
        {tr("Prod"),      {{"BASE_URL", "https://api.example.com"}, {"API_KEY", ""}, {"USER_ID", "1"}}},
    };
    int localhostIdx = m_envCombo->findText(tr("Localhost"));
    if (localhostIdx >= 0) {
        m_envCombo->setCurrentIndex(localhostIdx);
        m_envs = m_environments.value(m_envCombo->itemText(localhostIdx));
    }
    m_authCombo->addItems({tr("No Auth"), tr("Basic"), tr("Bearer")});
    m_usernameEdit->setPlaceholderText(tr("Username"));
    m_passwordEdit->setPlaceholderText(tr("Password"));
    m_bodyEdit->setPlaceholderText(tr("Request body (JSON)..."));
    m_timeoutSpin->setRange(1000, 120000);
    m_timeoutSpin->setValue(30000);
    m_timeoutSpin->setSuffix(" ms");

    QHBoxLayout *topLayout = new QHBoxLayout();
    topLayout->addWidget(m_methodCombo);
    topLayout->addWidget(m_envCombo);
    topLayout->addWidget(m_urlEdit, 1);
    topLayout->addWidget(m_timeoutSpin);
    topLayout->addWidget(m_authCombo);
    topLayout->addWidget(m_usernameEdit);
    topLayout->addWidget(m_passwordEdit);
    topLayout->addWidget(m_sendButton);

    m_responseHeaders->setHeaderLabels({tr("Key"), tr("Value")});
    m_responseHeaders->header()->setSectionResizeMode(QHeaderView::Stretch);
    m_responseBody->horizontalHeader()->setStretchLastSection(true);
    m_responseBody->verticalHeader()->setVisible(false);
    m_responseBody->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_responseBody->setSelectionBehavior(QAbstractItemView::SelectRows);

    m_tabs->addTab(m_responseBody, tr("Body"));
    m_tabs->addTab(m_responseHeaders, tr("Headers"));

    m_loadingWidget->setRange(0, 0);
    m_loadingWidget->setTextVisible(false);
    m_loadingWidget->hide();

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(m_bodyEdit);
    mainLayout->addWidget(m_tabs);
    mainLayout->addWidget(m_loadingWidget);
    mainLayout->addWidget(m_statusLabel);
    setLayout(mainLayout);

    connect(m_sendButton, &QPushButton::clicked, this, &HttpClientPanel::onSendClicked);
    connect(m_methodCombo, &QComboBox::currentTextChanged, this, [this](const QString &m) {
        Q_UNUSED(m);
        if (m_methodCombo->currentText() == "GET" || m_methodCombo->currentText() == "DELETE" || m_methodCombo->currentText() == "HEAD") {
            m_bodyEdit->setEnabled(false);
        } else {
            m_bodyEdit->setEnabled(true);
        }
    });
    connect(m_urlEdit, &QLineEdit::returnPressed, this, &HttpClientPanel::onUrlReturnPressed);
    connect(m_responseHeaders, &QTreeWidget::itemDoubleClicked, this, [this](QTreeWidgetItem *item, int column) {
        if (item && column == 1)
            QApplication::clipboard()->setText(item->text(1));
    });
    connect(m_envCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index) {
        QString name = m_envCombo->itemText(index);
        if (m_environments.contains(name))
            m_envs = m_environments[name];
        else
            m_envs.clear();
    });
    connect(m_authCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &HttpClientPanel::onAuthChanged);

    m_urlEdit->setPlaceholderText(tr("https://httpbin.org/get"));
}

HttpClientPanel::~HttpClientPanel()
{
    if (m_currentReply) {
        m_currentReply->abort();
        m_currentReply->deleteLater();
    }
}

void HttpClientPanel::setCurrentProjectPath(const QString &path)
{
    m_projectPath = path;
    if (m_envs.isEmpty()) {
        m_envs = {
            {"BASE_URL", "http://localhost:3000"},
            {"API_KEY", ""},
            {"USER_ID", "1"}
        };
    }
}

void HttpClientPanel::setEnvVariables(const QJsonObject &envs)
{
    m_envs.clear();
    for (auto it = envs.constBegin(); it != envs.constEnd(); ++it) {
        m_envs[it.key()] = it.value().toString();
    }
}

QString HttpClientPanel::substituteEnvVars(const QString &input) const
{
    if (m_envs.isEmpty())
        return input;

    QString result = input;
    QRegularExpression re(R"(\{\{(\w+)\}\})");
    QRegularExpressionMatchIterator it = re.globalMatch(result);
    int offset = 0;
    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();
        QString key = match.captured(1);
        QString value = m_envs.value(key, match.captured(0));
        result.replace(match.capturedStart(0) - offset, match.capturedLength(0), value);
        offset += match.capturedLength(0) - value.length();
    }
    return result;
}

void HttpClientPanel::onSendClicked()
{
    QString url = substituteEnvVars(m_urlEdit->text().trimmed());
    if (url.isEmpty())
        return;

    if (m_currentReply) {
        m_currentReply->abort();
        m_currentReply->deleteLater();
        m_currentReply = nullptr;
    }

    QNetworkRequest req = QNetworkRequest(QUrl(url));
    req.setRawHeader("User-Agent", "Scriptura/0.0.0-dev");

    applyBasicAuth(req);
    req.setTransferTimeout(m_timeoutSpin->value());

    QByteArray body;
    QString method = m_methodCombo->currentText();
    if (method == "POST" || method == "PUT" || method == "PATCH") {
        body = substituteEnvVars(m_bodyEdit->toPlainText()).toUtf8();
        if (!body.isEmpty())
            req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    }

     m_statusLabel->setText(tr("Sending..."));
     m_loadingWidget->show();
     m_sendButton->setEnabled(false);

     m_sendTimer.restart();
     m_currentReply = m_network->sendCustomRequest(req, method.toUtf8(), body.isEmpty() ? QByteArray() : body);
     connect(m_currentReply, &QNetworkReply::finished, this, &HttpClientPanel::onReplyFinished);
 }

 void HttpClientPanel::onMethodChanged(int index)
{
    Q_UNUSED(index);
}

void HttpClientPanel::onUrlReturnPressed()
{
    onSendClicked();
}

void HttpClientPanel::onReplyFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply)
        return;
    if (reply != m_currentReply) {
        // Stale/aborted reply (e.g. superseded by a newer request). Ignore it.
        reply->deleteLater();
        return;
    }

    RequestState state;
    state.httpMajor = 1;
    state.statusCode = m_currentReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    state.elapsedMs = m_sendTimer.isValid() ? m_sendTimer.elapsed() : 0;

    QList<QPair<QString, QString>> headerList;
    QList<QByteArray> rawHeaders = m_currentReply->rawHeaderList();
    for (const QByteArray &h : rawHeaders) {
        headerList.append({QString::fromUtf8(h), QString::fromUtf8(m_currentReply->rawHeader(h))});
    }

    if (m_currentReply->error() == QNetworkReply::NoError) {
        state.body = m_currentReply->readAll();
        state.statusText = tr("OK");
    } else if (state.statusCode > 0) {
        state.statusText = QString::number(state.statusCode);
    } else {
        state.statusText = m_currentReply->errorString();
    }

     state.headers = headerList;
     renderResponse(state);

     m_loadingWidget->hide();
     m_sendButton->setEnabled(true);
     m_statusLabel->setText(tr("Status: %1 | Time: %2ms").arg(state.statusCode).arg(state.elapsedMs));
    emit requestSent(m_urlEdit->text(), state.statusCode);

    m_currentReply->deleteLater();
    m_currentReply = nullptr;
}

void HttpClientPanel::onAuthChanged(int index)
{
    bool isBasic = index == 1;
    m_usernameEdit->setEnabled(isBasic);
    m_passwordEdit->setEnabled(isBasic);
}

void HttpClientPanel::onHeaderDoubleClicked(int row, int column)
{
    if (column == 1) {
        QString value = m_responseHeaders->topLevelItem(row)->text(1);
        QApplication::clipboard()->setText(value);
    }
}

void HttpClientPanel::onBodyTextChanged()
{
    // Auto-detect JSON
    Q_UNUSED(this)
}

void HttpClientPanel::saveRequest()
{
    QJsonObject req;
    req["method"] = m_methodCombo->currentText();
    req["url"] = m_urlEdit->text();
    req["body"] = QString::fromUtf8(m_bodyEdit->toPlainText().toUtf8());
    m_recentRequests.prepend(req);
    if (m_recentRequests.size() > 20)
        m_recentRequests.resize(20);
}

void HttpClientPanel::loadRecentRequests()
{
    Q_UNUSED(this)
}

void HttpClientPanel::renderResponse(const RequestState &state)
{
    m_responseHeaders->clear();
    renderResponseHeaders(state.headers);

    if (m_methodCombo->currentText() == "HEAD") {
        m_responseBody->setRowCount(0);
        m_responseBody->setColumnCount(1);
        m_responseBody->setHorizontalHeaderLabels({"No body for HEAD request"});
        return;
    }

    QString bodyStr = QString::fromUtf8(state.body);
    QByteArray data = state.body;
    QByteArray lower = data.toLower();

    if (lower.startsWith("{") || lower.startsWith("[")) {
        QJsonParseError err;
        QJsonDocument doc = QJsonDocument::fromJson(data, &err);
        if (err.error == QJsonParseError::NoError) {
            QJsonArray arr = doc.isArray() ? doc.array() : QJsonArray{doc.object()};
            if (doc.isObject()) {
                QJsonObject obj = doc.object();
                m_responseBody->setColumnCount(2);
                m_responseBody->setRowCount(obj.size());
                m_responseBody->setHorizontalHeaderLabels({tr("Key"), tr("Value")});
                int row = 0;
                for (auto it = obj.constBegin(); it != obj.constEnd(); ++it, ++row) {
                    m_responseBody->setItem(row, 0, new QTableWidgetItem(it.key()));
                    QTableWidgetItem *valItem = new QTableWidgetItem(it.value().isObject() || it.value().isArray()
                        ? QJsonDocument(it.value().toVariant().toJsonDocument()).toJson(QJsonDocument::Compact)
                        : it.value().toVariant().toString());
                    m_responseBody->setItem(row, 1, valItem);
                }
            } else {
                m_responseBody->setColumnCount(1);
                m_responseBody->setRowCount(arr.size());
                m_responseBody->setHorizontalHeaderLabels({"Value"});
                for (int i = 0; i < arr.size(); ++i) {
                    QJsonValue v = arr.at(i);
                    m_responseBody->setItem(i, 0, new QTableWidgetItem(v.isObject() || v.isArray()
                        ? QJsonDocument(v.toVariant().toJsonDocument()).toJson(QJsonDocument::Compact)
                        : v.toVariant().toString()));
                }
            }
            return;
        }
    }

    m_responseBody->setColumnCount(1);
    m_responseBody->setRowCount(1);
    m_responseBody->setHorizontalHeaderLabels({tr("Response")});
    m_responseBody->setItem(0, 0, new QTableWidgetItem(bodyStr));
}

void HttpClientPanel::renderResponseHeaders(const QList<QPair<QString, QString>> &headers)
{
    m_responseHeaders->clear();
    for (const auto &h : headers) {
        QTreeWidgetItem *item = new QTreeWidgetItem({h.first, h.second});
        m_responseHeaders->addTopLevelItem(item);
    }
    m_responseHeaders->resizeColumnToContents(0);
}

void HttpClientPanel::applyBasicAuth(QNetworkRequest &req)
{
    if (m_authCombo->currentIndex() == 1) {
        QString credentials = m_usernameEdit->text() + ":" + m_passwordEdit->text();
        req.setRawHeader("Authorization", "Basic " + credentials.toUtf8().toBase64());
    } else if (m_authCombo->currentIndex() == 2) {
        QString token = m_passwordEdit->text();
        if (!token.isEmpty())
            req.setRawHeader("Authorization", "Bearer " + token.toUtf8());
    }
}

void HttpClientPanel::clearHeaders()
{
    Q_UNUSED(this)
}
