/**
 * @Description 任务抽象基类
 * @Version 1.0.0
 * @Date 2024/8/27 20:47
 * @Github https://github.com/Programmer-Kenton
 * @Author Kenton
 */
#ifndef THREADPOOL_TASK_H
#define THREADPOOL_TASK_H

#include <iostream>
#include <memory>
#include "Any.h"
#include "Semaphore.h"

class Task;

// 实现接收提交到线程池的task任务执行完成后的返回值类型Result
class Result{
public:

    Result(std::shared_ptr<Task> task,bool isValid = true);

    ~Result() = default;

    // setVal方法 获取任务执行完的返回值
    void setVal(Any any);

    // get方法 用户调用这个方法获取task的返回值
    Any get();


private:
    // 存储任务的返回值
    Any _any;
    // 线程通信信号量
    Semaphore _sem;
    // 指向对应获取返回值的任务对象
    std::shared_ptr<Task> _task;
    // 返回值是否有效
    std::atomic_bool _isValid;
};

class Task {
public:

    Task();

    ~Task() =default;

    // 调用多态方法
    void exec();

    void setResult(Result *res);

    // 用户可以自定义任意任务类型 从Task继承 重写run方法 实现自定义任务处理
    virtual Any run() = 0;

private:
    Result* _result;
};


#endif //THREADPOOL_TASK_H
