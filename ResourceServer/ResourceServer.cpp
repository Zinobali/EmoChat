#include <iostream>
#include "ConfigMgr.h"
#include <thread>
#include <mutex>
#include <csignal>
#include <condition_variable>
#include "AsioIOServicePool.h"
#include "CServer.h"

void RunServer() {
    auto& config = ConfigMgr::GetInstance();
    auto server_name = config["SelfServer"]["Name"];

    try {
        auto pool = AsioIOServicePool::GetInstance();

        boost::asio::io_context  io_context;
        boost::asio::signal_set signals(io_context, SIGINT, SIGTERM);
        signals.async_wait([&io_context, pool](auto, auto) {
            io_context.stop();
            pool->Stop();
            });
        auto port_str = config["SelfServer"]["Port"];
        CServer s(io_context, atoi(port_str.c_str()));
        io_context.run();

    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}

int main() {
    RunServer();
}

