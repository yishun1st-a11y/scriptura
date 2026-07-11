#ifndef WORKSPACE_H
#define WORKSPACE_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QJsonObject>
#include <QJsonArray>

class Workspace : public QObject
{
    Q_OBJECT
public:
    explicit Workspace(QObject *parent = nullptr);

    bool load(const QString &path);
    bool save();
    bool saveAs(const QString &path);

    QStringList folders() const { return m_folders; }
    void setFolders(const QStringList &folders);
    
    QJsonObject settings() const { return m_settings; }
    void setSettings(const QJsonObject &settings);
    
    QStringList recentFiles() const { return m_recentFiles; }
    void setRecentFiles(const QStringList &files);
    void addRecentFile(const QString &file);
    
    QString path() const { return m_path; }
    bool isLoaded() const { return !m_path.isEmpty(); }

private:
    QString m_path;
    QStringList m_folders;
    QJsonObject m_settings;
    QStringList m_recentFiles;

    static constexpr int MAX_RECENT_FILES = 20;
};

#endif // WORKSPACE_H
