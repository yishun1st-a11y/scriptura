#include "workspace.h"
#include <QJsonDocument>
#include <QFile>
#include <QDir>
#include <QFileInfo>

Workspace::Workspace(QObject *parent)
    : QObject(parent)
{
}

bool Workspace::load(const QString &path)
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
    
    m_folders.clear();
    QJsonArray foldersArray = root["folders"].toArray();
    for (const QJsonValue &value : foldersArray) {
        QJsonObject folderObj = value.toObject();
        m_folders.append(folderObj["path"].toString());
    }
    
    m_settings = root["settings"].toObject();
    
    m_recentFiles.clear();
    QJsonArray recentArray = root["recentFiles"].toArray();
    for (const QJsonValue &value : recentArray) {
        m_recentFiles.append(value.toString());
    }
    
    m_path = path;
    return true;
}

bool Workspace::save()
{
    if (m_path.isEmpty()) {
        return false;
    }
    return saveAs(m_path);
}

bool Workspace::saveAs(const QString &path)
{
    QJsonObject root;
    
    QJsonArray foldersArray;
    for (const QString &folder : m_folders) {
        QJsonObject folderObj;
        folderObj["path"] = folder;
        foldersArray.append(folderObj);
    }
    root["folders"] = foldersArray;
    
    root["settings"] = m_settings;
    
    QJsonArray recentArray;
    for (const QString &file : m_recentFiles) {
        recentArray.append(file);
    }
    root["recentFiles"] = recentArray;
    
    QJsonDocument doc(root);
    
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }
    
    file.write(doc.toJson());
    file.close();
    
    m_path = path;
    return true;
}

void Workspace::setFolders(const QStringList &folders)
{
    m_folders = folders;
}

void Workspace::setSettings(const QJsonObject &settings)
{
    m_settings = settings;
}

void Workspace::setRecentFiles(const QStringList &files)
{
    m_recentFiles = files;
}

void Workspace::addRecentFile(const QString &file)
{
    m_recentFiles.removeAll(file);
    m_recentFiles.prepend(file);
    while (m_recentFiles.size() > MAX_RECENT_FILES) {
        m_recentFiles.removeLast();
    }
}
