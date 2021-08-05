### std::unique_lock 介绍

但是 lock_guard 最大的缺点也是简单，没有给程序员提供足够的灵活度，因此，C++11 标准中定义了另外一个与 Mutex RAII 相关类 unique_lock，该类与 lock_guard 类相似，也很方便线程对互斥量上锁，但它提供了更好的上锁和解锁控制。

顾名思义，unique_lock 对象以独占所有权的方式（ unique owership）管理 mutex 对象的上锁和解锁操作，所谓独占所有权，就是没有其他的 unique_lock 对象同时拥有某个 mutex 对象的所有权。

在构造(或移动(move)赋值)时，unique_lock 对象需要传递一个 Mutex 对象作为它的参数，新创建的 unique_lock 对象负责传入的 Mutex 对象的上锁和解锁操作。

std::unique_lock 对象也能保证在其自身析构时它所管理的 Mutex 对象能够被正确地解锁（即使没有显式地调用 unlock 函数）。因此，和 lock_guard 一样，这也是一种简单而又安全的上锁和解锁方式，尤其是在程序抛出异常后先前已被上锁的 Mutex 对象可以正确进行解锁操作，极大地简化了程序员编写与 Mutex 相关的异常处理代码。

值得注意的是，unique_lock 对象同样也不负责管理 Mutex 对象的生命周期，unique_lock 对象只是简化了 Mutex 对象的上锁和解锁操作，方便线程对互斥量上锁，即在某个 unique_lock 对象的声明周期内，它所管理的锁对象会一直保持上锁状态；而 unique_lock 的生命周期结束之后，它所管理的锁对象会被解锁，这一点和 lock_guard 类似，但 unique_lock 给程序员提供了更多的自由，我会在下面的内容中给大家介绍 unique_lock 的用法。

另外，与 lock_guard 一样，模板参数 Mutex 代表互斥量类型，例如 std::mutex 类型，它应该是一个基本的 BasicLockable 类型，标准库中定义几种基本的 BasicLockable 类型，分别 std::mutex, std::recursive_mutex, std::timed_mutex，std::recursive_timed_mutex (以上四种类型均已在上一篇博客中介绍)以及 std::unique_lock(本文后续会介绍 std::unique_lock)。(注：BasicLockable 类型的对象只需满足两种操作，lock 和 unlock，另外还有 Lockable 类型，在 BasicLockable 类型的基础上新增了 try_lock 操作，因此一个满足 Lockable 的对象应支持三种操作：lock，unlock 和 try_lock；最后还有一种 TimedLockable 对象，在 Lockable 类型的基础上又新增了 try_lock_for 和 try_lock_until 两种操作，因此一个满足 TimedLockable 的对象应支持五种操作：lock, unlock, try_lock, try_lock_for, try_lock_until)。

### std::unique_lock 构造函数

std::unique_lock 的构造函数的数目相对来说比 std::lock_guard 多，其中一方面也是因为 std::unique_lock 更加灵活，从而在构造 std::unique_lock 对象时可以接受额外的参数。总地来说，std::unique_lock 构造函数如下：

| default (1)        | `unique_lock() noexcept; `                                   |
| ------------------ | ------------------------------------------------------------ |
| locking (2)        | `explicit unique_lock(mutex_type& m); `                      |
| try-locking (3)    | `unique_lock(mutex_type& m, try_to_lock_t tag); `            |
| deferred (4)       | `unique_lock(mutex_type& m, defer_lock_t tag) noexcept; `    |
| adopting (5)       | `unique_lock(mutex_type& m, adopt_lock_t tag); `             |
| locking for (6)    | `template <class Rep, class Period> unique_lock(mutex_type& m, const chrono::duration<Rep,Period>& rel_time); ` |
| locking until (7)  | `template <class Clock, class Duration> unique_lock(mutex_type& m, const chrono::time_point<Clock,Duration>& abs_time); ` |
| copy [deleted] (8) | `unique_lock(const unique_lock&) = delete; `                 |
| move (9)           | `unique_lock(unique_lock&& x);`                              |

下面我们来分别介绍以上各个构造函数：

- (1) 默认构造函数

  新创建的 unique_lock 对象不管理任何 Mutex 对象。 

- (2) locking 初始化

  新创建的 unique_lock 对象管理 Mutex 对象 m，并尝试调用 m.lock() 对 Mutex 对象进行上锁，如果此时另外某个 unique_lock 对象已经管理了该 Mutex 对象 m，则当前线程将会被阻塞。

- (3) try-locking 初始化

  新创建的 unique_lock 对象管理 Mutex 对象 m，并尝试调用 m.try_lock() 对 Mutex 对象进行上锁，但如果上锁不成功，并不会阻塞当前线程。

- (4) deferred 初始化

  新创建的 unique_lock 对象管理 Mutex 对象 m，但是在初始化的时候并不锁住 Mutex 对象。 m 应该是一个没有当前线程锁住的 Mutex 对象。

- (5) adopting 初始化

  新创建的 unique_lock 对象管理 Mutex 对象 m， m 应该是一个已经被当前线程锁住的 Mutex 对象。(并且当前新创建的 unique_lock 对象拥有对锁(Lock)的所有权)。

- (6) locking 一段时间(duration)

  新创建的 unique_lock 对象管理 Mutex 对象 m，并试图通过调用 m.try_lock_for(rel_time) 来锁住 Mutex 对象一段时间(rel_time)。

- (7) locking 直到某个时间点(time point)

  新创建的 unique_lock 对象管理 Mutex 对象m，并试图通过调用 m.try_lock_until(abs_time) 来在某个时间点(abs_time)之前锁住 Mutex 对象。

