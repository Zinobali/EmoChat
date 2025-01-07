#include "ChatServiceImpl.h"

Status ChatServiceImpl::NotifyAddFriend(ServerContext* context, const AddFriendReq* request, AddFriendRsp* response) {
    // todo ...
    return Status();
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
