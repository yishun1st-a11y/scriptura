#ifndef TEST_EVENTBUS_H
#define TEST_EVENTBUS_H

#include <QObject>

class TestEventBus : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanupTestCase();
    void testPublishAndSubscribe();
    void testUnsubscribe();
    void testHasSubscribers();
    void testPublishWithNoSubscribers();
    void testMultipleSubscribers();
    void testExceptionInCallback();
    void testSingletonInstance();
};

#endif // TEST_EVENTBUS_H
