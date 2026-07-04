#ifndef TEST_PLUGINSYSTEM_H
#define TEST_PLUGINSYSTEM_H

#include <QObject>

class TestPluginSystem : public QObject
{
    Q_OBJECT

public:
    TestPluginSystem() = default;

private slots:
    void initTestCase();
    void cleanupTestCase();
    void testDependencyResolver_data();
    void testDependencyResolver();
    void testCircularDependency();
    void testMissingDependency();
    void testEventBus_publish();
    void testEventBus_subscribe();
    void testEventBus_unsubscribe();
    void testServiceLocator_register();
    void testServiceLocator_get();
    void testServiceLocator_unregister();
    void testPluginSettings_value();
    void testPluginSettings_setValue();
    void testPluginSettings_defaults();
    void testPermissionManager_check();
    void testPermissionManager_grant();
    void testPermissionManager_revoke();
    void testPluginManager_loadMetadata();
    void testPluginManager_dependencyGraph();
    void testPluginManager_pluginsWithFeature();
    void testPluginContext_settings();
    void testPluginContext_eventBus();
    void testPluginUpdater_registryUrl();
    void testPluginCrashHandler_disable();
    void testPluginCrashHandler_recentCrashes();
};

#endif // TEST_PLUGINSYSTEM_H
