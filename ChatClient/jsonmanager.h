#ifndef JSONMANAGER_H
#define JSONMANAGER_H

#include <QObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QDebug>
#include "message.h"
#include "user.h"

class JsonManager : public QObject
{
    Q_OBJECT
public:
    explicit JsonManager(QObject *parent = nullptr);

    // Message* jsonToMsg(QJsonObject *json);
    // QJsonObject* jsonFromMsg(Message *msg);

    User jsonToUser(const QJsonObject &json);
    QJsonObject jsonFromUser(const User &user);

    QVector<User> jsonToUserList(const QJsonArray &json);
    QJsonArray jsonFromUserList(const QVector<User> &userList);
    

signals:
};

#endif // JSONMANAGER_H
