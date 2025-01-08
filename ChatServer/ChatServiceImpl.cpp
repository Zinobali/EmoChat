#include "ChatServiceImpl.h"
#include "UserMgr.h"
#include "global.h"
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>

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
    // todo ...
    return Status();
}

Status ChatServiceImpl::NotifyTextChatMsg(ServerContext* context, const TextChatMsgReq* request, TextChatMsgRsp* response) {
    // todo ...

    return Status();
}

bool ChatServiceImpl::GetBaseInfo(const std::string& base_key, int uid, std::shared_ptr<UserInfo>& userinfo) {
    // todo ...

    return false;
}
