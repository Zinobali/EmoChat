#include "ChatGrpcClient.h"
#include "ConfigMgr.h"
#include <vector>

AddFriendRsp ChatGrpcClient::NotifyAddFriend(std::string server_ip, const AddFriendReq& request) {
    AddFriendRsp response;
    Defer defer([&response, &request]() {
        response.set_error(ErrorCodes::Success);
        response.set_apply_uid(request.apply_uid());
        response.set_to_uid(request.to_uid());
        });

    auto find_iter = pools_.find(server_ip);
    if (find_iter == pools_.end()) {
        return response;
    }

    auto& pool = find_iter->second;
    auto stub = pool->GetConnection();
    Defer release_conn([&pool, &stub]() {
        pool->ReleaseConnection(std::move(stub));
        });

    ClientContext context;
    Status status = stub->NotifyAddFriend(&context, request, &response);
    if (!status.ok()) {
        response.set_error(ErrorCodes::RPCFailed);
        return response;
    }

    return response;
}

AuthFriendRsp ChatGrpcClient::NotifyAuthFriend(std::string server_ip, const AuthFriendReq& request) {
    AuthFriendRsp response;
    response.set_error(ErrorCodes::Success);
    Defer defer([&response, &request]() {
        response.set_from_uid(request.from_uid());
        response.set_to_uid(request.to_uid());
        });

    auto find_iter = pools_.find(server_ip);
    if (find_iter == pools_.end()) {
        return response;
    }

    auto& pool = find_iter->second;
    ClientContext context;
    auto stub = pool->GetConnection();
    Defer release_conn([&pool, &stub]() {
        pool->ReleaseConnection(std::move(stub));
        });

    Status status = stub->NotifyAuthFriend(&context, request, &response);
    if (!status.ok()) {
        response.set_error(ErrorCodes::RPCFailed);
        return response;
    }

    return response;
}

TextChatMsgRsp ChatGrpcClient::NotifyTextChatMsg(std::string server_ip, const TextChatMsgReq& request, const Json::Value& return_value) {
    // 准备回复
    TextChatMsgRsp response;
    response.set_error(ErrorCodes::Success);
    Defer defer([&response, &request]() {
        response.set_from_uid(request.from_uid());
        response.set_to_uid(request.to_uid());
        for (const auto& text_data : request.text_msgs()) {
            auto* new_msg = response.add_text_msgs();
            new_msg->set_msg_id(text_data.msg_id());
            new_msg->set_msg_content(text_data.msg_content());
        }
        });

    // 判断rpc服务器是否连接
    auto find_iter = pools_.find(server_ip);
    if (find_iter == pools_.end()) {
        return response;
    }

    auto& pool = find_iter->second;
    ClientContext context;
    auto stub = pool->GetConnection();
    Defer release_conn([&pool, &stub]() {
        pool->ReleaseConnection(std::move(stub));
        });

    Status status = stub->NotifyTextChatMsg(&context, request, &response);
    if (!status.ok()) {
        response.set_error(ErrorCodes::RPCFailed);
        return response;
    }

    return response;
}

ChatGrpcClient::ChatGrpcClient() {
    auto& config = ConfigMgr::GetInstance();
    auto server_list = config["PeerServer"]["Servers"];

    std::vector<std::string> server_names;
    std::stringstream ss(server_list);
    std::string server_name;

    while (std::getline(ss, server_name, ',')) {
        server_names.push_back(server_name);
    }

    for (auto& name : server_names) {
        if (!config[name]["Name"].empty()) {
            pools_[config[name]["Name"]] = std::make_unique<ChatConnPool>(config[name]["Host"], config[name]["Port"], 2);
        }
    }
}
