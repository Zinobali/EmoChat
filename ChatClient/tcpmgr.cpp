#include "tcpmgr.h"
#include <QDataStream>
#include <QDebug>

TcpMgr::TcpMgr(QObject *parent)
    : QObject{parent}, host_(""), port_(0),
    b_recv_pending_(false), msg_id_(0), msg_len_(0)
{
    InitSignals();
}

void TcpMgr::InitSignals()
{
    connect(&socket_, &QTcpSocket::connected, this, [&](){
        emit sig_con_success(true);
    });

    connect(&socket_, &QTcpSocket::readyRead, this, [&](){
        buffer_.append(socket_.readAll());

        QDataStream stream(&buffer_, QIODevice::ReadOnly);
        stream.setVersion(QDataStream::Qt_5_15);
        stream.setByteOrder(QDataStream::BigEndian);

        while (true) {
            // 解析头部
            if(!b_recv_pending_){
                int head_len = sizeof(msg_id_) + sizeof(msg_len_);
                if(buffer_.size() < head_len){
                    return;
                }

                stream >> msg_id_ >> msg_len_;

                buffer_.remove(0, head_len);
                qDebug() << "Receive Message ID:" << msg_id_ << ", Message Length:" << msg_len_;
            }

            //消息体
            if(buffer_.size() < msg_len_){
                b_recv_pending_ = true; // 数据不够，等待更多数据到达
                return;
            }

            b_recv_pending_ = false;
            QByteArray msg_body = buffer_.mid(0, msg_len_);
            buffer_.remove(0, msg_len_);
            qDebug() << "receive body msg is " << msg_body;
        }
    });

    connect(&socket_, &QTcpSocket::errorOccurred, this, [&](auto error){
        Q_UNUSED(error);
        qDebug() << "Error:" << socket_.errorString();
    });

    connect(&socket_, &QTcpSocket::disconnected, this, [&](){
        qDebug() << "Disconnected from server.";
    });

    /*
     * 这里为什么采取信号槽，而不是public Send(RequestId reqId, QString data);
     * 因为信号槽的默认连接方式是Qt::AutoConnection，它会根据信号和槽所在的线程自动选择以下两种模式：
     * 1.直接连接（Direct Connection）
     *      如果信号和槽在同一个线程中执行，则信号的发射和槽的执行是同步的，没有队列。
     *      信号发出时，槽会立即执行。
     * 2.队列连接（Queued Connection）
     *      如果信号和槽不在同一个线程，则信号会进入目标对象所属线程的事件队列，并在事件循环中异步调用槽。
     *      这种模式下，信号和槽之间会有队列。
     */
    connect(this, &TcpMgr::sig_send_data, this, &TcpMgr::slot_send_data);
}

void TcpMgr::slot_tcp_connect(ServerInfo s)
{
    // 尝试连接到服务器
    qDebug() << "Connecting to server...";
    host_ = s.Host;
    port_ = s.Port.toUShort();
    socket_.connectToHost(host_, port_);
}

void TcpMgr::slot_send_data(RequestId reqId, QString msg)
{
    auto data = msg.toUtf8();
    auto id = static_cast<quint16>(reqId);
    auto len = static_cast<quint16>(data.size());
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setByteOrder(QDataStream::BigEndian);
    out << id << len;
    block.append(data); // 将数据体追加到缓冲区,而不是使用数据流，避免数据体被转换字节序
    socket_.write(block);
    qDebug() << "send id :" << id << ", send len:" << len << ", send data: " << msg;
}
