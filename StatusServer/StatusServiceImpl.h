#pragma once
#include <grpcpp/grpcpp.h>
#include "message.grpc.pb.h"
#include <unordered_map>

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

using message::GetChatServerReq;
using message::GetChatServerRsp;
using message::LoginReq;
using message::LoginRsp;
using message::StatusService;

class ChatServer
{
public:
    ChatServer();
    ChatServer(const ChatServer& other);
    ChatServer& operator=(const ChatServer& other);

public:
    std::string host_;
    std::string port_;
    std::string name_;
    int conn_count_;
};

class StatusServiceImpl : public StatusService::Service
{
public:
    StatusServiceImpl();
    Status GetChatServer(ServerContext* context, const GetChatServerReq* request, GetChatServerRsp* response) override;
    Status Login(ServerContext* context, const LoginReq* request, LoginRsp* response) override;

private:
    void insertToken(int uid, std::string token);
    std::shared_ptr<ChatServer> getChatServer();
    std::string generate_token();

private:
    std::unordered_map<std::string, std::shared_ptr<ChatServer>> servers_;
    std::mutex svr_mtx_;
};

