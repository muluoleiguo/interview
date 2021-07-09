//
// Created by jxq on 19-8-19.
//

// 单例模式

#ifndef MYMUDUO_SINGLETON_H
#define MYMUDUO_SINGLETON_H

#include <boost/core/noncopyable.hpp>
#include <pthread.h>

template <typename T>
class Singleton : boost::noncopyable
{
public:
    static T& instance()
    {
        pthread_once(&ponce_, &Singleton::init);
    }

private:
    Singleton();
    ~Singleton();

    static void init()
    {
        val_ = new T();
    }

    static pthread_once_t ponce_;
    static T* val_;
};

// 必须在头文件中定义static变量
template <typename T>
pthread_once_t Singleton<T>::ponce_ = PTHREAD_ONCE_INIT;

template <typename T>
T* Singleton<T>::val_ = NULL;

#endif //MYMUDUO_SINGLETON_H
