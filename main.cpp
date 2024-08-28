/**
 * @Description 线程池程序主函数
 * @Version 1.0.0
 * @Date 2024/8/27 11:23
 * @Github https://github.com/Programmer-Kenton
 * @Author Kenton
 */


#include "head/ThreadPool.h"
#include "head/MyTask.h"



int main() {

    {
        ThreadPool pool;
        pool.setMode(PoolMode::MODE_CACHED);
        // 开始启动线程池
        pool.start(2);

        // linux上，这些Result对象也是局部对象，要析构的！！！
        Result res1 = pool.submitTask(std::make_shared<MyTask>(1, 100000000));
        Result res2 = pool.submitTask(std::make_shared<MyTask>(100000001, 200000000));
        pool.submitTask(std::make_shared<MyTask>(100000001, 200000000));
        pool.submitTask(std::make_shared<MyTask>(100000001, 200000000));
        pool.submitTask(std::make_shared<MyTask>(100000001, 200000000));

        ULong sum1 = res1.get()._cast<ULong>();
        std::cout << sum1 << std::endl;
    } // 这里Result对象也要析构!!! 在vs下，条件变量析构会释放相应资源的


//    ThreadPool pool;
//
//    // 用户设置自己的线程池工作模式
//    pool.setMode(PoolMode::MODE_CACHED);
//    // 开启线程池
//    pool.start(4);
//
//    Result res1 = pool.submitTask(std::make_shared<MyTask>(1, 1000000));
//
//    try {
//        ULong sum1 = res1.get()._cast<ULong>();
//    }catch (const std::exception& e){
//        // std::cout << sum1 << std::endl;
//        std::cerr << "指针类型转换异常..." << e.what() << std::endl;
//    }


    std::cout << "主函数结束..." << std::endl;
    getchar();

#if 0
    // 如何设计Result机制
    Result res1 = pool.summitTask(std::make_shared<MyTask>(1,100));
    Result res2 = pool.summitTask(std::make_shared<MyTask>(1,100));
    Result res3 = pool.summitTask(std::make_shared<MyTask>(1,100));

    pool.summitTask(std::make_shared<MyTask>(1,100));
    pool.summitTask(std::make_shared<MyTask>(1,100));
    pool.summitTask(std::make_shared<MyTask>(1,100));

    // get返回一个Any类型 怎么转成具体的类型
    ULong sum1 = res1.get()._cast<ULong>();
    ULong sum2 = res2.get()._cast<ULong>();
    ULong sum3 = res3.get()._cast<ULong>();

    // Master-Slave线程模型
    // Master线程用来分解任务 然后给各个Slave线程分配任务
    // 等待各个Slave线程执行完任务 返回结果
    // Master线程合并各个任务结果并输出
    std::cout << (sum1 + sum2 + sum3) << std::endl;
    // pool.summitTask(std::make_shared<MyTask>());
    // pool.submitTask(std::make_shared<MyTask>());


    // std::this_thread::sleep_for(std::chrono::seconds(5));
    getchar();

#endif
}
