/**
 * @Description TODO
 * @Version 1.0.0
 * @Date 2024/8/27 20:47
 * @Github https://github.com/Programmer-Kenton
 * @Author Kenton
 */
#include "../head/Task.h"

void Task::exec() {
    if (_result != nullptr) {
        // 多态调用
        _result->setVal(run());
    }

}

void Task::setResult(Result *res) {
    _result = res;
}

Task::Task() : _result(nullptr){

}

Result::Result(std::shared_ptr<Task> task, bool isValid) : _isValid(isValid),_task(task) {
    _task->setResult(this);
}

void Result::setVal(Any any) {
    // 存储task的返回值
    this->_any = std::move(any);
    // 已经获取了任务的返回值 增加信号量资源
    _sem.post();
}

Any Result::get() {
    if (!_isValid){
        return "";
    }

    // task任务如果没有执行完 这里会阻塞用户的线程
    _sem.wait();
    return std::move(_any);
}
