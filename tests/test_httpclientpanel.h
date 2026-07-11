#ifndef TEST_HTTPCLIENTPANEL_H
#define TEST_HTTPCLIENTPANEL_H

#include <QObject>

class TestHttpClientPanel : public QObject
{
    Q_OBJECT
public slots:
    void testSubstituteNoEnvReturnsInputUnchanged();
    void testSubstituteBasicVar();
    void testSubstituteMissingVarLeavesPlaceholder();
    void testSubstituteMultipleOccurrences();
    void testSubstituteAdjacentVars();
    void testSubstituteEmptyInput();
    void testSubstituteNoMatch();
    void testSubstituteRepeatedVar();
};

#endif // TEST_HTTPCLIENTPANEL_H
