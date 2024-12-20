#include "MySQLMgr.h"
#include "ConfigMgr.h"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/functional/hash.hpp>
#include "global.h"

MySQLConnectionPool::MySQLConnectionPool(const std::string& host, const unsigned int port, const std::string& user, const std::string& pwd, size_t pool_size)
    :pool_size_(pool_size), b_stop_(false),
    settings_(SessionOption::HOST, host,
        SessionOption::PORT, port,
        SessionOption::USER, user,
        SessionOption::PWD, pwd
    ) {
    for (size_t i = 0; i < pool_size_; i++) {
        pool_.push(mysqlx::Session(settings_));
    }
}

MySQLConnectionPool::~MySQLConnectionPool() {
    Close();
}

std::shared_ptr<mysqlx::Session> MySQLConnectionPool::getConnection() {
    if (b_stop_) return nullptr;
    auto conn = pool_.pop();
    if (!conn) return nullptr;
    return conn;
}

void MySQLConnectionPool::releaseConnection(std::shared_ptr<mysqlx::Session>& conn) {
    if (b_stop_) return; // 如果池已停止，则不返回连接
    pool_.push(conn);
}

void MySQLConnectionPool::Close() {
    b_stop_ = true;
}

mysqlx::Session MySQLConnectionPool::createConnection() {
    try {
        return Session(settings_);
    }
    catch (const mysqlx::Error& err) {
        std::cerr << "Error creating MySQL connection: " << err.what() << std::endl;
        throw std::runtime_error("Failed to create MySQL connection");
    }
}

MySQLDao::MySQLDao() {
    auto& configMgr = ConfigMgr::GetInstance();
    auto host = configMgr["MySQL"]["Host"];
    auto port = atoi(configMgr["MySQL"]["XPort"].c_str());
    auto user = configMgr["MySQL"]["User"];
    auto pwd = configMgr["MySQL"]["Passwd"];
    schema_ = configMgr["MySQL"]["Schema"];
    pool_ = std::make_unique<MySQLConnectionPool>(host, port, user, pwd, 5);
}

int MySQLDao::RegisterUser(const std::string& name, const std::string& email, const std::string& pwd) {
    auto conn = pool_->getConnection();
    if (!conn) {
        std::cerr << "Failed to get a database connection." << std::endl;
        return 0;
    }
    Defer defer([&]() { pool_->releaseConnection(conn); });
    try {
        // 生成盐值
        auto salt = GenerateSalt();
        std::cout << "生成的盐值为: " << salt << std::endl;
        // 加密密码
        auto h_pwd = HashPassword(pwd, salt);
        std::cout << "加密后的密码为: " << h_pwd << std::endl;
        // 选择数据库
        conn->sql("USE " + schema_).execute();
        // 调用存储过程
        auto result = conn->sql("CALL reg_user(?, ?, ?, ?, @result)").bind(name, email, h_pwd, salt).execute();
        // 获取存储过程返回值
        auto out_result = conn->sql("SELECT @result").execute().fetchOne();
        if (!out_result) {
            return -1;
        }
        int result_value = out_result[0];
        std::cout << "Result: " << result_value << std::endl;
        return result_value;
    }
    catch (const mysqlx::Error& err) {
        std::cerr << "Error executing query: " << err.what() << std::endl;
        return -1;
    }
}

int MySQLMgr::RegUser(const std::string& name, const std::string& email, const std::string& pwd) {
    return dao_.RegisterUser(name, email, pwd);
}

bool MySQLMgr::CheckEmail(const std::string& name, const std::string& email) {
    return dao_.CheckEmail(name, email);
}

bool MySQLMgr::UpdatePwd(const std::string& email, const std::string& pwd) {
    return dao_.UpdatePwd(email, pwd);
}

bool MySQLMgr::EmailExist(const std::string& email) {
    return dao_.EmailExist(email);
}

bool MySQLMgr::NameExist(const std::string& name) {
    return dao_.NameExist(name);
}

std::string MySQLDao::GenerateSalt() {
    boost::uuids::random_generator generator;
    boost::uuids::uuid salt = generator();
    return boost::uuids::to_string(salt);
}

std::string MySQLDao::HashPassword(const std::string& pwd, const std::string& salt) {
    boost::hash<std::string> hasher;
    size_t value = hasher(pwd + salt); // 将盐值和密码组合后哈希
    return std::to_string(value);
}

bool MySQLDao::VerifyPassword(const std::string& pwd, const std::string& salt, const std::string& hash) {
    return HashPassword(pwd, salt) == hash;
}

bool MySQLDao::CheckEmail(const std::string& name, const std::string& email) {
    //todo
    return false;
}

bool MySQLDao::UpdatePwd(const std::string& email, const std::string& pwd) {
    auto conn = pool_->getConnection();
    if (!conn) {
        std::cerr << "Failed to get a database connection." << std::endl;
        return false;
    }
    Defer defer([&]() { pool_->releaseConnection(conn); });
    try {
        // 生成盐值和哈希密码
        auto salt = GenerateSalt();
        auto h_pwd = HashPassword(pwd, salt);

        auto users = conn->getSchema(schema_).getTable("user");
        auto res = users.update().set("hashed_password", h_pwd).set("salt", salt).where("email = :param1").bind("param1", email).execute();
        return res.getAffectedItemsCount() > 0;
    }
    catch (const mysqlx::Error& err) {
        std::cerr << "Error executing query: " << err.what() << std::endl;
        return false;
    }
}

bool MySQLDao::EmailExist(const std::string& email) {
    auto conn = pool_->getConnection();
    if (!conn) {
        std::cerr << "Failed to get a database connection." << std::endl;
        return false;
    }
    Defer defer([&]() { pool_->releaseConnection(conn); });
    try {
        auto users = conn->getSchema(schema_).getTable("user");
        auto res = users.select("email").where("email = :param1").bind("param1", email).execute().fetchOne();
        return !res.isNull();
    }
    catch (const mysqlx::Error& err) {
        std::cerr << "Error executing query: " << err.what() << std::endl;
        return false;
    }
}

bool MySQLDao::NameExist(const std::string& name) {
    //todo
    return false;
}
