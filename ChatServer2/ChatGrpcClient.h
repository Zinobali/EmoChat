#pragma once
#include <grpcpp/grpcpp.h>
#include "message.grpc.pb.h"
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>
#include "Singleton.h"
#include <unordered_map>
#include "GrpcConnPool.h"
#include "global.h"

using message::AddFriendReq;
using message::AddFriendRsp;

using message::AuthFriendReq;
using message::AuthFriendRsp;

using message::TextChatData;
using message::TextChatMsgReq;
using message::TextChatMsgRsp;

using message::ChatService;

class ChatGrpcClient : public Singleton<ChatGrpcClient>
{
    friend class Singleton<ChatGrpcClient>;
    typedef GrpcConnPool<typename ChatService> ChatConnPool;

public:
    AddFriendRsp NotifyAddFriend(std::string server_ip, const AddFriendReq &request);
    AuthFriendRsp NotifyAuthFriend(std::string server_ip, const AuthFriendReq &request);
    TextChatMsgRsp NotifyTextChatMsg(std::string server_ip, const TextChatMsgReq &request, const Json::Value &return_value);

private:
    ChatGrpcClient();
    std::unordered_map<std::string, std::unique_ptr<ChatConnPool>> pools_; // 一个服务器IP对应一个连接池
};
