#ifndef USERLIST_H
#define USERLIST_H

#include <QObject>
#include "user.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QMutex>
#include <QMutexLocker>
#include "socketmanager.h"


class UserList : public QObject
{
    Q_OBJECT
public:

    static UserList* getInstance();
    void setOnlineUser(int id,qintptr socketDescriptor);
    void setOfflineUser(int id);

    //getters and setters
    QSet<User*>* getUsers();
    void queryUserFromDB();//从数据库中获取用户
    User* getUserById(int id);//通过id查找用户
    qintptr getSocketDescriptorByUserId(int id);
    int getUserIdBySocketDescriptor(qintptr socketDescriptor);

private:
    UserList(QObject *parent = nullptr);//私有化构造函数
    ~UserList();

    static UserList* m_instance;
    QMutex m_mutex;
    QSet<User*> m_users;
    QMap<int,qintptr> m_onlineUsers;//用户在线情况(id,socketDescriptor)
};

#endif // USERLIST_H
