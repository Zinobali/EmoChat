#include "RedisMgr.h"
#include "ConfigMgr.h"

thread_local std::string RedisMgr::last_error_;

RedisMgr& RedisMgr::GetInstance() {
    static RedisMgr instance;
    return instance;
}

Redis& RedisMgr::GetRedis() {
    if (!redis_) {
        throw std::runtime_error("Redis instance not initialized.");
    }
    return *redis_;
}

bool RedisMgr::Connect(const std::string& host, int port, size_t pool_size, const std::string& password = "") {
    try {
        // 设置连接参数
        ConnectionOptions conn_opts;
        conn_opts.host = host;
        conn_opts.port = port;
        conn_opts.password = password;
        conn_opts.socket_timeout = std::chrono::seconds(5);
        // 设置连接池参数
        ConnectionPoolOptions pool_opts;
        pool_opts.size = pool_size;  // 连接池的最大连接数
        pool_opts.wait_timeout = std::chrono::seconds(2);  // 等待连接的超时时间
        pool_opts.connection_lifetime = std::chrono::minutes(10);  // 连接的生命周期
        // initialize the pool
        redis_ = std::make_unique<Redis>(conn_opts, pool_opts);
        return true;
    } catch (const Error& err) {
        last_error_ = err.what();
        return false;
    }
}

bool RedisMgr::Get(const std::string& key, std::string& value) {
    try {
        auto result = redis_->get(key);
        if (!result) {
            return false;
        }
        value = *result;
        return true;
    } catch (const Error& err) {
        last_error_ = err.what();
        return false;
    }
}

bool RedisMgr::Set(const std::string& key, const std::string& value) {
    try {
        redis_->set(key, value);
        return true;
    } catch (const Error& err) {
        last_error_ = err.what();
        return false;
    }
}

bool RedisMgr::LPush(const std::string& key, const std::string& value) {
    try {
        redis_->lpush(key, value);
        return true;
    } catch (const Error& err) {
        last_error_ = err.what();
        return false;
    }
}

bool RedisMgr::LPop(const std::string& key, std::string& value) {
    try {
        auto result = redis_->lpop(key);
        if (!result) {
            return false; // List is empty or key does not exist
        }
        value = *result;
        return true;
    } catch (const Error& err) {
        last_error_ = err.what();
        return false;
    }
}

bool RedisMgr::RPush(const std::string& key, const std::string& value) {
    try {
        redis_->rpush(key, value);
        return true;
    } catch (const Error& err) {
        last_error_ = err.what();
        return false;
    }
}

bool RedisMgr::RPop(const std::string& key, std::string& value) {
    try {
        auto result = redis_->rpop(key);
        if (!result) {
            return false;
        }
        value = *result;
        return true;
    } catch (const Error& err) {
        last_error_ = err.what();
        return false;
    }
}

bool RedisMgr::Del(const std::string& key) {
    try {
        redis_->del(key);
        return true;
    } catch (const Error& err) {
        last_error_ = err.what();
        return false;
    }
}

bool RedisMgr::HSet(const std::string& key, const std::string& field, const std::string& value) {
    try {
        redis_->hset(key, field, value);
        return true;
    } catch (const Error& err) {
        last_error_ = err.what();
        return false;
    }
}

std::string RedisMgr::HGet(const std::string& key, const std::string& field) {
    try {
        auto result = redis_->hget(key, field);
        if (!result) {
            return "";
        }
        return *result;
    } catch (const Error& err) {
        last_error_ = err.what();
        return "";
    }
}

bool RedisMgr::ExistsKey(const std::string& key) {
    try {
        return redis_->exists(key) > 0;
    } catch (const Error& err) {
        last_error_ = err.what();
        return false;
    }
}

const std::string& RedisMgr::GetLastError() const {
    return last_error_;
}

bool RedisMgr::HDel(const std::string& key, const std::string& field) {
    try {
        redis_->hdel(key, field);
        return true;
    } catch (const Error& err) {
        last_error_ = err.what();
        return false;
    }
}

RedisMgr::RedisMgr() {
    auto& config_mgr = ConfigMgr::GetInstance();
    auto host = config_mgr["Redis"]["Host"];  // Redis 服务器地址
    auto port = atoi(config_mgr["Redis"]["Port"].c_str());        // Redis 端口
    auto password = config_mgr["Redis"]["Passwd"]; // Redis 密码（如果有）
    Connect(host, port, 5, password);
}
