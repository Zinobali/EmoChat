#pragma once
#include <grpcpp/grpcpp.h>
#include "message.grpc.pb.h"
#include "Singleton.h"
#include <memory>
#include <atomic>
#include <queue>
#include <mutex>
#include <condition_variable>

using grpc::ClientContext;
using grpc::Channel;
using grpc::Status;

using message::LoginReq;
using message::LoginRsp;
using message::StatusService;

class StatusConnPool
{
public:
    StatusConnPool(const std::string& host, const std::string& port, size_t pool_size);
    ~StatusConnPool();

    std::unique_ptr<StatusService::Stub> getConnection();
    void releaseConnection(std::unique_ptr<StatusService::Stub>&& conn);
    void Close();

private:
    std::atomic_bool b_stop_;
    size_t pool_size_;
    std::string host_;
    std::string port_;
    std::queue<std::unique_ptr<StatusService::Stub>> conn_pool_;
    std::mutex mutex_;
    std::condition_variable cv_;
};

class StatusGrpcClient : public Singleton<StatusGrpcClient>
{
    friend class Singleton<StatusGrpcClient>;
public:
    LoginRsp Login(int uid, std::string token);

private:
    StatusGrpcClient();
    std::unique_ptr<StatusConnPool> pool_;
};

