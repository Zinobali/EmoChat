#include "chattcpserver.h"

ChatTcpServer *ChatTcpServer::m_instance = nullptr;

ChatTcpServer *ChatTcpServer::getInstance()
{
    if (m_instance == nullptr)
    {
        m_instance = new ChatTcpServer();
    }
    return m_instance;
}

ChatTcpServer::ChatTcpServer(QObject *parent)
    : QTcpServer{parent}
{
    m_userlist = UserList::getInstance();
    initMessageQueue();
    // m_instance->listen(QHostAddress::Any, 45678);
    qDebug() << "服务器已开启";
}

void ChatTcpServer::messageReadySlot(qint64 type, QByteArray data)
{
    MsgSocket *curSocket = qobject_cast<MsgSocket *>(sender()); // 获取发送者
    // 使用线程池处理消息
    MessageProcessor *processor = new MessageProcessor(type, data, curSocket);
    connect(processor, &MessageProcessor::userOnline, this, &ChatTcpServer::userOnline);
    QThreadPool::globalInstance()->start(processor);
}

void ChatTcpServer::sendMessageSlot()
{
    // qDebug() << "发送消息";
    // 出队
    Message *message = m_messageQueue->dequeue();
    // qDebug() << "消息出队成功";
    // 获取接收者id
    int receiverId = message->receiverId();
    // qDebug() << "接收者id:" << receiverId;
    // 获取socket
    qintptr socketDescriptor = UserList::getInstance()->getSocketDescriptorByUserId(receiverId);
    MsgSocket *receiverSocket = m_clients.value(socketDescriptor);
    // qDebug() << "获取Socket成功" << receiverSocket->socketDescriptor();
    if (receiverSocket != nullptr)
    {
        // 发送
        receiverSocket->sendMsg(message);
    }
    delete message;

    // 测试开始
    // {
    //     // 开一个定时器
    //     QTimer *timer = new QTimer(this);
    //     TextMessage *testMsg = new TextMessage(2, 1, QString("测试文本"));
    //     // 每一秒发送一次消息
    //     connect(timer, &QTimer::timeout, [=]()
    //             { receiverSocket->sendMsg(testMsg); });
    //     timer->start(1000);
    //     // delete testMsg;
    // }
    // 测试结束
}

void ChatTcpServer::incomingConnection(qintptr socketDescriptor)
{
    MsgSocket *client = new MsgSocket(this);
    client->setSocketDescriptor(socketDescriptor);
    connect(client, &MsgSocket::messageReady, this, &ChatTcpServer::messageReadySlot);
    connect(client, &MsgSocket::disconnected, this, &ChatTcpServer::onClientDisconnected);
    m_clients.insert(socketDescriptor, client);
}

void ChatTcpServer::onClientDisconnected()
{
    MsgSocket *client = qobject_cast<MsgSocket *>(sender());
    if (client != nullptr)
    {
        qintptr socketDescriptor = client->socketHandle();
        m_clients.remove(socketDescriptor); // 移除客户端
        client->deleteLater();
        // 用户离线信号
        emit userOffline(m_userlist->getUserIdBySocketDescriptor(socketDescriptor));
    }
}

QMap<qintptr, MsgSocket *> ChatTcpServer::clients() const
{
    return m_clients;
}

void ChatTcpServer::initMessageQueue()
{
    m_messageQueue = MessageQueue::getInstance();
    connect(m_messageQueue, &MessageQueue::messageEnqueue, this, &ChatTcpServer::sendMessageSlot);
}
