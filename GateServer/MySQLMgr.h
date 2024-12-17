#pragma once
#include <string>
#include <memory>
#include <mysqlx/xdevapi.h>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <iostream>
#include "Singleton.h"

using namespace mysqlx;

class MySQLConnectionPool {
public:
    MySQLConnectionPool(const std::string& host, const unsigned int port, const std::string& user, const std::string& pwd, size_t pool_size);
    ~MySQLConnectionPool();
    std::unique_ptr<mysqlx::Session> getConnection();
    void releaseConnection(std::unique_ptr<mysqlx::Session>& conn);
    void Close();

private:
    mysqlx::Session createConnection();

    SessionSettings settings_;
    size_t pool_size_;
    std::queue<std::unique_ptr<mysqlx::Session>> pool_;
    std::mutex mutex_;
    std::condition_variable cv_;
    std::atomic_bool b_stop_;
};

class MySQLDao
{
public:
    MySQLDao();

    int RegisterUser(const std::string& name, const std::string& email, const std::string& pwd);

private:
    std::unique_ptr<MySQLConnectionPool> pool_;
    std::string schema_;
};

class MySQLMgr : public Singleton<MySQLMgr>
{
    friend class Singleton<MySQLMgr>;
public:
    int RegUser(const std::string& name, const std::string& email, const std::string& pwd);

private:
    MySQLMgr() = default;
    MySQLDao dao_;
};

