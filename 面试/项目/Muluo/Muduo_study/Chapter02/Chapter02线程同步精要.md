# Chapter02

## 2.2 条件变量（condition variable）
[虚假唤醒(spurious wakeup)](https://blog.csdn.net/Tornado1102/article/details/76158390)
>> 倒计时（CountDownLatch）的两种用途：
>> 1. 主线程发起多个子线程，等这些子线程各自都完成一定的任务之后，主线程才继续执行。通常用于主线程等待多个子线程完成初始化。    
>> 2. 主线程发起多个子线程，子线程都等待主线程，主线程完成其他一些任务之后通知所有子线程开始执行。通常用于多个子线程等待主线程发出“起跑”命令。

## 2.4 封装MutexLock、MutexLockGuard、Condition
MutexLock:   
```cpp
class MutexLock : boost::noncopyable
{
public:
    MutexLock();    // 构造函数
    ~MutexLock();   // 析构函数
    void isLockedByThisThread(); 
    bool assertLocked();
    void lock();
    void unlock();
    pthread_mutex_t* getPthreadMutex();
private:
    pthread_mutex_t mutex_;
    pid_t holder_;    
}
```
MutexLockGuard:
```cpp
class MutexLockGuard : boost::noncopyable
{
public:
    MutexLockGuard();
    ~MutexLockGuard();
private:
    MutexLock& mutex_;
}
```

## 2.5 线程安全的Singleton实现
[pthread_once_t pthread_once](https://blog.csdn.net/rain_qingtian/article/details/11479737)

## 2.6 sleep(3)不是同步原语
>>生成代码中线程的等待可分为两种：一种是等待资源可用（要么等待在select/pool/epoll_wait上，要么等在条件变量上）；一种是等待进入临界区（等在mutex上）以便读写共享数据。

## 2.7 归纳与总结
>> * 线程同步的四项原则，尽量使用高层同步设施（线程池、队列、倒计时）。   
>> * 使用普通互斥器和条件变量完成剩余的同步任务，采用RAII惯用手法和[Scoped Locking](https://blog.csdn.net/zouxinfox/article/details/5848519)。