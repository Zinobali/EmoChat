#include "HttpConnection.h"
#include "LogicSystem.h"



HttpConnection::HttpConnection(net::io_context& io_context)
    : socket_(io_context),
    buffer_{ 8192 },
    timeout_(socket_.get_executor(), std::chrono::seconds(60)) {}

void HttpConnection::Start() {
    auto self(shared_from_this());
    http::async_read(socket_, buffer_, request_,
        [self](beast::error_code error, std::size_t _) {
            boost::ignore_unused(_);
            try {
                if (error) {
                    std::cout << "Http Read Error: " << error.message() << std::endl;
                    return;
                }

                // handle request
                self->HandleRequest();
                self->CheckDeadline();
            }
            catch (std::exception& e) {
                std::cout << "Http Read Exception: " << e.what() << std::endl;
            }
        });
}

tcp::socket& HttpConnection::GetSocket() {
    return socket_;
}



void HttpConnection::HandleRequest() {
    response_.version(request_.version()); // set version
    response_.keep_alive(false); // set short connection

    if (request_.method() == http::verb::get) {
        PreParseGetParams(); // preparse the get params
        bool success = LogicSystem::GetInstance()->HandleGet(get_url_, shared_from_this());
        if (!success) {
            response_.result(http::status::not_found);
            response_.set(http::field::content_type, "text/plain");
            beast::ostream(response_.body()) << "Url Not Found\r\n";
            SendResponse();
            return;
        }

        response_.result(http::status::ok);
        response_.set(http::field::content_type, "text/plain");
        response_.set(http::field::server, "GateServer");
        SendResponse();
        return;
    }

    if (request_.method() == http::verb::post) {
        auto success = LogicSystem::GetInstance()->HandlePost(request_.target(), shared_from_this());
        if (!success) {
            response_.result(http::status::not_found);
            response_.set(http::field::content_type, "text/plain");
            beast::ostream(response_.body()) << "Url Not Found\r\n";
            SendResponse();
            return;
        }

        response_.result(http::status::ok);
        response_.set(http::field::content_type, "text/plain");
        response_.set(http::field::server, "GateServer");
        SendResponse();
        return;
    }

}

void HttpConnection::SendResponse() {
    auto self(shared_from_this());
    response_.content_length(response_.body().size());
    http::async_write(socket_, response_,
        [self](beast::error_code error, std::size_t _) {
            boost::ignore_unused(_);
            try {
                if (error) {
                    std::cout << "Http Send Error: " << error.message() << std::endl;
                }
                self->socket_.shutdown(tcp::socket::shutdown_send, error);
                self->timeout_.cancel();
            }
            catch (std::exception& e) {
                std::cout << "Http Send Exception: " << e.what() << std::endl;
            }
        });
}


void HttpConnection::CheckDeadline() {
    auto self(shared_from_this());
    timeout_.async_wait(
        [self](beast::error_code error) {
            if (!error) {
                // Close socket to cancel any timeout operation.
                self->socket_.close(error);
            }
        });
}

void HttpConnection::PreParseGetParams() {
    auto t = request_.target();
    auto pos = t.find('?'); // find the param start position
    if (pos == std::string::npos) {
        get_url_ = t; // no params
        return;
    }

    get_url_ = t.substr(0, pos);
    std::string param_str = t.substr(pos + 1);
    std::string k, v;
    size_t p = 0;
    while ((p = param_str.find('&')) != std::string::npos) {
        auto pair = param_str.substr(0, p);
        size_t eq_pos = pair.find('=');
        if (eq_pos != std::string::npos) {
            k = UrlDecode(pair.substr(0, eq_pos));
            v = UrlDecode(pair.substr(eq_pos + 1));
            get_params_[k] = v;
        }
        param_str.erase(0, p + 1);
    }

    // handle the last param
    if (!param_str.empty()) {
        size_t eq_pos = param_str.find('=');
        if (eq_pos != std::string::npos) {
            k = UrlDecode(param_str.substr(0, eq_pos));
            v = UrlDecode(param_str.substr(eq_pos + 1));
            get_params_[k] = v;
        }
    }
}

unsigned char ToHex(unsigned char x) {
    return  x > 9 ? x + 55 : x + 48;
}

unsigned char FromHex(unsigned char x) {
    unsigned char y;
    if (x >= 'A' && x <= 'Z') y = x - 'A' + 10;
    else if (x >= 'a' && x <= 'z') y = x - 'a' + 10;
    else if (x >= '0' && x <= '9') y = x - '0';
    else assert(0);
    return y;
}

std::string UrlEncode(const std::string& str) {
    std::string strTemp = "";
    size_t length = str.length();
    for (size_t i = 0; i < length; i++) {
        //判断是否仅有数字和字母构成
        if (isalnum((unsigned char)str[i]) ||
            (str[i] == '-') ||
            (str[i] == '_') ||
            (str[i] == '.') ||
            (str[i] == '~'))
            strTemp += str[i];
        else if (str[i] == ' ') //为空字符
            strTemp += "+";
        else {
            //其他字符需要提前加%并且高四位和低四位分别转为16进制
            strTemp += '%';
            strTemp += ToHex((unsigned char)str[i] >> 4);
            strTemp += ToHex((unsigned char)str[i] & 0x0F);
        }
    }
    return strTemp;
}

std::string UrlDecode(const std::string& str) {
    std::string strTemp = "";
    size_t length = str.length();
    for (size_t i = 0; i < length; i++) {
        //还原+为空
        if (str[i] == '+') strTemp += ' ';
        //遇到%将后面的两个字符从16进制转为char再拼接
        else if (str[i] == '%') {
            assert(i + 2 < length);
            unsigned char high = FromHex((unsigned char)str[++i]);
            unsigned char low = FromHex((unsigned char)str[++i]);
            strTemp += high * 16 + low;
        }
        else strTemp += str[i];
    }
    return strTemp;
}
