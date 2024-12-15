#include "RedisMgr.h"
#include "ConfigMgr.h"

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

RedisMgr::RedisMgr() {
    try {
        auto& config_mgr = ConfigMgr::GetInstance();
        // 连接池配置
        ConnectionPoolOptions pool_opts;
        pool_opts.size = 5;  // 最大连接数
        // 连接配置
        ConnectionOptions conn_opts;
        conn_opts.host = config_mgr["Redis"]["Host"];  // Redis 服务器地址
        conn_opts.port = atoi(config_mgr["Redis"]["Port"].c_str());        // Redis 端口
        conn_opts.password = config_mgr["Redis"]["Passwd"]; // Redis 密码（如果有）
        // 创建 Redis 连接池
        redis_ = std::make_unique<Redis>(conn_opts, pool_opts);
        // 验证连接
        redis_->ping();
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to initialize Redis connection: " << e.what() << std::endl;
        throw;  // 继续抛出异常，让调用者处理
    }
}
