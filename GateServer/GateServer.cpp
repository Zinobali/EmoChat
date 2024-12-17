#include <csignal>
#include "CServer.h"
#include "ConfigMgr.h"
#include "global.h"
#include <iostream>
//#include <mysqlx/xdevapi.h>

//void testMysql() {
//    try {
//        // 获取配置信息
//        auto& configMgr = ConfigMgr::GetInstance();
//        auto host = configMgr["MySQL"]["Host"];
//        auto port = atoi(configMgr["MySQL"]["XPort"].c_str());
//        auto user = configMgr["MySQL"]["User"];
//        auto pwd = configMgr["MySQL"]["Passwd"];
//
//        // 创建 MySQL 会话
//        mysqlx::Session session(host, port, user, pwd);
//        std::cout << "MySQL 连接成功!" << std::endl;
//
//        // 选择数据库 "emo_chat"
//        mysqlx::Schema schema = session.getSchema("emo_chat", true);
//
//        // 创建测试表 "test"，如果不存在
//        schema.createCollection("test", true);
//        std::cout << "测试表 'test' 已创建或已存在." << std::endl;
//
//        // 插入测试数据
//        mysqlx::Table testTable = schema.getTable("test");
//        testTable.insert("id", "name")
//            .values(1, "Test Name")
//            .execute();
//        std::cout << "已插入测试数据." << std::endl;
//
//        // 查询测试数据
//        mysqlx::RowResult result = testTable.select("id", "name").execute();
//        for (mysqlx::Row row : result) {
//            std::cout << "ID: " << row[0] << ", Name: " << row[1] << std::endl;
//        }
//    }
//    catch (const mysqlx::Error& err) {
//        std::cerr << "MySQL 错误: " << err.what() << std::endl;
//    }
//    catch (const std::exception& ex) {
//        std::cerr << "标准异常: " << ex.what() << std::endl;
//    }
//    catch (...) {
//        std::cerr << "未知错误发生." << std::endl;
//    }
//}

int main() {
    //testMysql();
    try {
        auto& configMgr = ConfigMgr::GetInstance();
        std::string gate_port_str = configMgr["GateServer"]["Port"];
        USHORT port = atoi(gate_port_str.c_str());
        net::io_context io_context;
        net::signal_set signals(io_context, SIGINT, SIGTERM);
        signals.async_wait([&io_context](auto error, auto) {
            if (error) {
                return;
            }
            io_context.stop();
            });
        std::make_shared<CServer>(io_context, port)->Start();

        io_context.run();

    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}


