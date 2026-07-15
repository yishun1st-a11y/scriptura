#ifndef TEST_LENGTHPREFIXEDFRAMER_H
#define TEST_LENGTHPREFIXEDFRAMER_H

#include <QObject>

class TestLengthPrefixedFramer : public QObject
{
    Q_OBJECT
public slots:
    void testBasicFrameAndRead();
    void testPartialFrameAndReadSplitContent();
    void testMultipleMessages();
    void testInvalidContentLength();
    void testClearBuffer();
};

#endif // TEST_LENGTHPREFIXEDFRAMER_H
