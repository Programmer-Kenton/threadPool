/**
 * @Description 可以接收任意数据的类型 上帝类
 * @Version 1.0.0
 * @Date 2024/8/27 20:37
 * @Github https://github.com/Programmer-Kenton
 * @Author Kenton
 */
#ifndef THREADPOOL_ANY_H
#define THREADPOOL_ANY_H

#include <iostream>
#include <memory>

class Any {
public:
    Any() = default;

    Any(const Any &) = delete;

    Any &operator=(const Any &) = delete;

    Any(Any &&) = default;

    Any &operator=(Any &&) = default;

    ~Any() = default;

    // 接收任意其他类型的数据
    template<class T>
    Any(T data) : _base(std::make_unique<Derive<T>>(data)){

    }

    // 把Any对象里面存储的data数据提取出来
    template<class T>
    T _cast() {
        // 从_base找到它所指向的Derive对象 从它里面取出data成员变量
        // 基类指针->派生类指针 _base.get() = Base* 从智能指针中转换基类类型指针
        Derive <T> *pd = dynamic_cast<Derive <T>*>(_base.get());
        if (pd == nullptr) {
            // 转换异常
            throw "type is unmatch";
        }
        return pd->_data;
    }

private:
    // 基类类型
    class Base {
    public:
        virtual ~Base() = default;
    };

    // 派生类类型
    template<class T>
    class Derive : public Base {
    public:
        Derive(T data) : _data(data) {}

        // 保存任意的其他类型
        T _data;
    };

    // 定义基类指针
    std::unique_ptr<Base> _base;
};




#endif //THREADPOOL_ANY_H
