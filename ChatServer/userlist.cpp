#include "userlist.h"

// 类外初始化静态变量
UserList *UserList::m_instance = nullptr;

UserList::UserList(QObject *parent)
{
    queryUserFromDB();
}

UserList::~UserList()
{
    qDeleteAll(m_users);
    m_users.clear();
}

UserList *UserList::getInstance()
{
    if (m_instance == nullptr)
    {
        m_instance = new UserList();
    }
    return m_instance;
}

void UserList::setOnlineUser(int id, qintptr socketDescriptor)
{
    m_onlineUsers.insert(id, socketDescriptor);
}

void UserList::setOfflineUser(int id)
{
    m_onlineUsers.remove(id);
}

void UserList::queryUserFromDB()
{
    // qDebug() << "queryUserFromDB";
    QSqlQuery query;
    if (query.exec("SELECT id, username, password, avatar FROM user"))
    {
        while (query.next())
        {
            int id = query.value(0).toInt();
            QString username = query.value(1).toString();
            QString password = query.value(2).toString();
            QByteArray avatar = query.value(3).toByteArray();
            // 封装user
            User *user = new User(id, username, password, avatar);
            m_users.insert(user);
        }
    }
    else
    {
        qDebug() << "User查询失败:" << query.lastError().text();
    }
}

User *UserList::getUserById(int id)
{
    qDebug() << "开始查找用户";
    for (User *user : m_users)
    {
        if (user->id() == id)
        {
            qDebug() << "找到用户:" << user->username();
            return user;
        }
    }
    qDebug() << "未找到用户";
    return nullptr;
}

qintptr UserList::getSocketDescriptorByUserId(int id)
{
    return m_onlineUsers.value(id);
}

int UserList::getUserIdBySocketDescriptor(qintptr socketDescriptor)
{
    for (QMap<int,qintptr>::const_iterator it = m_onlineUsers.constBegin(); it != m_onlineUsers.constEnd(); it++)
    {
        if (it.value() == socketDescriptor)
        {
            return it.key();
        }
    }
    // 如果找不到对应的键，返回一个特殊值
    return -1;
}

QSet<User *> *UserList::getUsers()
{
    return &m_users;
}
