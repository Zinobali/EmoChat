#include "tcpfilemgr.h"
#include <QJsonDocument>
#include <QDataStream>

TcpFileMgr::TcpFileMgr(QObject *parent)
    : QObject{parent}
{
    connect(&socket_, &QTcpSocket::connected, this, &TcpFileMgr::slot_connected);
    connect(&socket_, &QTcpSocket::readyRead, this, &TcpFileMgr::slot_ready_read);
    connect(&socket_, &QTcpSocket::disconnected, this, &TcpFileMgr::slot_disconnected);
    connect(&socket_, &QTcpSocket::errorOccurred, this, &TcpFileMgr::slot_error_occured);
    // 发送数据信号
    connect(this, &TcpFileMgr::sig_send_data, this, &TcpFileMgr::slot_send_data);
}

TcpFileMgr::~TcpFileMgr()
{
}

void TcpFileMgr::ConnectToServer(const QString &host, quint16 port)
{
    socket_.connectToHost(host, port);
}

void TcpFileMgr::DisConnectFromServer()
{
    if (!IsConnected())
    {
        return;
    }

    socket_.disconnectFromHost();

    if (socket_.waitForDisconnected(1000))
    {
    }
    else
    {
        emit sig_net_error(tr("disconnect from server failed: %1").arg(socket_.errorString()));
    }
}

void TcpFileMgr::SendMsg(quint16 id, const QByteArray &data)
{
    emit sig_send_data(id, data);
}

bool TcpFileMgr::IsConnected()
{
    return socket_.state() == QAbstractSocket::ConnectedState;
}

void TcpFileMgr::processData()
{
    while (buffer_.size() >= TCP_HEAD_LEN)
    {
        // 读取消息头
        auto head_bytes = buffer_.left(TCP_HEAD_LEN);
        QDataStream stream(head_bytes);
        // 设置大端序
        stream.setByteOrder(QDataStream::BigEndian);
        // 读取消息id
        quint16 msg_id;
        stream >> msg_id;
        // 读取消息长度
        quint32 msg_len;
        stream >> msg_len;

        if (buffer_.size() >= TCP_HEAD_LEN + msg_len)
        {
            // 可以读取完整消息体
            QByteArray body = buffer_.mid(TCP_HEAD_LEN, msg_len);
            // 裁切缓冲区
            buffer_ = buffer_.mid(TCP_HEAD_LEN + msg_len);
            // 解析消息
            QJsonDocument jsonDoc = QJsonDocument::fromJson(body);
            if (jsonDoc.isNull() || !jsonDoc.isObject())
            {
                qDebug() << "File client json parse error.";
                DisConnectFromServer();
                return;
            }

            QJsonObject obj = jsonDoc.object();
            emit sig_logic_process(msg_id, obj);
        }
        else
        {
            // 没有读取完整消息体
            break;
        }
    }
}

void TcpFileMgr::slot_connected()
{
    emit sig_connected(true);
}

void TcpFileMgr::slot_disconnected()
{
    emit sig_net_error(tr("File server disconnected"));
}

void TcpFileMgr::slot_ready_read()
{
    QByteArray data = socket_.readAll();
    buffer_.append(data);
    processData();
}

void TcpFileMgr::slot_error_occured(QAbstractSocket::SocketError error)
{
    emit sig_net_error(tr("File server error: %1").arg(socket_.errorString()));
    if (socket_.state() != QAbstractSocket::ConnectedState)
    {
        return;
    }
    DisConnectFromServer();
}

void TcpFileMgr::slot_send_data(quint16 id, const QByteArray &data)
{
    if (!IsConnected())
    {
        emit sig_net_error(tr("File server disconnected"));
        return;
    }

    // 获取消息体长度
    quint32 msg_len = data.size();
    QByteArray send_data;
    QDataStream stream(&send_data, QIODevice::WriteOnly);
    // 设置大端序
    stream.setByteOrder(QDataStream::BigEndian);
    // 写入消息id
    stream << id;
    // 写入消息体长度
    stream << msg_len;
    // 写入消息体
    send_data.append(data);
    // 发送(异步)
    socket_.write(send_data);
}
