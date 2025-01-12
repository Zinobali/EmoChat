#pragma once
#include "CSession.h"
#include <map>

typedef std::map<std::string, std::shared_ptr<CSession>>::iterator Session_Iterator;
class CServer :public std::enable_shared_from_this<CServer>
{

public:
    CServer(net::io_context& ioc, const USHORT& port);
    ~CServer();
    void ClearSession(std::string sess_id);
    Session_Iterator ClearSession(Session_Iterator& it);

private:
    void StartAccept();
    void HandleAccept(std::shared_ptr<CSession> new_session, const boost::system::error_code& error);
    void checkDeadSessions();
    void StartCheckDeadSessions();

private:
    net::io_context& io_context_;
    USHORT port_;
    tcp::acceptor acceptor_;
    std::map<std::string, std::shared_ptr<CSession>> sessions_;
    std::mutex sess_mtx_;
};

