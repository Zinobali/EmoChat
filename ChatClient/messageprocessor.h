#ifndef MESSAGEPROCESSOR_H
#define MESSAGEPROCESSOR_H

#include <QObject>
#include "msgsocket.h"
#include "message.h"

class MessageProcessor : public QObject
{
    Q_OBJECT
public:
    explicit MessageProcessor(const MsgSocket *const curSocket);
    ~MessageProcessor();

    void processMessage(qint64 type, QByteArray data);

private:
    void onlineReplyHandler(Message *);
    void textMessageHandler(Message *);
signals:
    void friendListUpdated(QVector<User> friends);
    void textMessageReceived(int, QString);

private:
    const MsgSocket *const m_curSocket;
};

#endif // MESSAGEPROCESSOR_H
