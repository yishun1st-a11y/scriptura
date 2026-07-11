#include "servicelocator.h"
#include <QDebug>

// 靜態成員初始化
ServiceLocator* ServiceLocator::s_instance = nullptr;

ServiceLocator* ServiceLocator::instance()
{
    if (!s_instance) {
        s_instance = new ServiceLocator();
    }
    return s_instance;
}

void ServiceLocator::destroyInstance()
{
    if (s_instance) {
        delete s_instance;
        s_instance = nullptr;
    }
}

ServiceLocator::ServiceLocator(QObject* parent)
    : QObject(parent)
{
}

ServiceLocator::~ServiceLocator()
{
    QMutexLocker locker(&m_mutex);
    // 注意：我們不擁有服務物件的所有權，不進行刪除
    m_services.clear();
}

void ServiceLocator::unregisterService(const QString& id)
{
    QMutexLocker locker(&m_mutex);
    m_services.remove(id);
}

bool ServiceLocator::hasService(const QString& id) const
{
    QMutexLocker locker(&m_mutex);
    return m_services.contains(id);
}

QStringList ServiceLocator::registeredServices() const
{
    QMutexLocker locker(&m_mutex);
    return m_services.keys();
}