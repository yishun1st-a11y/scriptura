# Scriptura Plugin System 修復計畫

## 問題總覽

| # | 問題 | 嚴重程度 | 檔案位置 |
|---|------|----------|----------|
| 1 | `PluginContext::getPlugin()` 永遠返回 `nullptr` | 🔴 嚴重 | `plugincontext.cpp:70-82` |
| 2 | `EventBus::publish()` 死鎖風險 | 🔴 嚴重 | `eventbus.cpp:34-55` |
| 3 | 依賴驗證不完整 | 🟡 中等 | `dependencyresolver.cpp:75-120` |
| 4 | 部分載入失敗仍繼續初始化 | 🟡 中等 | `pluginmanager.cpp:66-76` |
| 5 | 無法取消訂閱 | 🟡 中等 | `plugincontext.cpp:89-94` |
| 6 | 無版本相容性檢查 | 🟢 低 | `plugininterface.h` |
| 7 | 無熱重載支援 | 🟢 低 | `pluginmanagerdialog.cpp` |
| 8 | 信號/回調雙重觸發 | 🟢 低 | `eventbus.cpp:34-55` |

---

## 修復方案

### 1. 修復 `PluginContext::getPlugin()` 類型安全問題

**根本原因**：`ScripturaPlugin` 是純抽象介面，不繼承 `QObject`，因此 `dynamic_cast<QObject*>` 永遠失敗。

**方案 A（推薦）**：讓 `ScripturaPlugin` 繼承 `QObject`

```cpp
// plugininterface.h
class ScripturaPlugin : public QObject
{
    Q_OBJECT
public:
    virtual ~ScripturaPlugin() = default;
    // ... 其他方法不變
};
Q_DECLARE_INTERFACE(ScripturaPlugin, "com.scriptura.plugin/1.0")
```

**影響**：
- 所有插件實例必須繼承 `QObject`
- `qobject_cast` 可以正常運作
- 現有插件需要修改繼承結構

**方案 B**：在 `PluginInfo` 中儲存額外的 `QObject*` 指標

```cpp
// pluginmanager.h
struct PluginInfo {
    // ... 現有欄位
    QObject* qObjectInstance;  // 新增
};
```

**影響**：
- 不需要修改 `ScripturaPlugin` 介面
- 但插件必須在 `initialize()` 中回傳 `QObject*`，或透過其他方式提供

**建議**：採用方案 A，因為 Qt Plugin 系統本來就預期插件是 `QObject` 子類。

---

### 2. 修復 `EventBus::publish()` 死鎖風險

**根本原因**：持有 mutex 期間呼叫回調，回調內若呼叫 `subscribe/unsubscribe` 會死鎖。

**修正**：

```cpp
void EventBus::publish(const QString& event, const QVariant& data)
{
    QList<Subscription> callbacks;
    
    {
        QMutexLocker locker(&m_mutex);
        if (!m_subscribers.contains(event)) {
            return;
        }
        callbacks = m_subscribers[event];  // 複製名單
    }  // 在此釋放鎖
    
    emit eventPublished(event, data);  // 信號在鎖外發佈
    
    for (const auto& subscription : callbacks) {
        try {
            subscription.callback(data);
        } catch (const std::exception& e) {
            qWarning() << "EventBus: Exception in callback for event" << event << ":" << e.what();
        } catch (...) {
            qWarning() << "EventBus: Unknown exception in callback for event" << event;
        }
    }
}
```

**注意**：複製 `QList<Subscription>` 是淺層複製，但 `std::function` 的複製是安全的（複製目標物件）。

---

### 3. 改進依賴驗證

**根本原因**：只檢查 metadata 是否存在，不檢查實際載入狀態。

**修正**：

