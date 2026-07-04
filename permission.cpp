#include "permission.h"
#include <QDebug>

bool PermissionManager::checkPermission(const QString& pluginId, Permission permission)
{
    if (!m_grantedPermissions.contains(pluginId)) {
        return false;
    }
    
    return m_grantedPermissions[pluginId].contains(permission);
}

void PermissionManager::requestPermission(const QString& pluginId, Permission permission)
{
    // 檢查是否已宣告
    if (!m_declaredPermissions.contains(pluginId) || 
        !m_declaredPermissions[pluginId].contains(permission)) {
        qWarning() << "Plugin" << pluginId << "requested undeclared permission:" << static_cast<int>(permission);
        return;
    }
    
    // 這裡可以觸發權限對話框
    // 目前預設授予所有宣告的權限
    grantPermission(pluginId, permission);
}

void PermissionManager::grantPermission(const QString& pluginId, Permission permission)
{
    if (!m_grantedPermissions.contains(pluginId)) {
        m_grantedPermissions[pluginId] = QList<Permission>();
    }
    
    if (!m_grantedPermissions[pluginId].contains(permission)) {
        m_grantedPermissions[pluginId].append(permission);
    }
}

void PermissionManager::revokePermission(const QString& pluginId, Permission permission)
{
    if (m_grantedPermissions.contains(pluginId)) {
        m_grantedPermissions[pluginId].removeOne(permission);
    }
}

QList<Permission> PermissionManager::grantedPermissions(const QString& pluginId) const
{
    if (!m_grantedPermissions.contains(pluginId)) {
        return QList<Permission>();
    }
    return m_grantedPermissions[pluginId];
}

void PermissionManager::setDeclaredPermissions(const QString& pluginId, const QList<Permission>& permissions)
{
    m_declaredPermissions[pluginId] = permissions;
}

QList<Permission> PermissionManager::declaredPermissions(const QString& pluginId) const
{
    if (!m_declaredPermissions.contains(pluginId)) {
        return QList<Permission>();
    }
    return m_declaredPermissions[pluginId];
}