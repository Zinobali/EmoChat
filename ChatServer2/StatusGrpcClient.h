#pragma once

#include "GrpcConnPool.h"
#include "message.grpc.pb.h"
#include "Singleton.h"

using message::LoginReq;
using message::LoginRsp;
using message::StatusService;

class StatusGrpcClient : public Singleton<StatusGrpcClient>
{
    friend class Singleton<StatusGrpcClient>;
public:
    LoginRsp Login(int uid, std::string token);

private:
    StatusGrpcClient();
    std::unique_ptr<GrpcConnPool<typename StatusService>> pool_;
};

