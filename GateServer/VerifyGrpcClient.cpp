#include "VerifyGrpcClient.h"
#include "ConfigMgr.h"
#include "global.h"

GetVerifyRsp VerifyGrpcClient::GetVerifyCode(std::string email) {
    ClientContext ctx;
    GetVerifyReq request;
    GetVerifyRsp response;
    request.set_email(email);
    auto stub = pool_->getConnection();
    Status status = stub->GetVerifyCode(&ctx, request, &response);

    if (!status.ok()) {
        std::cout << "Verify server is not connected." << std::endl;
        pool_->returnConnection(std::move(stub));
        response.set_error(static_cast<int>(ErrorCodes::RPCFailed));
        return response;
    }

    pool_->returnConnection(std::move(stub));
    return response;
}

VerifyGrpcClient::VerifyGrpcClient() {
    auto& configMgr = ConfigMgr::GetInstance();
    std::string host = configMgr["VerifyServer"]["Host"];
    std::string port = configMgr["VerifyServer"]["Port"];
    pool_.reset(new gRPCPool(2, host, port));
}

gRPCPool::gRPCPool(std::size_t pool_size, std::string host, std::string port)
    : host_(host), port_(port), b_stop_(false) {
    for (std::size_t i = 0; i < pool_size; ++i) {
        auto channel = grpc::CreateChannel(host_ + ":" + port_, grpc::InsecureChannelCredentials());
        connections_.push(VerifyService::NewStub(channel));
    }
}

gRPCPool::~gRPCPool() {
    std::lock_guard<std::mutex> lock(mutex_);
    Close();
}

gRPCPool::StubPtr gRPCPool::getConnection() {
    std::unique_lock<std::mutex> lock(mutex_);
    cv_.wait(lock, [this] {
        return b_stop_ || !connections_.empty();
        });

    if (b_stop_) {
        return nullptr;
    }

    auto conn = std::move(connections_.front());
    connections_.pop();
    return conn;
}

void gRPCPool::returnConnection(StubPtr conn) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (b_stop_) {
        return;
    }
    connections_.push(std::move(conn));
    cv_.notify_one();
}

void gRPCPool::Close() {
    b_stop_ = true;
    cv_.notify_all();
}
