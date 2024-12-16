#pragma once
#include <sw/redis++/redis++.h>
#include <memory>

using namespace sw::redis;

class RedisMgr
{
public:
    static RedisMgr& GetInstance();
    Redis& GetRedis();

    bool Connect(const std::string& host, int port, size_t pool_size, const std::string& password);
    bool Get(const std::string& key, std::string& value);
    bool Set(const std::string& key, const std::string& value);
    bool LPush(const std::string& key, const std::string& value);
    bool LPop(const std::string& key, std::string& value);
    bool RPush(const std::string& key, const std::string& value);
    bool RPop(const std::string& key, std::string& value);
    bool Del(const std::string& key);
    bool HSet(const std::string& key, const std::string& field, const std::string& value);
    std::string HGet(const std::string& key, const std::string& field);
    bool ExistsKey(const std::string& key);
    const std::string& GetLastError() const;

private:
    RedisMgr();
    RedisMgr(const RedisMgr&) = delete;
    RedisMgr& operator=(const RedisMgr&) = delete;

    std::unique_ptr<Redis> redis_;
    static thread_local std::string last_error_;
};

