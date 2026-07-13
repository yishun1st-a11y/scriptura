#include "eventbus.h"
#include <QDebug>

// 靜態成員初始化
EventBus* EventBus::s_instance = nullptr;

static QMutex s_instanceMutex;

EventBus* EventBus::instance()
{
    QMutexLocker locker(&s_instanceMutex);
    if (!s_instance) {
        s_instance = new EventBus();
    }
    return s_instance;
}

void EventBus::destroyInstance()
{
    QMutexLocker locker(&s_instanceMutex);
    if (s_instance) {
        delete s_instance;
        s_instance = nullptr;
    }
}

EventBus::EventBus(QObject* parent)
    : QObject(parent)
{
}

EventBus::~EventBus()
{
    QMutexLocker locker(&m_mutex);
    m_subscribers.clear();
}

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
    
    // 直接呼叫回調函數 (在鎖外執行，避免死鎖)
    for (const auto& subscription : callbacks) {
        // 防禦：若訂閱綁定了擁有者且擁有者已被銷毀，跳過以避免 use-after-free
        if (subscription.hasReceiver && subscription.receiver.isNull()) {
            continue;
        }
        try {
            subscription.callback(data);
        } catch (const std::exception& e) {
            qWarning() << "EventBus: Exception in callback for event" << event << ":" << e.what();
        } catch (...) {
            qWarning() << "EventBus: Unknown exception in callback for event" << event;
        }
    }
}

EventBus::SubscriptionId EventBus::subscribe(const QString& event, 
                                             std::function<void(const QVariant&)> callback,
                                             Qt::ConnectionType type)
{
    // 向後相容：未指定擁有者時不綁定生命週期
    return subscribe(event, nullptr, std::move(callback), type);
}

EventBus::SubscriptionId EventBus::subscribe(const QString& event,
                                             QObject* receiver,
                                             std::function<void(const QVariant&)> callback,
                                             Qt::ConnectionType type)
{
    Q_UNUSED(type); // Qt::ConnectionType 保留給未來的 Qt 信號連接使用

    QMutexLocker locker(&m_mutex);

    const SubscriptionId id = m_nextId++;
    Subscription sub;
    sub.id = id;
    sub.callback = std::move(callback);
    sub.receiver = receiver;
    sub.hasReceiver = (receiver != nullptr);
    m_subscribers[event].append(sub);

    // 綁定擁有者生命週期：擁有者銷毀時自動取消訂閱
    if (receiver) {
        connect(receiver, &QObject::destroyed, this, [this, event, id]() {
            unsubscribe(event, id);
        });
    }

    return id;
}

void EventBus::unsubscribeReceiver(QObject* receiver)
{
    if (!receiver) {
        return;
    }

    QMutexLocker locker(&m_mutex);

    for (auto it = m_subscribers.begin(); it != m_subscribers.end();) {
        QList<Subscription>& subscriptions = it.value();
        for (auto sit = subscriptions.begin(); sit != subscriptions.end();) {
            if (sit->receiver.data() == receiver) {
                sit = subscriptions.erase(sit);
            } else {
                ++sit;
            }
        }
        if (subscriptions.isEmpty()) {
            it = m_subscribers.erase(it);
        } else {
            ++it;
        }
    }
}

void EventBus::unsubscribe(const QString& event, SubscriptionId subscriptionId)
{
    QMutexLocker locker(&m_mutex);
    
    if (!m_subscribers.contains(event)) {
        return;
    }
    
    auto& subscriptions = m_subscribers[event];
    for (auto it = subscriptions.begin(); it != subscriptions.end(); ++it) {
        if (it->id == subscriptionId) {
            subscriptions.erase(it);
            break;
        }
    }
    
    // 如果沒有訂閱者了，移除該事件
    if (subscriptions.isEmpty()) {
        m_subscribers.remove(event);
    }
}

bool EventBus::hasSubscribers(const QString& event) const
{
    QMutexLocker locker(&m_mutex);
    return m_subscribers.contains(event) && !m_subscribers[event].isEmpty();
}