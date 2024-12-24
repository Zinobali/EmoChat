#include "LogicSystem.h"
#include "CSession.h"
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>
#include "global.h"
#include "StatusGrpcClient.h"
#include "RedisMgr.h"
#include "MySQLMgr.h"

LogicNode::LogicNode(std::shared_ptr<CSession> session, std::shared_ptr<RecvNode> recv_node)
    :session_(session), recv_node_(recv_node) {}

LogicSystem::LogicSystem()
    :b_stop_(false) {
    RegisterHandlers();
    worker_thread_ = std::thread(&LogicSystem::DealMsg, this);
}

void LogicSystem::DealMsg() {
    while (true) {
        std::unique_lock<std::mutex> lock(que_mtx_);
        cv_.wait(lock, [this]() {
            return b_stop_ || !logic_que_.empty();
            });

        if (b_stop_) {
            while (!logic_que_.empty()) {
                HandleMsg();
            }
            return;
        }

        HandleMsg();
    }
}

LogicSystem::~LogicSystem() {}

void LogicSystem::PostMsgToQue(std::shared_ptr<LogicNode> logic_node) {
    std::unique_lock<std::mutex> lock(que_mtx_);
    logic_que_.push(logic_node);
    if (logic_que_.size() == 1) {
        lock.unlock();
        cv_.notify_one();
    }
}

void LogicSystem::RegisterHandlers() {
    handlers_[MSG_IDS::MSG_CHAT_LOGIN] = [this](std::shared_ptr<CSession> session, const uint16_t& msg_id, const std::string& msg_data) {
        LoginHandler(session, msg_id, msg_data);
        };
}

void LogicSystem::LoginHandler(std::shared_ptr<CSession> session, const uint16_t& msg_id, const std::string& msg_data) {
    Json::Value root;
    Json::Reader reader;
    reader.parse(msg_data, root);
    auto uid = root["uid"].asInt();
    auto token = root["token"].asString();
    std::cout << "user login uid is: " << uid << ", user token  is: " << token << std::endl;
    // 从状态服务器获取用户信息
    auto rsp = StatusGrpcClient::GetInstance()->Login(uid, token);
    // 返回客户端
    Json::Value rsp_root;
    Defer defer([&rsp_root, this, session]() {
        auto return_id = toInt(MSG_IDS::MSG_CHAT_LOGIN_RSP);
        auto return_str = rsp_root.toStyledString();
        session->Send(return_id, return_str);
        });

    rsp_root["error"] = rsp.error();
    if (static_cast<ErrorCodes>(rsp.error()) != ErrorCodes::Success) {
        return;
    }

    auto iter = users_.find(uid);
    std::shared_ptr<UserInfo> user_info;
    if (iter == users_.end()) {
        // 从mysql获取用户信息
        user_info = MySQLMgr::GetInstance()->GetUser(uid);
        if (!user_info) {
            rsp_root["error"] = toInt(ErrorCodes::UidInvalid);
            return;
        }

        users_[uid] = user_info;
    } else {
        user_info = iter->second;
    }

    rsp_root["uid"] = uid;
    rsp_root["token"] = rsp.token();
    rsp_root["name"] = user_info->name;
}

void LogicSystem::HandleMsg() {
    auto node = logic_que_.front();
    std::cout << "logic node msg id is " << node->recv_node_->msg_id_ << std::endl;
    auto iter = handlers_.find(static_cast<MSG_IDS>(node->recv_node_->msg_id_));
    if (iter == handlers_.end()) {
        logic_que_.pop();
        std::cout << "msg id [" << node->recv_node_->msg_id_ << "] handler not found" << std::endl;
        return;
    }

    auto data = std::string(node->recv_node_->buffer_.cbegin(), node->recv_node_->buffer_.cend());
    iter->second(node->session_, node->recv_node_->msg_id_, data);
    logic_que_.pop();
}

