#include "LogicSystem.h"
#include "CSession.h"
#include "global.h"
#include "StatusGrpcClient.h"
#include "RedisMgr.h"
#include "MySQLMgr.h"
#include "ConfigMgr.h"
#include "UserMgr.h"
#include <regex>
#include "ChatGrpcClient.h"

LogicNode::LogicNode(std::shared_ptr<CSession> session, std::shared_ptr<RecvNode> recv_node)
    : session_(session), recv_node_(recv_node) {}

LogicSystem::LogicSystem()
    : b_stop_(false) {
    RegisterHandlers();
    worker_thread_ = std::thread(&LogicSystem::DealMsg, this);
}

void LogicSystem::DealMsg() {
    while (true) {
        std::unique_lock<std::mutex> lock(que_mtx_);
        cv_.wait(lock, [this]() { return b_stop_ || !logic_que_.empty(); });

        if (b_stop_) {
            while (!logic_que_.empty()) {
                HandleMsg();
            }
            return;
        }

        HandleMsg();
    }
}

LogicSystem::~LogicSystem() {}

void LogicSystem::PostMsgToQue(std::shared_ptr<LogicNode> logic_node) {
    std::unique_lock<std::mutex> lock(que_mtx_);
    logic_que_.push(logic_node);
    if (logic_que_.size() == 1) {
        lock.unlock();
        cv_.notify_one();
    }
}

void LogicSystem::RegisterHandlers() {
    // 登录
    handlers_[MSG_IDS::MSG_CHAT_LOGIN] = [this](std::shared_ptr<CSession> session, const uint16_t& msg_id, const std::string& msg_data) {
        LoginHandler(session, msg_id, msg_data);
        };
    // 搜索好友
    handlers_[MSG_IDS::ID_SEARCH_USER_REQ] = [this](std::shared_ptr<CSession> session, const uint16_t& msg_id, const std::string& msg_data) {
        SearchUserHandler(session, msg_id, msg_data);
        };
    // 添加好友
    handlers_[MSG_IDS::ID_ADD_FRIEND_REQ] = [this](std::shared_ptr<CSession> session, const uint16_t& msg_id, const std::string& msg_data) {
        AddFriendApplyHandler(session, msg_id, msg_data);
        };
    // 认证好友
    handlers_[MSG_IDS::ID_AUTH_FRIEND_REQ] = [this](std::shared_ptr<CSession> session, const uint16_t& msg_id, const std::string& msg_data) {
        AuthFriendApplyHandler(session, msg_id, msg_data);
        };
}

