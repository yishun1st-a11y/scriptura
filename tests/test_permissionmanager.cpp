#include <QTest>
#include "permission.h"
#include "test_permissionmanager.h"

void TestPermissionManager::testCheckPermissionNotGranted()
{
    PermissionManager pm;
    QVERIFY(!pm.checkPermission("plugin-a", Permission::FileRead));
}

void TestPermissionManager::testGrantAndCheckPermission()
{
    PermissionManager pm;
    pm.grantPermission("plugin-a", Permission::FileRead);
    QVERIFY(pm.checkPermission("plugin-a", Permission::FileRead));
}

void TestPermissionManager::testRevokePermission()
{
    PermissionManager pm;
    pm.grantPermission("plugin-a", Permission::FileRead);
    pm.revokePermission("plugin-a", Permission::FileRead);
    QVERIFY(!pm.checkPermission("plugin-a", Permission::FileRead));
}

void TestPermissionManager::testRequestPermissionUndeclared()
{
    PermissionManager pm;
    QTest::ignoreMessage(QtWarningMsg, R"(Plugin "plugin-a" requested undeclared permission: 0)");
    pm.requestPermission("plugin-a", Permission::FileRead);
    QVERIFY(!pm.checkPermission("plugin-a", Permission::FileRead));
}

void TestPermissionManager::testSetDeclaredPermissions()
{
    PermissionManager pm;
    QList<Permission> perms{Permission::FileRead, Permission::NetworkAccess};
    pm.setDeclaredPermissions("plugin-a", perms);

    QList<Permission> declared = pm.declaredPermissions("plugin-a");
    QCOMPARE(declared.size(), 2);
    QVERIFY(declared.contains(Permission::FileRead));
    QVERIFY(declared.contains(Permission::NetworkAccess));
}

void TestPermissionManager::testGrantedPermissionsList()
{
    PermissionManager pm;
    pm.grantPermission("plugin-a", Permission::FileRead);
    pm.grantPermission("plugin-a", Permission::FileWrite);

    QList<Permission> granted = pm.grantedPermissions("plugin-a");
    QCOMPARE(granted.size(), 2);
    QVERIFY(granted.contains(Permission::FileRead));
    QVERIFY(granted.contains(Permission::FileWrite));
}
