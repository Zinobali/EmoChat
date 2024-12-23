#pragma once
#include <boost/asio.hpp>
#include <memory>
#include <iostream>
#include <boost/beast.hpp>
#include <boost/beast/http.hpp>
#include <unordered_map>
#include "MsgNode.h"
#include "global.h"
#include <mutex>
#include <queue>

constexpr size_t MAX_SENDQUE = 1024;

namespace net = boost::asio;
namespace beast = boost::beast;
namespace http = beast::http;
using tcp = net::ip::tcp;

class LogicSystem;
class CServer;
class CSession :public std::enable_shared_from_this<CSession>
{
public:
    CSession(net::io_context& io_context, CServer* server);
    ~CSession();
    void Start();
    tcp::socket& GetSocket();
    std::string GetSessionId();
    void Close();
    void Send(const uint16_t& msg_id, const std::string& msg);


private:
    void AsyncReadHead();
    void AsyncReadBody(uint16_t length);
    //void CheckDeadline();
    void HandleReadHead(const boost::system::error_code& error, std::size_t bytes_transferred);
    void HandleReadBody(const boost::system::error_code& error, std::size_t bytes_transferred, std::size_t bytes_expected);
    void HandleError(const boost::system::error_code& error);
    void HandleWrite(const boost::system::error_code& error, std::size_t bytes_transferred);

private:
    CServer* server_;
    tcp::socket socket_;
    std::string sess_id_;
    std::vector<char> buffer_;
    std::shared_ptr<MsgNode> recv_head_node_; // 可复用
    std::shared_ptr<RecvNode> recv_msg_node_; // 不复用
    std::mutex send_mutex_;
    std::queue<std::shared_ptr<SendNode>> send_queue_;
    std::atomic_bool b_close_;

    //net::steady_timer timeout_;
};

