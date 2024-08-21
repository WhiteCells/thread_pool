#include "thread_pool.h"

ThreadPool &ThreadPool::getInstance() {
    static ThreadPool pool;
    return pool;
}

inline ThreadPool::ThreadPool(std::size_t thread_num)
    : stop_(false) {
    for (std::size_t i = 0; i < thread_num; ++i) {
        threads_.emplace_back([this]() {
            // thread does not stop until pool needs to be destroyed
            // and the task queue is empty
            while (true) {
                Task task;
                {
                    std::unique_lock<std::mutex> ulock(task_mtx_);
                    task_cv_.wait(ulock, [this]() {
                        return stop_.load() || !tasks_.empty();
                    });
                    if (stop_ && tasks_.empty()) {
                        return;
                    }
                    task = std::move(tasks_.front());
                    tasks_.pop();
                }
                task();
            }
        });
    }
}

inline ThreadPool::~ThreadPool() {
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