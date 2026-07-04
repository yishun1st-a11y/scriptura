#include "test_configvalidator.h"
#include <QtTest/QtTest>
#include "configvalidator.h"

void TestConfigValidator::initTestCase()
{
    // Set up test environment
}

void TestConfigValidator::cleanupTestCase()
{
    // Clean up
}

void TestConfigValidator::testValidateSettings_data()
{
    QTest::addColumn<QString>("key");
    QTest::addColumn<QVariant>("value");
    QTest::addColumn<bool>("expectedValid");

    QTest::newRow("valid theme") << "theme/selected" << QVariant(0) << true;
    QTest::newRow("invalid theme negative") << "theme/selected" << QVariant(-1) << false;
    QTest::newRow("invalid theme too large") << "theme/selected" << QVariant(101) << false;
    QTest::newRow("valid tab width") << "editor/tabWidth" << QVariant(4) << true;
    QTest::newRow("invalid tab width") << "editor/tabWidth" << QVariant(0) << false;
    QTest::newRow("valid sidebar collapsed") << "ui/sidebarCollapsed" << QVariant(true) << true;
}

void TestConfigValidator::testValidateSettings()
{
    QFETCH(QString, key);
    QFETCH(QVariant, value);
    QFETCH(bool, expectedValid);

    QSettings settings;
    settings.setValue(key, value);

    ConfigValidator validator;
    QStringList invalidKeys = validator.validateSettings();

    if (expectedValid) {
        QVERIFY(!invalidKeys.contains(key));
    } else {
        QVERIFY(invalidKeys.contains(key));
    }
}

void TestConfigValidator::testGetValidatedValue()
{
    QSettings settings;
    settings.setValue("editor/tabWidth", 8);

    ConfigValidator validator;
    int tabWidth = validator.getValidatedValue<int>("editor/tabWidth", 4);
    QCOMPARE(tabWidth, 8);

    // Test with invalid value - should return default
    settings.setValue("editor/tabWidth", 100); // Invalid
    int defaultTabWidth = validator.getValidatedValue<int>("editor/tabWidth", 4);
    QCOMPARE(defaultTabWidth, 4);
}

#include "test_configvalidator.moc"
