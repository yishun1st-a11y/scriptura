#include "pluginmanager.h"
#include "eventbus.h"
#include "servicelocator.h"
#include "versionfetcher.h"
#include <QDebug>
#include <QJsonArray>
#include <QStandardPaths>
#include <QFileInfo>
#include <QProcess>
#include <QDir>

const QString PluginManager::DISABLED_PLUGINS_FILE = "disabled_plugins.json";

PluginManager::PluginManager(QObject* parent)
    : QObject(parent)
    , m_context(nullptr)
{
    loadDisabledPlugins();
}

PluginManager::~PluginManager()
{
    saveDisabledPlugins();
    unloadAllPlugins();
}

void PluginManager::setContext(PluginContext* context)
{
    m_context = context;
}

bool PluginManager::loadPlugins(const QString& pluginPath)
{
    QDir dir(pluginPath);
    if (!dir.exists()) {
        qWarning() << "Plugin directory does not exist:" << pluginPath;
        return false;
    }
    
    // 設定目錄過濾條件
    dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
    dir.setSorting(QDir::DirsFirst);
    
    // 第一階段：掃描並載入元數據
    QList<QJsonObject> pluginMetadata;
    for (const QFileInfo& info : dir.entryInfoList()) {
        if (info.isDir()) {
            QString metadataFile = info.absoluteFilePath() + "/plugin.json";
            if (QFile::exists(metadataFile)) {
                QJsonObject metadata;
                if (loadPluginMetadata(info.absoluteFilePath(), metadata)) {
                    QString pluginId = metadata["id"].toString();
                    
                    // 跳過停用的插件
                    if (m_disabledPlugins.contains(pluginId)) {
                        qDebug() << "Plugin" << pluginId << "is disabled, skipping";
                        continue;
                    }
                    
                    pluginMetadata.append(metadata);
                    if (metadata.contains("id")) {
                        QString pid = metadata["id"].toString();
                        m_pluginPaths[pid] = info.absoluteFilePath();
                        m_pluginVersions[pid] = metadata["version"].toString();
                    }
                }
            }
        }
    }
    
    // 第二階段：驗證依賴關係（先做基本驗證，使用空集合因為尚未載入任何插件）
    QList<DependencyResolver::DependencyError> errors = m_resolver.validate(pluginMetadata, QSet<QString>());
    for (const auto& error : errors) {
        if (!error.isOptional) {
            qWarning() << "Missing required dependency:" << error.missingDependency
                       << "for plugin" << error.pluginId;
        }
    }
    
    // 第三階段：建立依賴圖
    if (!buildDependencyGraph(pluginMetadata)) {
        return false;
    }
    
    // 第四階段：按依賴順序載入
    QStringList loadOrder = topologicalSort();
    QSet<QString> failedPlugins;
    
    for (const QString& pluginId : loadOrder) {
        if (!loadPluginById(pluginId)) {
            qWarning() << "Failed to load plugin:" << pluginId;
            failedPlugins.insert(pluginId);
        }
    }
    
    // 第五階段：檢查是否有依賴失敗插件的插件，如果有則卸載
    for (const QString& pluginId : loadOrder) {
        if (failedPlugins.contains(pluginId)) {
            continue;  // 跳過已失敗的
        }
        
        if (m_plugins.contains(pluginId)) {
            const PluginInfo& info = m_plugins[pluginId];
            for (const QString& dep : info.dependencies) {
                if (failedPlugins.contains(dep)) {
                    qWarning() << "Skipping plugin" << pluginId << "due to failed dependency:" << dep;
                    unloadPlugin(pluginId);
                    failedPlugins.insert(pluginId);
                    break;
                }
            }
        }
    }
    
    // 第六階段：使用 actuallyLoaded 重新驗證依賴
    QSet<QString> loadedIds;
    for (auto it = m_plugins.constBegin(); it != m_plugins.constEnd(); ++it) {
        if (it.value().instance) {
            loadedIds.insert(it.key());
        }
    }
    
    QList<DependencyResolver::DependencyError> finalErrors = m_resolver.validate(pluginMetadata, loadedIds);
    for (const auto& error : finalErrors) {
        if (!error.isOptional) {
            qWarning() << "Dependency not loaded:" << error.missingDependency
                       << "for plugin" << error.pluginId;
        }
    }
    
    // 第七階段：初始化所有成功載入的插件
    initializePlugins();
    
    return failedPlugins.isEmpty();
}

