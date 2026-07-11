#include "permission.h"
#include <QDebug>
#include <QMessageBox>

namespace {
QString permissionName(Permission p)
{
    switch (p) {
        case Permission::FileRead: return QObject::tr("File Read");
        case Permission::FileWrite: return QObject::tr("File Write");
        case Permission::NetworkAccess: return QObject::tr("Network Access");
        case Permission::ProcessExecution: return QObject::tr("Process Execution");
        case Permission::SystemSettings: return QObject::tr("System Settings");
        case Permission::ClipboardAccess: return QObject::tr("Clipboard Access");
        case Permission::Notification: return QObject::tr("Notification");
    }
    return QObject::tr("Unknown");
}
}

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

    // 已經授予則不再詢問
    if (m_grantedPermissions.contains(pluginId) &&
        m_grantedPermissions[pluginId].contains(permission)) {
        return;
    }

    // 觸發權限對話框，取得使用者核准
    QMessageBox::StandardButton reply = QMessageBox::question(
        nullptr,
        QObject::tr("Permission Request"),
        QObject::tr("Plugin \"%1\" is requesting permission: %2.\n\nGrant this permission?")
            .arg(pluginId, permissionName(permission)),
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes)
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