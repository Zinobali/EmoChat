#pragma once
#include "Singleton.h"
#include "message.grpc.pb.h"
#include <grpcpp/grpcpp.h>
#include <queue>
#include<condition_variable>

using grpc::Channel;
using grpc::Status;
using grpc::ClientContext;

using message::VerifyService;
using message::GetVerifyReq;
using message::GetVerifyRsp;


class gRPCPool {
public:
    gRPCPool(std::size_t pool_size, std::string host, std::string port);
    ~gRPCPool();

    using StubPtr = std::unique_ptr<VerifyService::Stub>;
    StubPtr getConnection();
    void returnConnection(StubPtr conn);
    void Close();

private:
    std::atomic<bool> b_stop_;
    std::string host_;
    std::string port_;
    std::queue<StubPtr> connections_;
    std::mutex mutex_;
    std::condition_variable cv_;
};

class VerifyGrpcClient :public Singleton<VerifyGrpcClient>
{
    friend class Singleton<VerifyGrpcClient>;
public:

    GetVerifyRsp GetVerifyCode(std::string email);
    ~VerifyGrpcClient() = default;
private:
    VerifyGrpcClient();
    std::unique_ptr<gRPCPool> pool_;
};

