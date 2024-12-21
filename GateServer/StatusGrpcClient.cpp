#include "StatusGrpcClient.h"
#include "global.h"
#include "ConfigMgr.h"

StatusConnPool::StatusConnPool(const std::string& host, const std::string& port, size_t pool_size)
    :pool_size_(pool_size), host_(host), port_(port), b_stop_(false) {
    for (size_t i = 0; i < pool_size_; i++) {
        auto channel = grpc::CreateChannel(host_ + ":" + port_, grpc::InsecureChannelCredentials());
        conn_pool_.push(StatusService::NewStub(channel));
    }
}

StatusConnPool::~StatusConnPool() {
    std::lock_guard<std::mutex> lock(mutex_);
    Close();
}

std::unique_ptr<StatusService::Stub> StatusConnPool::getConnection() {
    std::unique_lock<std::mutex> lock(mutex_);
    cv_.wait(lock, [this]() {
        return b_stop_ || !conn_pool_.empty();
        });
    if (b_stop_) {
        return nullptr;
    }
    auto conn = std::move(conn_pool_.front());
    conn_pool_.pop();
    return conn;
}

void StatusConnPool::releaseConnection(std::unique_ptr<StatusService::Stub>&& conn) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (b_stop_) {
        return;
    }
    conn_pool_.push(std::move(conn));
    cv_.notify_one();
}

void StatusConnPool::Close() {
    b_stop_ = true;
    cv_.notify_all();
}

GetChatServerRsp StatusGrpcClient::GetChatServer(int uid) {
    ClientContext context;
    GetChatServerReq request;
    GetChatServerRsp response;
    request.set_uid(uid);

    auto conn = pool_->getConnection();
    Defer defer([this, &conn]() {
        pool_->releaseConnection(std::move(conn));
        });

    auto status = conn->GetChatServer(&context, request, &response);
    if (!status.ok()) {
        response.set_error(toInt(ErrorCodes::RPCFailed));
        return response;
    }

    return response;
}

StatusGrpcClient::StatusGrpcClient() {
    auto& config_mgr = ConfigMgr::GetInstance();
    auto host = config_mgr["StatusServer"]["Host"];
    auto port = config_mgr["StatusServer"]["Port"];
    pool_ = std::make_unique<StatusConnPool>(host, port, 2);
}
