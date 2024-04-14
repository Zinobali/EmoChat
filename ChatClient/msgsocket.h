#ifndef MSGSOCKET_H
#define MSGSOCKET_H

#include <QTcpSocket>
#include "message.h"
// #include "jsonmanager.h"

class MsgSocket : public QTcpSocket
{
    Q_OBJECT
public:
    explicit MsgSocket(QObject *parent = nullptr);
    //收发消息
    void sendMsg(Message *message);
    Message* readMsg();

    //getters and setters
    qintptr socketHandle() const;

    //重写setSocketDescriptor
    virtual bool setSocketDescriptor(qintptr socketDescriptor, SocketState state = ConnectedState, OpenMode openMode = ReadWrite) override;

public slots:
    void readyReadSlot();
    void disconnectedSlot();

private:
    void incomingData();

signals:
    void myReadyRead(MsgSocket* msgSocket);//重写的readyRead信号
    void myDisconnected(MsgSocket* msgSocket);//重写的disconnected信号
    void messageReady(qint64 type, QByteArray data);//把接收的数据发送出去

private:
    qintptr m_socketHandle;

};

Q_DECLARE_METATYPE(MsgSocket*)

#endif // MSGSOCKET_H
