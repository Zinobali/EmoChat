#include "usermgr.h"
#include <QDebug>

UserMgr::UserMgr()
{
}

bool UserMgr::CheckFriendById(int uid)
{
    // todo ...
    return false;
}

std::vector<std::shared_ptr<ApplyInfo>> UserMgr::apply_list() const
{
    std::vector<std::shared_ptr<ApplyInfo>> ret;
    for (auto apply_info : apply_map_)
    {
        ret.push_back(apply_info);
    }
    return ret;
}

bool UserMgr::AlreadyApply(int uid)
{
    return apply_map_.find(uid) != apply_map_.end();
}

void UserMgr::AddApplyList(std::shared_ptr<ApplyInfo> apply)
{
    apply_map_.insert(apply->_uid, apply);
}

void UserMgr::SetUserInfo(const std::shared_ptr<UserInfo> &user_info)
{
    user_info_ = user_info;
}

std::vector<std::shared_ptr<FriendInfo>> UserMgr::friend_list() const
{
    std::vector<std::shared_ptr<FriendInfo>> ret;
    for (auto friend_info : friend_map_)
    {
        ret.push_back(friend_info);
    }
    return ret;
}

void UserMgr::AppendApplyList(QJsonArray apply_array)
{
    for (const auto &apply_value : apply_array)
    {
        if (!apply_value.isObject())
        {
            continue;
        }
        auto obj = apply_value.toObject();
        auto name = obj["name"].toString();
        auto desc = obj["desc"].toString();
        auto icon = obj["icon"].toString();
        auto nick = obj["nick"].toString();
        auto sex = obj["sex"].toInt();
        auto uid = obj["uid"].toInt();
        auto status = obj["status"].toInt();
        auto apply_info = std::make_shared<ApplyInfo>(uid, name, desc, icon, nick, sex, status);
        AddApplyList(apply_info);
    }
}

UserMgr::~UserMgr()
{
}

int UserMgr::uid() const
{
    return user_info_->_uid;
}

void UserMgr::setUid(int newUid)
{
    user_info_->_uid = newUid;
}

QString UserMgr::name() const
{
    return user_info_->_name;
}

void UserMgr::setName(const QString &newName)
{
    user_info_->_name = newName;
}

QString UserMgr::token() const
{
    return token_;
}

void UserMgr::setToken(const QString &newToken)
{
    token_ = newToken;
}
