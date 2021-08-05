### std::lock_guard 介绍

std::lock_gurad 是 C++11 中定义的模板类。定义如下：

```c++
template <class Mutex> class lock_guard;
```

lock_guard 对象通常用于管理某个锁(Lock)对象，因此与 Mutex RAII 相关，方便线程对互斥量上锁，即在某个 lock_guard 对象的声明周期内，它所管理的锁对象会一直保持上锁状态；而 lock_guard 的生命周期结束之后，它所管理的锁对象会被解锁(注：类似 shared_ptr 等智能指针管理动态分配的内存资源 )。

模板参数 Mutex 代表互斥量类型，例如 std::mutex 类型，它应该是一个基本的 BasicLockable 类型，标准库中定义几种基本的 BasicLockable 类型，分别 std::mutex, std::recursive_mutex, std::timed_mutex，std::recursive_timed_mutex (以上四种类型均已在上一篇博客中介绍)以及 std::unique_lock(本文后续会介绍 std::unique_lock)。(注：BasicLockable 类型的对象只需满足两种操作，lock 和 unlock，另外还有 Lockable 类型，在 BasicLockable 类型的基础上新增了 try_lock 操作，因此一个满足 Lockable 的对象应支持三种操作：lock，unlock 和 try_lock；最后还有一种 TimedLockable 对象，在 Lockable 类型的基础上又新增了 try_lock_for 和 try_lock_until 两种操作，因此一个满足 TimedLockable 的对象应支持五种操作：lock, unlock, try_lock, try_lock_for, try_lock_until)。

在 lock_guard 对象构造时，传入的 Mutex 对象(即它所管理的 Mutex 对象)会被当前线程锁住。在lock_guard 对象被析构时，它所管理的 Mutex 对象会自动解锁，由于不需要程序员手动调用 lock 和 unlock 对 Mutex 进行上锁和解锁操作，因此这也是最简单安全的上锁和解锁方式，尤其是在程序抛出异常后先前已被上锁的 Mutex 对象可以正确进行解锁操作，极大地简化了程序员编写与 Mutex 相关的异常处理代码。

值得注意的是，lock_guard 对象并不负责管理 Mutex 对象的生命周期，lock_guard 对象只是简化了 Mutex 对象的上锁和解锁操作，方便线程对互斥量上锁，即在某个 lock_guard 对象的声明周期内，它所管理的锁对象会一直保持上锁状态；而 lock_guard 的生命周期结束之后，它所管理的锁对象会被解锁。

### std::lock_guard 构造函数

lock_guard 构造函数如下表所示：

| locking (1)       | `explicit lock_guard (mutex_type& m); `          |
| ----------------- | ------------------------------------------------ |
| adopting (2)      | `lock_guard (mutex_type& m, adopt_lock_t tag); ` |
| copy [deleted](3) | `lock_guard (const lock_guard&) = delete;`       |

1. locking 初始化
   - lock_guard 对象管理 Mutex 对象 m，并在构造时对 m 进行上锁（调用 m.lock()）。
2. adopting初始化
   - lock_guard 对象管理 Mutex 对象 m，与 locking 初始化(1) 不同的是， Mutex 对象 m 已被当前线程锁住。
3. 拷贝构造
   - lock_guard 对象的拷贝构造和移动构造(move construction)均被禁用，因此 lock_guard 对象不可被拷贝构造或移动构造。

我们来看一个简单的例子([参考](http://www.cplusplus.com/reference/mutex/lock_guard/lock_guard/))：



```c++
#include <iostream>       // std::cout
#include <thread>         // std::thread
#include <mutex>          // std::mutex, std::lock_guard, std::adopt_lock

std::mutex mtx;           // mutex for critical section

void print_thread_id (int id) {
  mtx.lock();
  std::lock_guard<std::mutex> lck(mtx, std::adopt_lock);
  std::cout << "thread #" << id << '\n';
}

int main ()
{
  std::thread threads[10];
  // spawn 10 threads:
  for (int i=0; i<10; ++i)
    threads[i] = std::thread(print_thread_id,i+1);

  for (auto& th : threads) th.join();

  return 0;
}
```



在 print_thread_id 中，我们首先对 mtx 进行上锁操作(mtx.lock();)，然后用 mtx 对象构造一个 lock_guard 对象(std::lock_guard<std::mutex> lck(mtx, std::adopt_lock);)，注意此时 Tag 参数为 std::adopt_lock，表明当前线程已经获得了锁，此后 mtx 对象的解锁操作交由 lock_guard 对象 lck 来管理，在 lck 的生命周期结束之后，mtx 对象会自动解锁。

lock_guard 最大的特点就是安全易于使用，请看下面例子([参考](http://www.cplusplus.com/reference/mutex/lock_guard/))，在异常抛出的时候通过 lock_guard 对象管理的 Mutex 可以得到正确地解锁。



```c++
#include <iostream>       // std::cout
#include <thread>         // std::thread
#include <mutex>          // std::mutex, std::lock_guard
#include <stdexcept>      // std::logic_error

std::mutex mtx;

void print_even (int x) {
  if (x%2==0) std::cout << x << " is even\n";
  else throw (std::logic_error("not even"));
}

void print_thread_id (int id) {
  try {
    // using a local lock_guard to lock mtx guarantees unlocking on destruction / exception:
    std::lock_guard<std::mutex> lck (mtx);
    print_even(id);
  }
  catch (std::logic_error&) {
    std::cout << "[exception caught]\n";
  }
}

int main ()
{
  std::thread threads[10];
  // spawn 10 threads:
  for (int i=0; i<10; ++i)
    threads[i] = std::thread(print_thread_id,i+1);

  for (auto& th : threads) th.join();

  return 0;
}
```





