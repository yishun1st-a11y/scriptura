#ifndef TEST_CONFIGVALIDATOR_H
#define TEST_CONFIGVALIDATOR_H

#include <QObject>

class TestConfigValidator : public QObject
{
    Q_OBJECT
public slots:
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

#endif // TEST_CONFIGVALIDATOR_H