bool PluginManager::loadPlugin(const QString& filePath)
{
    QDir pluginDir(filePath);
    if (!pluginDir.exists()) {
        return false;
    }
    
    QJsonObject metadata;
    if (!loadPluginMetadata(filePath, metadata)) {
        return false;
    }
    
    QString pluginId = metadata["id"].toString();
    if (pluginId.isEmpty()) {
        qWarning() << "Plugin metadata missing 'id' field";
        return false;
    }
    
    // 檢查是否已載入
    if (m_plugins.contains(pluginId)) {
        return true;
    }
    
    // 載入插件
    QString libraryName = metadata["library"].toString();
    if (libraryName.isEmpty()) {
        qWarning() << "Plugin" << pluginId << "has no library specified";
        return false;
    }
    
    // Handle platform-specific library extensions
    QString libraryPath = pluginDir.absoluteFilePath(libraryName);
    if (!QFile::exists(libraryPath)) {
        // Try platform-specific extension if the specified one doesn't exist
        QString baseName = libraryName;
        
        // Remove existing extension if present
        int lastDot = baseName.lastIndexOf('.');
        if (lastDot > 0) {
            baseName = baseName.left(lastDot);
        }
        
        // Try platform-specific extension
        #ifdef Q_OS_WIN
            QString platformLibrary = baseName + ".dll";
        #elif defined(Q_OS_MAC)
            QString platformLibrary = baseName + ".dylib";
        #else
            QString platformLibrary = baseName + ".so";
        #endif
        
        libraryPath = pluginDir.absoluteFilePath(platformLibrary);
        qDebug() << "Library not found at" << pluginDir.absoluteFilePath(libraryName) 
                 << ", trying platform-specific:" << libraryPath;
    }
    QPluginLoader* loader = new QPluginLoader(libraryPath, this);
    
    QObject* pluginObj = loader->instance();
    if (!pluginObj) {
        qWarning() << "Failed to load plugin library:" << libraryPath 
                   << loader->errorString();
        delete loader;
        return false;
    }
    
    ScripturaPlugin* plugin = qobject_cast<ScripturaPlugin*>(pluginObj);
    if (!plugin) {
        qWarning() << "Plugin does not implement ScripturaPlugin interface:" << pluginId;
        delete loader;
        return false;
    }
    
    // 檢查版本相容性
    if (!checkVersionCompatibility(metadata)) {
        qWarning() << "Plugin" << pluginId << "is not compatible with current Scriptura version";
        emit pluginIncompatible(pluginId, VersionFetcher::coreVersion());
        delete loader;
        return false;
    }
    
    // 儲存插件資訊
    PluginInfo info;
    info.filePath = filePath;
    info.metadata = metadata;
    info.loader = loader;
    info.instance = plugin;
    info.initialized = false;
    
    if (metadata.contains("dependencies")) {
        QJsonArray depArray = metadata["dependencies"].toArray();
        for (const QJsonValue& val : depArray) {
            info.dependencies.append(val.toString());
        }
    }
    
    m_plugins[pluginId] = info;
    
    // 發佈事件
    emit pluginLoaded(pluginId);
    
    return true;
}

