#include "tcpmgr.h"
#include <QDataStream>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include "usermgr.h"

TcpMgr::TcpMgr(QObject *parent)
    : QObject{parent}, host_(""), port_(0),
      b_recv_pending_(false), msg_id_(0), msg_len_(0)
{
    initSignals();
    initHandlers();
}

void TcpMgr::initSignals()
{
    connect(&socket_, &QTcpSocket::connected, this, [&]()
            { emit sig_con_success(true); });

    connect(&socket_, &QTcpSocket::readyRead, this, [&]()
            { handleRead(); });

    connect(&socket_, &QTcpSocket::errorOccurred, this, [&](auto error)
            {
        Q_UNUSED(error);
        qDebug() << "Error:" << socket_.errorString(); });

    connect(&socket_, &QTcpSocket::disconnected, this, [&]()
            { qDebug() << "Disconnected from server."; });

    /*
     * 这里为什么采取信号槽，而不是void Send(RequestId reqId, QString data);
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

void TcpMgr::initHandlers()
{
    // 登录回包
    handlers_.insert(RequestId::ID_CHAT_LOGIN_RSP, [this](RequestId id, QByteArray data)
                     { handleChatLoginRsp(id, data); });
    // 用户搜索回包
    handlers_.insert(RequestId::ID_SEARCH_USER_RSP, [this](RequestId id, QByteArray data)
                     { handleSearchUserRsp(id, data); });
    // 添加好友回包
    handlers_.insert(RequestId::ID_ADD_FRIEND_RSP, [this](RequestId id, QByteArray data)
                     { handleAddFriendRsp(id, data); });
    // 好友申请通知
    handlers_.insert(RequestId::ID_NOTIFY_ADD_FRIEND_REQ, [this](RequestId id, QByteArray data)
                     { handleAddFriendReq(id, data); });
}

void TcpMgr::handleRead()
{
    buffer_.append(socket_.readAll());

    QDataStream stream(&buffer_, QIODevice::ReadOnly);
    stream.setVersion(QDataStream::Qt_5_15);
    stream.setByteOrder(QDataStream::BigEndian);

    while (true)
    {
        // 解析头部
        if (!b_recv_pending_)
        {
            int head_len = sizeof(msg_id_) + sizeof(msg_len_);
            if (buffer_.size() < head_len)
            {
                return;
            }

            stream >> msg_id_ >> msg_len_;

            buffer_.remove(0, head_len);
            qDebug() << "Receive Message ID:" << msg_id_ << ", Message Length:" << msg_len_;
        }

        // 消息体
        if (buffer_.size() < msg_len_)
        {
            b_recv_pending_ = true; // 数据不够，等待更多数据到达
            return;
        }

        b_recv_pending_ = false;
        QByteArray msg_body = buffer_.mid(0, msg_len_);
        buffer_.remove(0, msg_len_);
        qDebug() << "receive body msg is " << msg_body;

        handleMsg(static_cast<RequestId>(msg_id_), msg_body);
    }
}

void TcpMgr::handleChatLoginRsp(RequestId id, QByteArray data)
{
    qDebug() << "handle id is: " << toInt(id) << " data is " << data;
    // 读取json
    auto jsonDoc = QJsonDocument::fromJson(data);
    if (jsonDoc.isNull() || !jsonDoc.isObject())
    {
        qDebug() << "Failed to create QJsonDocument.";
        return;
    }

    auto jsonObj = jsonDoc.object();
    if (!jsonObj.contains("error"))
    {
        auto error = toInt(ErrorCodes::ERR_JSON);
        qDebug() << "Login Failed, err is Json Parse Err" << error;
        return;
    }

    auto error = static_cast<ErrorCodes>(jsonObj["error"].toInt());
    if (error != ErrorCodes::SUCCESS)
    {
        qDebug() << "Login Failed, err is " << jsonObj["error"].toInt();
        emit sig_login_failed(error);
        return;
    }

    UserMgr::GetInstance()->setUid(jsonObj["uid"].toInt());
    UserMgr::GetInstance()->setName(jsonObj["name"].toString());
    UserMgr::GetInstance()->setToken(jsonObj["token"].toString());
    emit sig_switch_chatdlg();
}

void TcpMgr::handleMsg(RequestId id, QByteArray data)
{
    auto iter = handlers_.find(id);
    if (iter == handlers_.end())
    {
        qDebug() << "not found id [" << toInt(id) << "] to handle";
        return;
    }

    iter.value()(id, data);
}

void TcpMgr::handleSearchUserRsp(RequestId id, QByteArray data)
{
    qDebug() << "handle id is " << id << " data is " << data;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
    if (jsonDoc.isNull() || !jsonDoc.isObject())
    {
        qDebug() << "Failed to create QJsonDocument.";
        return;
    }

    auto jsonObj = jsonDoc.object();
    if (!jsonObj.contains("error"))
    {
        int error = ErrorCodes::ERR_JSON;
        qDebug() << "Search User Failed, err is Json Parse Err" << error;
        emit sig_user_search(nullptr);
        return;
    }

    if (jsonObj["error"].toInt() != ErrorCodes::SUCCESS)
    {
        qDebug() << "Search User Failed, err is " << jsonObj["error"].toInt();
        emit sig_user_search(nullptr);
        return;
    }

    auto search_info = std::make_shared<SearchInfo>(
        jsonObj["uid"].toInt(),
        jsonObj["name"].toString(),
        jsonObj["nick"].toString(),
        jsonObj["desc"].toString(),
        jsonObj["sex"].toInt(),
        jsonObj["icon"].toString());
    // 发送信号
    emit sig_user_search(search_info);
}

void TcpMgr::handleAddFriendRsp(RequestId id, QByteArray data)
{
    QJsonDocument jsonDoc = QJsonDocument::fromJson(data);

    if (jsonDoc.isNull() || !jsonDoc.isObject())
    {
        qDebug() << "Failed to create QJsonDocument.";
        return;
    }

    auto jsonObj = jsonDoc.object();
    if (!jsonObj.contains("error"))
    {
        int error = ErrorCodes::ERR_JSON;
        qDebug() << "Add Friend Failed, err is Json Parse Err" << error;
        return;
    }

    if (jsonObj["error"].toInt() != ErrorCodes::SUCCESS)
    {
        qDebug() << "Add Friend Failed, err is " << jsonObj["error"].toInt();
        return;
    }

    qDebug() << "Add Friend Success";
}

void TcpMgr::handleAddFriendReq(RequestId id, QByteArray data)
{
    QJsonDocument jsonDoc = QJsonDocument::fromJson(data);

    if (jsonDoc.isNull() || !jsonDoc.isObject())
    {
        qDebug() << "Failed to create QJsonDocument.";
        return;
    }

    auto jsonObj = jsonDoc.object();
    if (!jsonObj.contains("error"))
    {
        int error = ErrorCodes::ERR_JSON;
        qDebug() << "Add Friend Failed, err is Json Parse Err" << error;
        return;
    }

    if (jsonObj["error"].toInt() != ErrorCodes::SUCCESS)
    {
        qDebug() << "Add Friend Failed, err is " << jsonObj["error"].toInt();
        return;
    }

    // 一切正常，发送信号
    int from_uid = jsonObj["applyuid"].toInt();
    QString name = jsonObj["name"].toString();
    QString desc = jsonObj["desc"].toString();
    QString icon = jsonObj["icon"].toString();
    QString nick = jsonObj["nick"].toString();
    int sex = jsonObj["sex"].toInt();

    auto apply_info = std::make_shared<AddFriendApply>(from_uid, name, desc, icon, nick, sex);
    emit sig_friend_apply(apply_info);

    qDebug() << "Add Friend Success";
}

void TcpMgr::slot_tcp_connect(ServerInfo s)
{
    // 尝试连接到服务器
    qDebug() << "Connecting to server...";
    host_ = s.Host;
    port_ = s.Port.toUShort();
    socket_.connectToHost(host_, port_);
}

void TcpMgr::slot_send_data(RequestId reqId, QByteArray dataBytes)
{
    auto id = static_cast<quint16>(reqId);
    auto len = static_cast<quint16>(dataBytes.length());

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setByteOrder(QDataStream::BigEndian); // 设置大端序
    out << id << len;                         // 写入消息头
    block.append(dataBytes);                  // 将数据体追加到缓冲区,而不是使用数据流，避免数据体被转换字节序
    socket_.write(block);
    qDebug() << "tcp mgr send byte data is " << block;
}
