#ifndef SENDMESSAGE_H
#define SENDMESSAGE_H

#include <QObject>
#include <QRunnable>
#include "msgsocket.h"
#include "messagequeue.h"
#include "userlist.h"
#include "chattcpserver.h"

class SendMessage : public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit SendMessage(QObject *parent = nullptr);
    ~SendMessage();

signals:

public:
    virtual void run() override;

private:
    void sendMsg(Message* msg);
};

#endif // SENDMESSAGE_H