- (8) 拷贝构造 [被禁用]

  unique_lock 对象不能被拷贝构造。

- (9) 移动(move)构造

  新创建的 unique_lock 对象获得了由 x 所管理的 Mutex 对象的所有权(包括当前 Mutex 的状态)。调用 move 构造之后， x 对象如同通过默认构造函数所创建的，就不再管理任何 Mutex 对象了。

综上所述，由 (2) 和 (5) 创建的 unique_lock 对象通常拥有 Mutex 对象的锁。而通过 (1) 和 (4) 创建的则不会拥有锁。通过 (3)，(6) 和 (7) 创建的 unique_lock 对象，则在 lock 成功时获得锁。

关于unique_lock 的构造函数，请看下面例子([参考](http://www.cplusplus.com/reference/mutex/unique_lock/unique_lock/))：



```c++
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



### std::unique_lock 移动(move assign)赋值操作

std::unique_lock 支持移动赋值(move assignment)，但是普通的赋值被禁用了，

| move (1)           | `unique_lock& operator= (unique_lock&& x) noexcept; `   |
| ------------------ | ------------------------------------------------------- |
| copy [deleted] (2) | `unique_lock& operator= (const unique_lock&) = delete;` |

移动赋值(move assignment)之后，由 x 所管理的 Mutex 对象及其状态将会被新的 std::unique_lock 对象取代。

如果被赋值的对象之前已经获得了它所管理的 Mutex 对象的锁，则在移动赋值(move assignment)之前会调用 unlock 函数释放它所占有的锁。

调用移动赋值(move assignment)之后， x 对象如同通过默认构造函数所创建的，也就不再管理任何 Mutex 对象了。请看下面例子([参考](http://www.cplusplus.com/reference/mutex/unique_lock/operator=/))：



```c++
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



### std::unique_lock 主要成员函数

本节我们来看看 std::unique_lock 的主要成员函数。由于 std::unique_lock 比 std::lock_guard 操作灵活，因此它提供了更多成员函数。具体分类如下：

1. 上锁/解锁操作：lock，try_lock，try_lock_for，try_lock_until 和 unlock
2. 修改操作：移动赋值(move assignment)(前面已经介绍过了)，交换(swap)（与另一个 std::unique_lock 对象交换它们所管理的 Mutex 对象的所有权），释放(release)（返回指向它所管理的 Mutex 对象的指针，并释放所有权）
3. 获取属性操作：owns_lock（返回当前 std::unique_lock 对象是否获得了锁）、operator bool()（与 owns_lock 功能相同，返回当前 std::unique_lock 对象是否获得了锁）、mutex（返回当前 std::unique_lock 对象所管理的 Mutex 对象的指针）。

**std::unique_lock::lock**请看下面例子([参考](http://www.cplusplus.com/reference/mutex/unique_lock/lock/))：

上锁操作，调用它所管理的 Mutex 对象的 lock 函数。如果在调用 Mutex 对象的 lock 函数时该 Mutex 对象已被另一线程锁住，则当前线程会被阻塞，直到它获得了锁。

该函数返回时，当前的 unique_lock 对象便拥有了它所管理的 Mutex 对象的锁。如果上锁操作失败，则抛出 system_error 异常。



```c++
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



**std::unique_lock::try_lock**

上锁操作，调用它所管理的 Mutex 对象的 try_lock 函数，如果上锁成功，则返回 true，否则返回 false。

请看下面例子([参考](http://www.cplusplus.com/reference/mutex/unique_lock/try_lock/))：



```c++
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

  return 0;
}
```



**std::unique_lock::try_lock_for**

上锁操作，调用它所管理的 Mutex 对象的 try_lock_for 函数，如果上锁成功，则返回 true，否则返回 false。

请看下面例子([参考](http://www.cplusplus.com/reference/mutex/unique_lock/try_lock_for/))：



```c++
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



**std::unique_lock::try_lock_until**

上锁操作，调用它所管理的 Mutex 对象的 try_lock_for 函数，如果上锁成功，则返回 true，否则返回 false。

请看下面例子([参考](http://www.cplusplus.com/reference/mutex/unique_lock/try_lock_until/))：



```c++
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



**std::unique_lock::unlock**

解锁操作，调用它所管理的 Mutex 对象的 unlock 函数。

请看下面例子(参考)：



```c++
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



**std::unique_lock::release**

返回指向它所管理的 Mutex 对象的指针，并释放所有权。

请看下面例子([参考](http://www.cplusplus.com/reference/mutex/unique_lock/release/))：



```c++
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



**std::unique_lock::owns_lock**

返回当前 std::unique_lock 对象是否获得了锁。

请看下面例子(参考)：



```c++
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

  return 0;
}
```



**std::unique_lock::operator bool()

**

与 owns_lock 功能相同，返回当前 std::unique_lock 对象是否获得了锁。

请看下面例子([参考](http://www.cplusplus.com/reference/mutex/unique_lock/operator_bool/))：



```c++
#include <iostream>       // std::cout
#include <vector>         // std::vector
#include <thread>         // std::thread
#include <mutex>          // std::mutex, std::unique_lock, std::try_to_lock

std::mutex mtx;           // mutex for critical section

void print_star () {
  std::unique_lock<std::mutex> lck(mtx,std::try_to_lock);
  // print '*' if successfully locked, 'x' otherwise: 
  if (lck)
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

  return 0;
}
```



**std::unique_lock::mutex

**

返回当前 std::unique_lock 对象所管理的 Mutex 对象的指针。

请看下面例子([参考](http://www.cplusplus.com/reference/mutex/unique_lock/mutex/))：



```c++
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



