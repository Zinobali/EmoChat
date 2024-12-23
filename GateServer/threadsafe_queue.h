#pragma once
#include <memory>
#include <mutex>
#include <condition_variable>
#include <atomic>


template <typename T>
class threadsafe_queue
{
private:
    struct node
    {
        std::shared_ptr<T> data;
        std::unique_ptr<node> next;
    };
    std::mutex head_mtx_;
    std::mutex tail_mtx_;

    std::unique_ptr<node> head_;
    node* tail_; // 使用裸指针，不适用unique_ptr避免头尾指向同一个节点，二次释放
    std::condition_variable cv_;
    std::atomic_bool b_stop;

    node* get_tail() {
        std::lock_guard<std::mutex> tail_lock(tail_mtx_);
        return tail_;
    }

    std::unique_ptr<node> pop_head() {
        std::unique_ptr<node> old_head = std::move(head_);
        head_ = std::move(old_head->next); // 更新head
        return old_head;
    }

    bool wait_for_data(std::unique_lock<std::mutex>& lock) {
        cv_.wait(lock, [&]() {
            return b_stop || head_.get() != get_tail(); // 使用get_tail，保证线程安全
            });
        return !b_stop; // 如果 b_stop 为 true，返回 false，表示线程需要退出
    }

    std::unique_ptr<node> wait_pop_head() {
        std::unique_lock<std::mutex> head_lock(head_mtx_);
        if (!wait_for_data(head_lock)) {
            return nullptr;
        }
        return pop_head();
    }

    std::unique_ptr<node> wait_pop_head(T& value) {
        std::unique_lock<std::mutex> head_lock(head_mtx_);
        if (!wait_for_data(head_lock)) {
            return nullptr;
        }
        value = std::move(*head_->data);
        return pop_head();
    }

    std::unique_ptr<node> try_pop_head() {
        std::lock_guard<std::mutex> head_lock(head_mtx_);
        if (head_.get() == get_tail())
            return nullptr;
        return pop_head();
    }

    std::unique_ptr<node> try_pop_head(T& value) {
        std::lock_guard<std::mutex> head_lock(head_mtx_);
        if (head_.get() == get_tail())
            return nullptr;
        value = std::move(*head_->data);
        return pop_head();
    }

public:
    threadsafe_queue() : head_(new node), tail_(head_.get()) {}
    threadsafe_queue(const threadsafe_queue& other) = delete;
    threadsafe_queue& operator=(const threadsafe_queue& other) = delete;
    ~threadsafe_queue() { Stop(); }

    void Stop() {
        b_stop = true;
        cv_.notify_all();
        std::cout << "~threadsafe_queue()" << std::endl;
    }

    // 有可能返回空指针，请检查
    std::shared_ptr<T> pop() {
        const std::unique_ptr<node> old_head = wait_pop_head();
        return old_head->data;
    }

    void pop(T& value) {
        wait_pop_head(value);
    }

    std::shared_ptr<T> try_pop() {
        const std::unique_ptr<node> old_head = try_pop_head();
        if (!old_head)
            return nullptr;
        return old_head->data;
    }

    bool try_pop(T& value) {
        const std::unique_ptr<node> old_head = try_pop_head(value);
        if (!old_head)
            return false;
        return true;
    }

    bool empty() {
        /*
        * 为何要使用defer_lock
        * 避免两个线程同时访问head_mtx_和tail_mtx_由于访问顺序不同，导致死锁
        */
        // 使用 std::unique_lock 延迟加锁
        std::unique_lock<std::mutex> head_lock(head_mtx_, std::defer_lock);
        std::unique_lock<std::mutex> tail_lock(tail_mtx_, std::defer_lock);

        // 使用 std::lock 同时加锁
        std::lock(head_lock, tail_lock);
        return (head_.get() == tail_);
    }

    void push(std::shared_ptr<T> new_value) {
        std::unique_ptr<node> p(new node);
        node* const new_tail = p.get();
        {
            std::lock_guard<std::mutex> tail_lock(tail_mtx_);
            tail_->data = std::move(new_value);
            tail_->next = std::move(p);
            tail_ = new_tail;
        }
        cv_.notify_one();
    }

    void push(T new_value) {
        push(std::make_shared<T>(std::move(new_value)));
    }
};