#ifndef EVENTBUS_H
#define EVENTBUS_H

#include <QObject>
#include <QVariant>
#include <QMutex>
#include <QMutexLocker>
#include <functional>
#include <QHash>

/**
 * @file eventbus.h
 * @brief 定義事件總線系統，用於插件間非同步通訊
 * 
 * EventBus 實作了發佈-訂閱模式，允許插件之間：
 * - 發佈事件通知
 * - 訂閱感興趣的事件
 * - 取消訂閱事件
 */

/**
 * @class EventBus
 * @brief 全局事件總線，實作發佈-訂閱模式
 * 
 * 這是一個單例模式的類別，提供線程安全的事件發佈機制。
 * 插件可以使用它來進行解耦的通訊。
 */
class EventBus : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief 訂閱令牌，用於取消訂閱
     */
    using SubscriptionId = quint64;
    
    /**
     * @brief 無效的訂閱 ID
     */
    static constexpr SubscriptionId InvalidSubscriptionId = 0;

    /**
     * @brief 獲取 EventBus 單例實例
     * @return EventBus 單例指標
     */
    static EventBus* instance();
    
    /**
     * @brief 刪除單例實例
     * 
     * 在應用程式結束時呼叫以清理資源
     */
    static void destroyInstance();

    /**
     * @brief 發佈事件
     * @param event 事件名稱
     * @param data 事件資料 (可選)
     */
    void publish(const QString& event, const QVariant& data = QVariant());
    
    /**
     * @brief 訂閱事件
     * @param event 事件名稱
     * @param callback 事件回調函數
     * @param type Qt 連接類型 (預設為 Qt::AutoConnection)
     * @return 訂閱 ID，可用於取消訂閱
     */
    SubscriptionId subscribe(const QString& event, 
                           std::function<void(const QVariant&)> callback,
                           Qt::ConnectionType type = Qt::AutoConnection);
    
    /**
     * @brief 取消訂閱事件
     * @param event 事件名稱
     * @param subscriptionId 訂閱 ID
     */
    void unsubscribe(const QString& event, SubscriptionId subscriptionId);
    
    /**
     * @brief 檢查是否有訂閱者
     * @param event 事件名稱
     * @return 有訂閱者返回 true
     */
    bool hasSubscribers(const QString& event) const;

signals:
    /**
     * @brief 事件發佈信號
     * @param event 事件名稱
     * @param data 事件資料
     */
    void eventPublished(const QString& event, const QVariant& data);

private:
    /**
     * @brief 訂閱資訊
     */
    struct Subscription {
        SubscriptionId id;
        std::function<void(const QVariant&)> callback;
    };

    /**
     * @brief 私有建構函數 (單例模式)
     */
    EventBus(QObject* parent = nullptr);
    
    /**
     * @brief 私有解構函數
     */
    ~EventBus() override;
    
    // 禁止複製和賦值
    EventBus(const EventBus&) = delete;
    EventBus& operator=(const EventBus&) = delete;
    
    QHash<QString, QList<Subscription>> m_subscribers;
    mutable QMutex m_mutex;
    SubscriptionId m_nextId = 1;
    
    static EventBus* s_instance;
};

#endif // EVENTBUS_H