void LogicSystem::LoginHandler(std::shared_ptr<CSession> session, const uint16_t& msg_id, const std::string& msg_data) {
    Json::Value root;
    Json::Reader reader;
    reader.parse(msg_data, root);

    auto uid = root["uid"].asInt();
    auto token = root["token"].asString();
    std::cout << "user login uid is: " << uid << ", user token  is: " << token << std::endl;

    Json::Value return_value;
    Defer defer([&return_value, this, session]() {
        auto return_str = return_value.toStyledString();
        session->Send(MSG_IDS::MSG_CHAT_LOGIN_RSP, return_str); });

    // 从redis获取token
    auto uid_str = std::to_string(uid);
    auto token_key = USERTOKENPREFIX + uid_str;
    std::string token_value = "";
    bool token_ok = RedisMgr::GetInstance().Get(token_key, token_value);
    if (!token_ok) {
        return_value["error"] = ErrorCodes::UidInvalid;
        return;
    }

    if (token != token_value) {
        return_value["error"] = ErrorCodes::TokenInvalid;
        return;
    }
    // 验证通过
    return_value["error"] = ErrorCodes::Success;

    // 从redis或mysql获取用户信息
    auto base_key = USER_BASE_INFO + uid_str;
    auto user_info = std::make_shared<UserInfo>();
    bool user_info_ok = GetLatestBaseInfo(base_key, uid, user_info);
    if (!user_info_ok) {
        return_value["error"] = ErrorCodes::UidInvalid;
        return;
    }

    return_value["uid"] = uid;
    return_value["name"] = user_info->name;
    return_value["email"] = user_info->email;
    return_value["nick"] = user_info->nick;
    return_value["desc"] = user_info->desc;
    return_value["sex"] = user_info->sex;
    return_value["icon"] = user_info->icon;


    // 从数据库获取申请列表
    std::vector<std::shared_ptr<ApplyInfo>> apply_list;
    bool apply_ok = GetFriendApplyInfo(uid, apply_list);
    if (apply_ok) {
        for (auto& apply : apply_list) {
            Json::Value apply_value;
            apply_value["name"] = apply->_name;
            apply_value["uid"] = apply->_uid;
            apply_value["icon"] = apply->_icon;
            apply_value["nick"] = apply->_nick;
            apply_value["sex"] = apply->_sex;
            apply_value["desc"] = apply->_desc;
            apply_value["status"] = apply->_status;
            return_value["apply_list"].append(apply_value);
        }
    }

    // 获取好友列表
    std::vector < std::shared_ptr <UserInfo>> friend_list;
    bool friend_ok = GetFriendList(uid, friend_list);
    if (friend_ok) {
        for (auto& friend_info : friend_list) {
            Json::Value friend_value;
            friend_value["name"] = friend_info->name;
            friend_value["uid"] = friend_info->uid;
            friend_value["icon"] = friend_info->icon;
            friend_value["nick"] = friend_info->nick;
            friend_value["sex"] = friend_info->sex;
            friend_value["desc"] = friend_info->desc;
            friend_value["back"] = friend_info->back;
            return_value["friend_list"].append(friend_value);
        }
    }

    // 增加登录数量
    auto server_name = ConfigMgr::GetInstance()["SelfServer"]["Name"];
    auto login_count_str = RedisMgr::GetInstance().HGet(LOGIN_COUNT, server_name);
    int count = 0;
    if (!login_count_str.empty()) {
        count = std::stoi(login_count_str);
    }

    count++;
    auto count_str = std::to_string(count);
    RedisMgr::GetInstance().HSet(LOGIN_COUNT, server_name, count_str);
    // 绑定session和uid
    session->SetUserId(uid);

    // 为用户设置登录ip server的名字
    std::string ip_key = USERIPPREFIX + uid_str;
    RedisMgr::GetInstance().Set(ip_key, server_name);
    // uid和session绑定管理,方便以后踢人操作
    UserMgr::GetInstance()->SetUserSession(uid, session);
}

void LogicSystem::HandleMsg() {
    auto node = logic_que_.front();
    std::cout << "logic node msg id is " << node->recv_node_->msg_id_ << std::endl;
    auto iter = handlers_.find(static_cast<MSG_IDS>(node->recv_node_->msg_id_));
    if (iter == handlers_.end()) {
        logic_que_.pop();
        std::cout << "msg id [" << node->recv_node_->msg_id_ << "] handler not found" << std::endl;
        return;
    }

    auto data = std::string(node->recv_node_->buffer_.cbegin(), node->recv_node_->buffer_.cend());
    iter->second(node->session_, node->recv_node_->msg_id_, data);
    logic_que_.pop();
}

bool LogicSystem::GetLatestBaseInfo(const std::string& base_key, int uid, std::shared_ptr<UserInfo>& userinfo) {
    // 从mysql获取最新信息
    std::shared_ptr<UserInfo> user = nullptr;
    user = MySQLMgr::GetInstance()->GetUser(uid);
    if (user == nullptr) {
        return false;
    }

    userinfo = user;
    // 写入redis缓存
    Json::Value redis_root;
    redis_root["uid"] = uid;
    redis_root["name"] = userinfo->name;
    redis_root["email"] = userinfo->email;
    redis_root["nick"] = userinfo->nick;
    redis_root["desc"] = userinfo->desc;
    redis_root["sex"] = userinfo->sex;
    redis_root["icon"] = userinfo->icon;
    RedisMgr::GetInstance().Set(base_key, redis_root.toStyledString());
    return true;
}

