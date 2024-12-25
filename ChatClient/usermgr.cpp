#include "usermgr.h"
#include <QDebug>

UserMgr::UserMgr()
{}

UserMgr::~UserMgr()
{
    qDebug() << "~UserMgr()";
    qDebug() << "user id: " << uid_ << ", user name: " << name_ << ", user token: " << token_;
}

int UserMgr::uid() const
{
    return uid_;
}

void UserMgr::setUid(int newUid)
{
    uid_ = newUid;
}

QString UserMgr::name() const
{
    return name_;
}

void UserMgr::setName(const QString &newName)
{
    name_ = newName;
}

QString UserMgr::token() const
{
    return token_;
}

void UserMgr::setToken(const QString &newToken)
{
    token_ = newToken;
}
