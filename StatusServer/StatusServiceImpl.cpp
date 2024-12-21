#include "StatusServiceImpl.h"
#include "ConfigMgr.h"
#include "global.h"
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

std::string generate_token() {
    boost::uuids::uuid u = boost::uuids::random_generator()();
    std::string s = boost::uuids::to_string(u);
    return s;
}

StatusServiceImpl::StatusServiceImpl()
    :server_index_(0) {
    auto& config = ConfigMgr::GetInstance();
    ChatServer s;
    s.host = config["ChatServer1"]["Host"];
    s.port = config["ChatServer1"]["Port"];
    servers_.push_back(s);

    s.host = config["ChatServer2"]["Host"];
    s.port = config["ChatServer2"]["Port"];
    servers_.push_back(s);
}

Status StatusServiceImpl::GetChatServer(ServerContext* context, const GetChatServerReq* request, GetChatServerRsp* response) {
    std::string prefix("status server has received :  ");
    auto& s = servers_[server_index_++];
    server_index_ %= servers_.size();
    response->set_host(s.host);
    response->set_port(s.port);
    response->set_error(toInt(ErrorCodes::Success));
    response->set_token(generate_token());
    return Status::OK;
}
