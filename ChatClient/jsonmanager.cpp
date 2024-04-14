#include "jsonmanager.h"

JsonManager::JsonManager(QObject *parent)
    : QObject{parent}
{
}

// Message *JsonManager::jsonToMsg(QJsonObject *json)
// {
//     Message *msg = new Message();
//     msg->setSenderId(json->value("senderId").toInt());
//     msg->setRecipientId(json->value("recipientId").toInt());
//     msg->setTimestamp(QDateTime::fromString(json->value("timestamp").toString()));
//     msg->setContent(json->value("content").toString().toUtf8());
//     msg->setMsgType(static_cast<Message::TYPE>(json->value("msgType").toInt()));
//     msg->setIsSystemMsg(json->value("isSystemMsg").toBool());
//     return msg;
// }

// QJsonObject *JsonManager::jsonFromMsg(Message *msg)
// {
//     QJsonObject *json = new QJsonObject();
//     json->insert("senderId", msg->senderId());
//     json->insert("recipientId", msg->recipientId());
//     json->insert("timestamp", msg->timestamp().toString());
//     json->insert("content", QString::fromUtf8(msg->content()));
//     json->insert("msgType", static_cast<int>(msg->getMsgType()));
//     json->insert("isSystemMsg", msg->isSystemMsg());
//     return json;
// }

User JsonManager::jsonToUser(const QJsonObject &json)
{
    User user;
    user.setId(json.value("id").toInt());
    user.setUsername(json.value("username").toString());
    user.setAvatar(json.value("avatar").toString().toUtf8());
    return user;
}

QJsonObject JsonManager::jsonFromUser(const User &user)
{
    QJsonObject json;
    json.insert("id", user.id());
    json.insert("username", user.username());
    json.insert("avatar", QString::fromUtf8(user.avatar()));
    return json;
}

QVector<User> JsonManager::jsonToUserList(const QJsonArray &json)
{
    QVector<User> users;
    for (const QJsonValueConstRef &user : json)
    {
        if (user.isObject())
        {
            QJsonObject obj = user.toObject();
            users.append(jsonToUser(obj));
        }
    }
    return users;
}

QJsonArray JsonManager::jsonFromUserList(const QVector<User> &userList)
{
    QJsonArray json;
    for (const User &user : userList)
    {
        json.append(jsonFromUser(user));
    }
    return json;
}
