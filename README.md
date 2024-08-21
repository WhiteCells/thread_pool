### C++14 Thread Pool

build:
```sh
cmake -B build
cmake --build build
./build/thread_pool
```

usage:

```cpp
auto &pool = ThreadPool::getInstance();
auto res = pool.addTask([](int num) -> int {
    return num + 10;
}, 20);
std::cout << res.get() << std::endl;
```