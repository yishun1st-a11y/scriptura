#include "updater.h"
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QSettings>
#include <QDebug>
#include <QTimerEvent>

Updater::Updater(QObject *parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
    , m_timer(new QTimer(this))
    , m_updateCheckEnabled(true)
    , m_updateCheckInterval(7) // Check weekly by default
    , m_lastCheckedType(Stable)
{
    connect(m_networkManager, &QNetworkAccessManager::finished,
            this, &Updater::onNetworkReply);

    // Load settings
    QSettings settings;
    m_updateCheckEnabled = settings.value("updater/checkEnabled", true).toBool();
    m_updateCheckInterval = settings.value("updater/checkInterval", 7).toInt();

    // Setup periodic check
    connect(m_timer, &QTimer::timeout, this, [this]() { checkForUpdates(Stable); });
    m_timer->start(m_updateCheckInterval * 24 * 60 * 60 * 1000); // Convert days to milliseconds
}

Updater::~Updater()
{
    // Save settings
    QSettings settings;
    settings.setValue("updater/checkEnabled", m_updateCheckEnabled);
    settings.setValue("updater/checkInterval", m_updateCheckInterval);
}

void Updater::checkForUpdates()
{
    checkForUpdates(Stable);
}

void Updater::checkForUpdates(ReleaseType type)
{
    m_lastCheckedType = type;

    QNetworkRequest request;
    request.setUrl(QUrl(getGitHubApiUrl(type)));
    request.setHeader(QNetworkRequest::UserAgentHeader, "Scriptura-Updater");

    m_networkManager->get(request);
}

void Updater::setUpdateCheckEnabled(bool enabled)
{
    m_updateCheckEnabled = enabled;
}

bool Updater::isUpdateCheckEnabled() const
{
    return m_updateCheckEnabled;
}

void Updater::setUpdateCheckInterval(int days)
{
    m_updateCheckInterval = days;
    m_timer->start(m_updateCheckInterval * 24 * 60 * 60 * 1000);
}

QString Updater::latestVersion() const
{
    return m_latestVersion;
}

QString Updater::downloadUrl() const
{
    return m_downloadUrl;
}

Updater::ReleaseType Updater::lastCheckedType() const
{
    return m_lastCheckedType;
}

QString Updater::getLatestReleaseUrl() const
{
    return "https://github.com/jasonblanchard/scriptura/releases/latest";
}

QString Updater::getGitHubApiUrl(ReleaseType type) const
{
    if (type == PreRelease) {
        // Fetch all releases and filter for pre-releases
        return "https://api.github.com/repos/jasonblanchard/scriptura/releases?per_page=10";
    }
    return "https://api.github.com/repos/jasonblanchard/scriptura/releases/latest";
}

void Updater::onNetworkReply(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "Update check failed:" << reply->errorString();
        emit updateCheckFailed(reply->errorString());
        reply->deleteLater();
        return;
    }

    QByteArray data = reply->readAll();
    reply->deleteLater();

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);

    if (error.error != QJsonParseError::NoError) {
        qDebug() << "Failed to parse update response:" << error.errorString();
        emit updateCheckFailed(error.errorString());
        return;
    }

    if (m_lastCheckedType == PreRelease) {
        // For pre-release, the response is an array of releases
        if (!doc.isArray()) {
            emit updateCheckFailed("Invalid response format for pre-release");
            return;
        }

        QJsonArray releases = doc.array();
        if (releases.isEmpty()) {
            emit noUpdateAvailable();
            return;
        }

        // Find the first pre-release
        for (const QJsonValue &value : releases) {
            if (!value.isObject()) continue;
            QJsonObject obj = value.toObject();
            if (obj.value("prerelease").toBool()) {
                m_latestVersion = obj.value("tag_name").toString();
                m_downloadUrl = obj.value("html_url").toString();
                break;
            }
        }

        if (m_latestVersion.isEmpty()) {
            emit noUpdateAvailable();
            return;
        }
    } else {
        // For stable release, the response is a single object
        if (!doc.isObject()) {
            emit updateCheckFailed("Invalid response format");
            return;
        }

        QJsonObject obj = doc.object();
        m_latestVersion = obj["tag_name"].toString();
        m_downloadUrl = obj["html_url"].toString();
    }

    // Get current version from settings (or use a default)
    QSettings settings;
    QString currentVersion = settings.value("updater/currentVersion", "0.0.0").toString();

    // Compare versions (simple string comparison, works for semver)
    if (m_latestVersion != currentVersion && !m_latestVersion.isEmpty()) {
        emit updateAvailable(m_latestVersion, m_downloadUrl);
    } else {
        emit noUpdateAvailable();
    }
}
