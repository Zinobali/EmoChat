#pragma once
#include <string>
#include <memory>
#include <mysqlx/xdevapi.h>
#include <atomic>
#include <iostream>
#include "Singleton.h"
#include "threadsafe_queue.h"

using namespace mysqlx;

struct UserInfo {
    int uid = 0;
    int sex = 0;
    std::string name = "";
    std::string email = "";
    std::string hashed = "";
    std::string salt = "";
    std::string nick = "";
    std::string desc = "";
    std::string icon = "";
    std::string back = "";
};

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
    std::shared_ptr<UserInfo> GetUser(int uid);

private:
    std::string GenerateSalt(); // …˙≥…—Œ
    std::string HashPassword(const std::string& pwd, const std::string& salt); // π˛œ£√‹¬Î
    bool VerifyPassword(const std::string& pwd, const std::string& salt, const std::string& hash); // —È÷§√‹¬Î

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
    std::shared_ptr<UserInfo> GetUser(int uid);

private:
    MySQLMgr() = default;

    MySQLDao dao_;
};

