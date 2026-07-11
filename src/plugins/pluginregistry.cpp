#include "pluginregistry.h"

#include <QJsonDocument>
#include <QJsonArray>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QTimer>
#include <QDebug>

PluginRegistry::PluginRegistry(QObject *parent)
    : QObject(parent)
    , m_network(new QNetworkAccessManager(this))
    , m_timer(new QTimer(this))
    , m_checkIntervalDays(7)
{
    m_timer->setSingleShot(true);
    connect(m_timer, &QTimer::timeout, this, &PluginRegistry::checkForUpdates);
}

PluginRegistry::~PluginRegistry()
{
}

void PluginRegistry::setRegistryUrl(const QUrl &url)
{
    m_registryUrl = url;
}

QUrl PluginRegistry::registryUrl() const
{
    return m_registryUrl;
}

void PluginRegistry::setCheckInterval(int days)
{
    m_checkIntervalDays = days;
    if (m_timer->isActive())
        m_timer->start(m_checkIntervalDays * 24 * 60 * 60 * 1000);
}

void PluginRegistry::startPeriodicCheck()
{
    m_timer->start(m_checkIntervalDays * 24 * 60 * 60 * 1000);
}

void PluginRegistry::checkForUpdates()
{
    // Re-arm the periodic timer for the next interval.
    m_timer->start(m_checkIntervalDays * 24 * 60 * 60 * 1000);

    if (m_registryUrl.isEmpty())
        return;

    QNetworkRequest req(m_registryUrl);
    req.setRawHeader("User-Agent", "Scriptura/0.0.0-dev");
    req.setTransferTimeout(10000);

    QNetworkReply *reply = m_network->get(req);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray data = reply->readAll();
            QJsonDocument doc = QJsonDocument::fromJson(data);
            if (doc.isObject()) {
                m_manifest = doc.object();
                emit registryUpdated(m_manifest);
            }
        }
        reply->deleteLater();
    });
}

void PluginRegistry::installPlugin(const QString &pluginId, const QUrl &downloadUrl)
{
    if (downloadUrl.isEmpty())
        return;

    QNetworkRequest req(downloadUrl);
    QNetworkReply *reply = m_network->get(req);
    connect(reply, &QNetworkReply::finished, this, [this, reply, pluginId]() {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray data = reply->readAll();
            emit pluginDownloaded(pluginId, data);
        } else {
            emit installFailed(pluginId, reply->errorString());
        }
        reply->deleteLater();
    });
}

bool PluginRegistry::upgradeAvailable(const QString &pluginId, const QString &currentVersion) const
{
    if (!m_manifest.contains("plugins"))
        return false;

    QJsonArray plugins = m_manifest["plugins"].toArray();
    for (const QJsonValue &v : plugins) {
        if (!v.isObject())
            continue;
        QJsonObject obj = v.toObject();
        if (obj["id"].toString() == pluginId) {
            return obj["version"].toString() != currentVersion;
        }
    }
    return false;
}
