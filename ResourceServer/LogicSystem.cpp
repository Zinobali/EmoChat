#include "LogicSystem.h"
#include "CSession.h"
#include "global.h"
#include "ConfigMgr.h"
#include <boost/beast/core/detail/base64.hpp>

//using namespace boost::beast::detail::base64;

LogicNode::LogicNode(std::shared_ptr<CSession> session, std::shared_ptr<RecvNode> recv_node)
    : session_(session), recv_node_(recv_node) {}

LogicSystem::LogicSystem()
    : b_stop_(false) {
    RegisterHandlers();
    worker_thread_ = std::thread(&LogicSystem::DealMsg, this);
}

void LogicSystem::DealMsg() {
    while (true) {
        std::unique_lock<std::mutex> lock(que_mtx_);
        cv_.wait(lock, [this]() { return b_stop_ || !logic_que_.empty(); });

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
    // 登录
    handlers_[MSG_IDS::ID_UPLOAD_FILE_REQ] = [this](std::shared_ptr<CSession> session, const uint16_t& msg_id, const std::string& msg_data) {
        UploadFileReqHandler(session, msg_id, msg_data);
        };

};

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

std::string LogicSystem::Base64Decode(std::string& input) {
    std::string decoded;
    decoded.resize(boost::beast::detail::base64::decoded_size(input.size()));
    auto result = boost::beast::detail::base64::decode(&decoded[0], input.data(), input.size());
    decoded.resize(result.first);
    return decoded;
}

std::string LogicSystem::Base64Encode(std::string& input) {
    std::string encoded;
    encoded.resize(boost::beast::detail::base64::encoded_size(input.size()));
    boost::beast::detail::base64::encode(&encoded[0], input.data(), input.size());
    return encoded;
}

void LogicSystem::UploadFileReqHandler(std::shared_ptr<CSession> session, const uint16_t& msg_id, const std::string& msg_data) {
    Json::Reader reader;
    Json::Value root;
    reader.parse(msg_data, root);
    std::string data = root["data"].asString();

    Json::Value  rtvalue;
    Defer defer([&rtvalue, session]() {
        session->Send(MSG_IDS::ID_UPLOAD_FILE_RSP, rtvalue.toStyledString());
        });

    // 解码
    std::string decode_data = Base64Decode(data);

    auto seq = root["seq"].asInt();
    auto name = root["name"].asString();
    auto total_size = root["total_size"].asInt();
    auto trans_size = root["trans_size"].asInt();
    auto file_path = ConfigMgr::GetInstance().GetFileOutPath();
    auto file_path_str = (file_path / name).string();
    std::cout << "file_path_str is " << file_path_str << std::endl;
    std::ofstream outfile;

    //第一个包
    if (seq == 1) {
        // todo ... 改为md5文件去重
        // 打开文件，如果存在则清空，不存在则创建
        outfile.open(file_path_str, std::ios::binary | std::ios::trunc);
    } else {
        // 保存为文件
        outfile.open(file_path_str, std::ios::binary | std::ios::app);
    }

    if (!outfile) {
        std::cerr << "无法打开文件进行写入。" << std::endl;
        return;
    }

    outfile.write(decode_data.data(), decode_data.size());
    if (!outfile) {
        std::cerr << "写入文件失败。" << std::endl;
        return;
    }

    outfile.close();
    std::cout << "文件已成功保存为: " << name << std::endl;

    rtvalue["error"] = ErrorCodes::Success;
    rtvalue["total_size"] = total_size;
    rtvalue["seq"] = seq;
    rtvalue["name"] = name;
    rtvalue["trans_size"] = trans_size;
}












