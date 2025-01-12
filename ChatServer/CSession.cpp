#include "CSession.h"
#include "LogicSystem.h"
#include "CServer.h"
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>

CSession::CSession(net::io_context& io_context, CServer* server)
    : socket_(io_context), server_(server), buffer_(MAX_LENGTH), b_close_(false),
    sess_id_(boost::uuids::to_string(boost::uuids::random_generator{}())),
    recv_head_node_(std::make_shared<MsgNode>(HEAD_TOTAL_LEN))
    //, timeout_(socket_.get_executor(), std::chrono::seconds(60))
{}

CSession::~CSession() {}

void CSession::Start() {
    AsyncReadHead();
}

tcp::socket& CSession::GetSocket() {
    return socket_;
}

std::string CSession::GetSessionId() {
    return sess_id_;
}

void CSession::Close() {
    socket_.close();
    server_->ClearSession(sess_id_);
}

void CSession::Send(const uint16_t& msg_id, const std::string& msg) {
    std::lock_guard<std::mutex> lock(send_mutex_);
    auto size = send_queue_.size();
    if (size > MAX_SENDQUE) {
        std::cout << "session: " << sess_id_ << ", send queue is full, drop msg" << std::endl;
        return;
    }

    send_queue_.emplace(std::make_shared<SendNode>(msg_id, msg));
    if (size > 0) {
        return;
    }

    auto node = send_queue_.front();
    std::cout << "Send msg id: " << node->msg_id_ << ", node total_length: " << node->total_len_ << std::endl;
    auto self(shared_from_this());
    net::async_write(socket_, net::buffer(node->buffer_, node->total_len_),
        [this, self](const boost::system::error_code& error, std::size_t bytes_transferred) {
            HandleWrite(error, bytes_transferred);
        });
}

void CSession::SetUserId(int user_id) {
    user_id_ = user_id;
}

int CSession::GetUserId() {
    return user_id_;
}

void CSession::UpdateLastActiveTime() {
    last_active_time_ = std::chrono::system_clock::now();
}

const std::chrono::time_point<std::chrono::system_clock>& CSession::GetLastActiveTime() {
    return last_active_time_;
}

void CSession::AsyncReadHead() {
    recv_head_node_->Clear();
    auto self(shared_from_this()); // 保证当前对象的生命周期
    net::async_read(socket_, net::buffer(recv_head_node_->buffer_, HEAD_TOTAL_LEN),
        [this, self](const boost::system::error_code& error, std::size_t bytes_transferred) {
            HandleReadHead(error, bytes_transferred);
        });
}

void CSession::AsyncReadBody(uint16_t length) {
    auto self(shared_from_this());
    net::async_read(socket_, net::buffer(recv_msg_node_->buffer_, length),
        [this, self, length](const boost::system::error_code& error, std::size_t bytes_transferred) {
            HandleReadBody(error, bytes_transferred, length);
        });
}

//void CSession::CheckDeadline() {
//    // to do... heart beat check
//    auto self(shared_from_this());
//    timeout_.async_wait(
//        [self](beast::error_code error) {
//            if (!error) {
//                // Close socket to cancel any timeout operation.
//                self->socket_.close(error);
//            }
//        });
//}

void CSession::HandleReadHead(const boost::system::error_code& error, std::size_t bytes_transferred) {
    try {
        if (error) {
            std::cout << "AsyncReadHead error: " << error.message() << std::endl;
            HandleError(error);
            return;
        }

        if (bytes_transferred != HEAD_TOTAL_LEN) {
            std::cout << "AsyncReadHead incomplete read: expected " << HEAD_TOTAL_LEN << ", got " << bytes_transferred << std::endl;
            Close();
            return;
        }

        // 解析消息头
        recv_head_node_->cur_len_ = bytes_transferred;
        uint16_t msg_id, msg_len;
        std::memcpy(&msg_id, recv_head_node_->buffer_.data(), HEAD_ID_LEN);
        std::memcpy(&msg_len, recv_head_node_->buffer_.data() + HEAD_ID_LEN, HEAD_DATA_LEN);
        msg_id = boost::endian::big_to_native(msg_id);
        msg_len = boost::endian::big_to_native(msg_len);
        std::cout << "msg_id: " << msg_id << ", msg_len: " << msg_len << std::endl;

        if (msg_id > MAX_LENGTH || msg_len > MAX_LENGTH) {
            std::cout << "Invalid msg_id or msg_len: msg_id=" << msg_id << ", msg_len=" << msg_len << std::endl;
            Close();
            return;
        }
        // 准备读消息体
        recv_msg_node_ = std::make_shared<RecvNode>(msg_id, msg_len);
        AsyncReadBody(msg_len);
    } catch (const std::exception& e) {
        std::cout << "AsyncReadHead exception: " << e.what() << std::endl;
    }
}

void CSession::HandleReadBody(const boost::system::error_code& error, std::size_t bytes_transferred, std::size_t bytes_expected) {
    try {
        if (error) {
            std::cout << "AsyncReadBody error: " << error.message() << std::endl;
            HandleError(error);
            return;
        }

        if (bytes_transferred != bytes_expected) {
            std::cout << "AsyncReadBody incomplete read: expected " << bytes_expected << ", got " << bytes_transferred << std::endl;
            Close();
            return;
        }

        recv_msg_node_->cur_len_ = bytes_transferred;

        auto src_str = std::string(recv_msg_node_->buffer_.cbegin(), recv_msg_node_->buffer_.cend());
        std::cout << "receive data is: " << src_str << std::endl;

        LogicSystem::GetInstance()->PostMsgToQue(std::make_shared<LogicNode>(shared_from_this(), recv_msg_node_));

        //继续监听头部接受事件
        AsyncReadHead();
    } catch (const std::exception& e) {
        std::cout << "AsyncReadBody exception: " << e.what() << std::endl;
    }
}

void CSession::HandleError(const boost::system::error_code& error) {
    Defer defer([&]() { Close(); });
    if (error == net::error::eof) {
        std::cout << "Connection closed by peer." << std::endl;
        return;
    }
    if (error == net::error::operation_aborted) {
        std::cout << "Operation aborted." << std::endl;
        return;
    }
    std::cout << "Unhandled error: " << error.message() << std::endl;
    return;
}

void CSession::HandleWrite(const boost::system::error_code& error, std::size_t bytes_transferred) {
    try {
        if (error) {
            std::cout << "HandleWrite error: " << error.message() << std::endl;
            HandleError(error);
            return;
        }

        if (bytes_transferred != send_queue_.front()->total_len_) {
            std::cout << "HandleWrite incomplete write: expected " << send_queue_.front()->total_len_ << ", got " << bytes_transferred << std::endl;
            Close();
            return;
        }

        std::cout << "HandleWrite success, send length: " << bytes_transferred << std::endl;
        std::lock_guard<std::mutex> lock(send_mutex_);
        send_queue_.pop();

        if (!send_queue_.empty()) {
            auto node = send_queue_.front();
            auto self(shared_from_this());
            net::async_write(socket_, net::buffer(node->buffer_, node->total_len_),
                [this, self](const boost::system::error_code& error, std::size_t bytes_transferred) {
                    HandleWrite(error, bytes_transferred);
                });
        }
    } catch (const std::exception& e) {
        std::cerr << "Exception code : " << e.what() << std::endl;
    }
}
