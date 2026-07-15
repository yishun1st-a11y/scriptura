#include <QTest>
#include "lengthprefixedframer.h"
#include "test_lengthprefixedframer.h"

void TestLengthPrefixedFramer::testBasicFrameAndRead()
{
    LengthPrefixedFramer framer;
    QByteArray payload = "Hello, World!";
    QByteArray framed = LengthPrefixedFramer::frame(payload);
    framer.append(framed);
    QVERIFY(framer.canReadMessage());
    QByteArray read = framer.readMessage();
    QCOMPARE(read, payload);
    QVERIFY(!framer.canReadMessage());
}

void TestLengthPrefixedFramer::testPartialFrameAndReadSplitContent()
{
    LengthPrefixedFramer framer;
    QByteArray payload = "This is a test payload with some content";
    QByteArray framed = LengthPrefixedFramer::frame(payload);
    int split = framed.size() / 2;
    framer.append(framed.left(split));
    QVERIFY(!framer.canReadMessage());
    framer.append(framed.mid(split));
    QVERIFY(framer.canReadMessage());
    QByteArray read = framer.readMessage();
    QCOMPARE(read, payload);
}

void TestLengthPrefixedFramer::testMultipleMessages()
{
    LengthPrefixedFramer framer;
    QByteArray p1 = "First message";
    QByteArray p2 = "Second message with more data";
    QByteArray p3 = "Third";
    QByteArray framed = LengthPrefixedFramer::frame(p1) +
                        LengthPrefixedFramer::frame(p2) +
                        LengthPrefixedFramer::frame(p3);
    framer.append(framed);

    QVERIFY(framer.canReadMessage());
    QCOMPARE(framer.readMessage(), p1);
    QVERIFY(framer.canReadMessage());
    QCOMPARE(framer.readMessage(), p2);
    QVERIFY(framer.canReadMessage());
    QCOMPARE(framer.readMessage(), p3);
    QVERIFY(!framer.canReadMessage());
}

void TestLengthPrefixedFramer::testInvalidContentLength()
{
    LengthPrefixedFramer framer;
    QByteArray invalid = "Content-Length: abc\r\n\r\npayload";
    framer.append(invalid);
    QVERIFY(!framer.canReadMessage());
}

void TestLengthPrefixedFramer::testClearBuffer()
{
    LengthPrefixedFramer framer;
    QByteArray payload = "Test";
    framer.append(LengthPrefixedFramer::frame(payload));
    QVERIFY(framer.canReadMessage());
    framer.clear();
    QVERIFY(!framer.canReadMessage());
}
