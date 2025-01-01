#ifndef TCPMGR_H
#define TCPMGR_H

#include <QObject>
#include <singleton.h>
#include <QTcpSocket>
#include "global.h"
#include <functional>

struct ServerInfo
{
    int Uid;
    QString Host;
    QString Port;
    QString Token;
};

class SearchInfo
{
public:
    SearchInfo(int uid, QString name, QString nick, QString desc, int sex)
        : _uid(uid), _name(name), _nick(nick), _desc(desc), _sex(sex)
    {
    }
    int _uid;
    QString _name;
    QString _nick;
    QString _desc;
    int _sex;
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
    void initHttpHandlers();
    void handleRead();
    void handleChatLoginRsp(RequestId id, QByteArray data);
    void handleMsg(RequestId id, QByteArray data);

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
    void slot_send_data(RequestId reqId, QString msg);

signals:
    void sig_con_success(bool ok);
    void sig_send_data(RequestId reqId, QString msg);
    void sig_login_failed(ErrorCodes err);
    void sig_switch_chatdlg();
    void sig_user_search(std::shared_ptr<SearchInfo>);
};

#endif // TCPMGR_H
