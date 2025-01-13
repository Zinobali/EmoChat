#pragma once
#include "CSession.h"
#include <map>

class CServer :public std::enable_shared_from_this<CServer>
{

public:
    CServer(net::io_context& ioc, const USHORT& port);
    ~CServer();
    void ClearSession(std::string sess_id);

private:
    void StartAccept();
    void HandleAccept(std::shared_ptr<CSession> new_session, const boost::system::error_code& error);

private:
    net::io_context& io_context_;
    USHORT port_;
    tcp::acceptor acceptor_;
    std::map<std::string, std::shared_ptr<CSession>> sessions_;
    std::mutex sess_mtx_;
};

