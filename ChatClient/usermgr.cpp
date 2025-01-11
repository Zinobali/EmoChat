#include "usermgr.h"
#include <QDebug>

UserMgr::UserMgr()
{
}

bool UserMgr::CheckFriendById(int uid)
{
    return friend_map_.find(uid) != friend_map_.end();
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

std::shared_ptr<UserInfo> UserMgr::GetUserInfo()
{
    return user_info_;
}

QVector<std::shared_ptr<FriendInfo>> UserMgr::friend_list() const
{
    return friend_list_;
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

void UserMgr::AppendFriendList(QJsonArray friend_array)
{
    for (const auto &friend_value : friend_array)
    {
        if (!friend_value.isObject())
        {
            continue;
        }
        auto obj = friend_value.toObject();
        auto name = obj["name"].toString();
        auto desc = obj["desc"].toString();
        auto icon = obj["icon"].toString();
        auto nick = obj["nick"].toString();
        auto sex = obj["sex"].toInt();
        auto uid = obj["uid"].toInt();
        auto back = obj["back"].toString();

        auto friend_info = std::make_shared<FriendInfo>(uid, name, nick, icon, sex, desc, back);
        friend_list_.push_back(friend_info);                // 添加到列表中
        friend_map_.insert(friend_info->_uid, friend_info); // 添加到map中
    }
}

void UserMgr::AddFriend(std::shared_ptr<AuthInfo> auth_info)
{
    auto friend_info = std::make_shared<FriendInfo>(auth_info);
    friend_list_.push_back(friend_info);
    friend_map_.insert(friend_info->_uid, friend_info);
}

void UserMgr::AddFriend(std::shared_ptr<AuthRsp> auth_rsp)
{
    auto friend_info = std::make_shared<FriendInfo>(auth_rsp);
    friend_list_.push_back(friend_info);
    friend_map_.insert(friend_info->_uid, friend_info);
}

std::shared_ptr<FriendInfo> UserMgr::GetFriendById(int uid)
{
    auto iter = friend_map_.find(uid);
    if (iter == friend_map_.end())
    {
        return nullptr;
    }

    return iter.value();
}

QVector<std::shared_ptr<FriendInfo>> UserMgr::GetChatListPerPage()
{
    QVector<std::shared_ptr<FriendInfo>> ret;
    int begin = chat_list_load_count_;
    int end = begin + LOAD_PAGE_SIZE;

    if (begin >= friend_list_.size())
    {
        return ret; // 列表为空
    }

    if (end > friend_list_.size())
    {
        end = friend_list_.size(); // 保证end不越界
    }

    ret = friend_list_.mid(begin, end - begin);
    chat_list_load_count_ = end;
    return ret;
}

QVector<std::shared_ptr<FriendInfo>> UserMgr::GetContactsPerPage()
{
    QVector<std::shared_ptr<FriendInfo>> ret;
    int begin = contacts_load_count_;
    int end = begin + LOAD_PAGE_SIZE;

    if (begin >= friend_list_.size())
    {
        return ret; // 列表为空
    }

    if (end > friend_list_.size())
    {
        end = friend_list_.size(); // 保证end不越界
    }

    ret = friend_list_.mid(begin, end - begin);
    contacts_load_count_ = end;
    return ret;
}

bool UserMgr::IsChatListLoadFinish()
{
    return chat_list_load_count_ >= friend_list_.size();
}

bool UserMgr::IsContactsLoadFinish()
{
    return contacts_load_count_ >= friend_list_.size();
}

void UserMgr::AppendFriendChatMsg(int friend_id, std::vector<std::shared_ptr<TextChatData>> msgs)
{
    auto find_iter = friend_map_.find(friend_id);
    if (find_iter == friend_map_.end())
    {
        return;
    }

    find_iter.value()->AppendChatMsgs(msgs);
}

// void UserMgr::UpdateChatListLoadCount()
// {
//     int begin = chat_list_load_count_;
//     int end = begin + LOAD_PAGE_SIZE;

//     if (begin >= friend_list_.size())
//     {
//         return; // 列表为空
//     }

//     if (end > friend_list_.size())
//     {
//         end = friend_list_.size(); // 保证end不越界
//     }

//     chat_list_load_count_ = end;
// }

// void UserMgr::UpdateContactsLoadCount()
// {
//     int begin = contacts_load_count_;
//     int end = begin + LOAD_PAGE_SIZE;

//     if (begin >= friend_list_.size())
//     {
//         return; // 列表为空
//     }

//     if (end > friend_list_.size())
//     {
//         end = friend_list_.size(); // 保证end不越界
//     }

//     contacts_load_count_ = end;
// }

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