```cpp
QList<DependencyResolver::DependencyError> DependencyResolver::validate(
    const QList<QJsonObject>& plugins,
    const QSet<QString>& actuallyLoaded  // 新增參數
)
{
    QList<DependencyError> errors;
    QSet<QString> availablePlugins;
    
    // 收集所有可用插件 ID
    for (const QJsonObject& plugin : plugins) {
        QString id = plugin["id"].toString();
        if (!id.isEmpty()) {
            availablePlugins.insert(id);
        }
    }
    
    for (const QJsonObject& plugin : plugins) {
        QString pluginId = plugin["id"].toString();
        if (pluginId.isEmpty()) continue;
        
        if (plugin.contains("dependencies")) {
            QJsonArray depArray = plugin["dependencies"].toArray();
            for (const QJsonValue& val : depArray) {
                QString depId = val.toString();
                if (!actuallyLoaded.contains(depId)) {  // 檢查實際載入狀態
                    errors.append(DependencyError(pluginId, depId, false));
                }
            }
        }
        
        // 可選依賴保持不變
        if (plugin.contains("optionalDependencies")) {
            QJsonArray optDepArray = plugin["optionalDependencies"].toArray();
            for (const QJsonValue& val : optDepArray) {
                QString depId = val.toString();
                if (!availablePlugins.contains(depId)) {
                    qInfo() << "Optional dependency not found:" << depId << "for plugin" << pluginId;
                }
            }
        }
    }
    
    return errors;
}
```

**調用端修改**（`pluginmanager.cpp`）：

```cpp
// 載入完成後，收集實際載入的插件 ID
QSet<QString> loadedIds;
for (auto it = m_plugins.constBegin(); it != m_plugins.constEnd(); ++it) {
    if (it.value().instance) {
        loadedIds.insert(it.key());
    }
}

// 用 actuallyLoaded 重新驗證
QList<DependencyResolver::DependencyError> errors = m_resolver.validate(pluginMetadata, loadedIds);
```

---

### 4. 修改載入失敗處理策略

**修正**：

```cpp
bool PluginManager::loadPlugins(const QString& pluginPath)
{
    // ... 掃描與載入階段 ...
    
    QStringList loadOrder = topologicalSort();
    QSet<QString> failedPlugins;
    
    for (const QString& pluginId : loadOrder) {
        if (!loadPluginById(pluginId)) {
            qWarning() << "Failed to load plugin:" << pluginId;
            failedPlugins.insert(pluginId);
        }
    }
    
    // 檢查是否有依賴失敗插件的插件
    for (const QString& pluginId : loadOrder) {
        if (failedPlugins.contains(pluginId)) {
            continue;  // 跳過已失敗的
        }
        
        // 檢查此插件是否依賴任何失敗的插件
        if (m_plugins.contains(pluginId)) {
            const PluginInfo& info = m_plugins[pluginId];
            for (const QString& dep : info.dependencies) {
                if (failedPlugins.contains(dep)) {
                    qWarning() << "Skipping plugin" << pluginId << "due to failed dependency:" << dep;
                    unloadPlugin(pluginId);  // 卸載此插件
                    failedPlugins.insert(pluginId);
                    break;
                }
            }
        }
    }
    
    // 只初始化成功載入且依賴完整的插件
    initializePlugins();
    
    return !failedPlugins.isEmpty();  // 返回是否有任何失敗
}
```

---

### 5. 為 `PluginContext::subscribe()` 新增取消訂閱方法

**修正**：

```cpp
// plugincontext.h
class PluginContext : public QObject
{
    Q_OBJECT
public:
    // ... 現有方法 ...
    
    /**
     * @brief 訂閱事件並返回訂閱 ID
     * @param event 事件名稱
     * @param callback 事件回調函數
     * @return 訂閱 ID，可用於取消訂閱
     */
    EventBus::SubscriptionId subscribe(const QString& event, 
                                       std::function<void(const QVariant&)> callback);
    
    /**
     * @brief 取消訂閱事件
     * @param event 事件名稱
     * @param subscriptionId 訂閱 ID
     */
    void unsubscribe(const QString& event, EventBus::SubscriptionId subscriptionId);
    
private:
    // ... 現有欄位 ...
    QHash<QString, QList<EventBus::Subscription>> m_eventHandlers;
};

// plugincontext.cpp
EventBus::SubscriptionId PluginContext::subscribe(const QString& event, 
                                                  std::function<void(const QVariant&)> callback)
{
    EventBus::SubscriptionId id = EventBus::instance()->subscribe(event, callback);
    m_eventHandlers[event].append({id, callback});
    return id;
}

void PluginContext::unsubscribe(const QString& event, EventBus::SubscriptionId subscriptionId)
{
    EventBus::instance()->unsubscribe(event, subscriptionId);
    
    auto& subscriptions = m_eventHandlers[event];
    for (auto it = subscriptions.begin(); it != subscriptions.end(); ++it) {
        if (it->id == subscriptionId) {
            subscriptions.erase(it);
            break;
        }
    }
    
    if (subscriptions.isEmpty()) {
        m_eventHandlers.remove(event);
    }
}
```

