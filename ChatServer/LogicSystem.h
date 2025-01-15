#pragma once

#include "Singleton.h"
#include <map>
#include <functional>
#include <string>
#include "MsgNode.h"
#include "CSession.h"
#include "queue"
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>
#include "data.h"
#include "ThreadPool.h"

class LogicNode
{
    friend class LogicSystem;

public:
    LogicNode(std::shared_ptr<CSession> session, std::shared_ptr<RecvNode> recv_node);

private:
    std::shared_ptr<CSession> session_;
    std::shared_ptr<RecvNode> recv_node_;
};

typedef std::function<void(std::shared_ptr<CSession> session, const uint16_t& msg_id, const std::string& msg_data)> MsgHandler;

struct UserInfo;
class LogicSystem : public Singleton<LogicSystem>
{
    friend class ThreadPool;
    friend class Singleton<LogicSystem>;

public:
    ~LogicSystem();
    void PostMsgToQue(std::shared_ptr<LogicNode> logic_node);

private:
    LogicSystem();
    void DealMsg();
    void RegisterHandlers();
    void LoginHandler(std::shared_ptr<CSession> session, const uint16_t& msg_id, const std::string& msg_data);
    void HandleMsg();
    bool GetLatestBaseInfo(const std::string& base_key, int uid, std::shared_ptr<UserInfo>& userinfo);
    bool GetBaseInfo(const std::string& base_key, int uid, std::shared_ptr<UserInfo>& userinfo);
    void SearchUserHandler(std::shared_ptr<CSession> session, const uint16_t& msg_id, const std::string& msg_data);
    bool IsPureDigit(const std::string& str);
    void GetUserByUid(const std::string& str, Json::Value& return_value);
    void GetUserByName(const std::string& str, Json::Value& return_value);
    void AddFriendApplyHandler(std::shared_ptr<CSession> session, const uint16_t& msg_id, const std::string& msg_data);
    bool GetFriendApplyInfo(int to_uid, std::vector<std::shared_ptr<ApplyInfo>>& apply_list);
    void AuthFriendApplyHandler(std::shared_ptr<CSession> session, const uint16_t& msg_id, const std::string& msg_data);
    bool GetFriendList(int self_id, std::vector<std::shared_ptr<UserInfo>>& user_list);
    void ChatTextMsgHandler(std::shared_ptr<CSession> session, const uint16_t& msg_id, const std::string& msg_data);
    void HeartBeatHandler(std::shared_ptr<CSession> session, const uint16_t& msg_id, const std::string& msg_data);

private:
    std::thread worker_thread_;
    std::queue<std::shared_ptr<LogicNode>> logic_que_;
    std::mutex que_mtx_;
    std::condition_variable cv_;
    std::atomic_bool b_stop_;
    std::map<MSG_IDS, MsgHandler> handlers_;
    std::map<int, std::shared_ptr<UserInfo>> users_;
};
