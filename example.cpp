#include "src/thread_pool.h"
#include <iostream>

int main(int argc, char *argv[]) {
    std::vector<std::future<int>> results;
    ThreadPool &pool = ThreadPool::getInstance();
    for (int i = 0; i < 20; ++i) {
        results.emplace_back(pool.addTask([](int num) -> int {
            std::cout << "thread: " << num << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            return num * num;
        }, i));
    }
    for (auto &res : results) {
        std::cout << "ret: " << res.get() << std::endl;
    }
    return 0;
}