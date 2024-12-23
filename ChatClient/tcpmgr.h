#ifndef TCPMGR_H
#define TCPMGR_H

#include <QObject>
#include <singleton.h>
#include <QTcpSocket>
#include "global.h"

struct ServerInfo{
    int Uid;
    QString Host;
    QString Port;
    QString Token;
};

class TcpMgr : public QObject, public Singleton<TcpMgr>,
               public std::enable_shared_from_this<TcpMgr>
{
    Q_OBJECT
    friend class Singleton<TcpMgr>;
public:

private:
    explicit TcpMgr(QObject *parent = nullptr);
    QTcpSocket socket_;
    QString host_;
    quint16 port_;
    QByteArray buffer_;
    bool b_recv_pending_;
    quint16 msg_id_;
    quint16 msg_len_;

    void InitSignals();

public slots:
    void slot_tcp_connect(ServerInfo);
    void slot_send_data(RequestId reqId, QString msg);

signals:
    void sig_con_success(bool ok);
    void sig_send_data(RequestId reqId, QString msg);
    void sig_login_failed(int err);
};

#endif // TCPMGR_H
