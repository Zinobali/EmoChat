#pragma once

#include "Singleton.h"
#include <unordered_map>
#include <memory>
#include <mutex>

class CSession;
class UserMgr : public Singleton<UserMgr>
{
    friend class Singleton<UserMgr>;
public:
    ~UserMgr() = default;
    std::shared_ptr<CSession> GetSession(int uid);
    void SetUserSession(int uid, std::shared_ptr<CSession> session);
    void RemoveUserSession(int uid);

private:
    UserMgr() = default;

    std::mutex session_mutex_;
    std::unordered_map<int, std::shared_ptr<CSession>> uid_to_session_;
};

