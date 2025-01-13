#include "AsioIOServicePool.h"

AsioIOServicePool::IOService& AsioIOServicePool::GetIOService() {
    auto& ioService = ioServices_[next_IOService_];
    if (next_IOService_ == ioServices_.size()) {
        next_IOService_ = 0;
    }
    return ioService;
}

void AsioIOServicePool::Stop() {
    for (auto& w : works_) {
        w->get_io_context().stop(); // stop io_context
        w.reset(); // release work
    }
    for (auto& t : threads_) {
        t.join(); // join thread
    }
}

AsioIOServicePool::~AsioIOServicePool() {
    Stop();
}

AsioIOServicePool::AsioIOServicePool(std::size_t pool_size)
    :ioServices_(pool_size),
    works_(pool_size),
    next_IOService_(0) {

    for (std::size_t i = 0; i < pool_size; ++i) {
        works_[i] = std::make_unique<Work>(ioServices_[i]);
    }

    // Start all io_contexts.
    for (std::size_t i = 0; i < pool_size; ++i) {
        threads_.emplace_back([this, i]() {
            ioServices_[i].run();
            });
    }
}
