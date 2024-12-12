#pragma once
#include "Singleton.h"
#include "message.grpc.pb.h"
#include <grpcpp/grpcpp.h>
#include <memory>

using grpc::Channel;
using grpc::Status;
using grpc::ClientContext;

using message::VerifyService;
using message::GetVerifyReq;
using message::GetVerifyRsp;

class VerifyGrpcClient :public Singleton<VerifyGrpcClient>
{
    friend class Singleton<VerifyGrpcClient>;
public:
    //GetVerifyRsp GetVerifyCode(const std::string& email);
    GetVerifyRsp GetVerifyCode(std::string email);
    ~VerifyGrpcClient() = default;
private:
    VerifyGrpcClient();
    std::unique_ptr<VerifyService::Stub> stub_;
};

