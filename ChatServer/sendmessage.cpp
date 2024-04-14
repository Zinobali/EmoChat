#include "sendmessage.h"

SendMessage::SendMessage(QObject *parent)
    : QObject{parent}
{
    setAutoDelete(true);
    // 添加一行分割线
    qDebug() << "**********************************";
    qDebug() << "发送消息线程运行开始";
}

SendMessage::~SendMessage()
{
    qDebug() << "发送消息线程运行结束";
    qDebug() << "**********************************";
}

void SendMessage::sendMsg(Message *msg)
{
    // 获取clients
    QMap<qintptr, MsgSocket *> clients = ChatTcpServer::getInstance()->clients();
    // 根据receiverId找到对应的socket
    // 发送消息
}

void SendMessage::run()
{
    // 从队列中取出消息
    qDebug() << "发送消息进入run函数";
    MessageQueue *MsgQueue = MessageQueue::getInstance();

    while (true)
    {
        if (MsgQueue->isEmpty())
        {
            qDebug() << "当前消息队列为空，等待新消息...";
            QThread::sleep(5);
            continue;
        }
        qDebug() << "取出消息";
        Message *msg = MsgQueue->dequeue();
        qDebug() << "发送消息";
        sendMsg(msg);
        qDebug() << "释放消息";
        delete msg;
    }
}
