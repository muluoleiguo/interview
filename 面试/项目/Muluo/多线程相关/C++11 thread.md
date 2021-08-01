### c++11多线程的实现

`std::thread th1(proc1);`创建了一个名为th1的线程，并且线程th1开始执行。实例化`std::thread`类对象时，至少需要传递函数名作为参数。如果函数为有参函数,如`void proc2(int a,int b)`则`std::thread th2(proc2,a,b);`



**当线程启动后，一定要在和线程相关联的std::thread对象销毁前，对线程运用join()或者detach()方法**



**互斥锁**

互斥量mutex就是互斥锁，加锁的资源支持互斥访问；

**读写锁**

`shared_mutex`读写锁把对共享资源的访问者划分成读者和写者，多个读线程能同时读取共享资源，但只有一个写线程能同时读取共享资源

`shared_mutex`

* 通过`lock_shared`，`unlock_shared`进行读者的锁定与解锁；

* 通过`lock`，`unlock`进行写者的锁定与解锁。



#### **lock_guard:**

其原理是：声明一个局部的`std::lock_guard`对象，在其构造函数中进行加锁，在其析构函数中进行解锁。最终的结果就是：创建即加锁，作用域结束自动解锁。从而使用`std::lock_guard()`就可以替代`lock()`与`unlock()`。注意mutex或者shared_mutex可少不了





### **unique_lock:**

std::unique_lock类似于lock_guard,只是std::unique_lock用法更加丰富。 使用std::lock_guard后不能手动lock()与手动unlock();使用std::unique_lock后可以手动lock()与手动unlock(); 



std::lock_gurad也可以传入两个参数，第一个参数为adopt_lock标识时，表示构造函数中不再进行互斥量锁定，因此**此时需要提前手动锁定**。

std::unique_lock的第二个参数，除了可以是adopt_lock,还可以是try_to_lock与defer_lock;

try_to_lock: 尝试去锁定，**得保证锁处于unlock的状态**,然后尝试现在能不能获得锁；尝试用的lock()去锁定这个mutex，但如果没有锁定成功，会立即返回，不会阻塞在那里，并继续往下执行；

defer_lock: 始化了一个没有加锁的mutex;



### **condition_variable:**

std::condition_variable类搭配std::mutex类来使用，condition_variable对象的作用不是用来管理互斥量的，它的作用是用来同步线程，



`wait(locker) :`wait函数需要传入一个std::mutex（一般会传入std::unique_lock对象）,即上述的locker。wait函数会自动调用 locker.unlock() 释放锁（因为需要释放锁，所以要传入mutex）并阻塞当前线程，本线程释放锁使得其他的线程得以继续竞争锁。



一旦当前线程获得notify(通常是另外某个线程调用` notify_* `唤醒了当前线程)，`wait()` 函数此时再自动调用 `locker.lock()`上锁。



cond.notify_one(): 随机唤醒一个等待的线程

cond.notify_all(): 唤醒所有等待的线程



**2.3 异步线程**(不熟悉)

需要`#include<future>`

### **async与future：**

std::async是一个函数模板，用来启动一个异步任务，它返回一个std::future类模板对象，future对象起到了**占位**的作用. 占位是什么意思？就是说该变量现在无值，但将来会有值

刚实例化的future是没有储存值的，但在调用std::future对象的get()成员函数时，主线程会被阻塞直到异步线程执行结束，并把返回结果传递给std::future，即通过FutureObject.get()获取函数返回值。

```C++
#include <iostream>
#include <thread>
#include <mutex>
#include<future>
#include<Windows.h>
using namespace std;
double t1(const double a, const double b)
{
 double c = a + b;
 Sleep(3000);//假设t1函数是个复杂的计算过程，需要消耗3秒
 return c;
}

int main() 
{
 double a = 2.3;
 double b = 6.7;
 future<double> fu = async(t1, a, b);//创建异步线程线程，并将线程的执行结果用fu占位；
 cout << "正在进行计算" << endl;
 cout << "计算结果马上就准备好，请您耐心等待" << endl;
 cout << "计算结果：" << fu.get() << endl;//阻塞主线程，直至异步线程return
        //cout << "计算结果：" << fu.get() << endl;//取消该语句注释后运行会报错，因为future对象的get()方法只能调用一次。
 return 0;
}
```



### **shared_future**

std::future与std::shard_future的用途都是为了**占位**，但是两者有些许差别。std::future的get()成员函数是转移数据所有权;std::shared_future的get()成员函数是复制数据。 因此： **future对象的get()只能调用一次**；无法实现多个线程等待同一个异步线程，一旦其中一个线程获取了异步线程的返回值，其他线程就无法再次获取。 **std::shared_future对象的get()可以调用多次**；可以实现多个线程等待同一个异步线程，每个线程都可以获取异步线程的返回值。



### **原子类型atomic<>**

原子操作指“不可分割的操作”.互斥量的加锁一般是针对一个代码段，而原子操作针对的一般都是一个变量(操作变量时加锁防止他人干扰)。 std::atomic<>是一个模板类，使用该模板类实例化的对象，提供了一些保证原子性的成员函数来实现共享数据的常用操作。

std::atomic<>对象提供了常见的原子操作（通过调用成员函数实现对数据的原子操作）： store是原子写操作，load是原子读操作。exchange是于两个数值进行交换的原子操作。 **即使使用了std::atomic<>，也要注意执行的操作是否支持原子性**



