#ifndef SOCKETMANAGER_H
#define SOCKETMANAGER_H

#include <QObject>
#include <QHostAddress>

class SocketManager : public QObject
{
    Q_OBJECT
public:
    explicit SocketManager(QObject *parent = nullptr);

    qintptr socketDescriptor() const;
    void setSocketDescriptor(qintptr newSocketDescriptor);

    QHostAddress address() const;
    void setAddress(const QHostAddress &newAddress);

    quint16 port() const;
    void setPort(quint16 newPort);

signals:

private:
    qintptr m_socketDescriptor; // 标识符
    QHostAddress m_address;
    quint16 m_port;
};

#endif // SOCKETMANAGER_H