bool LogicSystem::GetBaseInfo(const std::string& base_key, int uid, std::shared_ptr<UserInfo>& userinfo) {
    // 尝试从redis获取用户信息
    std::string info_str = "";
    bool info_ok = RedisMgr::GetInstance().Get(base_key, info_str);
    if (info_ok) {
        Json::Value root;
        Json::Reader reader;
        reader.parse(info_str, root);

        userinfo->uid = root["uid"].asInt();
        userinfo->name = root["name"].asString();
        userinfo->email = root["email"].asString();
        userinfo->nick = root["nick"].asString();
        userinfo->desc = root["desc"].asString();
        userinfo->sex = root["sex"].asInt();
        userinfo->icon = root["icon"].asString();
        std::cout << "user login uid is  " << userinfo->uid << " name  is "
            << userinfo->name << " email is " << userinfo->email << std::endl;
    } else {
        // redis 没有，从mysql获取
        std::shared_ptr<UserInfo> user = nullptr;
        user = MySQLMgr::GetInstance()->GetUser(uid);
        if (user == nullptr) {
            return false;
        }

        userinfo = user;
        // 写入redis
        Json::Value redis_root;
        redis_root["uid"] = uid;
        redis_root["name"] = userinfo->name;
        redis_root["email"] = userinfo->email;
        redis_root["nick"] = userinfo->nick;
        redis_root["desc"] = userinfo->desc;
        redis_root["sex"] = userinfo->sex;
        redis_root["icon"] = userinfo->icon;
        RedisMgr::GetInstance().Set(base_key, redis_root.toStyledString());
    }

    return true;
}

void LogicSystem::SearchUserHandler(std::shared_ptr<CSession> session, const uint16_t& msg_id, const std::string& msg_data) {
    Json::Value root;
    Json::Reader reader;
    reader.parse(msg_data, root);
    auto uid_str = root["uid"].asString();
    std::cout << "search user uid is " << uid_str << std::endl;

    Json::Value return_value;
    Defer return_value_defer([&]() {
        session->Send(MSG_IDS::ID_SEARCH_USER_RSP, return_value.toStyledString());
        });

    bool is_number = IsPureDigit(uid_str);
    if (is_number) {
        GetUserByUid(uid_str, return_value);
    } else {
        GetUserByName(uid_str, return_value);
    }
}

bool LogicSystem::IsPureDigit(const std::string& str) {
    std::regex digit_regex("^[0-9]+$"); // 匹配纯数字
    return std::regex_match(str, digit_regex);
}

void LogicSystem::GetUserByUid(const std::string& str, Json::Value& return_value) {
    std::string base_key = USER_BASE_INFO + str;

    // 尝试从redis获取用户信息
    std::string info_str = "";
    bool redis_ok = RedisMgr::GetInstance().Get(base_key, info_str);
    if (redis_ok) {
        Json::Value root;
        Json::Reader reader;
        reader.parse(info_str, root);

        return_value["uid"] = root["uid"].asInt();
        return_value["name"] = root["name"].asString();
        return_value["email"] = root["email"].asString();
        return_value["nick"] = root["nick"].asString();
        return_value["desc"] = root["desc"].asString();
        return_value["sex"] = root["sex"].asInt();
        return_value["icon"] = root["icon"].asString();
        return_value["error"] = ErrorCodes::Success;
        return;
    }

    // redis 没有，从mysql获取
    std::shared_ptr<UserInfo> user_info = nullptr;
    user_info = MySQLMgr::GetInstance()->GetUser(std::stoi(str));
    if (user_info == nullptr) {
        return_value["error"] = ErrorCodes::UidInvalid;
        return;
    }

    // 写入redis
    Json::Value redis_root;
    redis_root["uid"] = user_info->uid;
    redis_root["name"] = user_info->name;
    redis_root["email"] = user_info->email;
    redis_root["nick"] = user_info->nick;
    redis_root["desc"] = user_info->desc;
    redis_root["sex"] = user_info->sex;
    redis_root["icon"] = user_info->icon;
    RedisMgr::GetInstance().Set(base_key, redis_root.toStyledString());

    // 返回给客户端
    return_value = redis_root;
    return_value["error"] = ErrorCodes::Success;
    return;
}