void PluginManager::unloadPlugin(const QString& id)
{
    if (!m_plugins.contains(id)) {
        return;
    }
    
    PluginInfo& info = m_plugins[id];
    
    // 執行 shutdown
    if (info.instance && info.initialized) {
        try {
            info.instance->shutdown();
        } catch (const std::exception& e) {
            qWarning() << "Exception during plugin shutdown:" << e.what();
        }
    }
    
    // 卸載插件
    if (info.loader) {
        info.loader->unload();
    }
    
    m_plugins.remove(id);
    emit pluginUnloaded(id);
}

void PluginManager::unloadAllPlugins()
{
    // 反向卸載 (後載入的先卸載)
    QStringList ids = m_plugins.keys();
    for (int i = ids.size() - 1; i >= 0; --i) {
        unloadPlugin(ids[i]);
    }
}

QList<ScripturaPlugin*> PluginManager::plugins() const
{
    QList<ScripturaPlugin*> result;
    for (auto it = m_plugins.constBegin(); it != m_plugins.constEnd(); ++it) {
        if (it.value().instance) {
            result.append(it.value().instance);
        }
    }
    return result;
}

ScripturaPlugin* PluginManager::getPlugin(const QString& id) const
{
    if (!m_plugins.contains(id)) {
        return nullptr;
    }
    return m_plugins[id].instance;
}

bool PluginManager::isLoaded(const QString& id) const
{
    return m_plugins.contains(id) && m_plugins[id].initialized;
}

QList<ScripturaPlugin*> PluginManager::pluginsWithFeature(PluginFeature feature) const
{
    QList<ScripturaPlugin*> result;
    for (auto it = m_plugins.constBegin(); it != m_plugins.constEnd(); ++it) {
        if (it.value().instance && it.value().instance->hasFeature(feature)) {
            result.append(it.value().instance);
        }
    }
    return result;
}

void PluginManager::publishEvent(const QString& event, const QVariant& data)
{
    if (m_eventHandlers.contains(event)) {
        for (const auto& subscription : m_eventHandlers[event]) {
            try {
                subscription.callback(data);
            } catch (const std::exception& e) {
                qWarning() << "Exception in event handler for" << event << ":" << e.what();
            }
        }
    }
    
    // 同時發佈到 EventBus
    EventBus::instance()->publish(event, data);
}

quint64 PluginManager::subscribeToEvent(const QString& event, 
                                         std::function<void(const QVariant&)> callback)
{
    quint64 id = m_nextSubscriptionId++;
    m_eventHandlers[event].append({id, callback});
    return id;
}

bool PluginManager::loadPluginMetadata(const QString& filePath, QJsonObject& metadata)
{
    QString metadataFile = filePath + "/plugin.json";
    
    QFile file(metadataFile);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open plugin metadata:" << metadataFile;
        return false;
    }
    
    QByteArray content = file.readAll();
    file.close();
    
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(content, &error);
    
    if (error.error != QJsonParseError::NoError) {
        qWarning() << "Failed to parse plugin metadata:" << error.errorString();
        return false;
    }
    
    if (!doc.isObject()) {
        qWarning() << "Plugin metadata is not a JSON object";
        return false;
    }
    
    metadata = doc.object();
    return true;
}

bool PluginManager::checkDependencies(const QJsonObject& metadata)
{
    if (!metadata.contains("dependencies")) {
        return true;
    }
    
    QJsonArray depArray = metadata["dependencies"].toArray();
    for (const QJsonValue& val : depArray) {
        QString depId = val.toString();
        if (!m_plugins.contains(depId)) {
            return false;
        }
    }
    
    return true;
}

bool PluginManager::resolveDependencies()
{
    // 這個方法在 buildDependencyGraph 中處理
    return true;
}

