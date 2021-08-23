接着上上节 thread ，本节主要介绍mutex的内容，练习代码[地址](https://github.com/jorionwen/threadtest/tree/master/mutex)。
 <mutex>：该头文件主要声明了与互斥量(mutex)相关的类，包括 std::mutex 系列类，std::lock_guard, std::unique_lock, 以及其他的类型和函数。

## 2、 [**mutex**](http://www.cplusplus.com/reference/mutex/)

mutex 头文件中主要包含  Mutexes、lock 和相关的类型(Other types)和公共函数。

| (一)、 Mutexes |

|                                                              |                                                              |
| ------------------------------------------------------------ | ------------------------------------------------------------ |
| [**mutex**](http://www.cplusplus.com/reference/mutex/mutex/) | mutex互斥锁是一个可锁的对象，它被设计成在关键的代码段需要独占访问时发出信号，从而阻止具有相同保护的其他线程并发执行并访问相同的内存位置。 |
| [**recursive_mutex**](http://www.cplusplus.com/reference/mutex/recursive_mutex/) | 递归互斥锁是一个可锁的对象，就像互斥一样，但是允许同一个线程获得对互斥对象的多个级别的所有权。 |
| [**timed_mutex**](http://www.cplusplus.com/reference/mutex/timed_mutex/) | 定时的互斥信号是一个时间可锁定的对象,旨在当关键部分的代码需要独占访问时,就像一个普通互斥,但另外支持定时try-lock请求。 |
| [**recursive_timed_mutex**](http://www.cplusplus.com/reference/mutex/recursive_timed_mutex/) | 一个递归的互斥对象结合的特点recursive_mutex timed_mutex到单个类的特性:它同时支持由一个线程获取多个锁级别也定时try-lock请求。 |

上面是 mutex头文件中 mutexs中的相关类。上面类主要是几种锁对象。

- **std::mutex**
  - (constructor)  不允许拷贝；互斥对象不能被复制/移动，初始状态为未锁定。



```c
          default (1） constexpr mutex() noexcept;
          copy [deleted] (2) mutex (const mutex&) = delete;
```

- [**lock**](http://www.cplusplus.com/reference/mutex/mutex/lock/) 锁住互斥对象

  1. 如果互斥对象当前没有被任何线程锁定，则调用线程锁定它(从这一点开始，直到它的成员解锁被调用，线程拥有互斥对象)。
  2. 如果互斥锁当前被另一个线程锁定，则调用线程的执行将被阻塞，直到其他线程解锁(其他非锁定线程继续执行它们)。
  3. 如果互斥锁当前被相同的线程所锁定，调用此函数，则会产生死锁(未定义的行为)。

  [示例1](https://github.com/jorionwen/threadtest/blob/master/mutex/example1.cpp)：

  

  ```c
  // mutex::lock/unlock
  #include <iostream>       // std::cout
  #include <thread>         // std::thread
  #include <mutex>          // std::mutex
  std::mutex mtx;           // mutex for critical section
  void print_thread_id (int id) {
    // critical section (exclusive access to std::cout signaled by locking mtx):
    mtx.lock();
    std::cout << "thread #" << id << '\n';
    mtx.unlock();
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

- [**try_lock**](http://www.cplusplus.com/reference/mutex/mutex/try_lock/) 试图锁定互斥锁，不阻塞。
   若互斥对象当前没有被任何线程锁定，则调用线程锁定它；
   若互斥锁当前被另一个线程锁定，则该函数失败并返回false，没有阻塞；若互斥锁当前被相同的线程所锁定，调用此函数，则会产生死锁。
   [示例2](https://github.com/jorionwen/threadtest/blob/master/mutex/example2.cpp)：

  

  ```c
  // mutex::try_lock example
  #include <iostream>       // std::cout
  #include <thread>         // std::thread
  #include <mutex>          // std::mutex
  
  volatile int counter (0); // non-atomic counter
  std::mutex mtx;           // locks access to counter
  
  void attempt_10k_increases () {
      for (int i=0; i<10000; ++i) {
          if (mtx.try_lock()) {   // only increase if currently not locked:
                ++counter;
                mtx.unlock();
          }
     }
  }
  
  int main ()
  {
    std::thread threads[10];
    // spawn 10 threads:
    for (int i=0; i<10; ++i)
      threads[i] = std::thread(attempt_10k_increases);
  
    for (auto& th : threads) th.join();
    std::cout << counter << " successful increases of the counter.\n";
  
    return 0;
  }
  ```

- [**unlock**](http://www.cplusplus.com/reference/mutex/mutex/unlock/)
   解锁互斥锁，释放所有权。

- [**native_handle**](http://www.cplusplus.com/reference/mutex/mutex/native_handle/)
   获取本地处理，如果库实现支持它，这个成员函数只存在于类互斥中。如果存在,它返回一个值用于访问特定于实现的信息相关联的对象。

- [**recursive_mutex**](http://www.cplusplus.com/reference/mutex/recursive_mutex/)
   递归锁基本函数同上，只是std::recursive_mutex 允许同一个线程对互斥量多次上锁（即递归上锁），来获得对互斥量对象的多层所有权，std::recursive_mutex 释放互斥量时需要调用与该锁层次深度相同次数的 unlock()。

- [**timed_mutex**](http://www.cplusplus.com/reference/mutex/timed_mutex/)
   timed_mutex锁比较mutex所多了两个成员函数try_lock_for 和 try_lock_until。

  - [**try_lock_for**](http://www.cplusplus.com/reference/mutex/timed_mutex/try_lock_for/)
     传入时间段，在时间范围内未获得所就阻塞住线程，如果在此期间其他线程释放了锁，则该线程可以获得对互斥量的锁，如果超时，则返回 false。
  - [**try_lock_until**](http://www.cplusplus.com/reference/mutex/timed_mutex/try_lock_until/)
     同上面的解释，只是传入参数为一个未来的一个时间点。

  [示例3](https://github.com/jorionwen/threadtest/blob/master/mutex/example3.cpp)：

  

  ```c
  // timed_mutex::try_lock_for example
  #include <iostream>       // std::cout
  #include <chrono>         // std::chrono::milliseconds
  #include <thread>         // std::thread
  #include <mutex>          // std::timed_mutex
  
  std::timed_mutex mtx;
  
  void fireworks () {
    // waiting to get a lock: each thread prints "-" every 200ms:
    while (!mtx.try_lock_for(std::chrono::milliseconds(2))) {
      std::cout << "-";
    }
    // got a lock! - wait for 1s, then this thread prints "*"
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::cout << "*\n";
    mtx.unlock();
  }
  
  int main ()
  {
    std::thread threads[10];
    // spawn 10 threads:
    for (int i=0; i<10; ++i) {
      threads[i] = std::thread(fireworks);
    }
    for (auto& th : threads) th.join();
    return 0;
  }
  ```

  [示例4](https://github.com/jorionwen/threadtest/blob/master/mutex/example4.cpp)：

  

  ```c
  // timed_mutex::try_lock_until example
  #include <iostream>       // std::cout
  #include <chrono>         // std::chrono::system_clock
  #include <thread>         // std::thread
  #include <mutex>          // std::timed_mutex
  #include <ctime>          // std::time_t, std::tm, std::localtime, std::mktime
  
  std::timed_mutex cinderella;
  
  // gets time_point for next midnight:
  std::chrono::time_point<std::chrono::system_clock> midnight() {
    using std::chrono::system_clock;
    std::time_t tt = system_clock::to_time_t (system_clock::now());
    struct std::tm * ptm = std::localtime(&tt);
    ++ptm->tm_mday; ptm->tm_hour=0; ptm->tm_min=0; ptm->tm_sec=0;
    return system_clock::from_time_t (mktime(ptm));
  }
  
  void carriage() {
    if (cinderella.try_lock_until(midnight())) {
      std::cout << "ride back home on carriage\n";
      cinderella.unlock();
    }
    else
      std::cout << "carriage reverts to pumpkin\n";
  }
  
  void ball() {
    cinderella.lock();
    std::cout << "at the ball...\n";
    cinderella.unlock();
  }
  
  int main ()
  {
    std::thread th1 (ball);
    std::thread th2 (carriage);
    th1.join();
    th2.join();
    return 0;
  }
  ```

- [**recursive_timed_mutex**](http://www.cplusplus.com/reference/mutex/recursive_timed_mutex/)
   同上面分析，递归的时间锁，允许同一个线程对互斥量多次上锁（即递归上锁）， 不再赘述。

(二)、Locks 模板类

|                                                              |                               |        |
| ------------------------------------------------------------ | ----------------------------- | ------ |
| [**lock_guard**](http://www.cplusplus.com/reference/mutex/lock_guard/) | Lock guard (class template )  | 模版类 |
| [**unique_lock**](http://www.cplusplus.com/reference/mutex/unique_lock/) | Unique lock (class template ) | 模版类 |

- [ lock_guard ](http://www.cplusplus.com/reference/mutex/lock_guard/)
   定义： template <class Mutex> class lock_guard;
   锁保护是一个通过将互斥对象保持锁定来管理互斥对象的对象。
   在构造上，互斥对象被调用线程锁定，并且在销毁时，互斥锁被解锁。它是最简单的锁，作为具有自动持续时间的对象特别有用，直到它的上下文结束。这样，它保证在抛出异常时，可以正确地锁定互斥对象。
   注意，lock_guard对象不以任何方式管理互斥对象的生命周期:互斥对象的持续时间至少要延长到锁定它的lock_guard的销毁为止。

> 模板参数 Mutex 代表互斥量类型，例如 std::mutex 类型，它应该是一个基本的 BasicLockable 类型，标准库中定义几种基本的 BasicLockable 类型，分别 std::mutex, std::recursive_mutex, std::timed_mutexstd::recursive_timed_mutex 以及 std::unique_lock。 (注：BasicLockable 类型的对象只需满足两种操作，lock 和 unlock，另外还有 Lockable 类型，在 BasicLockable 类型的基础上新增了 try_lock 操作，因此一个满足 Lockable 的对象应支持三种操作：lock，unlock 和 try_lock；最后还有一种 TimedLockable 对象，在 Lockable 类型的基础上又新增了 try_lock_for 和 try_lock_until 两种操作，因此一个满足 TimedLockable 的对象应支持五种操作：lock, unlock, try_lock, try_lock_for, try_lock_until)。

构造函数[**(constructor)**](http://www.cplusplus.com/reference/mutex/lock_guard/lock_guard/)
 locking (1)  explicit lock_guard (mutex_type& m);
 adopting (2)      lock_guard (mutex_type& m, adopt_lock_t tag);
 copy [deleted](3)    lock_guard (const lock_guard&) = delete;
 (1)锁定初始化：对象管理m，并锁定它(通过调用m.lock())。
 (2)采用初始化：对象管理m，它是已经当前被构造线程锁定的互斥对象。
 (3)复制构造： 删除(lock_guard对象不能被复制/移动)。
 注：lock_guard的对象保持锁定并管理m(通过调用它的成员解锁)来解锁它，当lock_guard的对象析构的时候，mtx将会被解锁。
 [示例5](https://github.com/jorionwen/threadtest/blob/master/mutex/example5.cpp)：



```c
  // constructing lock_guard with adopt_lock
  #include <iostream>       // std::cout
  #include <thread>         // std::thread
  #include <mutex>          // std::mutex, std::lock_guard, std::adopt_lock

  std::mutex mtx;           // mutex for critical section

  void print_thread_id (int id) {
    mtx.lock();
    std::lock_guard<std::mutex> lck (mtx, std::adopt_lock);
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

- [unique_lock](http://www.cplusplus.com/reference/mutex/unique_lock/)
   定义： template <class Mutex> class unique_lock;
   unique_lock与 lock_guard 类相似，但它提供了更好的上锁和解锁控制。unique_lock 对象以独占所有权的方式(官方叫做unique ownership)管理 mutex 对象的上锁和解锁操作；独占所有权，就是没有其他的 unique_lock 对象同时拥有某个 mutex 对象的所有权。
   unique_lock对象在析构的时候一定保证互斥量为解锁状态；因此它作为具有自动持续时间的对象特别有用，因为它保证在抛出异常时，互斥对象被正确地解锁。不过,注意unique_lock对象并不以任何方式管理互斥对象的生命周期:互斥对象的持续时间将延长至少直到unique_lock管理它的毁灭。

1. 构造函数(constructor):



```c
       default       (1)    unique_lock() noexcept;
       locking       (2)    explicit unique_lock (mutex_type& m);
       try-locking   (3)    unique_lock (mutex_type& m, try_to_lock_t tag);
       deferred      (4)    unique_lock (mutex_type& m, defer_lock_t tag) noexcept;
       adopting      (5)    unique_lock (mutex_type& m, adopt_lock_t tag);
       locking for   (6)    template <class Rep, class Period> 
                            unique_lock (mutex_type& m, const chrono::duration<Rep,Period>& rel_time);
       locking until (7)    template <class Clock, class Duration>
                            unique_lock (mutex_type& m, const chrono::time_point<Clock,Duration>& abs_time);
       copy [deleted] (8)  unique_lock (const unique_lock&) = delete;
       move           (9)    unique_lock (unique_lock&& x);
```

下面我们来分别介绍以上各个构造函数：
 (1) 默认构造函数
 unique_lock 对象不管理任何 Mutex 对象m。

(2) locking 初始化
 unique_lock 对象管理 Mutex 对象 m，并调用 m.lock() 对 Mutex 对象进行上锁，如果其他 unique_lock 对象已经管理了m，该线程将会被阻塞。

(3) try-locking 初始化
 unique_lock 对象管理 Mutex 对象 m，并调用 m.try_lock() 对 Mutex 对象进行上锁，但如果上锁不成功，不会阻塞当前线程。

(4) deferred 初始化
 unique_lock 对象管理 Mutex 对象 m并不锁住m。 m 是一个没有被当前线程锁住的 Mutex 对象。

(5) adopting 初始化
 unique_lock 对象管理 Mutex 对象 m， m 应该是一个已经被当前线程锁住的 Mutex 对象。(当前unique_lock 对象拥有对锁(lock)的所有权)。

(6) locking 一段时间(duration)
 新创建的 unique_lock 对象管理 Mutex 对象 m，通过调用 m.try_lock_for(rel_time) 来锁住 Mutex 对象一段时间(rel_time)。

(7) locking 直到某个时间点(time point)
 新创建的 unique_lock 对象管理 Mutex 对象m，通过调用 m.try_lock_until(abs_time) 来在某个时间点(abs_time)之前锁住 Mutex 对象。

(8) 拷贝构造 [被禁用]
 unique_lock 对象不能被拷贝构造。

(9) 移动(move)构造
 新创建的 unique_lock 对象获得了由 x 所管理的 Mutex 对象的所有权(包括当前 Mutex 的状态)。调用 move 构造之后， x 对象如同通过默认构造函数所创建的，就不再管理任何 Mutex 对象了。

[示例6](https://github.com/jorionwen/threadtest/blob/master/mutex/example6.cpp)：



```c
  // unique_lock constructor example
  #include <iostream>       // std::cout
  #include <thread>         // std::thread
  #include <mutex>          // std::mutex, std::lock, std::unique_lock
                                        // std::adopt_lock, std::defer_lock
  std::mutex foo,bar;

  void task_a () {
    std::lock (foo,bar);         // simultaneous lock (prevents deadlock)
    std::unique_lock<std::mutex> lck1 (foo,std::adopt_lock);
    std::unique_lock<std::mutex> lck2 (bar,std::adopt_lock);
    std::cout << "task a\n";
    // (unlocked automatically on destruction of lck1 and lck2)
  }

  void task_b () {
    // foo.lock(); bar.lock(); // replaced by:
    std::unique_lock<std::mutex> lck1, lck2;
    lck1 = std::unique_lock<std::mutex>(bar,std::defer_lock);
    lck2 = std::unique_lock<std::mutex>(foo,std::defer_lock);
    std::lock (lck1,lck2);       // simultaneous lock (prevents deadlock)
    std::cout << "task b\n";
    // (unlocked automatically on destruction of lck1 and lck2)
  }

  int main ()
  {
    std::thread th1 (task_a);
    std::thread th2 (task_b);

    th1.join();
    th2.join();
  
    return 0;
  }
```

1. 其他成员函数

| Lock/unlock                                                  | 上锁和解锁过程                                               |                              |
| ------------------------------------------------------------ | ------------------------------------------------------------ | ---------------------------- |
| [**lock**](http://www.cplusplus.com/reference/mutex/unique_lock/lock/) | Lock mutex (public member function )                         | 调用被托管的互斥对象的成员锁 |
| [**try_lock**](http://www.cplusplus.com/reference/mutex/unique_lock/try_lock/) | Lock mutex if not locked (public member function )           | 尝试上锁                     |
| [**try_lock_for**](http://www.cplusplus.com/reference/mutex/unique_lock/try_lock_for/) | Try to lock mutex during time span (public member function ) | 尝试在时间段上锁             |
| [**try_lock_until**](http://www.cplusplus.com/reference/mutex/unique_lock/try_lock_until/) | Try to lock mutex until time point (public member function ) | 尝试在时间点到之前上锁       |
| [**unlock**](http://www.cplusplus.com/reference/mutex/unique_lock/unlock/) | Unlock mutex (public member function )                       | 解锁                         |

| Modifiers                                                    | 修改                                              |                  |
| ------------------------------------------------------------ | ------------------------------------------------- | ---------------- |
| [**operator=**](http://www.cplusplus.com/reference/mutex/unique_lock/operator%3D/) | Move-assign unique_lock (public member function ) | 同move操作       |
| [**swap**](http://www.cplusplus.com/reference/mutex/unique_lock/swap/) | Swap unique locks (public member function )       | 交换两个互斥对象 |
| [**release**](http://www.cplusplus.com/reference/mutex/unique_lock/release/) | Release mutex (public member function )           | 释放锁           |

| Observers                                                    | 获取操作                                                |
| ------------------------------------------------------------ | ------------------------------------------------------- |
| [**owns_lock**](http://www.cplusplus.com/reference/mutex/unique_lock/owns_lock/) | Owns lock (public member function )                     |
| [**lock**](http://www.cplusplus.com/reference/mutex/lock/)   | Return whether it owns a lock (public member function ) |
| [**mutex**](http://www.cplusplus.com/reference/mutex/unique_lock/mutex/) | Get mutex (public member function )                     |

(1)std::unique_lock::lock
 锁定互斥对象,调用被托管的互斥对象的成员锁。
 对已经被其他线程锁定的互斥对象调用锁，阻塞当前线程(等待)，直到锁释放。当函数返回时，对象在互斥锁上拥有一个锁，如果调用锁定失败,system_error异常。
 [示例7](https://github.com/jorionwen/threadtest/blob/master/mutex/example7.cpp)：



```c
// unique_lock::lock/unlock
#include <iostream>       // std::cout
#include <thread>         // std::thread
#include <mutex>          // std::mutex, std::unique_lock, std::defer_lock

std::mutex mtx;           // mutex for critical section

void print_thread_id (int id) {
  std::unique_lock<std::mutex> lck (mtx,std::defer_lock);
  // critical section (exclusive access to std::cout signaled by locking lck):
  lck.lock();
  std::cout << "thread #" << id << '\n';
  lck.unlock();
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

(2)std::unique_lock::try_lock
 如果互斥量没有锁定就锁定；调用try_lock 管理mutex对象，并使用返回值设置拥有状态；如果拥有的状态在调用前为真或者如果对象目前管理没有互斥对象,函数抛出一个system_error异常。
 [示例8](https://github.com/jorionwen/threadtest/blob/master/mutex/example8.cpp)：



```c
// unique_lock::try_lock example
#include <iostream>       // std::cout
#include <vector>         // std::vector
#include <thread>         // std::thread
#include <mutex>          // std::mutex, std::unique_lock, std::defer_lock

std::mutex mtx;           // mutex for critical section

void print_star () {
  std::unique_lock<std::mutex> lck(mtx,std::defer_lock);
  // print '*' if successfully locked, 'x' otherwise: 
  if (lck.try_lock())
    std::cout << '*';
  else                    
    std::cout << 'x';
}

int main ()
{
  std::vector<std::thread> threads;
  for (int i=0; i<500; ++i)
    threads.emplace_back(print_star);

  for (auto& x: threads) x.join();
  std::cout << "\n";
  return 0;
}
```

(3) std::unique_lock::try_lock_for



```kotlin
template <class Rep, class Period>
bool try_lock_for (const chrono::duration<Rep,Period>& rel_time);
```

在时间范围span内锁定互斥锁;调用try_lock_for管理时间的mutex对象,并使用返回值设置拥有状态;如果在调用之前拥有状态已经真或者如果对象目前管理没有互斥对象,函数抛出一个system_error异常。
 [示例9](https://github.com/jorionwen/threadtest/blob/master/mutex/example9.cpp)：



```c
// unique_lock::try_lock_for example
#include <iostream>       // std::cout
#include <chrono>         // std::chrono::milliseconds
#include <thread>         // std::thread
#include <mutex>          // std::timed_mutex, std::unique_lock, std::defer_lock

std::timed_mutex mtx;

void fireworks () {
  std::unique_lock<std::timed_mutex> lck(mtx,std::defer_lock);
  // waiting to get a lock: each thread prints "-" every 200ms:
  while (!lck.try_lock_for(std::chrono::milliseconds(200))) {
    std::cout << "-";
  }
  // got a lock! - wait for 1s, then this thread prints "*"
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  std::cout << "*\n";
}

int main ()
{
  std::thread threads[10];
  // spawn 10 threads:
  for (int i=0; i<10; ++i)
    threads[i] = std::thread(fireworks);

  for (auto& th : threads) th.join();

  return 0;
}
```

(4)std::unique_lock::try_lock_until
 同上面描述，传入一个时间点而不是时间段。示例：同上改动，不再述。

(5)std::unique_lock::unlock
 调用unlock对托管的mutex对象进行解锁，并将拥有的状态设置为false；如果调用之前拥有状态是错误,函数抛出一个system_error异常与operation_not_permitted错误条件。
 [示例10](https://github.com/jorionwen/threadtest/blob/master/mutex/example10.cpp)：



```c
// unique_lock::lock/unlock
#include <iostream>       // std::cout
#include <thread>         // std::thread
#include <mutex>          // std::mutex, std::unique_lock, std::defer_lock

std::mutex mtx;           // mutex for critical section

void print_thread_id (int id) {
  std::unique_lock<std::mutex> lck (mtx,std::defer_lock);
  // critical section (exclusive access to std::cout signaled by locking lck):
  lck.lock();
  std::cout << "thread #" << id << '\n';
  lck.unlock();
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

(6) std::unique_lock::operator=



```cpp
move (1)    unique_lock& operator= (unique_lock&& x) noexcept;
copy [deleted] (2) unique_lock& operator= (const unique_lock&) = delete;
```

使用x的mutex对象替换掉当前对象的mutex，同时获取x的状态。替换掉的x将不再有mutex对象，如果对象在调用之前对其托管的mutex对象拥有一个锁，那么它的解锁成员在被替换之前就被调用了。；unique_lock对象不能被复制。
 [示例11](https://github.com/jorionwen/threadtest/blob/master/mutex/example11.cpp)：



```c
// unique_lock::operator= example
#include <iostream>       // std::cout
#include <thread>         // std::thread
#include <mutex>          // std::mutex, std::unique_lock

std::mutex mtx;           // mutex for critical section

void print_fifty (char c) {
  std::unique_lock<std::mutex> lck;         // default-constructed
  lck = std::unique_lock<std::mutex>(mtx);  // move-assigned
  for (int i=0; i<50; ++i) { std::cout << c; }
  std::cout << '\n';
}

int main ()
{
  std::thread th1 (print_fifty,'*');
  std::thread th2 (print_fifty,'$');

  th1.join();
  th2.join();

  return 0;
}
```

(7) std::unique_lock::swap
 与x交换内容，包括托管的互斥对象和它们当前拥有的状态。

(8) std::unique_lock::release
 返回一个指向托管的互斥对象的指针，并释放对它的所有权;调用后unique_lock不再管理mutex对象(像默认构造一样)；注意，该函数不会锁定或释放返回的互斥对象。
 [示例12](https://github.com/jorionwen/threadtest/blob/master/mutex/example12.cpp)：



```c
// unique_lock::release example
#include <iostream>       // std::cout
#include <vector>         // std::vector
#include <thread>         // std::thread
#include <mutex>          // std::mutex, std::unique_lock

std::mutex mtx;
int count = 0;

void print_count_and_unlock (std::mutex* p_mtx) {
  std::cout << "count: " << count << '\n';
  p_mtx->unlock();
}

void task() {
  std::unique_lock<std::mutex> lck(mtx);
  ++count;
  print_count_and_unlock(lck.release());
}

int main ()
{
  std::vector<std::thread> threads;
  for (int i=0; i<10; ++i)
    threads.emplace_back(task);

  for (auto& x: threads) x.join();

  return 0;
}
```

(9) std::unique_lock::owns_lock
 返回unique_lock对象拥有的一个锁。unique_lock锁定一个mutex，在没有解锁或者释放unique_lock之前返回为真，其他情况返回为假；是unique_lock::operator bool的别名。
 [示例13](https://github.com/jorionwen/threadtest/blob/master/mutex/example13.cpp)：



```c
// unique_lock::operator= example
#include <iostream>       // std::cout
#include <vector>         // std::vector
#include <thread>         // std::thread
#include <mutex>          // std::mutex, std::unique_lock, std::try_to_lock

std::mutex mtx;           // mutex for critical section

void print_star () {
  std::unique_lock<std::mutex> lck(mtx,std::try_to_lock);
  // print '*' if successfully locked, 'x' otherwise: 
  if (lck.owns_lock())
    std::cout << '*';
  else                    
    std::cout << 'x';
}

int main ()
{
  std::vector<std::thread> threads;
  for (int i=0; i<500; ++i)
    threads.emplace_back(print_star);

  for (auto& x: threads) x.join();
  std::cout << "\n";
  return 0;
}
```

(10) std::unique_lock::operator bool
 同上面。

(11) std::unique_lock::mutex
 返回一个指向托管的mutex对象的指针。注意unique_lock不释放互斥对象管理的所有权。如果它拥有一个互斥锁,它仍然是负责释放它在某一时刻(像当unique_lock被销毁的时候)。
 [示例14](https://github.com/jorionwen/threadtest/blob/master/mutex/example14.cpp)：



```c
// unique_lock::mutex example
#include <iostream>       // std::cout
#include <thread>         // std::thread
#include <mutex>          // std::mutex, std::unique_lock, std::defer_lock

class MyMutex : public std::mutex {
  int _id;
public:
  MyMutex (int id) : _id(id) {}
  int id() {return _id;}
};

MyMutex mtx (101);

void print_ids (int id) {
  std::unique_lock<MyMutex> lck (mtx);
  std::cout << "thread #" << id << " locked mutex " << lck.mutex()->id() << '\n';
}

int main ()
{
  std::thread threads[10];
  // spawn 10 threads:
  for (int i=0; i<10; ++i)
    threads[i] = std::thread(print_ids,i+1);

  for (auto& th : threads) th.join();

  return 0;
}
```

| (三)、Othertypes其他类型 |

|                                                              |                                           |               |
| ------------------------------------------------------------ | ----------------------------------------- | ------------- |
| [**once_flag**](http://www.cplusplus.com/reference/mutex/once_flag/) | Flag argument type for call_once (class ) | once_flag     |
| [**adopt_lock_t**](http://www.cplusplus.com/reference/mutex/adopt_lock_t/) | Type of adopt_lock (class )               | adopt_lock_t  |
| [**defer_lock_t**](http://www.cplusplus.com/reference/mutex/defer_lock_t/) | Type of defer_lock (class )               | defer_lock_t  |
| [**try_to_lock_t**](http://www.cplusplus.com/reference/mutex/try_to_lock_t/) | Type of try_to_lock (class )              | try_to_lock_t |

- [**once_flag**](http://www.cplusplus.com/reference/mutex/once_flag/)
   此类型的对象用作call_once的参数。
   在不同的线程中使用相同的对象在不同的调用上调用call_once，如果同时调用，则会单个执行。
   它是一个不可复制的、不可移动的、可构造的类。

  struct once_flag {
   constexpr once_flag() noexcept;
   once_flag (const once_flag&) = delete;
   once_flag& operator= (const once_flag&) = delete;
   };

- [**adopt_lock_t**](http://www.cplusplus.com/reference/mutex/adopt_lock_t/)
   这是一个空的类，用作adopt_lock的使用的类型。
   向unique_lock或lock_guard的构造函数传递使用过的锁，使该对象不锁定互斥对象，并假设它已经被当前线程锁定。
   struct defer_lock_t {};   // 空类，只是作为adopt_lock类型

  

  ```cpp
  constexpr adopt_lock_t adopt_lock {}; 
  ```

定义adopt_lock 值用作对unique_lock或lock_guard的构造函数的可能参数。
 使用adopt_lock构造unique_lock对象不锁定构建中的互斥对象，只是假设它已经被当前线程锁定了；这个值是一个没有状态的编译时常量，只是用来消除构造函数签名之间的歧义。

- [**defer_lock_t**](http://www.cplusplus.com/reference/mutex/defer_lock_t/)
   这是一个空类，用作延迟锁的类型。
   将延迟锁传递给unique_lock的构造函数，使它不会在构造上自动锁定互斥对象，初始化对象为不拥有锁。
   struct defer_lock_t {};
   constexpr defer_lock_t defer_lock {};
   定义defer_lock用于unique_lock的构造函数的可能的参数。使用延迟锁构造的unique_lock对象不将互斥对象自动锁定在构造上，并初始化它们不拥有锁。同上面的一样，这个值是一个没有状态的编译时常量，它只是用来消除构造函数签名之间的歧义。
- [**try_to_lock_t**](http://www.cplusplus.com/reference/mutex/try_to_lock_t/)
   try_to_lock_t是用于try_to_lock类型的空类。
   将try_to_lock传递给unique_lock的构造函数，使它通过调用它的try_lock成员来锁定互斥对象，代替lock。
   struct try_to_lock_t {};
   constexpr try_to_lock_t try_to_lock {};
   try_to_lock 用于unique_lock的构造函数的可能的参数；使用try_to_lock构造的unique_lock对象试图通过调用其try_lock成员而不是锁成员来锁定互斥对象。

(四)、mutex中的函数

|                                                              |                                                   |            |
| :----------------------------------------------------------: | :-----------------------------------------------: | :--------: |
| [**try_lock**](http://www.cplusplus.com/reference/mutex/try_lock/) | Try to lock multiple mutexes (function template ) | 试锁互斥量 |
|  [**lock**](http://www.cplusplus.com/reference/mutex/lock/)  |    Lock multiple mutexes (function template )     |     锁     |
| [**call_once**](http://www.cplusplus.com/reference/mutex/call_once/) |   Call function once (public member function )    | call_once  |

- [**try_lock**](http://www.cplusplus.com/reference/mutex/try_lock/)
   std::try_lock是一个模版函数：



```c
      template <class Mutex1, class Mutex2, class... Mutexes>
      int try_lock (Mutex1& a, Mutex2& b, Mutexes&... cde);
```

使用try_lock成员函数(非阻塞)式的锁定对象a,b...等。函数为每个参数调用try_lock成员函数(首先是a，然后是b，最后是cde中的其他函数)，直到所有调用都是成功的，或者只要调用失败(返回false或抛出异常)。如果函数结束是因为调用失败，则对所有调用try_lock成功的对象调用解锁，函数将返回锁定失败的对象的参数序号。没有对参数列表中的其余对象执行其他调用。
 [示例15](https://github.com/jorionwen/threadtest/blob/master/mutex/example15.cpp)：



```c
// std::lock example
#include <iostream>       // std::cout
#include <thread>         // std::thread
#include <mutex>          // std::mutex, std::try_lock

std::mutex foo,bar;

void task_a () {
  foo.lock();
  std::cout << "task a\n";
  bar.lock();
  // ...
  foo.unlock();
  bar.unlock();
}

void task_b () {
  int x = try_lock(bar,foo);
  if (x==-1) {
    std::cout << "task b\n";
    // ...
    bar.unlock();
    foo.unlock();
  }
  else {
    std::cout << "[task b failed: mutex " << (x?"foo":"bar") << " locked]\n";
  }
}

int main ()
{
  std::thread th1 (task_a);
  std::thread th2 (task_b);

  th1.join();
  th2.join();

  return 0;
}
```

- [**lock**](http://www.cplusplus.com/reference/mutex/lock/)
   std::lock同样为一个模版函数



```c
      template <class Mutex1, class Mutex2, class... Mutexes>
      void lock (Mutex1& a, Mutex2& b, Mutexes&... cde);
```

锁定所有的参数互斥对象，阻塞当前调用线程；该函数使用一个未指定的调用序列来锁定对象，该序列调用其成员锁、try_lock和解锁，以确保所有参数都被锁定在返回(不产生任何死锁)。
 如果函数不能锁定所有对象(例如，因为其中一个内部调用抛出异常)，则函数首先解锁所有成功锁定的对象(如果有的话)。
 [示例16](https://github.com/jorionwen/threadtest/blob/master/mutex/example16.cpp):



```c
  // std::lock example
  #include <iostream>       // std::cout
  #include <thread>         // std::thread
  #include <mutex>          // std::mutex, std::lock

  std::mutex foo,bar;

  void task_a () {
    // foo.lock(); bar.lock(); // replaced by:
    std::lock (foo,bar);
    std::cout << "task a\n";
    foo.unlock();
    bar.unlock();
  }
  
  void task_b () {
    // bar.lock(); foo.lock(); // replaced by:
    std::lock (bar,foo);
    std::cout << "task b\n";
    bar.unlock();
    foo.unlock();
  }

  int main ()
  {
    std::thread th1 (task_a);
    std::thread th2 (task_b);

    th1.join();
    th2.join();

    return 0;
  }
```

- [**call_once**](http://www.cplusplus.com/reference/mutex/call_once/)
   std::call_once 公有模版函数



```c
       template <class Fn, class... Args>
          void call_once (once_flag& flag, Fn&& fn, Args&&... args);
```

call_once调用将args 作为fn的参数调用fn，除非另一个线程已经(或正在执行)使用相同的flag调用执行call_once。如果已经有一个线程使用相同flag调用call_once,会使得当前变为被动执行，所谓被动执行不执行fn也不返回直到恢复执行后返回。这这个时间点上所有的并发调用这个函数相同的flag都是同步的。
 注意,一旦一个活跃调用返回了,所有当前被动执行和未来可能的调用call_once相同相同的flag也还不会成为积极执行。
 [示例17](https://github.com/jorionwen/threadtest/blob/master/mutex/example17.cpp)：



```c
  // call_once example
  #include <iostream>       // std::cout
  #include <thread>         // std::thread, std::this_thread::sleep_for
  #include <chrono>         // std::chrono::milliseconds
  #include <mutex>          // std::call_once, std::once_flag

  int winner;
  void set_winner (int x) { winner = x; }
  std::once_flag winner_flag;

  void wait_1000ms (int id) {
    // count to 1000, waiting 1ms between increments:
    for (int i=0; i<1000; ++i)
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    // claim to be the winner (only the first such call is executed):
    std::call_once (winner_flag,set_winner,id);
  }

  int main ()
  {
    std::thread threads[10];
    // spawn 10 threads:
    for (int i=0; i<10; ++i)
      threads[i] = std::thread(wait_1000ms,i+1);

    std::cout << "waiting for the first among 10 threads to count 1000   ms...\n";

    for (auto& th : threads) th.join();
    std::cout << "winner thread: " << winner << '\n';
  
    return 0;
  }
```

