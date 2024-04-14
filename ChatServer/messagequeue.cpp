#include "messagequeue.h"

MessageQueue* MessageQueue::m_instance = nullptr;
MessageQueue *MessageQueue::getInstance()
{
    if(m_instance == nullptr)
    {
        m_instance = new MessageQueue();
    }
    return m_instance;
}

MessageQueue::MessageQueue(QObject *parent)
    : QObject{parent}
{
}

void MessageQueue::enqueue(Message* msg)
{
    QMutexLocker locker(&m_mutex);
    m_msgQueue.enqueue(msg);
    emit messageEnqueue();
}

Message* MessageQueue::dequeue()
{
    QMutexLocker locker(&m_mutex);
    return m_msgQueue.dequeue();
}

bool MessageQueue::isEmpty()
{
    QMutexLocker locker(&m_mutex);
    return m_msgQueue.isEmpty();
}
