#include "message.h"
Message::Message(QObject *parent) : QObject{parent}
{
}

int Message::senderId() const
{
    return m_senderId;
}

int Message::receiverId() const
{
    return m_receiverId;
}

TextMessage::TextMessage(QObject *parent)
    : Message(parent)
{
}

TextMessage::TextMessage(const int &senderId, const int &receiverId, const QString &text, QObject *parent)
    : Message(parent), m_text(text)
{
    m_senderId = senderId;
    m_receiverId = receiverId;
}

Message::TYPE TextMessage::type() const
{
    return Message::TEXT_MSG;
}

QByteArray TextMessage::serialize() const
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << m_senderId << m_receiverId << m_text;
    return data;
}

void TextMessage::deserialize(const QByteArray &data)
{
    QDataStream stream(data);
    stream >> m_senderId >> m_receiverId >> m_text;
}

ImageMessage::ImageMessage(QObject *parent)
    : Message(parent)
{
}

ImageMessage::ImageMessage(const int &senderId, const int &receiverId, const QByteArray &imageData, QObject *parent)
    : Message(parent), m_imageData(imageData)
{
    m_senderId = senderId;
    m_receiverId = receiverId;
}

Message::TYPE ImageMessage::type() const
{
    return Message::IMAGE_MSG;
}

QByteArray ImageMessage::serialize() const
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << m_senderId << m_receiverId << m_imageData;
    return data;
}

void ImageMessage::deserialize(const QByteArray &data)
{
    QDataStream stream(data);
    stream >> m_senderId >> m_receiverId >> m_imageData;
}

FileMessage::FileMessage(QObject *parent)
    : Message(parent)
{
}

FileMessage::FileMessage(const int &senderId, const int &receiverId, const QString &fileName, const QByteArray &fileData, QObject *parent)
    : Message(parent), m_fileName(fileName), m_fileData(fileData)
{
    m_senderId = senderId;
    m_receiverId = receiverId;
}

Message::TYPE FileMessage::type() const
{
    return Message::FILE_MSG;
}

QByteArray FileMessage::serialize() const
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << m_senderId << m_receiverId << m_fileName << m_fileData;
    return data;
}

void FileMessage::deserialize(const QByteArray &data)
{
    QDataStream stream(data);
    stream >> m_senderId >> m_receiverId >> m_fileName >> m_fileData;
}

//     case Message::FILE_MSG:
//     {
//         QStringList fileInfo = data.toStringList();
//         if (fileInfo.size() != 2) return nullptr;
//         QString fileName = fileInfo[0];
//         QByteArray fileData = QFile::encodeName(fileInfo[1]);
//         return new FileMessage(senderId, receiverId, fileName, fileData);
//     }

Message *MessageFactory::createMessage(const Message::TYPE &type, const QByteArray &data)
{
    switch (type)
    {
    case Message::TEXT_MSG:
    {
        TextMessage *msg = new TextMessage();
        msg->deserialize(data);
        return msg;
    }
    case Message::IMAGE_MSG:
    {
        ImageMessage *msg = new ImageMessage();
        msg->deserialize(data);
        return msg;
    }
    case Message::FILE_MSG:
    {
        FileMessage *msg = new FileMessage();
        msg->deserialize(data);
        return msg;
    }
    case Message::SIGN_IN:
    {
        SignInMessage *msg = new SignInMessage();
        msg->deserialize(data);
        return msg;
    }
    case Message::SIGN_IN_REPLY:
    {
        SignInReplyMessage *msg = new SignInReplyMessage();
        msg->deserialize(data);
        return msg;
    }
    case Message::ONLINE:
    {
        onlineMessage *msg = new onlineMessage();
        msg->deserialize(data);
        return msg;
    }
    case Message::ONLINE_REPLY:
    {
        onlineReplyMessage *msg = new onlineReplyMessage();
        msg->deserialize(data);
        return msg;
    }

    default:
        return nullptr;
    }
}

