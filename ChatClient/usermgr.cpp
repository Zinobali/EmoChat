#include "usermgr.h"
#include <QDebug>

UserMgr::UserMgr()
{
    test_function(); // 测试函数，后续删除
}

void UserMgr::test_function() // 测试函数，后续删除
{
    uid_ = 43998;
    token_ = "ahoaghaoigaoighadg";
    name_ = "旻山君";
}

std::vector<std::shared_ptr<ApplyInfo> > UserMgr::apply_list() const
{
    return apply_list_;
}

void UserMgr::setApply_list(const std::vector<std::shared_ptr<ApplyInfo> > &newApply_list)
{
    apply_list_ = newApply_list;
}

std::vector<std::shared_ptr<FriendInfo> > UserMgr::friend_list() const
{
    return friend_list_;
}

void UserMgr::setFriend_list(const std::vector<std::shared_ptr<FriendInfo> > &newFriend_list)
{
    friend_list_ = newFriend_list;
}

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
