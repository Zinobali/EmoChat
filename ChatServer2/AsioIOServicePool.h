#pragma once
#include <boost/asio.hpp>
#include "Singleton.h"
#include <vector>

class AsioIOServicePool :public Singleton<AsioIOServicePool>
{
    friend class Singleton<AsioIOServicePool>;
public:
    using IOService = boost::asio::io_context;
    using Work = boost::asio::io_context::work;
    using WorkPtr = std::unique_ptr<Work>;

    IOService& GetIOService();
    void Stop();
    ~AsioIOServicePool();
private:
    //AsioIOServicePool(std::size_t pool_size = std::thread::hardware_concurrency());
    AsioIOServicePool(std::size_t pool_size = 2);

    std::vector<IOService> ioServices_;
    std::vector<WorkPtr> works_;
    std::vector<std::thread> threads_;
    std::size_t next_IOService_;
};

