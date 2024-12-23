#include <iostream>
#include "ConfigMgr.h"
#include "AsioIOServicePool.h"
#include <boost/asio.hpp>
#include <csignal>
#include "CServer.h"

void RunServer() {
    try {
        auto pool = AsioIOServicePool::GetInstance();
        boost::asio::io_context ioc;
        boost::asio::signal_set signals(ioc, SIGINT, SIGTERM);
        signals.async_wait([&ioc, pool](auto, auto) {
            ioc.stop();
            pool->Stop();
            });

        auto& config = ConfigMgr::GetInstance();
        auto port = config["SelfServer"]["Port"];
        CServer server(ioc, std::atoi(port.c_str()));
        ioc.run();
    }
    catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}

int main() {
    RunServer();
}
