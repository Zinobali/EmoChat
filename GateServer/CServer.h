#pragma once
#include "HttpConnection.h"


class CServer :public std::enable_shared_from_this<CServer>
{
public:
    CServer(net::io_context& ioc, const USHORT& port);
    ~CServer();

    void Start();

private:
    net::io_context& ioContext_;
    tcp::acceptor acceptor_;
    tcp::socket socket_;
};

