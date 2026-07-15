#include <QTest>
#include <QTemporaryDir>
#include <QSettings>
#include <QJsonObject>
#include "pluginsettings.h"
#include "test_pluginsettings.h"

void TestPluginSettings::initTestCase()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    m_settingsPath = dir.filePath("settings.ini");
    QSettings::setPath(QSettings::NativeFormat, QSettings::UserScope, dir.path());
    QSettings::setPath(QSettings::NativeFormat, QSettings::SystemScope, dir.path());
}

void TestPluginSettings::cleanupTestCase()
{
    QSettings::setPath(QSettings::NativeFormat, QSettings::UserScope, QString());
    QSettings::setPath(QSettings::NativeFormat, QSettings::SystemScope, QString());
}

void TestPluginSettings::testValueReturnsDefault()
{
    PluginSettings settings("test-plugin");
    QCOMPARE(settings.value("missing", "default").toString(), QString("default"));
}

void TestPluginSettings::testSetAndGetValue()
{
    PluginSettings settings("test-plugin");
    settings.setValue("key", "value");
    QCOMPARE(settings.value("key", "").toString(), QString("value"));
}

void TestPluginSettings::testSetDefaults()
{
    PluginSettings settings("test-plugin");
    QJsonObject defaults;
    defaults["font"] = "Monospace";
    defaults["fontSize"] = 12;
    settings.setDefaults(defaults);

    QCOMPARE(settings.value("font", "").toString(), QString("Monospace"));
    QCOMPARE(settings.value("fontSize", 0), 12);
}

void TestPluginSettings::testResetToDefaults()
{
    PluginSettings settings("test-plugin");
    settings.setValue("key", "value");
    settings.resetToDefaults();
    QCOMPARE(settings.value("key", "default").toString(), QString("default"));
}

void TestPluginSettings::testTypedValueAccess()
{
    PluginSettings settings("test-plugin");
    settings.setValue("count", 42);
    QCOMPARE(settings.value<int>("count", 0), 42);
}

void TestPluginSettings::testOwnsSettings()
{
    PluginSettings settings("owning-plugin");
    settings.setValue("own-key", "own-value");
    QCOMPARE(settings.value("own-key", "").toString(), QString("own-value"));
}
