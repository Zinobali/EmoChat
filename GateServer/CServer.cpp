#include "CServer.h"

CServer::CServer(net::io_context& ioc, const USHORT& port)
    :ioContext_(ioc),
    acceptor_(ioc, tcp::endpoint(tcp::v4(), port)),
    socket_(ioc) {}

CServer::~CServer() {
    std::cout << "~CServer()" << std::endl;
}

void CServer::Start() {
    auto self(shared_from_this());
    acceptor_.async_accept(socket_, [self](const boost::system::error_code& error) {
        try {
            if (error) {
                self->Start();
                return;
            }
            // 未出现错误，则创建一个连接
            std::make_shared<HttpConnection>(std::move(self->socket_))->Start();
            self->Start();
        }
        catch (std::exception& e) {
            std::cout << "exception in CServer::Start(): " << e.what() << std::endl;
        }
        });
}