void PluginManager::initializePlugins()
{
    for (auto it = m_plugins.begin(); it != m_plugins.end(); ++it) {
        if (it->instance && !it->initialized) {
            if (m_context) {
                bool ok = false;
                try {
                    ok = it->instance->initialize(m_context);
                } catch (const std::exception& e) {
                    qWarning() << "Exception during plugin initialization:" << it.key() << e.what();
                    ok = false;
                }
                if (ok) {
                    it->initialized = true;
                    setupPluginConnections(it->instance);
                    qDebug() << "Plugin initialized:" << it.key();
                } else {
                    qWarning() << "Plugin initialization failed:" << it.key();
                    emit pluginError(it.key(), "Initialization failed");
                }
            } else {
                it->initialized = true;
            }
        }
    }
}

void PluginManager::setupPluginConnections(ScripturaPlugin* plugin)
{
    Q_UNUSED(plugin);
    // 未來可以設定插件信號連接
}

bool PluginManager::buildDependencyGraph(const QList<QJsonObject>& pluginMetadata)
{
    m_dependencyGraph.clear();
    
    for (const QJsonObject& metadata : pluginMetadata) {
        QString id = metadata["id"].toString();
        if (id.isEmpty()) {
            continue;
        }
        
        QStringList deps;
        if (metadata.contains("dependencies")) {
            QJsonArray depArray = metadata["dependencies"].toArray();
            for (const QJsonValue& val : depArray) {
                deps.append(val.toString());
            }
        }
        
        m_dependencyGraph[id] = deps;
    }
    
    return true;
}

QStringList PluginManager::topologicalSort()
{
    // 將依賴圖轉換為 QJsonObject 列表
    QList<QJsonObject> plugins;
    for (const QString& id : m_dependencyGraph.keys()) {
        QJsonObject obj;
        obj["id"] = id;
        QJsonArray deps;
        for (const QString& dep : m_dependencyGraph[id]) {
            deps.append(dep);
        }
        obj["dependencies"] = deps;
        plugins.append(obj);
    }
    
    return m_resolver.topologicalSort(plugins);
}

bool PluginManager::loadPluginById(const QString& pluginId)
{
    if (m_plugins.contains(pluginId)) {
        return true;
    }
    
    if (!m_allowedPlugins.isEmpty() && !m_allowedPlugins.contains(pluginId)) {
        qWarning() << "Plugin not allowed (install via Plugin Manager):" << pluginId;
        return false;
    }
    
    if (m_pluginPaths.contains(pluginId)) {
        return loadPlugin(m_pluginPaths[pluginId]);
    }
    
    qWarning() << "Plugin path not found for:" << pluginId;
    return false;
}

void PluginManager::setAllowedPlugins(const QSet<QString>& allowed)
{
    m_allowedPlugins = allowed;
}

void PluginManager::addAllowedPlugin(const QString& id)
{
    m_allowedPlugins.insert(id);
}

void PluginManager::removeAllowedPlugin(const QString& id)
{
    m_allowedPlugins.remove(id);
}

bool PluginManager::isAllowed(const QString& id) const
{
    if (m_allowedPlugins.isEmpty()) {
        return true;
    }
    return m_allowedPlugins.contains(id);
}

QSet<QString> PluginManager::allowedPlugins() const
{
    return m_allowedPlugins;
}

bool PluginManager::checkVersionCompatibility(const QJsonObject& metadata) const
{
    if (!metadata.contains("incompatible_with")) {
        return true;  // 沒有標記不相容，視為相容
    }
    
    QString currentVersion = VersionFetcher::coreVersion();
    QJsonArray incompatibleArray = metadata["incompatible_with"].toArray();
    QStringList incompatibleVersions;
    for (const QJsonValue& val : incompatibleArray) {
        incompatibleVersions.append(val.toString());
    }
    
    return !incompatibleVersions.contains(currentVersion);
}

