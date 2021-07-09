//
// Created by jxq on 19-8-19.
//

// 条件变量

#ifndef MYMUDUO_CONDITION_H
#define MYMUDUO_CONDITION_H

#include <boost/core/noncopyable.hpp>
#include "Mutex.h"

class Condition : boost::noncopyable
{
public:
    explicit Condition(MutexLock& mutex) : mutex_(mutex)
    {
        pthread_cond_init(&cond_, NULL);
    }

    ~Condition()
    {
        pthread_cond_destroy(&cond_);
    }

    void wait()
    {
        pthread_cond_wait(&cond_, mutex_.getPthreadMutex());
    }

    void notify()
    {
        pthread_cond_signal(&cond_);
    }

    void notifyAll()
    {
        pthread_cond_broadcast(&cond_);
    }


private:
    MutexLock& mutex_;
    pthread_cond_t cond_;
};

#endif //MYMUDUO_CONDITION_H
