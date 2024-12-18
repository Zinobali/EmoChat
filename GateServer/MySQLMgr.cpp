#include "MySQLMgr.h"
#include "ConfigMgr.h"

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
    try {
        if (!conn) {
            return 0;
        }
        // 选择数据库
        conn->sql("USE " + schema_).execute();
        // 调用存储过程
        auto result = conn->sql("CALL reg_user(?, ?, ?, @result)").bind(name, email, pwd).execute();
        // 获取存储过程返回值
        auto out_result = conn->sql("SELECT @result").execute().fetchOne();
        if (!out_result) {
            pool_->releaseConnection(conn);
            return -1;
        }
        int result_value = out_result[0];
        std::cout << "Result: " << result_value << std::endl;
        pool_->releaseConnection(conn);
        return result_value;
    }
    catch (const mysqlx::Error& err) {
        pool_->releaseConnection(conn);
        std::cerr << "Error executing query: " << err.what() << std::endl;
        return -1;
    }
}

int MySQLMgr::RegUser(const std::string& name, const std::string& email, const std::string& pwd) {
    return dao_.RegisterUser(name, email, pwd);
}
