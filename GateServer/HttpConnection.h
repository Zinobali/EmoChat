#pragma once
#include <boost/asio.hpp>
#include <memory>
#include <iostream>
#include <boost/beast.hpp>
#include <boost/beast/http.hpp>
#include <unordered_map>

namespace net = boost::asio;
using tcp = net::ip::tcp;
namespace beast = boost::beast;
namespace http = beast::http;

std::string UrlEncode(const std::string& str);
std::string UrlDecode(const std::string& str);

class LogicSystem;
class HttpConnection :public std::enable_shared_from_this<HttpConnection>
{
    friend class LogicSystem;
public:
    HttpConnection(net::io_context& io_context);
    void Start();
    tcp::socket& GetSocket();

private:
    void HandleRequest();
    void SendResponse();
    void CheckDeadline();
    void PreParseGetParams();


    tcp::socket socket_;
    beast::flat_buffer buffer_;

    http::request<http::dynamic_body> request_;
    http::response<http::dynamic_body> response_;

    net::steady_timer timeout_;

    std::string get_url_;
    std::unordered_map<std::string, std::string> get_params_;
};

