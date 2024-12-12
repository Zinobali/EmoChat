#include "VerifyGrpcClient.h"
#include "global.h"

//GetVerifyRsp VerifyGrpcClient::GetVerifyCode(const std::string& email) {
GetVerifyRsp VerifyGrpcClient::GetVerifyCode(std::string email) {
    ClientContext ctx;
    GetVerifyReq request;
    GetVerifyRsp response;
    request.set_email(email);

    Status status = stub_->GetVerifyCode(&ctx, request, &response);

    if (!status.ok()) {
        response.set_error(static_cast<int>(ErrorCodes::RPCFailed));
        return response;
    }

    return response;
}

VerifyGrpcClient::VerifyGrpcClient() {
    std::shared_ptr<Channel> channel = grpc::CreateChannel("127.0.0.1:50051", grpc::InsecureChannelCredentials());
    stub_ = VerifyService::NewStub(channel);
}
