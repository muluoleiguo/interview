# Chapter04
## 4.1 基本线程原语的选用
>> 11个最基本的Pthreads函数是：  
>> 2个：线程的创建和等待结束(join)。封装为muduo::Thread。  
>> 4个：mutex的创建、销毁、加锁、解锁。封装为muduo::MutexLock。
>> 5个：条件变量的创建、销毁、等待、通知、广播。封装为：muduo::Condition。

>> 封装的其他原语：
>> * pthread_once，封装为muduo::Singleton<T>。其实不如直接使用全局变量。
>> * pthread_key*， 封装为muduo::ThreadLocal<T>。可以考虑使用__thread代替。

## 4.4 线程的创建和销毁的守则 
>>线程销毁的几种方式 
>> * 自然死亡。从线程主函数返回，线程正常退出。
>> * 非正常死亡。从线程主函数抛出异常或线程触发segfault信号等非法操作。
>> * 自杀。 在线程调用pthread_exit()来立刻退出线程。
>> * 他杀。 其他线程调用pthread_cancel()来强制终止某个线程。

## 4.5善用__thread关键字
>> __thread使用规则：只能用于修饰POD类型，不能修饰class类型，因为无法自动调用构造函数和析构函数。__thread可以用于修饰全局变量、函数内的静态变量，但是不能用于修饰函数的局部变量或class的普通成员变量。  
>> __thread变量是每个线程有一份独立实体，各个线程的变量值互不干扰。