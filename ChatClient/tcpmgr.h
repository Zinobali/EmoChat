#ifndef TCPMGR_H
#define TCPMGR_H

#include "global.h"
#include "userdata.h"
#include <QObject>
#include <QTcpSocket>
#include <functional>
#include <singleton.h>

struct ServerInfo
{
    int Uid;
    QString Host;
    QString Port;
    QString Token;
};

typedef std::function<void(RequestId id, QByteArray data)> TcpHandler;
class TcpMgr : public QObject, public Singleton<TcpMgr>, public std::enable_shared_from_this<TcpMgr>
{
    Q_OBJECT
    friend class Singleton<TcpMgr>;

public:
private:
    explicit TcpMgr(QObject *parent = nullptr);
    void initSignals();
    void initHandlers();
    void handleRead();
    void handleMsg(RequestId id, QByteArray data);
    bool parseJson(const QByteArray &data, QJsonObject &obj);    // 解析json
    bool checkErrorCode(const QJsonObject &obj, ErrorCodes &ec); // 检查错误码
    void handleChatLoginRsp(RequestId id, QByteArray data);
    void handleSearchUserRsp(RequestId id, QByteArray data);
    void handleAddFriendRsp(RequestId id, QByteArray data);
    void handleAddFriendReq(RequestId id, QByteArray data);

private:
    QTcpSocket socket_;
    QString host_;
    quint16 port_;
    QByteArray buffer_;
    bool b_recv_pending_;
    quint16 msg_id_;
    quint16 msg_len_;
    QMap<RequestId, TcpHandler> handlers_;

public slots:
    void slot_tcp_connect(ServerInfo);
    void slot_send_data(RequestId reqId, QByteArray dataBytes);

signals:
    void sig_con_success(bool ok);
    void sig_send_data(RequestId reqId, QByteArray dataBytes);
    void sig_login_failed(ErrorCodes err);
    void sig_switch_chatdlg();
    void sig_user_search(std::shared_ptr<SearchInfo>);
    void sig_auth_rsp(std::shared_ptr<AuthRsp>);
    void sig_friend_apply(std::shared_ptr<AddFriendApply>);
};

#endif // TCPMGR_H
