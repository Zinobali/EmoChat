#include "LogicSystem.h"
#include "CSession.h"
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>
#include "global.h"

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
    std::cout << "user login uid is: " << root["uid"].asInt() << ", user token  is: " << root["token"].asString() << std::endl;

    auto return_str = root.toStyledString();
    session->Send(msg_id, return_str);
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