---

### 6. 新增插件版本相容性檢查

#### 6.1 現有問題：缺乏版本管理

目前 [`plugininterface.h`](plugininterface.h:110) 使用硬編碼的介面 ID：

```cpp
Q_DECLARE_INTERFACE(ScripturaPlugin, "com.scriptura.plugin/1.0")
```

**問題**：
- 當 Scriptura Core 更新時（例如 `PluginContext` 新增方法、`ScripturaPlugin` 介面變更），舊插件**不會自動失效**
- 舊插件可能呼叫已刪除或已變更的方法，導致**未定義行為**（崩潰、資料損壞）
- 使用者更新 Core 後，舊插件突然無法運作，但系統沒有明確的錯誤訊息

#### 6.2 實作方案：簡化版本檢查

**原則**：不進行複雜的語義化版本比較，只做二元的「相容 / 不相容」判斷。

**`plugin.json` 格式**：

```json
{
    "id": "com.scriptura.git",
    "name": "Git Plugin",
    "version": "1.0.0",
    "dependencies": [],
    "incompatible_with": ["0.3.0", "0.4.0"]
}
```

**載入時檢查**：

```cpp
// pluginmanager.cpp
bool PluginManager::loadPlugin(const QString& filePath)
{
    // ... 現有載入邏輯 ...
    
    ScripturaPlugin* plugin = qobject_cast<ScripturaPlugin*>(pluginObj);
    if (!plugin) {
        qWarning() << "Plugin does not implement ScripturaPlugin interface:" << pluginId;
        delete loader;
        return false;
    }
    
    // 檢查插件是否標記為與當前 Core 不相容
    if (metadata.contains("incompatible_with")) {
        QStringList incompatibleVersions = metadata["incompatible_with"].toArray().toQStringList();
        QString currentCoreVersion = VersionFetcher::coreVersion();  // 從 GitHub Release 取得
        
        if (incompatibleVersions.contains(currentCoreVersion)) {
            qWarning() << "Plugin" << pluginId 
                       << "is not compatible with current Scriptura version" << currentCoreVersion;
            emit pluginIncompatible(pluginId, currentCoreVersion);
            delete loader;
            return false;
        }
    }
    
    // ... 儲存插件資訊 ...
}
```

**意義**：
- 插件開發者只在「確定不相容」時才需要更新 `plugin.json`
- 不需要維護版本範圍語法（`>=`, `~`, `^`）
- Core 更新後，舊插件預設視為相容，只有明確標記的才拒絕載入
- 錯誤訊息直接顯示：「這個插件與當前版本不相容」

#### 6.3 版本來源：內嵌於二進位（非硬編碼在邏輯中）

**關鍵問題**：如果從 GitHub API 取得「最新版本」，但使用者**沒有更新**，程式會誤以為自己是最新版，導致版本檢查失效。

**正確方案**：版本號在**建置時**寫入二進位，執行時直接讀取。

**方案 A（推薦）**：CI/CD 建置時注入版本

```cmake
# CMakeLists.txt - 在 CI 中由 GitHub Actions 注入
if(DEFINED ENV{GITHUB_REF})
    string(REGEX MATCH "refs/tags/v([0-9.]+)" _ $ENV{GITHUB_REF})
    set(SCRIPTURA_VERSION "${CMAKE_MATCH_1}")
else()
    set(SCRIPTURA_VERSION "0.0.0-dev")  # 開發版本
endif()

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/version.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/version.h"
)
```

```cpp
// version.h.in
#pragma once
#define SCRIPTURA_VERSION "@SCRIPTURA_VERSION@"
```

```cpp
// versionfetcher.h
class VersionFetcher : public QObject
{
    Q_OBJECT
public:
    static QString coreVersion();  // 從內嵌的 SCRIPTURA_VERSION 取得
};
```

```cpp
// versionfetcher.cpp
QString VersionFetcher::coreVersion()
{
    return QString(SCRIPTURA_VERSION);
}
```

**方案 B**：執行時從應用程式中讀取

```cpp
// 在 Linux 上從 /proc/self/exe 或 AppImage 中讀取版本
// 在 Windows 上從資源或版本資訊讀取
// 在 macOS 上從 Info.plist 讀取
```

