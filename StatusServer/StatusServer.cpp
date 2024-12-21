#include <iostream>
#include "ConfigMgr.h"
#include "StatusServiceImpl.h"
#include <boost/asio.hpp>
#include <csignal>
#include <thread>
#include <memory>

void RunServer() {
    // 构建grpc服务器
    auto& config = ConfigMgr::GetInstance();
    auto server_address(config["StatusServer"]["Host"] + ":" + config["StatusServer"]["Port"]);
    StatusServiceImpl service;
    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);

    auto server = builder.BuildAndStart();
    std::cout << "Status Server listening on " << server_address << std::endl;

    // 构建asio服务器
    boost::asio::io_context io_context;
    boost::asio::signal_set signals(io_context, SIGINT, SIGTERM);
    signals.async_wait([&server, &io_context](auto& error, auto) {
        if (!error) {
            server->Shutdown();
            io_context.stop();
        }
        });

    std::thread([&io_context]() { io_context.run(); }).detach();

    server->Wait();
}

int main() {
    try {
        RunServer();
    }
    catch (std::exception const& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}

