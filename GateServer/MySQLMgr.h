#pragma once
#include <string>
#include <memory>
#include <mysqlx/xdevapi.h>
#include <atomic>
#include <iostream>
#include "Singleton.h"
#include "threadsafe_queue.h"

using namespace mysqlx;

class MySQLConnectionPool {
public:
    MySQLConnectionPool(const std::string& host, const unsigned int port, const std::string& user, const std::string& pwd, size_t pool_size);
    ~MySQLConnectionPool();
    std::shared_ptr<mysqlx::Session> getConnection();
    void releaseConnection(std::shared_ptr<mysqlx::Session>& conn);
    void Close();

private:
    mysqlx::Session createConnection();

    SessionSettings settings_;
    size_t pool_size_;
    threadsafe_queue<mysqlx::Session> pool_;
    std::atomic_bool b_stop_;
};

struct UserInfo {
    int uid;
    std::string name;
    std::string email;
    std::string hashed;
    std::string salt;
};

class MySQLDao
{
public:
    MySQLDao();

    int RegisterUser(const std::string& name, const std::string& email, const std::string& pwd);
    bool CheckEmail(const std::string& name, const std::string& email);
    bool UpdatePwd(const std::string& email, const std::string& pwd);
    bool EmailExist(const std::string& email);
    bool NameExist(const std::string& name);
    bool CheckPwd(const std::string& email, const std::string& pwd, UserInfo& user_info);

private:
    std::string GenerateSalt(); // 生成盐
    std::string HashPassword(const std::string& pwd, const std::string& salt); // 哈希密码
    bool VerifyPassword(const std::string& pwd, const std::string& salt, const std::string& hash); // 验证密码
    int hashEmail(const std::string& email);
    int generateUid(const std::string& email); // 生成uid
    bool isUidExist(int uid);
    std::unique_ptr<MySQLConnectionPool> pool_;
    std::string schema_;

};

class MySQLMgr : public Singleton<MySQLMgr>
{
    friend class Singleton<MySQLMgr>;
public:
    int RegUser(const std::string& name, const std::string& email, const std::string& pwd);
    bool CheckEmail(const std::string& name, const std::string& email);
    bool UpdatePwd(const std::string& email, const std::string& pwd);
    bool EmailExist(const std::string& email);
    bool NameExist(const std::string& name);
    bool CheckPwd(const std::string& email, const std::string& pwd, UserInfo& user_info);

private:
    MySQLMgr() = default;

    MySQLDao dao_;
};