bool PluginManager::disablePlugin(const QString& id)
{
    if (!m_plugins.contains(id)) {
        return false;
    }
    
    PluginInfo& info = m_plugins[id];
    if (info.state == PluginState::Disabled) {
        return true;
    }
    
    // 執行 shutdown
    if (info.instance && info.initialized) {
        try {
            info.instance->shutdown();
        } catch (const std::exception& e) {
            qWarning() << "Exception during plugin shutdown:" << e.what();
        }
    }
    
    // 卸載插件
    if (info.loader) {
        info.loader->unload();
        delete info.loader;
        info.loader = nullptr;
    }
    
    info.instance = nullptr;
    info.initialized = false;
    info.state = PluginState::Disabled;
    
    m_disabledPlugins.insert(id);
    saveDisabledPlugins();
    
    emit pluginUnloaded(id);
    return true;
}

bool PluginManager::enablePlugin(const QString& id)
{
    if (!m_disabledPlugins.contains(id)) {
        return false;
    }
    
    m_disabledPlugins.remove(id);
    saveDisabledPlugins();
    
    // 嘗試重新載入
    if (m_pluginPaths.contains(id)) {
        return loadPlugin(m_pluginPaths[id]);
    }
    
    return false;
}

bool PluginManager::reloadPlugin(const QString& id)
{
    if (!m_plugins.contains(id)) {
        return false;
    }
    
    // 先停用
    if (!disablePlugin(id)) {
        return false;
    }
    
    // 從停用列表中移除
    m_disabledPlugins.remove(id);
    saveDisabledPlugins();
    
    // 重新載入
    if (m_pluginPaths.contains(id)) {
        return loadPlugin(m_pluginPaths[id]);
    }
    
    return false;
}

bool PluginManager::isDisabled(const QString& id) const
{
    return m_disabledPlugins.contains(id);
}

QSet<QString> PluginManager::disabledPlugins() const
{
    return m_disabledPlugins;
}

void PluginManager::loadDisabledPlugins()
{
    QString configPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QString filePath = configPath + "/" + DISABLED_PLUGINS_FILE;
    
    QFile file(filePath);
    if (!file.exists() || !file.open(QIODevice::ReadOnly)) {
        return;
    }
    
    QByteArray content = file.readAll();
    file.close();
    
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(content, &error);
    
    if (error.error != QJsonParseError::NoError) {
        qWarning() << "Failed to parse disabled plugins file:" << error.errorString();
        return;
    }
    
    if (doc.isArray()) {
        QJsonArray array = doc.array();
        for (const QJsonValue& val : array) {
            m_disabledPlugins.insert(val.toString());
        }
    }
}

void PluginManager::saveDisabledPlugins() const
{
    QString configPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QDir dir(configPath);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    
    QString filePath = configPath + "/" + DISABLED_PLUGINS_FILE;
    
    QJsonArray array;
    for (const QString& id : m_disabledPlugins) {
        array.append(id);
    }
    
    QJsonDocument doc(array);
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
        file.close();
    }
}

QString PluginManager::pluginsInstallDir() const
{
    QString dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/plugins";
    QDir().mkpath(dir);
    return dir;
}

bool PluginManager::installPluginFromArchive(const QString &pluginId, const QByteArray &archiveData)
{
    if (pluginId.isEmpty() || archiveData.isEmpty())
        return false;

    QString targetDir = pluginsInstallDir() + "/" + pluginId;
    QDir(targetDir).removeRecursively();
    QDir().mkpath(targetDir);

    QString archivePath = targetDir + "/__download.zip";
    {
        QFile f(archivePath);
        if (!f.open(QIODevice::WriteOnly)) {
            return false;
        }
        f.write(archiveData);
        f.close();
    }

    bool extracted = false;
    if (!QStandardPaths::findExecutable("unzip").isEmpty()) {
        QProcess unzip;
        unzip.setWorkingDirectory(targetDir);
        unzip.start("unzip", {"-o", archivePath});
        extracted = unzip.waitForFinished(30000) && unzip.exitStatus() == QProcess::NormalExit;
    }
    QFile::remove(archivePath);

    if (!extracted)
        return false;

    addAllowedPlugin(pluginId);
    m_pluginPaths.remove(pluginId);
    loadPluginById(pluginId);
    return true;
}
