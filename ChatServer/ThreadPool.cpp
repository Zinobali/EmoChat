#include "ThreadPool.h"

ThreadPool::~ThreadPool() {
    stop();
}

std::size_t ThreadPool::IdleThreadNum() {
    return thread_num_;
}

ThreadPool::ThreadPool(std::size_t num)
    :stop_(false) {
    if (num <= 1) {
        thread_num_ = 2;
    } else {
        thread_num_ = num;
    }
    start();
}

void ThreadPool::start() {
    for (std::size_t i = 0; i < thread_num_; ++i) {
        pool_.emplace_back([this]() {
            while (true) {
                Task task;
                {
                    // 使用局部作用域释放锁
                    std::unique_lock<std::mutex> lock(cond_mutex_);
                    cond_.wait(lock, [this]() {
                        return !tasks_.empty() || stop_.load();
                        });

                    // 如果停止，并且任务队列为空，则退出循环
                    if (stop_.load() && tasks_.empty()) {
                        return;
                    }

                    // 从任务队列中取出一个任务
                    task = std::move(tasks_.front());
                    tasks_.pop();
                }
                if (task.valid()) {
                    // 执行任务
                    thread_num_--;
                    task();
                    thread_num_++;
                }
            }
            });
    }
}

void ThreadPool::stop() {
    stop_.store(true);
    cond_.notify_all();
    for (auto& thread : pool_) {
        if (thread.joinable()) {
            std::cout << "join thread: " << thread.get_id() << std::endl;
            thread.join();
        }
    }
}