SignInMessage::SignInMessage(QObject *parent)
    : Message(parent)
{
}

SignInMessage::SignInMessage(const int &senderId, const int &receiverId, const QString &password, QObject *parent)
    : Message(parent), m_password(password)
{
    m_senderId = senderId;
    m_receiverId = receiverId;
}

Message::TYPE SignInMessage::type() const
{
    return Message::SIGN_IN;
}

QByteArray SignInMessage::serialize() const
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << m_senderId << m_receiverId << m_password;
    return data;
}

void SignInMessage::deserialize(const QByteArray &data)
{
    QDataStream stream(data);
    stream >> m_senderId >> m_receiverId >> m_password;
}

QString SignInMessage::password() const
{
    return m_password;
}

SignInReplyMessage::SignInReplyMessage(QObject *parent)
    : Message(parent)
{
}

SignInReplyMessage::SignInReplyMessage(const int &senderId, const int &receiverId, const bool &isValid, const User *user, QObject *parent)
    : Message(parent), m_isValid(isValid)
{
    m_senderId = senderId;
    m_receiverId = receiverId;
    if (user != nullptr)
    {
        m_user = *user;
    }
}

Message::TYPE SignInReplyMessage::type() const
{
    return Message::SIGN_IN_REPLY;
}

QByteArray SignInReplyMessage::serialize() const
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    JsonManager jsonManager;
    QJsonObject json = jsonManager.jsonFromUser(m_user);
    stream << m_senderId << m_receiverId << m_isValid << json;
    return data;
}

void SignInReplyMessage::deserialize(const QByteArray &data)
{
    QDataStream stream(data);
    JsonManager jsonManager;
    QJsonObject json;
    stream >> m_senderId >> m_receiverId >> m_isValid >> json;
    m_user = jsonManager.jsonToUser(json);
}

bool SignInReplyMessage::isValid() const
{
    return m_isValid;
}

User SignInReplyMessage::user() const
{
    return m_user;
}

onlineMessage::onlineMessage(QObject *parent)
    : Message(parent)
{
}

onlineMessage::onlineMessage(const int &senderId, const int &receiverId, QObject *parent)
    : Message(parent)
{
    m_senderId = senderId;
    m_receiverId = receiverId;
}

Message::TYPE onlineMessage::type() const
{
    return Message::ONLINE;
}

QByteArray onlineMessage::serialize() const
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << m_senderId << m_receiverId;
    return data;
}

void onlineMessage::deserialize(const QByteArray &data)
{
    QDataStream stream(data);
    stream >> m_senderId >> m_receiverId;
}

onlineReplyMessage::onlineReplyMessage(QObject *parent)
    : Message(parent)
{
}

onlineReplyMessage::onlineReplyMessage(const int &senderId, const int &receiverId, QObject *parent)
    : Message(parent)
{
    m_senderId = senderId;
    m_receiverId = receiverId;
    QSet<User *> *users = UserList::getInstance()->getUsers();
    for (User *user : *users)
    {
        if (user->id() == m_receiverId)
        {
            continue; // skip the sender
        }
        m_friends.append(*user);
    }
}

Message::TYPE onlineReplyMessage::type() const
{
    return Message::ONLINE_REPLY;
}

QByteArray onlineReplyMessage::serialize() const
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    JsonManager jsonManager;
    QJsonArray json = jsonManager.jsonFromUserList(m_friends);
    stream << m_senderId << m_receiverId << json;
    return data;
}

void onlineReplyMessage::deserialize(const QByteArray &data)
{
    QDataStream stream(data);
    JsonManager jsonManager;
    QJsonArray json;
    stream >> m_senderId >> m_receiverId >> json;
    m_friends = jsonManager.jsonToUserList(json);
}

QVector<User> onlineReplyMessage::friends() const
{
    return m_friends;
}
