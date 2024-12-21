#pragma once
#include <grpcpp/grpcpp.h>
#include "message.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

using message::GetChatServerReq;
using message::GetChatServerRsp;
using message::StatusService;

struct ChatServer
{
    std::string host;
    std::string port;
};

class StatusServiceImpl : public StatusService::Service
{
public:
    StatusServiceImpl();
    virtual Status GetChatServer(ServerContext* context, const GetChatServerReq* request, GetChatServerRsp* response) override;

    std::vector<ChatServer> servers_;
    int server_index_;
};

