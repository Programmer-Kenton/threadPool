/**
 * @Description TODO
 * @Version 1.0.0
 * @Date 2024/8/27 11:23
 * @Github https://github.com/Programmer-Kenton
 * @Author Kenton
 */



#include "../head/ThreadPool.h"

ThreadPool::ThreadPool()
        : _initThreadSize(0),
          _taskSize(0),
          _taskQueMaxThreadHold(TASK_MAX_THREADHOLD),
          _poolMode(PoolMode::MODE_FIXED),
          _isPoolRunning(false),
          _idleThreadSize(0),
          _threadSizeHold(THREAD_MAX_THREADHOLD),
          _curThreadSize(0) {

}

ThreadPool::~ThreadPool() {
    _isPoolRunning = false;

    // 等待线程池里面所有的线程返回 有两种状态 阻塞&正在执行
    std::unique_lock<std::mutex> lock(_taskQueMtx);
    _notEmpty.notify_all();
    _exitCond.wait(lock, [&]() -> bool {
        return _threads.size() == 0;
    });
}

void ThreadPool::setMode(PoolMode mode) {
    // 若线程池已经启动则直接返回 即必须在线程池启动前指明线程池运行模式
    if (checkRunningState()) return;
    _poolMode = mode;
}


void ThreadPool::setTaskQueMaxThreadHold(int threadHold) {
    if (checkRunningState()) return;
    _taskQueMaxThreadHold = threadHold;
}

void ThreadPool::start(int initThreadSize) {
    // 设置线程池的运行状态
    _isPoolRunning = true;

    // 记录初始线程个数
    _initThreadSize = initThreadSize;
    _curThreadSize = initThreadSize;

    // 创建线程对象
    for (int i = 0; i < _initThreadSize; ++i) {
        // 创建thread线程对象的时候把线程函数给thread线程对象
        auto ptr = std::make_unique<Thread>(std::bind(&ThreadPool::threadFunc, this, std::placeholders::_1));
        // _threads.emplace_back(std::move(ptr));
        int threadId = ptr->getId();
        _threads.emplace(threadId, std::move(ptr));
    }

    // 启动所有线程 std::vector<Thread*> _threads
    for (int j = 0; j < _initThreadSize; ++j) {
        // 执行线程函数
        _threads[j]->start();
        // 记录初始空闲线程的数量
        _idleThreadSize++;
    }
}

void ThreadPool::threadFunc(int threadId) {
    // std::cout << "begin threadFunc() tid = " << std::this_thread::get_id() << std::endl;

    // 记录执行任务时间
    auto lastTime = std::chrono::high_resolution_clock().now();

    // 所有任务必须执行完成 线程池才可以回收所有线程资源
    for (;;) {
        std::shared_ptr<Task> task;
        {
            // 先获取锁
            std::unique_lock<std::mutex> lock(_taskQueMtx);

            std::cout << "tid:" << std::this_thread::get_id() << " 尝试获取任务..." << std::endl;

            // cached模式下 有可能已经创建了很多线程 但是空闲时间超过60s 应该把多余线程结束并回收
            // 超过_initThreadSize的线程结束回收
            // 当前时间 - 上一次执行线程的时间 > 60s
            // 每一秒钟返回一次 怎么区分 超时返回还是有任务待执行返回
            // 锁 + 双重判断
            while (_taskQue.size() == 0) {

                // 线程池结束 回收线程资源
                if (!_isPoolRunning) {
                    _threads.erase(threadId);
                    std::cout << "threadId = " << std::this_thread::get_id() << " exit" << std::endl;
                    _exitCond.notify_all();
                    // 线程函数结束 线程结束
                    return;
                }
                if (_poolMode == PoolMode::MODE_CACHED) {
                    // 条件变量超时返回
                    if (std::cv_status::timeout == _notEmpty.wait_for(lock, std::chrono::seconds(1))) {
                        auto now = std::chrono::high_resolution_clock().now();
                        // 使用duration_cast把毫秒转换成秒
                        auto dur = std::chrono::duration_cast<std::chrono::seconds>(now - lastTime);
                        if (dur.count() >= 60 && _curThreadSize > _initThreadSize) {
                            // 回收当前线程
                            // 记录线程相关变量值的修改
                            // 把线程对象从线程队列中删除 这里通过线程id找到线程进行删除
                            _threads.erase(threadId);
                            _curThreadSize--;
                            _idleThreadSize--;
                            std::cout << "真实线程id = " << std::this_thread::get_id() << " 退出了" << std::endl;
                            return;
                        }
                    }
                } else {
                    // 等待_notEmpty条件
                    // _notEmpty.wait(lock, [&]() -> bool {
                    //    return _taskQue.size() > 0;
                    // });
                    _notEmpty.wait(lock);

                }

//
//                if (!_isPoolRunning){
//                    _threads.erase(threadId);
//                    std::cout << "threadId = " << std::this_thread::get_id() << " exit" << std::endl;
//                    _exitCond.notify_all();
//                    // 结束线程函数就是结束当前线程了
//                    return;
//                }
            }

            // 线程池结束 回收线程资源
//            if (!_isPoolRunning){
//                break;
//            }

            // 消费一个线程去执行任务
            _idleThreadSize--;

            std::cout << "tid:" << std::this_thread::get_id() << " 获取任务成功..." << std::endl;

            // 从任务队列中取一个任务
            task = _taskQue.front();
            _taskQue.pop();
            _taskSize--;

            // 如果依然有剩余任务 继续通知其他的线程继续消费执行任务
            if (_taskQue.size() > 0) {
                _notEmpty.notify_all();
            }
            // 取出一个任务进行通知可以继续提交生产
            _notFull.notify_all();
            // 出作用域当前线程释放锁
        }


        // 当前线程负责执行这个任务
        if (task != nullptr) {
            // task->run();
            // 把任务的返回值setVal方法给到Result
            task->exec();
        }
        // 任务执行完毕 回收线程放入队列
        _idleThreadSize++;
        // 更新线程执行完任务的时间
        lastTime = std::chrono::high_resolution_clock().now();
    }


    // std::cout << "end threadFunc() tid = " << std::this_thread::get_id() << std::endl;
}

