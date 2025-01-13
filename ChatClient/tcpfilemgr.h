#ifndef TCPFILEMGR_H
#define TCPFILEMGR_H

#include <QObject>
#include <singleton.h>
#include <QTcpSocket>
#include <QJsonObject>
#include <QByteArray>

constexpr int TCP_HEAD_LEN = 6;
constexpr int TCP_ID_LEN = 2;
constexpr int TCP_MSG_LEN = 4;
constexpr int MAX_BLOCK_LEN = 1024 * 2; // 2k == 2048

class TcpFileMgr : public QObject, public Singleton<TcpFileMgr>
{
    friend class Singleton<TcpFileMgr>;
    Q_OBJECT

public:
    ~TcpFileMgr();

    void ConnectToServer(const QString &host, quint16 port); // 连接到服务器
    void DisConnectFromServer();                             // 断开连接
    void SendMsg(quint16 id, const QByteArray &data);
    bool IsConnected();

private:
    explicit TcpFileMgr(QObject *parent = nullptr); // 私有化构造函数
    void processData();

private:
    QTcpSocket socket_;
    QByteArray buffer_;

public slots:
    void slot_connected();
    void slot_disconnected();
    void slot_ready_read();
    void slot_error_occured(QAbstractSocket::SocketError error);
    void slot_send_data(quint16 id, const QByteArray &data);

signals:
    void sig_send_data(quint16 id, const QByteArray &data);
    void sig_connected(bool ok);
    void sig_net_error(QString error);
    void sig_logic_process(quint16 id, const QJsonObject &data);
};

#endif // TCPFILEMGR_H
