#include "socketmanager.h"

SocketManager::SocketManager(QObject *parent)
    : QObject{parent}
{}

qintptr SocketManager::socketDescriptor() const
{
    return m_socketDescriptor;
}

void SocketManager::setSocketDescriptor(qintptr newSocketDescriptor)
{
    m_socketDescriptor = newSocketDescriptor;
}

QHostAddress SocketManager::address() const
{
    return m_address;
}

void SocketManager::setAddress(const QHostAddress &newAddress)
{
    m_address = newAddress;
}

quint16 SocketManager::port() const
{
    return m_port;
}

void SocketManager::setPort(quint16 newPort)
{
    m_port = newPort;
}
