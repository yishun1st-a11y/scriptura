#include <QTest>
#include <QTemporaryDir>
#include <QFile>
#include <QSettings>
#include <QJsonObject>
#include "configvalidator.h"
#include "test_configvalidator.h"

class TestConfigValidator : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanupTestCase();
    void testValidTabWidthAccepted();
    void testInvalidTabWidthRejected();
    void testValidBooleanAccepted();
    void testValidVersionStringAccepted();
    void testValidateSettingsReturnsEmptyOnCleanSettings();
    void testResetInvalidSettingsRestoresDefaults();
    void testGetValidatedValueReturnsDefaultForInvalid();
private:
    QString m_settingsPath;
};

void TestConfigValidator::initTestCase()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    m_settingsPath = dir.filePath("settings.ini");
    QSettings::setPath(QSettings::NativeFormat, QSettings::UserScope, dir.path());
    QSettings::setPath(QSettings::NativeFormat, QSettings::SystemScope, dir.path());
}

void TestConfigValidator::cleanupTestCase()
{
    QSettings::setPath(QSettings::NativeFormat, QSettings::UserScope, QString());
    QSettings::setPath(QSettings::NativeFormat, QSettings::SystemScope, QString());
}

void TestConfigValidator::testValidTabWidthAccepted()
{
    ConfigValidator validator;

    QSettings settings;
    settings.setValue("editor/tabWidth", 4);
    settings.sync();

    QStringList invalid = validator.validateSettings();
    QVERIFY(!invalid.contains("editor/tabWidth"));
}

void TestConfigValidator::testInvalidTabWidthRejected()
{
    ConfigValidator validator;

    QSettings settings;
    settings.setValue("editor/tabWidth", 99);
    settings.sync();

    QStringList invalid = validator.validateSettings();
    QVERIFY(invalid.contains("editor/tabWidth"));
}

void TestConfigValidator::testValidBooleanAccepted()
{
    ConfigValidator validator;

    QSettings settings;
    settings.setValue("ui/sidebarCollapsed", true);
    settings.sync();

    QStringList invalid = validator.validateSettings();
    QVERIFY(!invalid.contains("ui/sidebarCollapsed"));
}

void TestConfigValidator::testValidVersionStringAccepted()
{
    ConfigValidator validator;

    QSettings settings;
    settings.setValue("updater/currentVersion", "1.2.3");
    settings.sync();

    QStringList invalid = validator.validateSettings();
    QVERIFY(!invalid.contains("updater/currentVersion"));
}

void TestConfigValidator::testValidateSettingsReturnsEmptyOnCleanSettings()
{
    ConfigValidator validator;

    QSettings settings;
    settings.clear();
    settings.sync();

    QStringList invalid = validator.validateSettings();
    QCOMPARE(invalid, QStringList());
}

void TestConfigValidator::testResetInvalidSettingsRestoresDefaults()
{
    ConfigValidator validator;

    QSettings settings;
    settings.setValue("editor/tabWidth", 99);
    settings.setValue("updater/checkInterval", 999);
    settings.sync();

    validator.resetInvalidSettings();

    QCOMPARE(settings.value("editor/tabWidth").toInt(), 4);
    QCOMPARE(settings.value("updater/checkInterval").toInt(), 7);
}

void TestConfigValidator::testGetValidatedValueReturnsDefaultForInvalid()
{
    ConfigValidator validator;

    QSettings settings;
    settings.setValue("editor/tabWidth", 99);
    settings.sync();

    int val = validator.getValidatedValue<int>("editor/tabWidth", 4);
    QCOMPARE(val, 4);
}

