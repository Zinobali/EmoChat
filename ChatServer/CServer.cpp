#include "CServer.h"
#include "AsioIOServicePool.h"
#include "UserMgr.h"

CServer::CServer(net::io_context& ioc, const USHORT& port)
    :io_context_(ioc), port_(port),
    acceptor_(ioc, tcp::endpoint(tcp::v4(), port)) {
    std::cout << "Chat Server1 started on port: " << acceptor_.local_endpoint().port() << std::endl;
    StartAccept();
    //StartCheckDeadSessions();
}

CServer::~CServer() {
    std::cout << "~CServer()" << std::endl;
}

void CServer::ClearSession(std::string sess_id) {
    if (sessions_.find(sess_id) != sessions_.end()) {
        //移除用户和session的关联
        UserMgr::GetInstance()->RemoveUserSession(sessions_[sess_id]->GetUserId());
    }

    {
        std::lock_guard<std::mutex> lock(sess_mtx_);
        sessions_.erase(sess_id);
    }
}

Session_Iterator CServer::ClearSession(Session_Iterator& it) {
    //移除用户和session的关联
    UserMgr::GetInstance()->RemoveUserSession(it->second->GetUserId());

    std::lock_guard<std::mutex> lock(sess_mtx_);
    return sessions_.erase(it);

}

void CServer::StartAccept() {
    auto& io_context = AsioIOServicePool::GetInstance()->GetIOService();
    auto new_session = std::make_shared<CSession>(io_context, this);
    // 改用lambda表达式，更帅气
    acceptor_.async_accept(new_session->GetSocket(), [this, new_session](const boost::system::error_code& ec) {
        HandleAccept(new_session, ec);
        });
}

void CServer::HandleAccept(std::shared_ptr<CSession> new_session, const boost::system::error_code& error) {
    if (error) {
        std::cout << "session accept failed, error is " << error.what() << std::endl;
        return;
    }

    new_session->Start();
    std::cout << "session accept success, session id is " << new_session->GetSessionId() << std::endl;
    {
        std::lock_guard<std::mutex> lock(sess_mtx_);
        sessions_.emplace(new_session->GetSessionId(), new_session);
    }
    StartAccept();
}

void CServer::checkDeadSessions() {
    std::lock_guard<std::mutex> lock(sess_mtx_);
    // 遍历map
    for (auto it = sessions_.begin(); it != sessions_.end();) {
        auto last_active_time = it->second->GetLastActiveTime();
        auto now = std::chrono::system_clock::now();
        if (last_active_time + std::chrono::seconds(45) < now) {
            // 45s没有活跃，则关闭session
            it->second->Close();
            it = ClearSession(it);
        } else {
            ++it;
        }
    }
}

void CServer::StartCheckDeadSessions() {
    if (sessions_.empty()) {
        return;
    }
    // 调用轮询函数，每 10 到 15 秒
    std::thread([this] {
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(15));
            checkDeadSessions();
        }
        }).detach();
}
