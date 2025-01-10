#pragma once
#include <string>
#include <memory>
#include <mysqlx/xdevapi.h>
#include <atomic>
#include <iostream>
#include "Singleton.h"
#include "threadsafe_queue.h"
#include "data.h"

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
    std::shared_ptr<UserInfo> GetUser(const std::string& name);
    bool AddFriendApply(int from_id, int to_id);
    bool GetFriendApplyInfo(int to_uid, std::vector<std::shared_ptr<ApplyInfo>>& apply_list, int offset, int limit);

private:
    std::string GenerateSalt(); // 生成盐
    std::string HashPassword(const std::string& pwd, const std::string& salt); // 哈希密码
    bool VerifyPassword(const std::string& pwd, const std::string& salt, const std::string& hash); // 验证密码
    /// <summary>
    ///     获取mysqlx::Value的值，如果为空，则返回默认值(可能有更好的方式，暂时不启用)
    /// </summary>
    /// <typeparam name="T"></typeparam>
    /// <param name="value"></param>
    /// <param name="default_value"></param>
    /// <returns></returns>
    template<typename T>
    T getValueOrDefault(const mysqlx::Value& value, const T& default_value) {
        return value.isNull() ? default_value : value.get<T>();
    }


private:
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
    std::shared_ptr<UserInfo> GetUser(const std::string& name);
    bool AddFriendApply(int from_id, int to_id);
    bool GetFriendApplyInfo(int to_uid, std::vector<std::shared_ptr<ApplyInfo>>& apply_list, int offset, int limit);

private:
    MySQLMgr() = default;

    MySQLDao dao_;
};

