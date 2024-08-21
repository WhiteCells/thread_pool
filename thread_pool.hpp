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
#include <iostream>

class ThreadPool {
    using Task = std::function<void()>;
public:
    static ThreadPool &getInstance();
    template<typename Func, typename ...Args>
    auto addTask(Func &&func, Args &&...)
        -> std::future<std::result_of_t<Func(Args ...)>>;

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

ThreadPool &ThreadPool::getInstance() {
    static ThreadPool pool;
    return pool;
}

template<typename Func, typename ...Args>
auto ThreadPool::addTask(Func &&func, Args &&...args)
-> std::future<std::result_of_t<Func(Args...)>> {
    // Func return type
    using FuncReturnType = std::result_of_t<Func(Args...)>;
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

ThreadPool::ThreadPool(std::size_t thread_num)
    : stop_(false) {
    for (std::size_t i = 0; i < thread_num; ++i) {
        threads_.emplace_back([this]() {
            // thread does not stop until pool needs to be destroyed
            // and the task queue is empty
            while (true) {
                std::unique_lock<std::mutex> ulock(task_mtx_);
                task_cv_.wait(ulock, [this]() {
                    return stop_ || !tasks_.empty();
                });
                if (stop_ && tasks_.empty()) {
                    return;
                }
                Task task = std::move(tasks_.front());
                tasks_.pop();
                ulock.unlock();
                task();
            }
        });
    }
}

ThreadPool::~ThreadPool() {
    std::cout << __func__ << std::endl;
    {
        std::lock_guard<std::mutex> lock(task_mtx_);
        stop_.store(true);
    }
    task_cv_.notify_all();
    for (auto &t : threads_) {
        t.join();
    }
}

#endif // _THREAD_POOL_H_