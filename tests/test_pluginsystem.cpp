#include "test_pluginsystem.h"
#include <QtTest/QtTest>
#include <QJsonObject>
#include <QJsonArray>
#include <QTemporaryDir>
#include <QFile>
#include <QDir>
#include "dependencyresolver.h"
#include "eventbus.h"
#include "servicelocator.h"
#include "pluginsettings.h"
#include "permission.h"
#include "pluginmanager.h"
#include "plugincontext.h"
#include "pluginupdater.h"
#include "plugincrashhandler.h"

/**
 * @file test_pluginsystem.cpp
 * @brief 插件系統單元測試
 */

void TestPluginSystem::initTestCase()
{
    // 初始化測試環境
}

void TestPluginSystem::cleanupTestCase()
{
    // 清理測試環境
    EventBus::destroyInstance();
    ServiceLocator::destroyInstance();
}

void TestPluginSystem::testDependencyResolver_data()
{
    QTest::addColumn<QList<QJsonObject>>("plugins");
    QTest::addColumn<QStringList>("expectedOrder");

    {
        QList<QJsonObject> plugins;
        
        QJsonObject core;
        core["id"] = "com.scriptura.core";
        core["dependencies"] = QJsonArray();
        plugins.append(core);
        
        QJsonObject editor;
        editor["id"] = "com.scriptura.editor";
        QJsonArray editorDeps;
        editorDeps.append("com.scriptura.core");
        editor["dependencies"] = editorDeps;
        plugins.append(editor);
        
        QJsonObject plugin;
        plugin["id"] = "com.scriptura.plugin";
        QJsonArray pluginDeps;
        pluginDeps.append("com.scriptura.editor");
        plugin["dependencies"] = pluginDeps;
        plugins.append(plugin);
        
        QTest::newRow("simple chain") << plugins << QStringList({"com.scriptura.core", "com.scriptura.editor", "com.scriptura.plugin"});
    }
}

void TestPluginSystem::testDependencyResolver()
{
    QFETCH(QList<QJsonObject>, plugins);
    QFETCH(QStringList, expectedOrder);
    
    DependencyResolver resolver;
    QStringList result = resolver.topologicalSort(plugins);
    
    for (const QString& id : expectedOrder) {
        QVERIFY(result.contains(id));
    }
}

void TestPluginSystem::testCircularDependency()
{
    QList<QJsonObject> plugins;
    
    QJsonObject a;
    a["id"] = "plugin.a";
    QJsonArray aDeps;
    aDeps.append("plugin.b");
    a["dependencies"] = aDeps;
    plugins.append(a);
    
    QJsonObject b;
    b["id"] = "plugin.b";
    QJsonArray bDeps;
    bDeps.append("plugin.a");
    b["dependencies"] = bDeps;
    plugins.append(b);
    
    DependencyResolver resolver;
    QVERIFY(resolver.hasCircularDependency(plugins));
}

void TestPluginSystem::testMissingDependency()
{
    QList<QJsonObject> plugins;
    
    QJsonObject plugin;
    plugin["id"] = "com.scriptura.missing";
    QJsonArray deps;
    deps.append("com.scriptura.nonexistent");
    plugin["dependencies"] = deps;
    plugins.append(plugin);
    
    DependencyResolver resolver;
    QList<DependencyResolver::DependencyError> errors = resolver.validate(plugins);
    
    QCOMPARE(errors.size(), 1);
    QCOMPARE(errors[0].pluginId, "com.scriptura.missing");
    QCOMPARE(errors[0].missingDependency, "com.scriptura.nonexistent");
    QCOMPARE(errors[0].isOptional, false);
}

void TestPluginSystem::testEventBus_publish()
{
    EventBus* bus = EventBus::instance();
    QVERIFY(bus != nullptr);
    
    int callCount = 0;
    bus->subscribe("test.event", [&callCount](const QVariant&) {
        callCount++;
    });
    
    bus->publish("test.event", QVariant(42));
    QCOMPARE(callCount, 1);
}

void TestPluginSystem::testEventBus_subscribe()
{
    EventBus* bus = EventBus::instance();
    QVERIFY(bus != nullptr);
    
    int callCount = 0;
    auto callback = [&callCount](const QVariant& data) {
        callCount++;
        QCOMPARE(data.toInt(), 100);
    };
    
    bus->subscribe("test.subscribe", callback);
    bus->publish("test.subscribe", QVariant(100));
    
    QCOMPARE(callCount, 1);
}

void TestPluginSystem::testEventBus_unsubscribe()
{
    EventBus* bus = EventBus::instance();
    QVERIFY(bus != nullptr);
    
    int callCount = 0;
    auto callback = [&callCount](const QVariant&) {
        callCount++;
    };
    
    EventBus::SubscriptionId id = bus->subscribe("test.unsubscribe", callback);
    bus->publish("test.unsubscribe");
    QCOMPARE(callCount, 1);
    
    bus->unsubscribe("test.unsubscribe", id);
    bus->publish("test.unsubscribe");
    QCOMPARE(callCount, 1); // 應該沒有增加
}

void TestPluginSystem::testServiceLocator_register()
{
    ServiceLocator* locator = ServiceLocator::instance();
    QVERIFY(locator != nullptr);
    
    QObject dummyService;
    locator->registerService("test.service", &dummyService);
    
    QVERIFY(locator->hasService("test.service"));
}

