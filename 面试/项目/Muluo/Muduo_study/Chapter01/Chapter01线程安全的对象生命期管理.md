# Chapter01

## 1.1当析构函数遇到多线程

### 1.1.2 MutexLock与MutexLockGuard

>> 代码见&2.4
>> MutexLock封装临界区（critical section），这是一个简单的资源类，用RAII(RAII全称是“Resource Acquisition is Initialization”，直译过来是“资源获取即初始化”,也就是说在构造函数中申请分配资源，在析构函数中释放资源)手法封装互斥器的创建与销毁。在Linux下是pthread_mutex_t，默认是不可重入的（&2.1.1）。MutexLock一般是别的class的数据成员。   

[RAII参考链接](https://www.cnblogs.com/jiangbin/p/6986511.html)
>> RAII ：通常的做法是owner持有指向child的shared_ptr,child持有指向owner的weak_ptr。

>> MutexLockGuard封装临界区的进入和退出，即加锁和解锁。MutexLockGuard一般是个栈上的对象，它的作用域刚好等于临界区域。

>> 这两个class都不允许拷贝构造和赋值，使用原则见&2.1。

### 1.1.3 一个线程安全的Counter示例
[关键字mutable](https://blog.csdn.net/aaa123524457/article/details/80967330)
[关键字const](https://www.cnblogs.com/kevinWu7/p/10163449.html)

## 1.4 线程安全的Oberver有多难
>> 在面向对象程序设计中，对象的关系主要有三种：composition(组合/复合)，aggregation(关联/联系)，association(聚合)。

[智能指针boost::scoped_str](https://www.cnblogs.com/helloamigo/p/3572533.html) [关键字explicit](https://blog.csdn.net/guoyunfei123/article/details/89003369)

## 1.6 神器shared_ptr/weak_ptr
[shared_ptr/weak_ptr](https://blog.csdn.net/ingvar08/article/details/79200424) [智能指针](https://blog.csdn.net/qingdujun/article/details/74858071)

## 1.7 插曲：系统地避免各种指针错误
>> c++可能出现的内存问题大致有这么几个方面：
>>> 1. 缓冲区溢出(buffer overrun)：用std::vector<char>/std::string或自己编写Buffer class来管理缓冲区，自动记住缓冲区的长度，并通过成员函数而不是裸指针来修改缓冲区。
>>> 2. 空悬指针/野指针:用shared_ptr/weak_ptr。
>>> 3. 重复释放(double delete)：用scoped_ptr，只在对象析构的时候释放一次。
>>> 4. 内存泄露(memory leak)：用scoped_ptr，对象析构的时候自动释放内存。
>>> 5. 不配对的new[]/delete:把new统统替换为std::vector/scoped_array。
>>> 6. 内存碎片(memory fragmentation): &9.2.1和&A.1.8探讨。

## 1.8 应用到Observer上
[可重入锁与不可重入锁](https://blog.csdn.net/qq_29519041/article/details/86583945)



