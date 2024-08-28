/**
 * @Description TODO
 * @Version 1.0.0
 * @Date 2024/8/27 20:49
 * @Github https://github.com/Programmer-Kenton
 * @Author Kenton
 */
#include "../head/MyTask.h"

MyTask::MyTask(int begin, int end) : _begin(begin), _end(end){

}


Any MyTask::run() {
    std::cout << "begin tid = " << std::this_thread::get_id() << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    ULong sum = 0;
    for (ULong i = _begin; i < _end; ++i) {
        sum += i;
    }
    std::cout << "end tid = " << std::this_thread::get_id() << std::endl;
    return sum;
}


