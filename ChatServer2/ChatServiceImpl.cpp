#include "ChatServiceImpl.h"
#include "UserMgr.h"
#include "global.h"
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>
#include "MySQLMgr.h"
#include "RedisMgr.h"

Status ChatServiceImpl::NotifyAddFriend(ServerContext* context, const AddFriendReq* request, AddFriendRsp* response) {
    Defer defer([request, response]() {
        response->set_error(ErrorCodes::Success);
        response->set_apply_uid(request->apply_uid());
        response->set_to_uid(request->to_uid());
        });

    // 查找是否在本服务器
    auto to_uid = request->to_uid();
    auto session = UserMgr::GetInstance()->GetSession(to_uid);
    // 不在线直接返回，之前已存入数据库
    if (!session) {
        return Status::OK;
    }

    // 在线直接发送通知
    Json::Value return_value;
    return_value["error"] = ErrorCodes::Success;
    return_value["applyuid"] = request->apply_uid();
    return_value["name"] = request->name();
    return_value["desc"] = request->description();
    return_value["icon"] = request->icon();
    return_value["sex"] = request->sex();
    return_value["nick"] = request->nickname();

    session->Send(MSG_IDS::ID_NOTIFY_ADD_FRIEND_REQ, return_value.toStyledString());
    return Status::OK;
}

Status ChatServiceImpl::NotifyAuthFriend(ServerContext* context, const AuthFriendReq* request, AuthFriendRsp* response) {
    Defer defer([request, response]() {
        response->set_error(ErrorCodes::Success);
        response->set_to_uid(request->to_uid());
        response->set_from_uid(request->from_uid());
        });

    // 查找是否在本服务器
    auto to_uid = request->to_uid();
    auto from_uid = request->from_uid();
    auto session = UserMgr::GetInstance()->GetSession(to_uid);
    // 用户不在本服务器直接返回
    if (!session) {
        return Status::OK;
    }

    // 在线直接发送通知
    Json::Value return_value;
    return_value["error"] = ErrorCodes::Success;
    return_value["fromuid"] = from_uid;
    return_value["touid"] = to_uid;

    std::string base_key = USER_BASE_INFO + std::to_string(from_uid);
    auto user_info = std::make_shared<UserInfo>();
    bool info_ok = GetBaseInfo(base_key, from_uid, user_info);
    if (info_ok) {
        return_value["name"] = user_info->name;
        return_value["nick"] = user_info->nick;
        return_value["icon"] = user_info->icon;
        return_value["sex"] = user_info->sex;
    } else {
        return_value["error"] = ErrorCodes::UidInvalid;
    }

    session->Send(MSG_IDS::ID_NOTIFY_AUTH_FRIEND_REQ, return_value.toStyledString());
    return Status::OK;
}

Status ChatServiceImpl::NotifyTextChatMsg(ServerContext* context, const TextChatMsgReq* request, TextChatMsgRsp* response) {
    // 查找是否在本服务器
    auto to_uid = request->to_uid();
    auto session = UserMgr::GetInstance()->GetSession(to_uid);
    response->set_error(ErrorCodes::Success);

    // 不在线直接返回
    if (!session) {
        return Status::OK;
    }

    // todo ...优化，发送到redis，然后由redis触发

    // 在线就准备发送
    Json::Value return_value;
    return_value["error"] = ErrorCodes::Success;
    return_value["fromuid"] = request->from_uid();
    return_value["touid"] = request->to_uid();

    // 准备数组
    Json::Value text_array;
    for (auto& msg : request->text_msgs()) {
        Json::Value element;
        element["content"] = msg.msg_content();
        element["msgid"] = msg.msg_id();
        text_array.append(element);
    }
    return_value["text_array"] = text_array;

    // 发送
    session->Send(MSG_IDS::ID_NOTIFY_TEXT_CHAT_MSG_REQ, return_value.toStyledString());
    return Status::OK;
}

bool ChatServiceImpl::GetBaseInfo(const std::string& base_key, int uid, std::shared_ptr<UserInfo>& userinfo) {
    // 尝试从redis获取用户信息
    std::string info_str = "";
    bool info_ok = RedisMgr::GetInstance().Get(base_key, info_str);
    if (info_ok) {
        Json::Value root;
        Json::Reader reader;
        reader.parse(info_str, root);

        userinfo->uid = root["uid"].asInt();
        userinfo->name = root["name"].asString();
        userinfo->email = root["email"].asString();
        userinfo->nick = root["nick"].asString();
        userinfo->desc = root["desc"].asString();
        userinfo->sex = root["sex"].asInt();
        userinfo->icon = root["icon"].asString();
        std::cout << "user login uid is  " << userinfo->uid << " name  is "
            << userinfo->name << " email is " << userinfo->email << std::endl;
    } else {
        // redis 没有，从mysql获取
        std::shared_ptr<UserInfo> user = nullptr;
        user = MySQLMgr::GetInstance()->GetUser(uid);
        if (user == nullptr) {
            return false;
        }

        userinfo = user;
        // 写入redis
        Json::Value redis_root;
        redis_root["uid"] = uid;
        redis_root["name"] = userinfo->name;
        redis_root["email"] = userinfo->email;
        redis_root["nick"] = userinfo->nick;
        redis_root["desc"] = userinfo->desc;
        redis_root["sex"] = userinfo->sex;
        redis_root["icon"] = userinfo->icon;
        RedisMgr::GetInstance().Set(base_key, redis_root.toStyledString());
    }

    return true;
}