void LogicSystem::GetUserByName(const std::string& str, Json::Value& return_value) {
    std::string base_key = NAME_INFO + str;

    // 尝试从redis获取用户信息
    std::string info_str = "";
    bool redis_ok = RedisMgr::GetInstance().Get(base_key, info_str);
    if (redis_ok) {
        Json::Value root;
        Json::Reader reader;
        reader.parse(info_str, root);

        return_value["uid"] = root["uid"].asInt();
        return_value["name"] = root["name"].asString();
        return_value["email"] = root["email"].asString();
        return_value["nick"] = root["nick"].asString();
        return_value["desc"] = root["desc"].asString();
        return_value["sex"] = root["sex"].asInt();
        return_value["icon"] = root["icon"].asString();
        return_value["error"] = ErrorCodes::Success;
        return;
    }

    // redis 没有，从mysql获取
    std::shared_ptr<UserInfo> user_info = nullptr;
    user_info = MySQLMgr::GetInstance()->GetUser(str);
    if (user_info == nullptr) {
        return_value["error"] = ErrorCodes::UidInvalid;
        return;
    }

    // 写入redis
    Json::Value redis_root;
    redis_root["uid"] = user_info->uid;
    redis_root["name"] = user_info->name;
    redis_root["email"] = user_info->email;
    redis_root["nick"] = user_info->nick;
    redis_root["desc"] = user_info->desc;
    redis_root["sex"] = user_info->sex;
    redis_root["icon"] = user_info->icon;
    RedisMgr::GetInstance().Set(base_key, redis_root.toStyledString());

    // 返回给客户端
    return_value = redis_root;
    return_value["error"] = ErrorCodes::Success;
    return;
}

void LogicSystem::AddFriendApplyHandler(std::shared_ptr<CSession> session, const uint16_t& msg_id, const std::string& msg_data) {
    Json::Reader reader;
    Json::Value root;
    reader.parse(msg_data, root);
    auto uid = root["uid"].asInt();
    auto apply_name = root["applyname"].asString();
    auto back_name = root["bakname"].asString();
    auto to_uid = root["touid"].asInt();
    std::cout << "user login uid is  " << uid << " applyname  is " << apply_name << " bakname is " << back_name << " touid is " << to_uid << std::endl;

    Json::Value return_value;
    return_value["error"] = ErrorCodes::Success;
    Defer return_value_defer([&]() {
        session->Send(MSG_IDS::ID_ADD_FRIEND_RSP, return_value.toStyledString());
        });

    // 先更新数据库
    MySQLMgr::GetInstance()->AddFriendApply(uid, to_uid);

    // 查询redis，找到对方的server ip
    auto to_ip_key = USERIPPREFIX + std::to_string(to_uid);
    auto to_ip_value = std::string();
    bool ip_ok = RedisMgr::GetInstance().Get(to_ip_key, to_ip_value);
    if (!ip_ok) {
        return_value["error"] = ErrorCodes::UidInvalid;
        return;
    }

    auto& config = ConfigMgr::GetInstance();
    auto self_server_name = config["SelfServer"]["Name"];

    // 查自己信息
    auto base_key = USER_BASE_INFO + std::to_string(uid);
    auto apply_info = std::make_shared<UserInfo>();
    bool info_ok = GetBaseInfo(base_key, uid, apply_info);

    // 判断是否在同一个服务器
    if (to_ip_value == self_server_name) {
        // 获取对方session
        auto session = UserMgr::GetInstance()->GetSession(to_uid);
        if (session) {
            Json::Value notify_value;
            notify_value["error"] = ErrorCodes::Success;
            notify_value["applyuid"] = to_uid;
            notify_value["name"] = apply_name;
            notify_value["desc"] = "";
            // 添加通知对方的基本信息
            if (info_ok) {
                notify_value["icon"] = apply_info->icon;
                notify_value["sex"] = apply_info->sex;
                notify_value["nick"] = apply_info->nick;
            }
            session->Send(MSG_IDS::ID_NOTIFY_ADD_FRIEND_REQ, notify_value.toStyledString());
        }
        return;
    }

    // 跨服务器通知
    AddFriendReq add_request;
    add_request.set_apply_uid(uid);
    add_request.set_to_uid(to_uid);
    add_request.set_name(apply_name);
    add_request.set_description("");
    if (info_ok) {
        add_request.set_icon(apply_info->icon);
        add_request.set_sex(apply_info->sex);
        add_request.set_nickname(apply_info->nick);
    }

    ChatGrpcClient::GetInstance()->NotifyAddFriend(to_ip_value, add_request);
}

