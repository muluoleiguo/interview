//
// Created by jxq on 19-8-18.
//

// 互斥器

#ifndef MYMUDUO_MUTEX_H
#define MYMUDUO_MUTEX_H

#include <boost/core/noncopyable.hpp>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <assert.h>

class MutexLock : boost::noncopyable
{
public:
    MutexLock() : holder_(0)
    {
        pthread_mutex_init(&mutex_, NULL);
    }

    ~MutexLock()
    {
        assert(holder_ == 0);   // 表明没有被使用
        pthread_mutex_destroy(&mutex_);
    }

    bool isLockedByThisThread()
    {
        return CurrentThread::tid() == holder_;
    }

    void assertLocked()
    {
        assert(isLockedByThisThread());
    }

    void lock()     // 仅供MutexLockGuard调用，严禁用户代码调用
    {
        pthread_mutex_lock(&mutex_);    // 这两行顺序不能反
        holder_ = CurrentThread::tid();
    }

    void unlock()   // 仅供MutexLockGuard调用，严禁用户代码调用
    {
        holder_ = 0;                    // 这两行顺序不能反
        pthread_mutex_unlock(&mutex_);
    }

    pthread_mutex_t* getPthreadMutex()  // 仅供Condition调用，严禁用户代码调用
    {
        return &mutex_;
    }


private:
    pthread_mutex_t mutex_;
    pid_t holder_;  // 进程id
};

class MutexLockGuard : boost::noncopyable
{
public:
    explicit MutexLockGuard(MutexLock mutex) : mutex_(mutex)
    {
        mutex.lock();
    }

    ~MutexLockGuard()
    {
        mutex_.unlock();
    }

private:
    MutexLock& mutex_;
};

#define MutexLockGuard(x) static_assert(false, "missing mutex guard var name")

#endif //MYMUDUO_MUTEX_H
