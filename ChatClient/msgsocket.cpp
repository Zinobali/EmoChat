#include "msgsocket.h"

MsgSocket::MsgSocket(QObject *parent)
    : QTcpSocket{parent}
{
    connect(this, &MsgSocket::readyRead, this, &MsgSocket::incomingData);
    connect(this, &MsgSocket::disconnected, this, &MsgSocket::disconnectedSlot);
}

void MsgSocket::sendMsg(Message *message)
{
    qDebug() << "开始发送message";
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_6);
    out << (qint64)(0) << (qint64)(0); // 占位符,第一个写大小，第二个写消息类型
    out << message->serialize();       // 序列化
    // 写入大小
    out.device()->seek(0);
    out << qint64(block.size() - sizeof(qint64) * 2);
    qDebug() << "发送blocksize()" << (block.size() - sizeof(qint64) * 2);
    // 写入消息类型
    out.device()->seek(sizeof(qint64));
    out << static_cast<qint64>(message->type());
    // 发送
    write(block);
    qDebug() << "message发送完毕";
}

Message *MsgSocket::readMsg()
{
    qint64 blockSize = 0;
    qDebug() << "开始读取message";
    QDataStream in(this);
    in.setVersion(QDataStream::Qt_6_6);
    // 验证完整性
    if (bytesAvailable() < sizeof(qint64))
    {
        qDebug() << "读取blockSize失败";
        return nullptr;
    }
    in >> blockSize;
    qDebug() << "读取blockSize成功:" << blockSize;
    if (bytesAvailable() < blockSize)
    {
        qDebug() << "数据大小不匹配";
        return nullptr;
    }
    qint64 type;
    in >> type;
    QByteArray data;
    in >> data;
    MessageFactory factory;
    Message *msg = factory.createMessage(static_cast<Message::TYPE>(type), data);
    qDebug() << "message读取完毕";
    return msg;
}

void MsgSocket::readyReadSlot()
{
    emit myReadyRead(this);
}

void MsgSocket::disconnectedSlot()
{
    emit myDisconnected(this);
}

void MsgSocket::incomingData()
{
    qint64 blockSize = 0;
    qDebug() << "开始读取message";
    QDataStream in(this);
    in.setVersion(QDataStream::Qt_6_6);
    // 验证完整性
    if (bytesAvailable() < sizeof(qint64))
    {
        qDebug() << "读取blockSize失败";
        return;
    }
    in >> blockSize;
    qDebug() << "读取blockSize成功:" << blockSize;
    if (bytesAvailable() < blockSize)
    {
        qDebug() << "数据大小不匹配";
        return;
    }
    qint64 type;
    in >> type;
    QByteArray data;
    in >> data;
    emit messageReady(type, data);
    qDebug() << "message读取完毕";
}

qintptr MsgSocket::socketHandle() const
{
    return m_socketHandle;
}

bool MsgSocket::setSocketDescriptor(qintptr socketDescriptor, SocketState state, OpenMode openMode)
{
    this->m_socketHandle = socketDescriptor;
    return QTcpSocket::setSocketDescriptor(socketDescriptor, state, openMode);
}
