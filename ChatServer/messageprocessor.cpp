#include "messageprocessor.h"

MessageProcessor::MessageProcessor(qint64 type, const QByteArray &data, const MsgSocket * const curSocket)
    : m_type(static_cast<Message::TYPE>(type)), m_data(data), m_curSocket(curSocket)
{
    setAutoDelete(true);
}

MessageProcessor::~MessageProcessor()
{
    qDebug() << "读任务结束";
    qDebug() << "*****************************";
}

void MessageProcessor::run()
{
    qDebug() << "*****************************";
    qDebug() << "进入消息处理线程";
    qDebug() << "消息类型" << m_type;
    MessageFactory factory;
    Message* msg = factory.createMessage(m_type, m_data);
    if(msg == nullptr)
    {
        qDebug() << "消息解析失败";
        return;
    }
    switch (m_type)
    {
        case Message::SIGN_IN:
            msg = signinHandler(msg);
            break;
        case Message::SIGN_UP:
            msg = signupHandler(msg);
            break;
        case Message::ONLINE:
            msg = onlineHandler(msg);
            break;
        default:
            break;
    }
    if(msg != nullptr)
    {
        MessageQueue::getInstance()->enqueue(msg);
        qDebug() << "消息入队";
    }
}

Message* MessageProcessor::signinHandler(Message *msg)
{
    SignInMessage* signinMsg = qobject_cast<SignInMessage*>(msg);
    //先填充
    int uid = signinMsg->senderId();
    qDebug() << "用户id:" << uid;

    UserList::getInstance()->setOnlineUser(uid,m_curSocket->socketDescriptor());
    qDebug() << "setOnlineUser";
    //判断
    User* user = UserList::getInstance()->getUserById(uid);
    qDebug() << "getUserById返回signinHandler";

    bool signInResult = false;
    if(user == nullptr)
    {
        qDebug() << "user == nullptr";
        return nullptr;
    }
    if(user->checkPassword(signinMsg->password()))
    {
        qDebug() << "密码验证成功";
        signInResult = true;
    }
    if(msg != nullptr)
    {
        delete msg;
        msg = nullptr;
    }
    qDebug() << "登录回执消息生成";
    return new SignInReplyMessage(Message::SYSTEM_ID,uid,signInResult,user);
}

Message* MessageProcessor::signupHandler(Message *msg)
{
}

Message* MessageProcessor::onlineHandler(Message *msg)
{
    onlineMessage* onlineMsg = qobject_cast<onlineMessage*>(msg);

    int uid = onlineMsg->senderId();

    //判断
    User* user = UserList::getInstance()->getUserById(uid);
    if(user == nullptr)
    {
        qDebug() << "用户不存在";
        return nullptr;
    }
    //更新ui窗口状态
    emit userOnline(uid);
    if(msg != nullptr)
    {
        delete msg;
        msg = nullptr;
    }
    //回复
    return new onlineReplyMessage(Message::SYSTEM_ID,uid);
}
