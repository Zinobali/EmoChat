#pragma once
#include <memory>
#include <mutex>

template <typename T>
class Singleton
{
protected:
    Singleton() = default;
    Singleton(const Singleton<T>&) = delete;
    Singleton& operator=(const Singleton<T>&) = delete;

    static std::shared_ptr<T> instance_;

public:
    static std::shared_ptr<T> GetInstance() {
        static std::once_flag f;

        std::call_once(f, [&]() {
            instance_ = std::shared_ptr<T>(new T);
            });

        return instance_;
    }

    ~Singleton() = default;

};

template <typename T>
std::shared_ptr<T> Singleton<T>::instance_ = nullptr;