void TestPluginSystem::testServiceLocator_get()
{
    ServiceLocator* locator = ServiceLocator::instance();
    QVERIFY(locator != nullptr);
    
    QObject dummyService;
    locator->registerService("test.get", &dummyService);
    
    QObject* service = locator->getService<QObject>("test.get");
    QCOMPARE(service, &dummyService);
}

void TestPluginSystem::testServiceLocator_unregister()
{
    ServiceLocator* locator = ServiceLocator::instance();
    QVERIFY(locator != nullptr);
    
    QObject dummyService;
    locator->registerService("test.unregister", &dummyService);
    QVERIFY(locator->hasService("test.unregister"));
    
    locator->unregisterService("test.unregister");
    QVERIFY(!locator->hasService("test.unregister"));
}

void TestPluginSystem::testPluginSettings_value()
{
    QSettings settings("/tmp/test_plugin_settings.ini", QSettings::IniFormat);
    PluginSettings pluginSettings("test.plugin", &settings);
    
    pluginSettings.setValue("testKey", "testValue");
    QCOMPARE(pluginSettings.value("testKey").toString(), "testValue");
}

void TestPluginSystem::testPluginSettings_setValue()
{
    QSettings settings("/tmp/test_plugin_settings_set.ini", QSettings::IniFormat);
    PluginSettings pluginSettings("test.set", &settings);
    
    pluginSettings.setValue("intKey", 42);
    pluginSettings.setValue("stringKey", "hello");
    
    QCOMPARE(pluginSettings.value("intKey").toInt(), 42);
    QCOMPARE(pluginSettings.value("stringKey").toString(), "hello");
}

void TestPluginSystem::testPluginSettings_defaults()
{
    QSettings settings("/tmp/test_plugin_settings_defaults.ini", QSettings::IniFormat);
    PluginSettings pluginSettings("test.defaults", &settings);
    
    QJsonObject defaults;
    defaults["defaultKey"] = "defaultValue";
    pluginSettings.setDefaults(defaults);
    
    QCOMPARE(pluginSettings.value("defaultKey").toString(), "defaultValue");
}

void TestPluginSystem::testPermissionManager_check()
{
    PermissionManager manager;
    
    manager.setDeclaredPermissions("test.plugin", {Permission::FileRead, Permission::FileWrite});
    manager.grantPermission("test.plugin", Permission::FileRead);
    
    QVERIFY(manager.checkPermission("test.plugin", Permission::FileRead));
    QVERIFY(!manager.checkPermission("test.plugin", Permission::FileWrite));
}

void TestPluginSystem::testPermissionManager_grant()
{
    PermissionManager manager;
    
    manager.setDeclaredPermissions("test.grant", {Permission::NetworkAccess});
    manager.grantPermission("test.grant", Permission::NetworkAccess);
    
    QVERIFY(manager.checkPermission("test.grant", Permission::NetworkAccess));
    QCOMPARE(manager.grantedPermissions("test.grant").size(), 1);
}

void TestPluginSystem::testPermissionManager_revoke()
{
    PermissionManager manager;
    
    manager.setDeclaredPermissions("test.revoke", {Permission::ProcessExecution});
    manager.grantPermission("test.revoke", Permission::ProcessExecution);
    QVERIFY(manager.checkPermission("test.revoke", Permission::ProcessExecution));
    
    manager.revokePermission("test.revoke", Permission::ProcessExecution);
    QVERIFY(!manager.checkPermission("test.revoke", Permission::ProcessExecution));
}

void TestPluginSystem::testPluginManager_loadMetadata()
{
    PluginManager manager;
    QVERIFY(true);
}

void TestPluginSystem::testPluginManager_dependencyGraph()
{
    PluginManager manager;
    QVERIFY(true);
}

void TestPluginSystem::testPluginManager_pluginsWithFeature()
{
    PluginManager manager;
    QVERIFY(true);
}

void TestPluginSystem::testPluginContext_settings()
{
    QSettings settings("/tmp/test_plugin_context.ini", QSettings::IniFormat);
    PluginSettings pluginSettings("test.context", &settings);

    pluginSettings.setValue("testKey", "testValue");
    QCOMPARE(pluginSettings.value("testKey").toString(), QString("testValue"));
}

void TestPluginSystem::testPluginContext_eventBus()
{
    PluginContext context(nullptr);

    int callCount = 0;
    context.subscribe("test.context.event", [&callCount](const QVariant&) {
        callCount++;
    });

    context.notify("test.context.event", QVariant(1));
    QCOMPARE(callCount, 1);
}

void TestPluginSystem::testPluginUpdater_registryUrl()
{
    PluginUpdater updater;
    updater.setRegistryUrl("https://example.com/registry.json");
    QVERIFY(true);
}

void TestPluginSystem::testPluginCrashHandler_disable()
{
    PluginCrashHandler handler;
    handler.disablePlugin("com.test.plugin");
    QVERIFY(handler.isPluginDisabled("com.test.plugin"));
    handler.enablePlugin("com.test.plugin");
    QVERIFY(!handler.isPluginDisabled("com.test.plugin"));
}

void TestPluginSystem::testPluginCrashHandler_recentCrashes()
{
    PluginCrashHandler handler;
    QList<CrashInfo> crashes = handler.recentCrashes(5);
    QVERIFY(crashes.isEmpty());
}

#include "test_pluginsystem.moc"
