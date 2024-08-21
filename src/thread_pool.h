#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_

#include <thread>
#include <queue>
#include <future>
#include <atomic>
#include <mutex>
#include <vector>
#include <functional>
#include <condition_variable>
#include <type_traits>
#include <iostream>

class ThreadPool {
    using Task = std::function<void()>;
public:
    static ThreadPool &getInstance();
    template<typename Func, typename ...Args>
    auto addTask(Func &&func, Args &&...)
        -> std::future<std::invoke_result_t<Func, Args...>>;

private:
    explicit ThreadPool(std::size_t thread_num = std::thread::hardware_concurrency());
    ThreadPool(const ThreadPool &) = delete;
    ThreadPool &operator=(const ThreadPool &) = delete;
    ~ThreadPool();

    std::vector<std::thread> threads_;
    std::queue<Task> tasks_;
    std::mutex task_mtx_;
    std::condition_variable task_cv_;
    std::atomic_bool stop_;
};

template<typename Func, typename ...Args>
inline auto ThreadPool::addTask(Func &&func, Args &&...args)
-> std::future<std::invoke_result_t<Func, Args...>> {
    // Func return type
    using FuncReturnType = std::invoke_result_t<Func, Args...>;
    // addTask return type
    using TaskReturnType = std::future<FuncReturnType>;
    // packaged_task type
    using PackagedTaskType = std::packaged_task<FuncReturnType()>;

    auto ptask = std::make_shared<PackagedTaskType>(std::bind(func, args...));
    TaskReturnType res = ptask->get_future();
    {
        std::lock_guard<std::mutex> ulock(task_mtx_);
        if (stop_) {
            throw std::runtime_error("add task error");
        }
        tasks_.emplace([ptask]() {
            (*ptask)();
        });
    }
    task_cv_.notify_one();
    return res;
}


#endif // _THREAD_POOL_H_