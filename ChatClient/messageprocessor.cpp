#include "messageprocessor.h"

MessageProcessor::MessageProcessor(const MsgSocket *const curSocket)
    : m_curSocket(curSocket)
{
}

MessageProcessor::~MessageProcessor()
{
}

void MessageProcessor::processMessage(qint64 type, QByteArray data)
{
    Message::TYPE msgType = static_cast<Message::TYPE>(type);
    MessageFactory factory;
    Message *msg = factory.createMessage(msgType, data);
    if (msg == nullptr)
    {
        qDebug() << "消息解析失败";
        return;
    }
    switch (msgType)
    {
    case Message::TEXT_MSG:
        textMessageHandler(msg);
        break;
    case Message::ONLINE_REPLY:
        onlineReplyHandler(msg);
        break;
    default:
        break;
    }
    delete msg;
    msg = nullptr;
}

void MessageProcessor::onlineReplyHandler(Message * message)
{
    onlineReplyMessage* onlineMsg = qobject_cast<onlineReplyMessage*>(message);
    QVector<User> friends = onlineMsg->friends();
    emit friendListUpdated(friends);
}

void MessageProcessor::textMessageHandler(Message *message)
{
    qDebug() << "收到文本消息";
    TextMessage* textMsg = qobject_cast<TextMessage*>(message);
    int senderId = textMsg->senderId();
    qDebug() << "senderId" << senderId;
    QString text = textMsg->text();
    qDebug() << "消息内容：" << text;
    emit textMessageReceived(senderId, text);
}
