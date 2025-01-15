#pragma once
#include "Singleton.h"
#include <functional>
#include <thread>
#include <future>
#include <queue>
#include <atomic>
#include <iostream>

class ThreadPool : public Singleton<ThreadPool>
{
    using Task = std::packaged_task<void()>;
    friend class Singleton<ThreadPool>;
public:
    ~ThreadPool();
    std::size_t IdleThreadNum();

    template<typename F, typename... Args>
    auto Commit(F&& func, Args&&... args)
        -> std::future<decltype(std::forward<F>(func)(std::forward<Args>(args)...))> {
        using RetType = decltype(std::forward<F>(func)(std::forward<Args>(args)...));

        if (stop_.load()) {
            return std::future<RetType>();
        }

        using TaskWrapper = std::packaged_task<RetType()>;
        auto task = std::make_shared<TaskWrapper>(
            std::bind(std::forward<F>(func), std::forward<Args>(args)...)
        );

        std::future<RetType> ret = task->get_future();
        {
            std::lock_guard<std::mutex> lock(cond_mutex_);
            tasks_.emplace([task]() {
                (*task)();
                });
        }
        cond_.notify_one();
        return ret;
    }

private:
    ThreadPool(std::size_t num = std::thread::hardware_concurrency());
    void start();
    void stop();

    std::atomic_size_t thread_num_;
    std::atomic_bool stop_;

    std::queue<Task> tasks_;
    std::vector<std::thread> pool_;
    std::mutex cond_mutex_;
    std::condition_variable cond_;
};

