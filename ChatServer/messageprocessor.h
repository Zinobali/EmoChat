#ifndef MESSAGEPROCESSOR_H
#define MESSAGEPROCESSOR_H

#include <QObject>
#include <QRunnable>
#include "messagequeue.h"
#include "msgsocket.h"
#include "userlist.h"

class MessageProcessor : public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit MessageProcessor(qint64 type, const QByteArray &data, const MsgSocket* const curSocket);
    ~MessageProcessor();

private:
    Message* signinHandler(Message* msg);
    Message* signupHandler(Message* msg);
    Message* onlineHandler(Message* msg);

public:
    virtual void run() override;

signals:
    void userOnline(const int &uid);

private:
    Message::TYPE m_type;
    QByteArray m_data;
    const MsgSocket* const m_curSocket;
};

#endif // MESSAGEPROCESSOR_H
