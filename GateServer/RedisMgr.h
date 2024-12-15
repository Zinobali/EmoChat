#pragma once
#include <sw/redis++/redis++.h>
#include <memory>

using namespace sw::redis;

class RedisMgr
{
public:
    static RedisMgr& GetInstance();
    Redis& GetRedis();

private:
    RedisMgr();
    RedisMgr(const RedisMgr&) = delete;
    RedisMgr& operator=(const RedisMgr&) = delete;

    std::unique_ptr<Redis> redis_;
};

