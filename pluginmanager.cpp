#include "pluginmanager.h"
#include "eventbus.h"
#include "servicelocator.h"
#include <QDebug>

PluginManager::PluginManager(QObject* parent)
    : QObject(parent)
    , m_context(nullptr)
{
}

PluginManager::~PluginManager()
{
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
                    pluginMetadata.append(metadata);
                    if (metadata.contains("id")) {
                        m_pluginPaths[metadata["id"].toString()] = info.absoluteFilePath();
                    }
                }
            }
        }
    }
    
    // 第二階段：驗證依賴關係
    QList<DependencyResolver::DependencyError> errors = m_resolver.validate(pluginMetadata);
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
    for (const QString& pluginId : loadOrder) {
        if (!loadPluginById(pluginId)) {
            qWarning() << "Failed to load plugin:" << pluginId;
        }
    }
    
    // 第五階段：初始化所有插件
    initializePlugins();
    
    return true;
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
    
    QString libraryPath = pluginDir.absoluteFilePath(libraryName);
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