**優點**：
- 版本號與實際執行的二進位完全一致
- 不依賴網路請求
- 即使使用者未更新，版本檢查仍然正確
- 插件開發者只需要在 `plugin.json` 中列出不相容的版本字串

#### 6.4 使用者體驗：版本不符時的處理

當插件因版本不符無法載入時：

1. **Plugin Manager 中顯示狀態**：
   - 🟢 正常
   - 🔴 不相容（這個插件與當前版本不相容）

2. **啟動時提示**：
   ```
   警告：插件 "Git Plugin" 與 Scriptura 當前版本不相容。
   請聯繫插件作者取得更新版本。
   ```

---

### 7. 新增插件熱重載支援

**修正 `PluginManagerDialog`**：

```cpp
// pluginmanagerdialog.h
class PluginManagerDialog : public QDialog
{
    Q_OBJECT
public slots:
    void disableSelectedPlugin();   // 新增：停用但不刪除
    void enableSelectedPlugin();    // 新增：啟用已停用的插件
    
private:
    struct PluginInfo {
        // ... 現有欄位
        bool enabled;               // 新增
    };
};
```

**實作**：
- 新增 `disabled_plugins.json` 記錄停用的插件
- `loadPlugins()` 載入時跳過停用的插件
- 提供 UI 按鈕切換啟用/停用狀態

---

### 8. 修正 EventBus 信號與回調雙重觸發

**方案**：移除 `eventPublished` 信號，僅保留回調機制。

```cpp
// eventbus.h - 移除 signal
class EventBus : public QObject
{
    Q_OBJECT
public:
    // ... 移除 eventPublished 信號 ...
    
    void publish(const QString& event, const QVariant& data = QVariant());
    // ... 其他方法不變 ...
};
```

**替代方案**：如果外部需要 Qt 信號機制，可新增一個可選的參數：

```cpp
void publish(const QString& event, const QVariant& data = QVariant(), bool emitSignal = false);
```

---

### 9. 新增應用程式內更新功能（Settings）

**需求**：在 Settings 中提供更新按鈕，讓使用者可以直接更新 Scriptura Core。

**兩個選項**：

1. **Latest Release（推薦）**
   - 從 GitHub Releases 取得最新穩定版本
   - 標籤顯示「recommended」
   - 直接下載並安裝

2. **Latest Pre-release**
   - 從 GitHub Releases 取得最新 Pre-release 版本
   - 點擊後顯示警告對話框：
     ```
     警告：這是一個預覽版本，可能不穩定。
     更新風險自負。
     是否繼續？
     ```
   - 確認後下載並安裝

**實作**：

```cpp
// updater.h (擴充)
class Updater : public QObject
{
    Q_OBJECT
public:
    enum class ReleaseType {
        Stable,      // 最新穩定版
        PreRelease   // 最新預覽版
    };
    
    struct ReleaseInfo {
        QString version;
        QString tagName;
        QString downloadUrl;
        QString changelog;
        bool isPreRelease;
        QDateTime publishedAt;
    };
    
    QList<ReleaseInfo> checkForUpdates(ReleaseType type);
    bool downloadAndInstall(const ReleaseInfo& release);
    
signals:
    void updateAvailable(const ReleaseInfo& release);
    void noUpdateAvailable();
    void updateProgress(int percentage);
    void updateCompleted();
    void updateFailed(const QString& error);
};
```

