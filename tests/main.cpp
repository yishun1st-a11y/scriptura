#include <QtTest/QtTest>
#include <QApplication>
#include "test_pluginsystem.h"
#include "test_configvalidator.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    TestPluginSystem pluginTests;
    TestConfigValidator configTests;

    int result = 0;

    result |= QTest::qExec(&pluginTests, argc, argv);
    result |= QTest::qExec(&configTests, argc, argv);

    return result;
}
