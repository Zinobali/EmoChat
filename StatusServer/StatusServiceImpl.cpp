#include "StatusServiceImpl.h"
#include "ConfigMgr.h"
#include "global.h"
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include "RedisMgr.h"
#include <climits>
#include <vector>
#include <sstream>

ChatServer::ChatServer()
    :host_(""), port_(""), name_(""), conn_count_(0) {}

ChatServer::ChatServer(const ChatServer& other)
    : host_(other.host_), port_(other.port_), name_(other.name_), conn_count_(other.conn_count_) {}

ChatServer& ChatServer::operator=(const ChatServer& other) {
    if (&other == this) {
        return *this;
    }

    host_ = other.host_;
    port_ = other.port_;
    name_ = other.name_;
    conn_count_ = other.conn_count_;
    return *this;
}

StatusServiceImpl::StatusServiceImpl() {
    auto& config = ConfigMgr::GetInstance();
    auto server_list = config["ChatServers"]["Name"];

    std::vector<std::string> server_names;
    std::stringstream ss(server_list);
    std::string server_name;

    while (std::getline(ss, server_name, ',')) {
        server_names.push_back(server_name);
    }

    for (auto& name : server_names) {
        if (config[name]["Name"].empty()) {
            continue;
        }

        auto server = std::make_shared<ChatServer>();
        server->name_ = config[name]["Name"];
        server->host_ = config[name]["Host"];
        server->port_ = config[name]["Port"];
        servers_[server->name_] = server;
    }
}

Status StatusServiceImpl::GetChatServer(ServerContext* context, const GetChatServerReq* request, GetChatServerRsp* response) {
    auto s = getChatServer();

    response->set_host(s->host_);
    response->set_port(s->port_);
    response->set_error(toInt(ErrorCodes::Success));
    response->set_token(generate_token());
    insertToken(request->uid(), response->token());
    return Status::OK;
}

Status StatusServiceImpl::Login(ServerContext* context, const LoginReq* request, LoginRsp* response) {
    auto uid = request->uid();
    auto token = request->token();

    std::string uid_str = std::to_string(uid);
    std::string key = USERTOKENPREFIX + uid_str;
    std::string token_value;
    auto success = RedisMgr::GetInstance().Get(key, token_value);
    if (!success) {
        response->set_error(toInt(ErrorCodes::UidInvalid));
        return Status::OK;
    }

    if (token != token_value) {
        response->set_error(toInt(ErrorCodes::TokenInvalid));
        return Status::OK;
    }

    response->set_error(toInt(ErrorCodes::Success));
    response->set_uid(uid);
    response->set_token(token);
    return Status::OK;
}


void StatusServiceImpl::insertToken(int uid, std::string token) {
    auto uid_str = std::to_string(uid);
    auto key = USERTOKENPREFIX + uid_str;
    RedisMgr::GetInstance().Set(key, token);
}

std::shared_ptr<ChatServer> StatusServiceImpl::getChatServer() {
    std::lock_guard<std::mutex> lock(svr_mtx_);
    auto minServer = servers_.begin()->second;
    auto count_str = RedisMgr::GetInstance().HGet(LOGIN_COUNT, minServer->name_);
    if (count_str.empty()) {
        //不存在则默认设置为最大
        minServer->conn_count_ = INT_MAX;
    } else {
        minServer->conn_count_ = std::stoi(count_str);
    }

    for (auto& s : servers_) {
        if (s.second->name_ == minServer->name_) {
            continue;
        }

        auto count_str = RedisMgr::GetInstance().HGet(LOGIN_COUNT, s.second->name_);
        if (count_str.empty()) {
            s.second->conn_count_ = INT_MAX;
        } else {
            s.second->conn_count_ = std::stoi(count_str);
        }

        if (minServer->conn_count_ > s.second->conn_count_) {
            minServer = s.second;
        }
    }

    return minServer;
}

std::string StatusServiceImpl::generate_token() {
    boost::uuids::uuid u = boost::uuids::random_generator()();
    std::string s = boost::uuids::to_string(u);
    return s;
}