```cpp
// updater.cpp
QList<Updater::ReleaseInfo> Updater::checkForUpdates(ReleaseType type)
{
    QList<ReleaseInfo> result;
    
    QNetworkAccessManager manager;
    QNetworkRequest request(QUrl("https://api.github.com/repos/jason1015-coder/scriptura/releases"));
    QNetworkReply* reply = manager.get(request);
    
    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();
    
    if (reply->error() == QNetworkReply::NoError) {
        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
        QJsonArray releases = doc.array();
        
        for (const QJsonValue& val : releases) {
            QJsonObject release = val.toObject();
            bool isPreRelease = release["prerelease"].toBool();
            
            if (type == ReleaseType::Stable && isPreRelease) {
                continue;  // 跳過預覽版
            }
            
            // 找到第一個符合條件的版本
            QJsonArray assets = release["assets"].toArray();
            for (const QJsonValue& assetVal : assets) {
                QJsonObject asset = assetVal.toObject();
                QString name = asset["name"].toString();
                
                // 根據平台選擇對應的安裝檔
                if (isCurrentPlatform(name)) {
                    ReleaseInfo info;
                    info.version = release["tag_name"].toString().remove(0, 1);  // 移除 'v'
                    info.tagName = release["tag_name"].toString();
                    info.downloadUrl = asset["browser_download_url"].toString();
                    info.changelog = release["body"].toString();
                    info.isPreRelease = isPreRelease;
                    info.publishedAt = QDateTime::fromString(release["published_at"].toString(), Qt::ISODate);
                    result.append(info);
                    break;
                }
            }
            
            if (!result.isEmpty()) break;  // 只需要最新的一個版本
        }
    }
    
    reply->deleteLater();
    return result;
}
```

**Settings UI**：

```cpp
// settingsdialog.h
class SettingsDialog : public QDialog
{
    Q_OBJECT
public slots:
    void checkForStableUpdate();
    void checkForPreReleaseUpdate();
    
private:
    QPushButton* m_updateStableButton;
    QPushButton* m_updatePreReleaseButton;
    QLabel* m_currentVersionLabel;
    QLabel* m_latestVersionLabel;
};
```

```cpp
// settingsdialog.cpp
void SettingsDialog::checkForStableUpdate()
{
    QList<Updater::ReleaseInfo> releases = m_updater->checkForUpdates(Updater::ReleaseType::Stable);
    
    if (releases.isEmpty()) {
        QMessageBox::information(this, tr("更新檢查"), tr("已是最新版本。"));
        return;
    }
    
    const Updater::ReleaseInfo& release = releases.first();
    QString msg = tr("發現新版本：%1\n\n%2")
                  .arg(release.version)
                  .arg(release.changelog.left(500));  // 只顯示前 500 字元
    
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        tr("更新可用"),
        msg,
        QMessageBox::Yes | QMessageBox::No
    );
    
    if (reply == QMessageBox::Yes) {
        m_updater->downloadAndInstall(release);
    }
}

void SettingsDialog::checkForPreReleaseUpdate()
{
    // 顯示警告對話框
    QMessageBox::warning(this, tr("警告"),
        tr("這是一個預覽版本，可能不穩定。\n"
           "更新風險自負。\n"
           "是否繼續？"));
    
    // 使用者確認後才檢查
    if (QMessageBox::question(this, tr("確認"), tr("是否繼續檢查預覽版本？")) 
        != QMessageBox::Yes) {
        return;
    }
    
    // ... 同 checkForStableUpdate() 邏輯 ...
}
```

**下載與安裝流程**：

1. 下載對應平台的安裝檔（`.AppImage` / `.exe` / `.dmg`）
2. 顯示進度條
3. 下載完成後：
   - Linux：替換現有執行檔，提示使用者重新啟動
   - Windows：執行安裝程式
   - macOS：掛載 DMG，提示使用者拖曳至 Applications

---

## 實作優先順序

| 優先度 | 項目 | 理由 |
|--------|------|------|
| P0 | #1 getPlugin() 類型安全 | 插件間協作完全失效 |
| P0 | #2 EventBus 死鎖 | 多執行緒下會凍結 |
| P0 | #6 版本檢查 | Core 更新後舊插件會崩潰 |
| P1 | #4 載入失敗處理 | 影響穩定性 |
| P1 | #5 取消訂閱方法 | 資源洩漏風險 |
| P1 | #7 熱重載 | 使用者體驗改善 |
| P1 | #9 應用程式內更新 | 使用者體驗改善 |
| P2 | #3 依賴驗證 | 改善錯誤訊息 |
| P2 | #8 信號雙重觸發 | 輕微效能問題 |

---

## 測試建議

每個修復完成後，應新增對應的單元測試：

1. **getPlugin()**：測試兩個插件互相獲取實例
2. **EventBus 死鎖**：測試回調內訂閱/取消訂閱不會凍結
3. **依賴驗證**：測試載入失敗的依賴能被正確識別
4. **取消訂閱**：測試訂閱後取消，回調不再被呼叫
5. **版本檢查**：測試不相容版本被正確拒絕
6. **熱重載**：測試停用後重新啟用，插件狀態正確恢復
