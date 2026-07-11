#include <QTest>
#include <QJsonObject>
#include "httpclientpanel.h"
#include "test_httpclientpanel.h"

class TestHttpClientPanel : public QObject
{
    Q_OBJECT
private slots:
    void testSubstituteNoEnvReturnsInputUnchanged();
    void testSubstituteBasicVar();
    void testSubstituteMissingVarLeavesPlaceholder();
    void testSubstituteMultipleOccurrences();
    void testSubstituteAdjacentVars();
    void testSubstituteEmptyInput();
    void testSubstituteNoMatch();
    void testSubstituteRepeatedVar();
};

void TestHttpClientPanel::testSubstituteNoEnvReturnsInputUnchanged()
{
    HttpClientPanel panel;
    QCOMPARE(panel.substituteEnvVarsForTest("hello world"), QString("hello world"));
}

void TestHttpClientPanel::testSubstituteBasicVar()
{
    HttpClientPanel panel;
    QJsonObject envs;
    envs["BASE_URL"] = "http://localhost:3000";
    envs["API_KEY"] = "secret123";
    panel.setEnvVariables(envs);

    QCOMPARE(panel.substituteEnvVarsForTest("{{BASE_URL}}/api"),
             QString("http://localhost:3000/api"));
}

void TestHttpClientPanel::testSubstituteMissingVarLeavesPlaceholder()
{
    HttpClientPanel panel;
    QJsonObject envs;
    envs["BASE_URL"] = "http://localhost";
    panel.setEnvVariables(envs);

    QCOMPARE(panel.substituteEnvVarsForTest("{{BASE_URL}}/{{MISSING}}"),
             QString("http://localhost/{{MISSING}}"));
}

void TestHttpClientPanel::testSubstituteMultipleOccurrences()
{
    HttpClientPanel panel;
    QJsonObject envs;
    envs["TOKEN"] = "abc";
    panel.setEnvVariables(envs);

    QCOMPARE(panel.substituteEnvVarsForTest("{{TOKEN}} {{TOKEN}}"),
             QString("abc abc"));
}

void TestHttpClientPanel::testSubstituteAdjacentVars()
{
    HttpClientPanel panel;
    QJsonObject envs;
    envs["HOST"] = "localhost";
    envs["PORT"] = "8080";
    panel.setEnvVariables(envs);

    QCOMPARE(panel.substituteEnvVarsForTest("{{HOST}}:{{PORT}}"),
             QString("localhost:8080"));
}

void TestHttpClientPanel::testSubstituteEmptyInput()
{
    HttpClientPanel panel;
    QJsonObject envs;
    envs["X"] = "1";
    panel.setEnvVariables(envs);

    QCOMPARE(panel.substituteEnvVarsForTest(""), QString(""));
}

void TestHttpClientPanel::testSubstituteNoMatch()
{
    HttpClientPanel panel;
    QJsonObject envs;
    envs["X"] = "1";
    panel.setEnvVariables(envs);

    QCOMPARE(panel.substituteEnvVarsForTest("no placeholders here"),
             QString("no placeholders here"));
}

void TestHttpClientPanel::testSubstituteRepeatedVar()
{
    HttpClientPanel panel;
    QJsonObject envs;
    envs["BASE"] = "v1";
    panel.setEnvVariables(envs);

    QString input  = "{{BASE}}/a/{{BASE}}/b";
    QString expect = "v1/a/v1/b";
    QCOMPARE(panel.substituteEnvVarsForTest(input), expect);
}

