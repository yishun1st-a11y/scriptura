#include <QTest>
#include <QVariant>
#include "eventbus.h"
#include "test_eventbus.h"

static int g_eventACallCount = 0;
static int g_eventBCallCount = 0;

void TestEventBus::initTestCase()
{
    g_eventACallCount = 0;
    g_eventBCallCount = 0;
}

void TestEventBus::cleanupTestCase()
{
    EventBus::destroyInstance();
}

void TestEventBus::testPublishAndSubscribe()
{
    EventBus* bus = EventBus::instance();
    QVERIFY(bus != nullptr);

    bool called = false;
    bus->subscribe("test-event", [&called](const QVariant& data) {
        called = true;
        QCOMPARE(data.toString(), QString("hello"));
    });

    bus->publish("test-event", "hello");
    QVERIFY(called);
}

void TestEventBus::testUnsubscribe()
{
    EventBus* bus = EventBus::instance();
    QVERIFY(bus != nullptr);

    int callCount = 0;
    EventBus::SubscriptionId id = bus->subscribe("unsub-event", [&callCount](const QVariant&) {
        ++callCount;
    });

    bus->publish("unsub-event");
    QCOMPARE(callCount, 1);

    bus->unsubscribe("unsub-event", id);
    bus->publish("unsub-event");
    QCOMPARE(callCount, 1);
}

void TestEventBus::testHasSubscribers()
{
    EventBus* bus = EventBus::instance();
    QVERIFY(bus != nullptr);

    QVERIFY(!bus->hasSubscribers("nonexistent-event"));

    EventBus::SubscriptionId id = bus->subscribe("has-sub-event", [](const QVariant&) {});
    QVERIFY(bus->hasSubscribers("has-sub-event"));

    bus->unsubscribe("has-sub-event", id);
    QVERIFY(!bus->hasSubscribers("has-sub-event"));
}

void TestEventBus::testPublishWithNoSubscribers()
{
    EventBus* bus = EventBus::instance();
    QVERIFY(bus != nullptr);

    bus->publish("no-subscribers");
    bus->publish("no-subscribers", 42);
}

void TestEventBus::testMultipleSubscribers()
{
    EventBus* bus = EventBus::instance();
    QVERIFY(bus != nullptr);

    int countA = 0;
    int countB = 0;

    bus->subscribe("multi-event", [&countA](const QVariant&) { ++countA; });
    bus->subscribe("multi-event", [&countB](const QVariant&) { ++countB; });

    bus->publish("multi-event");
    QCOMPARE(countA, 1);
    QCOMPARE(countB, 1);
}

void TestEventBus::testExceptionInCallback()
{
    EventBus* bus = EventBus::instance();
    QVERIFY(bus != nullptr);

    bool secondCalled = false;
    bus->subscribe("exception-event", [](const QVariant&) {
        throw std::runtime_error("test exception");
    });
    bus->subscribe("exception-event", [&secondCalled](const QVariant&) {
        secondCalled = true;
    });

    bus->publish("exception-event");
    QVERIFY(secondCalled);
}

void TestEventBus::testSingletonInstance()
{
    EventBus* a = EventBus::instance();
    EventBus* b = EventBus::instance();
    QCOMPARE(a, b);
    QVERIFY(a != nullptr);

    EventBus::destroyInstance();
    EventBus* c = EventBus::instance();
    QVERIFY(c != nullptr);
}