Result ThreadPool::submitTask(std::shared_ptr<Task> sp) {
    // 获取锁
    // unique_lock 它在构造时自动获取锁，并在析构时自动释放锁
    std::unique_lock<std::mutex> lock(_taskQueMtx);

    // 线程的通信 等待任务队列有空余
    // while (_taskQue.size() == _taskQueMaxThreadHold){_notFull.wait(lock);}
    // 用户提交任务最长阻塞时间不能超过1秒 否则判断提交任务失败 返回 --->服务降级
    if (!_notFull.wait_for(
            lock, std::chrono::seconds(1),
            [&]() -> bool {
                return _taskQue.size() < (size_t) _taskQueMaxThreadHold;
            })) {
        // 表示_notFull等待1s 条件依然没有满足
        std::cerr << "task queue is full,submit task failed" << std::endl;
        // return task->getResult(); 线程执行完task task对象就被析构掉了
        return Result(sp, true);
    }

    // 如果有空余 放入任务队列
    _taskQue.emplace(sp);
    _taskSize++;

    // 通知新放了任务 任务队列肯定不为空 _notEmpty通知分配线程执行任务
    _notEmpty.notify_all();

    // cached模式 任务比较紧急 场景:小而快的任务 根据任务数量和空闲线程的数量 判断是否需要创建新的线程
    if (_poolMode == PoolMode::MODE_CACHED && _taskSize > _idleThreadSize && _curThreadSize < _threadSizeHold) {
        std::cout << "创建了新线程 id = " << std::this_thread::get_id() << std::endl;
        // 创建新线程对象
        auto ptr = std::make_unique<Thread>(std::bind(&ThreadPool::threadFunc, this, std::placeholders::_1));
        int threadId = ptr->getId();
        _threads.emplace(threadId, std::move(ptr));
        // 启动线程
        _threads[threadId]->start();
        // 修改线程个数相关的变量
        _curThreadSize++;
        // 空闲线程数量+1
        _idleThreadSize++;
    }
    // 返回任务的Result对象
    return Result(sp);
}

bool ThreadPool::checkRunningState() const {
    return _isPoolRunning;
}

void ThreadPool::setThreadSizeHold(int threadHold) {
    if (checkRunningState()) return;
    if (_poolMode == PoolMode::MODE_CACHED) {
        _threadSizeHold = threadHold;
    }

}

int Thread::_generateId = 0;


void Thread::start() {
    // 创建一个线程用来执行线程函数
    std::thread t(_func, _threadId);
    // 设置分离线程
    t.detach();
}

Thread::Thread(Thread::ThreadFunc func) : _func(std::move(func)), _threadId(_generateId++) {

}

Thread::~Thread() {

}

int Thread::getId() const {
    return _threadId;
}


