#include "CServer.h"
#include "AsioIOServicePool.h"

CServer::CServer(net::io_context& ioc, const USHORT& port)
    :ioContext_(ioc),
    acceptor_(ioc, tcp::endpoint(tcp::v4(), port)) {}

CServer::~CServer() {
    std::cout << "~CServer()" << std::endl;
}

void CServer::Start() {
    auto self(shared_from_this());
    auto& io_context = AsioIOServicePool::GetInstance()->GetIOService();
    auto new_con = std::make_shared<HttpConnection>(io_context);
    acceptor_.async_accept(new_con->GetSocket(), [self, new_con](const boost::system::error_code& error) {
        try {
            if (error) {
                self->Start();
                return;
            }

            new_con->Start();
            self->Start(); // 继续接收连接
        }
        catch (std::exception& e) {
            std::cout << "exception in CServer::Start(): " << e.what() << std::endl;
            self->Start();
        }
        });
}
