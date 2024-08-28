/**
 * @Description TODO
 * @Version 1.0.0
 * @Date 2024/8/27 20:39
 * @Github https://github.com/Programmer-Kenton
 * @Author Kenton
 */
#include "../head/Semaphore.h"

void Semaphore::wait() {
    // 获取锁
    std::unique_lock<std::mutex> lock(_mtx);
    // 等待信号量有资源 若没有资源则阻塞当前线程
    _cond.wait(lock,[&]()->bool{
        return _resLimit > 0;
    });
    _resLimit--;
}

void Semaphore::post() {
    std::unique_lock<std::mutex> lock(_mtx);
    _resLimit++;
    _cond.notify_all();
}

Semaphore::Semaphore(int limit) : _resLimit(limit){

}
