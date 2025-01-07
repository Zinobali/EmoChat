#include <iostream>
#include "ConfigMgr.h"
#include "AsioIOServicePool.h"
#include <boost/asio.hpp>
#include <csignal>
#include "CServer.h"
#include "RedisMgr.h"
#include "ChatServiceImpl.h"

void RunServer() {
    auto& config = ConfigMgr::GetInstance();
    auto server_name = config["SelfServer"]["Name"];
    RedisMgr::GetInstance().HSet(LOGIN_COUNT, server_name, "0");

    // 启动grpc服务器
    auto grpc_server_host = config["SelfServer"]["Host"];
    auto grpc_server_port = config["SelfServer"]["Port"];
    auto grpc_server_address = grpc_server_host + ":" + grpc_server_port;
    ChatServiceImpl service;
    ServerBuilder builder;
    builder.AddListeningPort(grpc_server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    auto grpc_server = builder.BuildAndStart();
    std::cout << "RPC Server listening on " << grpc_server_address << std::endl;
    // 跑在单线程
    std::thread grpc_server_thread([&grpc_server]() {
        grpc_server->Wait();
        });

    try {
        auto pool = AsioIOServicePool::GetInstance();
        boost::asio::io_context ioc;
        boost::asio::signal_set signals(ioc, SIGINT, SIGTERM);
        signals.async_wait([&ioc, pool, &grpc_server](auto, auto) {
            ioc.stop();
            pool->Stop();
            grpc_server->Shutdown();
            });

        auto port = config["SelfServer"]["Port"];
        CServer server(ioc, std::atoi(port.c_str()));
        ioc.run();
        // 清除redis中保存的登录状态
        RedisMgr::GetInstance().HDel(LOGIN_COUNT, server_name);
        grpc_server_thread.join();
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        RedisMgr::GetInstance().HDel(LOGIN_COUNT, server_name);
    }
}

int main() {
    RunServer();
}
