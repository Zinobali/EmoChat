#ifndef CHATTCPSERVER_H
#define CHATTCPSERVER_H

#include <QTcpServer>
#include <QThreadPool>
#include "msgsocket.h"
#include "userlist.h"
#include "messageprocessor.h"
#include "sendmessage.h"
#include "QTimer"


class ChatTcpServer : public QTcpServer
{
    Q_OBJECT
public:
    static ChatTcpServer* getInstance();
    QMap<qintptr, MsgSocket *> clients() const;

private:
    explicit ChatTcpServer(QObject *parent = nullptr);//构造函数
    void messageReadySlot(qint64 type, QByteArray data);//读取消息槽
    void sendMessageSlot();//发送消息槽
    void initMessageQueue();//初始化消息队列

signals:
    void ClientConnection(qintptr socketDescriptor);
    void messageReady(qint64 type, QByteArray data);
    void userOnline(const int &uid);
    void userOffline(const int &uid);

protected:
    virtual void incomingConnection(qintptr socketDescriptor) override;
    void onClientDisconnected();

private:
    static ChatTcpServer* m_instance;
    QMap<qintptr, MsgSocket*> m_clients;
    MessageQueue* m_messageQueue;
    UserList* m_userlist;
};

#endif // CHATTCPSERVER_H
