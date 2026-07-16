#ifndef TEST_PERMISSIONMANAGER_H
#define TEST_PERMISSIONMANAGER_H

#include <QObject>

class TestPermissionManager : public QObject
{
    Q_OBJECT
private slots:
    void testCheckPermissionNotGranted();
    void testGrantAndCheckPermission();
    void testRevokePermission();
    void testRequestPermissionUndeclared();
    void testSetDeclaredPermissions();
    void testGrantedPermissionsList();
};

#endif // TEST_PERMISSIONMANAGER_H
