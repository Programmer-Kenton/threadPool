/**
 * @Description 线程池类
 * @Version 1.0.0
 * @Date 2024/8/27 11:23
 * @Github https://github.com/Programmer-Kenton
 * @Author Kenton
 */
#ifndef THREADPOOL_THREADPOOL_H
#define THREADPOOL_THREADPOOL_H

#include <vector>
#include <queue>
#include <memory>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <thread>
#include <iostream>
#include <utility>
#include <unordered_map>
#include "Task.h"

// 初始默认任务队列最大上限阈值
const int TASK_MAX_THREADHOLD = INT32_MAX;

// 初始默认线程队列最大上限阈值
const int THREAD_MAX_THREADHOLD = 1024;

// 线程最大空闲时间 单位是秒
const int THREAD_MAX_IDLE_TIME = 60;

// 线程池支持的模式 强类型枚举
enum class PoolMode {
    // 固定数量的线程
    MODE_FIXED,
    // 线程数量可动态增长
    MODE_CACHED,
};

// 线程类型
class Thread {

public:
    // 线程函数对象类型
    using ThreadFunc = std::function<void(int)>;

    // 线程构造
    Thread(ThreadFunc func);

    // 线程析构
    ~Thread();

    // 启动线程
    void start();

    // 获取线程id
    int getId() const;

private:
    ThreadFunc _func;

    static int _generateId;

    // 线程id
    int _threadId;
};




// 线程池类型
class ThreadPool {
public:

    // 线程池构造
    ThreadPool();

    // 线程池析构
    ~ThreadPool();

    // 开启线程池 同时设置初始的线程数量
    void start(int initThreadSize = std::thread::hardware_concurrency());

    // 设置线程池的工作模式
    void setMode(PoolMode mode);

    // 设置task任务队列上限阈值
    void setTaskQueMaxThreadHold(int threadHold);

    // 给线程池提交任务 用户调用该接口 传入任务对象 生产任务
    Result submitTask(std::shared_ptr<Task> sp);

    ThreadPool(const ThreadPool&) = delete;

    ThreadPool &operator=(const ThreadPool&) = delete;

    // 设置cached模式下线程阈值
    void setThreadSizeHold(int threadHold);

private:
    // 线程列表
    // std::vector<std::unique_ptr<Thread>> _threads;
    // 无须的键值对表 键为线程id 值为线程对象智能指针
    std::unordered_map<int,std::unique_ptr<Thread>> _threads;

    // 初始的线程数量
    size_t _initThreadSize;

    // 记录当前线程池里线程的总数量
    std::atomic_int _curThreadSize;

    // 线程的上限阈值
    int _threadSizeHold;

    // 空闲线程数量
    std::atomic_int _idleThreadSize;

    // 任务队列
    std::queue<std::shared_ptr<Task>> _taskQue;

    // 任务数量
    std::atomic_int _taskSize;

    // 任务队列的上限阈值
    int _taskQueMaxThreadHold;

    // 保证任务队列的线程安全
    std::mutex _taskQueMtx;

    // 任务队列不满
    std::condition_variable _notFull;

    // 任务队列不空
    std::condition_variable _notEmpty;

    // 等待线程资源全部回收
    std::condition_variable _exitCond;

    // 当前线程池的工作模式
    PoolMode _poolMode;

    // 定义线程函数 执行任务由线程池决定
    // 线程池的所有线程从任务队列中消费任务
    void threadFunc(int threadId);

    // 当前线程池的启动状态
    std::atomic_bool _isPoolRunning;

    // 检查线程池的运行状态
    bool checkRunningState() const;
};


#endif //THREADPOOL_THREADPOOL_H
