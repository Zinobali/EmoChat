#include "StatusGrpcClient.h"
#include "global.h"
#include "ConfigMgr.h"

LoginRsp StatusGrpcClient::Login(int uid, std::string token) {
    ClientContext context;
    LoginReq request;
    LoginRsp response;
    request.set_uid(uid);
    request.set_token(token);

    auto stub = pool_->GetConnection();
    Defer defer([&stub, this]() {
        pool_->ReleaseConnection(std::move(stub));
        });
    auto status = stub->Login(&context, request, &response);
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
    pool_ = std::make_unique<GrpcConnPool<typename StatusService>>(host, port, 2);
}
