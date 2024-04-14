#include "user.h"

User::User(const User &other)
{
    m_id = other.m_id;
    m_password = other.m_password;
    m_username = other.m_username;
    m_avatar = other.m_avatar;
}

User::User(QObject *parent)
    : QObject{parent}
{
    m_id = -1;
    m_password = "";
    m_username = "";
    m_avatar = QByteArray();
}

User::User(int id, QString username, QString password, QByteArray avatar, QObject *parent)
    : QObject{parent},
      m_id(id),
      m_username(username),
      m_password(password),
      m_avatar(avatar)
{
}

int User::id() const
{
    return m_id;
}

void User::setId(int newId)
{
    m_id = newId;
}

QString User::username() const
{
    return m_username;
}

void User::setUsername(const QString &newUsername)
{
    m_username = newUsername;
}

QByteArray User::avatar() const
{
    return m_avatar;
}

void User::setAvatar(const QByteArray &newAvatar)
{
    m_avatar = newAvatar;
}

bool User::checkPassword(const QString &password)
{
    if (password == this->m_password)
    {
        return true;
    }
    return false;
}

User &User::operator=(const User &other)
{
    if (this != &other)
    {
        m_id = other.m_id;
        m_username = other.m_username;
        m_password = other.m_password;
        m_avatar = other.m_avatar;
    }
    return *this;
}

bool User::operator==(const User &other) const
{
    return m_id == other.m_id && m_username == other.m_username && m_password == other.m_password && m_avatar == other.m_avatar;
}
