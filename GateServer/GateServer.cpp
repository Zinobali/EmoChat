#include <csignal>
#include "CServer.h"
#include "ConfigMgr.h"
#include "global.h"
#include <iostream>

int main() {
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