bool LogicSystem::GetFriendApplyInfo(int to_uid, std::vector<std::shared_ptr<ApplyInfo>>& apply_list) {
    // 获取数据库中的申请信息
    return MySQLMgr::GetInstance()->GetFriendApplyInfo(to_uid, apply_list, 0, 10);
}

void LogicSystem::AuthFriendApplyHandler(std::shared_ptr<CSession> session, const uint16_t& msg_id, const std::string& msg_data) {
    Json::Reader reader;
    Json::Value root;
    reader.parse(msg_data, root);

    auto uid = root["fromuid"].asInt();
    auto touid = root["touid"].asInt();
    auto back_name = root["back"].asString();
    std::cout << "from " << uid << " auth friend to " << touid << std::endl;

    Json::Value return_value;
    return_value["error"] = ErrorCodes::Success;
    Defer return_value_defer([&]() {
        session->Send(MSG_IDS::ID_AUTH_FRIEND_RSP, return_value.toStyledString());
        });

    std::string base_key = USER_BASE_INFO + std::to_string(touid);
    auto user_info = std::make_shared<UserInfo>();
    bool b_info = GetBaseInfo(base_key, touid, user_info);
    if (!b_info) {
        return_value["error"] = ErrorCodes::UidInvalid;
        return;
    }

    return_value["name"] = user_info->name;
    return_value["nick"] = user_info->nick;
    return_value["icon"] = user_info->icon;
    return_value["sex"] = user_info->sex;
    return_value["uid"] = touid;

    // 更新数据库
    MySQLMgr::GetInstance()->AuthFriendApply(uid, touid, back_name);

    // 查redis, 找到对方的server ip
    auto to_str = std::to_string(touid);
    auto to_ip_key = USERIPPREFIX + to_str;
    auto to_ip_value = std::string();
    bool ip_ok = RedisMgr::GetInstance().Get(to_ip_key, to_ip_value);
    if (!ip_ok) {
        return_value["error"] = ErrorCodes::UidInvalid;
        return;
    }

    auto& config = ConfigMgr::GetInstance();
    auto self_server_name = config["SelfServer"]["Name"];
    // 判断是否在同一个服务器
    if (to_ip_value == self_server_name) {
        // 获取对方session
        auto session = UserMgr::GetInstance()->GetSession(touid);
        if (session) {
            Json::Value notify_value;
            notify_value["error"] = ErrorCodes::Success;
            notify_value["fromuid"] = uid;
            notify_value["touid"] = touid;
            std::string base_key = USER_BASE_INFO + std::to_string(uid);
            auto user_info = std::make_shared<UserInfo>();
            bool b_info = GetBaseInfo(base_key, uid, user_info);
            if (b_info) {
                notify_value["name"] = user_info->name;
                notify_value["nick"] = user_info->nick;
                notify_value["icon"] = user_info->icon;
                notify_value["sex"] = user_info->sex;
            } else {
                notify_value["error"] = ErrorCodes::UidInvalid;
            }

            session->Send(MSG_IDS::ID_NOTIFY_AUTH_FRIEND_REQ, notify_value.toStyledString());
        }
        return;
    }

    AuthFriendReq auth_request;
    auth_request.set_from_uid(uid);
    auth_request.set_to_uid(touid);

    ChatGrpcClient::GetInstance()->NotifyAuthFriend(to_ip_value, auth_request);
}

bool LogicSystem::GetFriendList(int self_id, std::vector<std::shared_ptr<UserInfo>>& user_list) {
    return MySQLMgr::GetInstance()->GetFriendList(self_id, user_list);
}
