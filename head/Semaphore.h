/**
 * @Description 信号量类
 * @Version 1.0.0
 * @Date 2024/8/27 20:39
 * @Github https://github.com/Programmer-Kenton
 * @Author Kenton
 */
#ifndef THREADPOOL_SEMAPHORE_H
#define THREADPOOL_SEMAPHORE_H

#include <iostream>
#include <memory>
#include <mutex>
#include <condition_variable>

class Semaphore {
public:

    Semaphore(int limit = 0);

    ~Semaphore() = default;

    // 获取一个信号量资源
    void wait();

    // 增加一个信号量资源
    void post();

private:
    int _resLimit;
    std::mutex _mtx;
    std::condition_variable _cond;
};


#endif //THREADPOOL_SEMAPHORE_H
