#ifndef MESSAGEQUEUE_H
#define MESSAGEQUEUE_H

#include <QObject>
#include <QQueue>
#include "message.h"
#include <QMutex>
#include <QMutexLocker>

class MessageQueue : public QObject
{
    Q_OBJECT
public:
    static MessageQueue* getInstance();
    void enqueue(Message* msg);
    Message* dequeue();
    bool isEmpty();

private:
    explicit MessageQueue(QObject *parent = nullptr);

signals:
    void messageEnqueue();    

private:
    static MessageQueue* m_instance;
    QQueue<Message*> m_msgQueue;
    QMutex m_mutex;
};

#endif // MESSAGEQUEUE_H
