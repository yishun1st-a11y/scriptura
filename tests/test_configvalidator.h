#ifndef TEST_CONFIGVALIDATOR_H
#define TEST_CONFIGVALIDATOR_H

#include <QObject>

class TestConfigValidator : public QObject
{
    Q_OBJECT

public:
    TestConfigValidator() = default;

private slots:
    void initTestCase();
    void cleanupTestCase();
    void testValidateSettings_data();
    void testValidateSettings();
    void testGetValidatedValue();
};

#endif // TEST_CONFIGVALIDATOR_H
