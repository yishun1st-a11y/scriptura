#ifndef LENGTHPREFIXEDFRAMER_H
#define LENGTHPREFIXEDFRAMER_H

#include <QObject>
#include <QByteArray>
#include <QMutex>
#include <QRegularExpression>

class LengthPrefixedFramer : public QObject
{
    Q_OBJECT
public:
    explicit LengthPrefixedFramer(QObject *parent = nullptr);
    void append(const QByteArray &data);
    bool canReadMessage() const;
    QByteArray readMessage();
    static QByteArray frame(const QByteArray &payload);
    void clear();

private:
    QByteArray m_buffer;
    mutable QMutex m_mutex;
};

#endif // LENGTHPREFIXEDFRAMER_H
