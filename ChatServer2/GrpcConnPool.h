#pragma once
#include <grpcpp/grpcpp.h>
#include <memory>
#include <atomic>
#include <queue>

using grpc::Channel;
using grpc::Status;
using grpc::ClientContext;

template <typename StubService>
class GrpcConnPool
{
public:
    GrpcConnPool(std::string host, std::string port, std::size_t pool_size)
        : pool_size_(pool_size), host_(host), port_(port), b_stop_(false) {
        for (size_t i = 0; i < pool_size; i++) {
            std::shared_ptr <grpc::Channel> channel = grpc::CreateChannel(host_ + ":" + port_, grpc::InsecureChannelCredentials());
            connections_.push(StubService::NewStub(channel));
        }
    }

    ~GrpcConnPool() {
        Close();
    }

    std::unique_ptr<typename StubService::Stub> GetConnection() {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this]() {
            return b_stop_ || !connections_.empty();
            });

        if (b_stop_) {
            return nullptr;
        }

        auto conn = std::move(connections_.front());
        connections_.pop();
        return conn;
    }

    void ReleaseConnection(std::unique_ptr<typename StubService::Stub> conn) {
        std::unique_lock<std::mutex> lock(mutex_);
        if (b_stop_) {
            return;
        }

        connections_.push(std::move(conn));
        cv_.notify_one();
    }

    void Close() {
        b_stop_ = true;
        cv_.notify_all();
    }

private:
    std::size_t pool_size_;
    std::string host_;
    std::string port_;
    std::atomic_bool b_stop_;
    std::queue<std::unique_ptr<typename StubService::Stub>> connections_;
    std::mutex mutex_;
    std::condition_variable cv_;
};
