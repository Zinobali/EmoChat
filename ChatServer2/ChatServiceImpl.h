#pragma once

#include <grpcpp/grpcpp.h>
#include "message.grpc.pb.h"
#include "data.h"

using grpc::Server;
using grpc::ServerContext;
using grpc::ServerBuilder;
using grpc::Status;

using message::AddFriendReq;
using message::AddFriendRsp;

using message::AuthFriendReq;
using message::AuthFriendRsp;

using message::TextChatData;
using message::TextChatMsgReq;
using message::TextChatMsgRsp;

using message::ChatService;


class ChatServiceImpl final : public ChatService::Service
{
public:
    ChatServiceImpl() = default;
    virtual Status NotifyAddFriend(ServerContext* context, const AddFriendReq* request, AddFriendRsp* response);
    virtual Status NotifyAuthFriend(ServerContext* context, const AuthFriendReq* request, AuthFriendRsp* response);
    virtual Status NotifyTextChatMsg(ServerContext* context, const TextChatMsgReq* request, TextChatMsgRsp* response);
    bool GetBaseInfo(const std::string& base_key, int uid, std::shared_ptr<UserInfo>& userinfo);

    //virtual Status ReplyAddFriend(ServerContext* context, const ReplyFriendReq* request, ReplyFriendRsp* response);
//virtual Status SendChatMsg(ServerContext* context, const SendChatMsgReq* request, SendChatMsgRsp* response);

};

