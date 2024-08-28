/**
 * @Description 自定义的执行任务
 * @Version 1.0.0
 * @Date 2024/8/27 20:49
 * @Github https://github.com/Programmer-Kenton
 * @Author Kenton
 */
#ifndef THREADPOOL_MYTASK_H
#define THREADPOOL_MYTASK_H

#include <iostream>
#include <thread>
#include "Task.h"

using ULong = long long;

class MyTask : public Task{
public:

    MyTask(int begin, int end);

    // 如何设计让run函数的返回值表示任意类型
    // run方法最终在线程池分配的线程中做执行
    Any run() override;

private:
    int _begin;
    int _end;
};


#endif //THREADPOOL_MYTASK_H
