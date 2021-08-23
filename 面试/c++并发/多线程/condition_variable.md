## 一、 <condition_variable>头文件结构

<condition_variable>头文件主要包含了与条件变量相关的类和函数。主要含有类condition_variable和condition_variable_any，枚举cv_status，函数notify_all_at_thread_exit。

| Classes                                                      | description                             |
| ------------------------------------------------------------ | --------------------------------------- |
| [**condition_variable**](http://www.cplusplus.com/reference/condition_variable/condition_variable/) | 提供了std::unique_lock 相关联的条件变量 |
| [**condition_variable_any**](http://www.cplusplus.com/reference/condition_variable/condition_variable_any/) | 提供与任何锁类型相关联的条件变量        |

| Enum classes                                                 | description                              |
| ------------------------------------------------------------ | ---------------------------------------- |
| [**cv_status**](http://www.cplusplus.com/reference/condition_variable/cv_status/) | 列出在条件变量上限时等待的可能结果(枚举) |

| Functions                                                    | description                                          |
| ------------------------------------------------------------ | ---------------------------------------------------- |
| [**notify_all_at_thread_exit**](http://www.cplusplus.com/reference/condition_variable/notify_all_at_thread_exit/) | 当这个线程完全完成(函数)时，调度调用notify_all来调用 |

结构定义类似：



```c
namespace std {
 
    class condition_variable;
    class condition_variable_any;
 
    void notify_all_at_thread_exit(condition_variable& cond, unique_lock<mutex> lk);
 
    enum class cv_status {
        no_timeout,
        timeout
    };
}
```

## 二、Classes

### 1. class condition_variable;

- 条件变量类是一个同步原语，它可以用来阻塞一个线程或多个线程，直到另一个线程同时修改一个共享变量(条件)，并通知条件变量。
- 条件变量是能够阻塞调用线程的对象，直到通知恢复为止。
   它使用unique_lock(在互斥锁上)来锁定线程，当它的一个等待函数被调用时。线程仍然被阻塞，直到被另一个线程唤醒，该线程调用同一个条件变量对象上的通知函数。
- 类型条件变量的对象总是使用unique_lock < mutex >等待:对于可以使用任何类型的可锁定类型的选项，参见condition_variable_any。
   [示例1](https://github.com/jorionwen/threadtest/blob/master/condition_variable/example1.cpp)：



```c
// condition_variable example
#include <iostream>           // std::cout
#include <thread>             // std::thread
#include <mutex>              // std::mutex, std::unique_lock
#include <condition_variable> // std::condition_variable

std::mutex mtx;
std::condition_variable cv;
bool ready = false;

void print_id (int id) {
  std::unique_lock<std::mutex> lck(mtx);
  while (!ready) cv.wait(lck);
  // ...
  std::cout << "thread " << id << '\n';
}

void go() {
  std::unique_lock<std::mutex> lck(mtx);
  ready = true;
  cv.notify_all();
}

int main ()
{
  std::thread threads[10];
  // spawn 10 threads:
  for (int i=0; i<10; ++i)
    threads[i] = std::thread(print_id,i);

  std::cout << "10 threads ready to race...\n";
  go();                       // go!

  for (auto& th : threads) th.join();

  return 0;
}
```

[示例2](https://github.com/jorionwen/threadtest/blob/master/condition_variable/example2.cpp)：



```c
#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
 
std::mutex m;
std::condition_variable cv;
std::string data;
bool ready = false;
bool processed = false;
 
void worker_thread()
{
    // Wait until main() sends data
    std::unique_lock<std::mutex> lk(m);
    cv.wait(lk, []{return ready;});
 
    // after the wait, we own the lock.
    std::cout << "Worker thread is processing data\n";
    data += " after processing";
 
    // Send data back to main()
    processed = true;
    std::cout << "Worker thread signals data processing completed\n";
 
    // Manual unlocking is done before notifying, to avoid waking up
    // the waiting thread only to block again (see notify_one for details)
    lk.unlock();
    cv.notify_one();
}
 
int main()
{
    std::thread worker(worker_thread);
 
    data = "Example data";
    // send data to the worker thread
    {
        std::lock_guard<std::mutex> lk(m);
        ready = true;
        std::cout << "main() signals data ready for processing\n";
    }
    cv.notify_one();
 
    // wait for the worker
    {
        std::unique_lock<std::mutex> lk(m);
        cv.wait(lk, []{return processed;});
    }
    std::cout << "Back in main(), data = " << data << '\n';
 
    worker.join();
    return 0;
}
```

#### 1. Member functions

1. [**(constructor)构造函数**](http://www.cplusplus.com/reference/condition_variable/condition_variable/condition_variable/)
    定义：



```c
       condition_variable();                                      default (1)
       condition_variable (const condition_variable&) = delete;   copy [deleted] (2)
       operator=   [deleted]
```

\*  构造一个condition_variable对象。
 \*  条件变量不能被复制/移动(这个类型的拷贝构造函数和赋值操作符都被删除)。
 \*  如果构建失败，将抛出system_error异常, 根据库的实现，它可能会在其他情况下抛出异常。

| exception type                                        | error condition                                              | description                      |
| ----------------------------------------------------- | ------------------------------------------------------------ | -------------------------------- |
| [system_error](http://www.cplusplus.com/system_error) | [errc::resource_unavailable_try_again](http://www.cplusplus.com/errc) | 资源限制(除了内存分配)防止初始化 |

1. [**(destructor)析构函数**](http://www.cplusplus.com/reference/condition_variable/condition_variable/~condition_variable/)
    \* 破坏condition_variable对象。
    \* 在此条件下被阻塞的任何线程在调用此析构函数之前都将被通知。在这个析构函数被调用之后，没有线程将开始等待。
    **注意**：
    *1. 如果所有线程都被通知，调用析构函数是安全的。它们不需要退出各自的等待函数:一些线程可能还在等待重新获得相关的锁，或者等待调度在重新获得它之后运行。*
    *2.程序员必须确保在启动析构函数时，没有线程试图等待\*this，特别是当等待线程调用循环中的等待函数或使用执行谓词的等待函数的重载时。*

#### 2. Wait functions

1. [**std::condition_variable::wait**](http://www.cplusplus.com/reference/condition_variable/condition_variable/wait/)
    void wait (unique_lock<mutex>& lck);                    unconditional (1)
    template <class Predicate>                              predicate (2)
    void wait (unique_lock<mutex>& lck, Predicate pred);
    1、 当前线程的执行(该线程将锁定lck的互斥锁)将被阻塞，直到被通知。
    2、 在阻塞线程的时刻，该函数将自动调用lck.unlock()，允许其他锁定的线程继续运行。
    3、 一旦通知(显式地，通过其他线程)，函数就unblock并调用lck. lock()，在调用函数时将lck留在相同的状态。然后函数返回(注意，最后一个互斥锁可能会在返回之前阻塞线程)。
    4、一般情况下，函数会被另一个线程的调用唤醒，无论是对成员notify_one还是成员notify_all。但是某些实现可能产生虚假的唤醒调用，而不需要调用这些函数。因此，该函数的用户将确保满足恢复的条件。
    5、 定义(2)如果指定pred,如果pred返回false，则函数只会阻塞，并且只有当它变为true时，通知才能解除线程(这对于检查伪唤醒调用特别有用)。
    类似这样的实现: `while (!pred()) wait(lck);`
    [示例3](https://github.com/jorionwen/threadtest/blob/master/condition_variable/example3.cpp)：



```c
  // condition_variable::wait (with predicate)
  #include <iostream>           // std::cout
  #include <thread>             // std::thread, std::this_thread::yield
  #include <mutex>              // std::mutex, std::unique_lock
  #include <condition_variable> // std::condition_variable

  std::mutex mtx;
  std::condition_variable cv;

  int cargo = 0;
  bool shipment_available() {return cargo!=0;}

  void consume (int n) {
    for (int i=0; i<n; ++i) {
      std::unique_lock<std::mutex> lck(mtx);
      cv.wait(lck,shipment_available);
      // consume:
      std::cout << cargo << '\n';
      cargo=0;
    }
  }

  int main ()
  {
    std::thread consumer_thread (consume,10);

    // produce 10 items when needed:
    for (int i=0; i<10; ++i) {
      while (shipment_available()) std::this_thread::yield();
      std::unique_lock<std::mutex> lck(mtx);
      cargo = i+1;
      cv.notify_one();
    }

    consumer_thread.join();

    return 0;
  }
```

[示例4](https://github.com/jorionwen/threadtest/blob/master/condition_variable/example4.cpp)：



```php
  #include <iostream>
  #include <condition_variable>
  #include <thread>
  #include <chrono>
 
  std::condition_variable cv;
  std::mutex cv_m; // This mutex is used for three purposes:
                 // 1) to synchronize accesses to i
                 // 2) to synchronize accesses to std::cerr
                 // 3) for the condition variable cv
  int i = 0;
 
  void waits()
  {
      std::unique_lock<std::mutex> lk(cv_m);
      std::cerr << "Waiting... \n";
      cv.wait(lk, []{return i == 1;});
      std::cerr << "...finished waiting. i == 1\n";
  }
 
  void signals()
  {
      std::this_thread::sleep_for(std::chrono::seconds(1));
      {
          std::lock_guard<std::mutex> lk(cv_m);
          std::cerr << "Notifying...\n";
      }
      cv.notify_all();
 
      std::this_thread::sleep_for(std::chrono::seconds(1));
      {
          std::lock_guard<std::mutex> lk(cv_m);
          i = 1;
          std::cerr << "Notifying again...\n";
      }
      cv.notify_all();
  }
 
  int main()
  {
      std::thread t1(waits), t2(waits), t3(waits), t4(signals);
      t1.join(); 
      t2.join(); 
      t3.join();
     t4.join();
      return 0;
  }
```

1. [**std::condition_variable::wait_for**](http://www.cplusplus.com/reference/condition_variable/condition_variable/wait_for/)



```c
        (1) unconditional   
        template <class Rep, class Period>
            cv_status wait_for (unique_lock<mutex>& lck,
                      const chrono::duration<Rep,Period>& rel_time);
        (2) predicate   
        template <class Rep, class Period, class Predicate>
           bool wait_for (unique_lock<mutex>& lck,
                      const chrono::duration<Rep,Period>& rel_time, Predicate pred);
```

1、当前执行线程在rel_time时间内或者在被通知之前(该线程将锁定lck的互斥锁)被阻塞(如果后者先发生的话)。
 2、在阻塞线程的时刻，该函数将自动调用lck.解锁()，允许其他锁定的线程继续运行。
 3、一旦通知或一次rel_time时间段已经过了，函数将会unblocks并调用lck. lock()，将lck与调用函数时的状态保持一致。然后函数返回(注意，最后一个互斥锁可能会在返回之前阻塞线程)。
 4、一般情况下，函数会被另一个线程的调用唤醒，无论是对成员notify_one还是成员notify_all。但是某些实现可能产生虚假的唤醒调用，而不需要调用这些函数。因此，该函数的用户将确保满足恢复的条件。
 5、如果指定pred(定义2)，如果pred返回false，则函数只会阻塞，并且只有当它变为true时，通知才能解除线程(这对于检查伪唤醒调用特别有用)。它的行为好像是:
 `return wait_until (lck, chrono::steady_clock::now() + rel_time, std::move(pred));`
 [示例5](https://github.com/jorionwen/threadtest/blob/master/condition_variable/example5.cpp)：



```c
  // condition_variable::wait_for example
  #include <iostream>           // std::cout
  #include <thread>             // std::thread
  #include <chrono>             // std::chrono::seconds
  #include <mutex>              // std::mutex, std::unique_lock
  #include <condition_variable> // std::condition_variable, std::cv_status

  std::condition_variable cv;

  int value;

  void read_value() {
    std::cin >> value;
    cv.notify_one();
  }

  int main ()
  {
    std::cout << "Please, enter an integer (I'll be printing dots): \n";
    std::thread th (read_value);

    std::mutex mtx;
    std::unique_lock<std::mutex> lck(mtx);
    while (cv.wait_for(lck,std::chrono::seconds(1))==std::cv_status::timeout) {
      std::cout << '.' << std::endl;
    }
    std::cout << "You entered: " << value << '\n';

    th.join();

    return 0;
  }
```

[示例6](https://github.com/jorionwen/threadtest/blob/master/condition_variable/example6.cpp)：



```php
  #include <iostream>
  #include <atomic>
  #include <condition_variable>
  #include <thread>
  #include <chrono>
  //using namespace std::chrono_literals;
 
  std::condition_variable cv;
  std::mutex cv_m;
  int i;
 
  void waits(int idx)
  {
      std::unique_lock<std::mutex> lk(cv_m);
      if(cv.wait_for(lk, idx*std::chrono::milliseconds(100), []{return i == 1;})) 
          std::cerr << "Thread " << idx << " finished waiting. i == " << i << '\n';
      else
          std::cerr << "Thread " << idx << " timed out. i == " << i << '\n';
  }
 
  void signals()
  {
      std::this_thread::sleep_for(std::chrono::milliseconds(120));
      std::cerr << "Notifying...\n";
      cv.notify_all();
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      {
          std::lock_guard<std::mutex> lk(cv_m);
          i = 1;
      }
      std::cerr << "Notifying again...\n";
      cv.notify_all();
  }
 
  int main()
  {
      std::thread t1(waits, 1), t2(waits, 2), t3(waits, 3), t4(signals);
      t1.join(); t2.join(), t3.join(), t4.join();
      return 0;
  }
```

1. [**std::condition_variable::wait_until**](http://www.cplusplus.com/reference/condition_variable/condition_variable/wait_until/)



```c
        (1) unconditional   
        template <class Clock, class Duration>
            cv_status wait_until (unique_lock<mutex>& lck,
                        const chrono::time_point<Clock,Duration>& abs_time);
        (2) predicate   
        template <class Clock, class Duration, class Predicate>
             bool wait_until (unique_lock<mutex>& lck,
                        const chrono::time_point<Clock,Duration>& abs_time,
                        Predicate pred);
```

1、当前线程的执行(该线程将锁定lck的互斥锁)会被阻塞，直到被通知或到达abs_time时间点，无论那一个第一次发生。
 2、在线程还处在阻塞时，该函数将自动调用lck.unlock()，允许其他锁定的线程继续运行。
 3、一旦被通知或一旦abs_time时间到了，函数就会接触阻塞状态并调用lck. lock()。在调用函数时将lck与函数的状态保持在同一状态。然后函数返回(注意，最后一个互斥锁可能会在返回之前阻塞线程)。
 4、一般情况下，函数会被另一个线程的调用唤醒，无论是对成员notify_one还是成员notify_all。但是某些实现可能产生虚假的唤醒调用，而不需要调用这些函数。因此，该函数的使用者将要确保满足恢复的条件。
 5、(定义2)如果指定pred，如果pred返回false，则函数只会阻塞，并且只有当它变为true时，通知才能解除线程(这对于检查伪唤醒调用特别有用)。它的行为好像是:



```c
while (!pred())
      if ( wait_until(lck,abs_time) == cv_status::timeout)
          return pred();
return true;
```

[示例7](https://github.com/jorionwen/threadtest/blob/master/condition_variable/example7.cpp)：



```php
  #include <iostream>
  #include <atomic>
  #include <condition_variable>
  #include <thread>
  #include <chrono>
  //using namespace std::chrono_literals;
 
  std::condition_variable cv;
  std::mutex cv_m;
  std::atomic<int> i{0};
 
  void waits(int idx)
  {
      std::unique_lock<std::mutex> lk(cv_m);
      auto now = std::chrono::system_clock::now();
      if(cv.wait_until(lk, now + idx*std::chrono::milliseconds(100), [](){return i == 1;}))
          std::cerr << "Thread " << idx << " finished waiting. i == " << i << '\n';
      else
          std::cerr << "Thread " << idx << " timed out. i == " << i << '\n';
  }
 
  void signals()
  {
      std::this_thread::sleep_for(std::chrono::milliseconds(120));
      std::cerr << "Notifying...\n";
      cv.notify_all();
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      i = 1;
      std::cerr << "Notifying again...\n";
      cv.notify_all();
  }
 
  int main()
  {
      std::thread t1(waits, 1), t2(waits, 2), t3(waits, 3), t4(signals);
      t1.join(); 
      t2.join();
      t3.join();
      t4.join();
      return 0;
  }
```

#### 3. Notify functions

1. [**std::condition_variable::notify_one**](http://www.cplusplus.com/reference/condition_variable/condition_variable/notify_one/)
    void notify_one() noexcept;      //定义
    1、打开当前等待该条件的一个线程。如果没有线程在等待，则该函数什么都不做。如果多于一个，则不指定哪个线程被选中。
    2、该指令针对这个单独的条件变量。这使得notify_one()不可能被延迟，并且在调用notify_one()之后才开始等待。
    3、通知线程不需要将锁保存在与等待线程所持有的相同互斥锁上;事实上，这样做是一个悲观的方法，因为通知的线程会立即阻塞，等待通知线程释放锁。然而，一些实现(特别是许多pthreads实现)识别这种情况，并避免这种“匆忙而等待”的场景，通过将等待的线程从条件变量的队列直接传递到通知调用的互斥锁的队列中，而不将其唤醒。
    [示例8](https://github.com/jorionwen/threadtest/blob/master/condition_variable/example8.cpp)：



```c
  // condition_variable::notify_one
  #include <iostream>           // std::cout
  #include <thread>             // std::thread
  #include <mutex>              // std::mutex, std::unique_lock
  #include <condition_variable> // std::condition_variable

  std::mutex mtx;
  std::condition_variable produce,consume;

  int cargo = 0;     // shared value by producers and consumers

  void consumer () {
    std::unique_lock<std::mutex> lck(mtx);
    while (cargo==0) consume.wait(lck);
    std::cout << cargo << '\n';
    cargo=0;
    produce.notify_one();
  }

  void producer (int id) {
    std::unique_lock<std::mutex> lck(mtx);
    while (cargo!=0) produce.wait(lck);
    cargo = id;
    consume.notify_one();
  }

  int main ()
  {
    std::thread consumers[10],producers[10];
    // spawn 10 consumers and 10 producers:
    for (int i=0; i<10; ++i) {
      consumers[i] = std::thread(consumer);
      producers[i] = std::thread(producer,i+1);
    }

    // join them back:
    for (int i=0; i<10; ++i) {
      producers[i].join();
      consumers[i].join();
    }

    return 0;
 }
```

[示例9](https://github.com/jorionwen/threadtest/blob/master/condition_variable/example9.cpp)：



```php
  #include <iostream>
  #include <condition_variable>
  #include <thread>
  #include <chrono>
 
  std::condition_variable cv;
  std::mutex cv_m;
  int i = 0;
  bool done = false;
 
  void waits()
  {
      std::unique_lock<std::mutex> lk(cv_m);
      std::cout << "Waiting... \n";
      cv.wait(lk, []{return i == 1;});
      std::cout << "...finished waiting. i == 1\n";
      done = true;
  }
 
  void signals()
  {
      std::this_thread::sleep_for(std::chrono::seconds(1));
      std::cout << "Notifying falsely...\n";
      cv.notify_one(); // waiting thread is notified with i == 0.
                       // cv.wait wakes up, checks i, and goes back to waiting
 
      std::unique_lock<std::mutex> lk(cv_m);
      i  = 1;
      while (!done) 
      {
          std::cout << "Notifying true change...\n";
          lk.unlock();
          cv.notify_one(); // waiting thread is notified with i == 1, cv.wait     returns
          std::this_thread::sleep_for(std::chrono::seconds(1));
          lk.lock();
      }
  }
 
  int main()
  {
      std::thread t1(waits), t2(signals);
      t1.join(); 
      t2.join();
      return 0;
  }
```

1. [**std::condition_variable::notify_all**](http://www.cplusplus.com/reference/condition_variable/condition_variable/notify_all/)
    void notify_all() noexcept;  // 定义
    解锁当前等待该条件的所有线程。如果没有线程在等待，则该函数什么都不做。
    **注意：**
    *1、notify_one()/notify_all()和wait()/wait_for()/wait_until() 三个原子部分中的每一个影响都在一个完全的顺序(解锁+等待, 唤醒, 锁)中，可以被视为一个原子变量的修改顺序:该命令是特定于这个单独的条件变量的。这使得notify_one()不可能被延迟，并且在调用notify_one()之后才开始等待。
    2、通知线程不需要将锁保存在与等待线程所持有的相同互斥锁上;事实上，这样做是一个悲观的方法，因为通知的线程会立即阻塞，等待通知线程释放锁。*
    [示例10](https://github.com/jorionwen/threadtest/blob/master/condition_variable/example10.cpp)：



```c
  // condition_variable::notify_all
  #include <iostream>           // std::cout
  #include <thread>             // std::thread
  #include <mutex>              // std::mutex, std::unique_lock
  #include <condition_variable> // std::condition_variable

  std::mutex mtx;
  std::condition_variable cv;
  bool ready = false;

  void print_id (int id) {
    std::unique_lock<std::mutex> lck(mtx);
    while (!ready) cv.wait(lck);
    // ...
    std::cout << "thread " << id << '\n';
  }

  void go() {
    std::unique_lock<std::mutex> lck(mtx);
    ready = true;
    cv.notify_all();
  }

  int main ()
  {
    std::thread threads[10];
    // spawn 10 threads:
    for (int i=0; i<10; ++i)
      threads[i] = std::thread(print_id,i);

    std::cout << "10 threads ready to race...\n";
    go();                       // go!

    for (auto& th : threads) th.join();

    return 0;
  }
```

[示例11](https://github.com/jorionwen/threadtest/blob/master/condition_variable/example11.cpp)：



```php
  #include <iostream>
  #include <condition_variable>
  #include <thread>
  #include <chrono>
 
  std::condition_variable cv;
  std::mutex cv_m; // This mutex is used for three purposes:
                   // 1) to synchronize accesses to i
                   // 2) to synchronize accesses to std::cerr
                   // 3) for the condition variable cv
  int i = 0;
 
  void waits()
  {
      std::unique_lock<std::mutex> lk(cv_m);
      std::cerr << "Waiting... \n";
      cv.wait(lk, []{return i == 1;});
      std::cerr << "...finished waiting. i == 1\n";
  }
   
  void signals()
  {
      std::this_thread::sleep_for(std::chrono::seconds(1));
      {
          std::lock_guard<std::mutex> lk(cv_m);
          std::cerr << "Notifying...\n";
      }
      cv.notify_all();
 
      std::this_thread::sleep_for(std::chrono::seconds(1));
 
      {
          std::lock_guard<std::mutex> lk(cv_m);
          i = 1;
          std::cerr << "Notifying again...\n";
      }
      cv.notify_all();
  }
 
  int main()
  {
      std::thread t1(waits), t2(waits), t3(waits), t4(signals);
      t1.join(); 
      t2.join(); 
      t3.join();
      t4.join();
      return 0;
  }
```

### 2.  class condition_variable_any;

1、与条件变量一样，除了它的等待函数可以将任何可锁类型作为参数(条件变量对象只能使用unique_lock < mutex >)。除此之外，它们是相同的。

2、std::condition_variable_any是std::condition_variable的泛化。而std::condition_variable 只能用于std::unique_lock < std::mutex >，condition_variable_any可以操作任何满足基本要求的锁。参见std::condition_变量，用于描述条件变量的语义。类std::condition_variable_any是一个标准布局类。它不可拷贝构造的，移动可构造的，不可拷贝复制和移动复制。

3、如果锁是std::unique_lock,std::condition_variable可以提供更好的性能。
 **注意：**
 *std::condition_variable_any与自定义的可锁类型是用来提供方便的可中断等待:自定义锁操作会像预期的那样锁定相关的互斥对象，并且在接收到中断信号时也执行必要的设置来通知这个条件变量。*

#### 1. Member functions

1. [**(constructor)构造函数**](http://www.cplusplus.com/reference/condition_variable/condition_variable_any/condition_variable_any/)
    default (1)     condition_variable_any();
    copy [deleted] (2) condition_variable_any (const condition_variable_any&) = delete;
    构造std::condition_variable_any类型的对象. 不能通过拷贝和移动构造(这个类型的拷贝构造函数和赋值操作符都被删除)。

**注意：**
 *如果构造失败，会抛出一个系统异常：
*

![img](https:////upload-images.jianshu.io/upload_images/3414858-4e59414dedf0fc61.png?imageMogr2/auto-orient/strip|imageView2/2/w/503/format/webp)



*根据库的实现，它可能会在其他情况下抛出异常如[bad_alloc](http://www.cplusplus.com/bad_alloc)无法分配内存。
 operator= [deleted] 不允许复制。*



1. [**(destructor)析构函数**](http://www.cplusplus.com/reference/condition_variable/condition_variable_any/~condition_variable_any/)
    析构condition_variable_any对象。在此条件下被阻塞的任何线程在调用此析构函数之前都将被通知。在这个析构函数被调用之后，没有线程将开始等待。
    **注意：**
    *1、 如果所有线程都被通知，调用析构函数是安全的。它们不需要退出各自的等待函数:一些线程可能还在等待重新获得相关的锁，或者等待调度在重新获得它之后运行。
    2、 程序员必须确保在启动析构函数时，没有线程试图等待，特别是当等待线程调用循环中的等待函数或使用执行谓词的等待函数的重载时。*

#### 2. Wait functions

1. [**std::condition_variable_any::wait**]
    ([http://www.cplusplus.com/reference/condition_variable/condition_variable_any/wait/](http://www.cplusplus.com/reference/condition_variable/condition_variable_any/wait/))



```c
        unconditional (1)   template <class Lock> void wait (Lock& lck);
        predicate (2)       template <class Lock, class Predicate>
                                  void wait (Lock& lck, Predicate pred);
```

1、当前线程（当前正在锁定lck）的执行将被阻止，直到通知为止。
 2、在阻止线程的时刻，该函数自动调用lck.unlock()，允许其他锁定线程继续。
 3、一旦通知（显式地，通过某个其他线程），函数解除阻塞和调用lck.lock()，使lck处于与调用函数时相同的状态。然后函数返回（注意，这个最后的互斥锁定可能在返回之前再次阻止线程）。
 4、通常，该函数被通知通过另一个线程的呼叫唤醒成员notify_one 或 notify_all。但是某些实现可能会产生虚假的唤醒呼叫，而不会调用这些功能。因此，该功能的用户应确保其恢复条件得到满足。
 5、定义(2): 如果指定pred，如果pred返回false，则函数只会阻塞，并且只有当它变为true时，通知才能解除线程(这对于检查伪唤醒调用特别有用)。表现为：



```c
         while  （! pred （）） { 
              wait （ lock ）; 
         }
```

[示例12](https://github.com/jorionwen/threadtest/blob/master/condition_variable/example12.cpp)：



```c
  // condition_variable_any::wait (with predicate)
  #include <iostream>           // std::cout
  #include <thread>             // std::thread, std::this_thread::yield
  #include <mutex>              // std::mutex
  #include <condition_variable> // std::condition_variable_any

  std::mutex mtx;
  std::condition_variable_any cv;

  int cargo = 0;
  bool shipment_available() {return cargo!=0;}

  void consume (int n) {
    for (int i=0; i<n; ++i) {
      mtx.lock();
      cv.wait(mtx,shipment_available);
      // consume:
      std::cout << cargo << '\n';
      cargo=0;
      mtx.unlock();
    }
  }

  int main ()
  {
    std::thread consumer_thread (consume,10);

    // produce 10 items when needed:
    for (int i=0; i<10; ++i) {
      while (shipment_available()) std::this_thread::yield();
      mtx.lock();
      cargo = i+1;
      cv.notify_one();
      mtx.unlock();
    }

    consumer_thread.join();

    return 0;
  }
```

2.[**std::condition_variable_any::wait_for**](http://www.cplusplus.com/reference/condition_variable/condition_variable_any/wait_for/)



```c
       unconditional (1)    
            template <class Lock, class Rep, class Period>
                cv_status wait_for (Lock& lck,
                           const chrono::duration<Rep,Period>& rel_time);
       predicate (2)    
            template <class Lock, class Rep, class Period, class Predicate>
                 bool wait_for (Lock& lck,
                      const chrono::duration<Rep,Period>& rel_time, Predicate pred);
```

1、当前线程（当前锁定lck）的执行在rel_time期间被阻塞，或者直到通知（如果后者首先发生）。
 2、在阻止线程的时刻，该函数自动调用lck.unlock()，允许其他锁定线程继续。
 3、一旦通知或一旦rel_time已经过去，该函数解除阻塞和调用lck.lock()，使lck处于与调用函数时相同的状态。然后函数返回（注意，这个最后的互斥锁定可能在返回之前再次阻止线程）。
 4、通常，该函数被通知通过另一个线程的呼叫唤醒成员notify_one 或 notify_one。但是某些实现可能会产生虚假的唤醒呼叫，而不会调用这些功能。因此，该功能的用户应确保其恢复条件得到满足。
 5、版本(2):如果指定pred，如果pred返回false，则函数只会阻塞，并且只有当它变为true时，通知才能解除线程(这对于检查伪唤醒调用特别有用)。可以理解为



```c
       return wait_until (lck, chrono::steady_clock::now() + rel_time, std::move(pred));
```

[示例13](https://github.com/jorionwen/threadtest/blob/master/condition_variable/example13.cpp)：



```c
  // condition_variable_any::wait_for example
  #include <iostream>           // std::cout
  #include <thread>             // std::thread
  #include <chrono>             // std::chrono::seconds
  #include <mutex>              // std::mutex
  #include <condition_variable> // std::condition_variable_any,       std::cv_status

  std::condition_variable_any cv;

  int value;

  void read_value() {
    std::cin >> value;
    cv.notify_one();
  }

  int main ()
  {
    std::cout << "Please, enter an integer (I'll be printing dots): ";
    std::thread th (read_value);

    std::mutex mtx;
    mtx.lock();
    while   (cv.wait_for(mtx,std::chrono::seconds(1))==std::cv_status::timeout) {
      std::cout << '.';
    }
    std::cout << "You entered: " << value << '\n';
    mtx.unlock();

    th.join();

    return 0;
  }
```

1. [**std::condition_variable_any::wait_until**](http://www.cplusplus.com/reference/condition_variable/condition_variable_any/wait_until/)



```c
       unconditional (1)    
       template <class Lock, class Clock, class Duration>
          cv_status wait_until (Lock& lck,
                                const chrono::time_point<Clock,Duration>& abs_time);
       predicate (2)    
       template <class Lock, class Clock, class Duration, class Predicate>
          bool wait_until (Lock& lck,
                           const chrono::time_point<Clock,Duration>& abs_time, 
                           Predicate pred);
```

1、当前线程的执行（当前锁定lck）将被阻塞，直到通知或直到abs_time，以先发生者为准。
 2、在阻止线程的时刻，该函数自动调用lck.unlock()，允许其他锁定线程继续。
 3、一旦通知或一旦为abs_time，函数解除阻塞和调用lck.lock()，使lck处于与调用函数时相同的状态。然后函数返回（注意，这个最后的互斥锁定可能在返回之前再次阻止线程）。
 4、通常，该函数被通知通过另一个线程的呼叫唤醒成员notify_one 或会员 notify_one。但是某些实现可能会产生虚假的唤醒呼叫，而不会调用这些功能。因此，该功能的用户应确保其恢复条件得到满足。
 5、版本2：如果指定pred，如果pred返回false，则函数只会阻塞，并且只有当它变为true时，通知才能解除线程(这对于检查伪唤醒调用特别有用)。表现为：



```c
        while (!pred())
          if ( wait_until(lck,abs_time) == cv_status::timeout)
            return pred();
        return true;
```

[示例14](https://github.com/jorionwen/threadtest/blob/master/condition_variable/example14.cpp)：



```php
#include <iostream>
#include <atomic>
#include <condition_variable>
#include <thread>
#include <chrono>
//using namespace std::chrono_literals;
 
std::condition_variable cv;
std::mutex cv_m;
std::atomic<int> i{0};
 
void waits(int idx)  {
    std::unique_lock<std::mutex> lk(cv_m);
    auto now = std::chrono::system_clock::now();
    if(cv.wait_until(lk, now + idx*std::chrono::milliseconds(100), [](){return i == 1;}))
         std::cerr << "Thread " << idx << " finished waiting. i == " << i << '\n';
     else
         std::cerr << "Thread " << idx << " timed out. i == " << i << '\n';
}
 
void signals() {
    std::this_thread::sleep_for(std::chrono::milliseconds(120));
    std::cerr << "Notifying...\n";
    cv.notify_all();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    i = 1;
    std::cerr << "Notifying again...\n";
    cv.notify_all();
}
 
int main() {
    std::thread t1(waits, 1), t2(waits, 2), t3(waits, 3), t4(signals);
    t1.join(); 
    t2.join();
    t3.join();
    t4.join();
    return 0;
}
```

#### 3.  Notify functions

1. [**std::condition_variable_any::notify_one**](http://www.cplusplus.com/reference/condition_variable/condition_variable_any/notify_one/)
    同上面std::condition_variable的函数。
    unblock当前等待该条件的一个线程。如果没有线程在等待，则该函数什么都不做。如果多于一个，则不指定哪个线程被选中。
    [示例15](https://github.com/jorionwen/threadtest/blob/master/condition_variable/example15.cpp)：



```c
  // condition_variable_any::notify_one
  #include <iostream>           // std::cout
  #include <thread>             // std::thread
  #include <mutex>              // std::mutex
  #include <condition_variable> // std::condition_variable_any

  std::mutex mtx;
  std::condition_variable_any cv;

  int cargo = 0;     // shared value by producers and consumers

 void consumer () {
      mtx.lock();
      while (cargo==0) cv.wait(mtx);
      std::cout << cargo << '\n';
      cargo=0;
      mtx.unlock();
  }

  void producer (int id) {
      mtx.lock();
      cargo = id;
      cv.notify_one();
      mtx.unlock();
  }
  
  int main ()
  {
    std::thread consumers[10],producers[10];

    // spawn 10 consumers and 10 producers:
    for (int i=0; i<10; ++i) {
      consumers[i] = std::thread(consumer);
      producers[i] = std::thread(producer,i+1);
    }

    // join them back:
    for (int i=0; i<10; ++i) {
      producers[i].join();
      consumers[i].join();
    }

    return 0;
  }
```

本例子在vs2013运行结果 1-10，在ubuntu14.04，会出现死锁等待情况。

1. [**std::condition_variable_any::notify_all**](http://www.cplusplus.com/reference/condition_variable/condition_variable_any/notify_all/)
    同上面std::condition_variable的函数。打开当前等待该条件的所有线程。如果没有线程在等待，则该函数什么都不做。
    [示例16](https://github.com/jorionwen/threadtest/blob/master/condition_variable/example16.cpp)：



```c
  // condition_variable_any::notify_all
  #include <iostream>           // std::cout
  #include <thread>             // std::thread
  #include <mutex>              // std::mutex
  #include <condition_variable> // std::condition_variable_any

  std::mutex mtx;
  std::condition_variable_any cv;
  bool ready = false;

  void print_id (int id) {
    mtx.lock();
    while (!ready) cv.wait(mtx);
    // ...
    std::cout << "thread " << id << '\n';
    mtx.unlock();
  }

  void go() {
    mtx.lock();
    ready = true;
    cv.notify_all();
    mtx.unlock();
  }

  int main ()
  {
    std::thread threads[10];
    // spawn 10 threads:
    for (int i=0; i<10; ++i)
      threads[i] = std::thread(print_id,i);

    std::cout << "10 threads ready to race...\n";
    go();                       // go!

    for (auto& th : threads) th.join();

    return 0;
  }
```

## 三、 Enum classes

### 1. [**std::cv_status**](http://www.cplusplus.com/reference/condition_variable/cv_status/)



```cpp
    enum class cv_status;   定义
```

条件变量状态：
 表示是否由于超时而返回的函数。
 这种类型的值由condition_variable和condition_variable_any的成员wait_for和wait_until返回。
 定义类似：



```c
 enum class cv_status { no_timeout, timeout };
cv_status::no_timeout :  该函数返回没有超时
cv_status::timeout : 该函数由于达到其时间限制（timeout）而返回。
```

## 四、Functions

### 1. [**std::notify_all_at_thread_exit**](http://www.cplusplus.com/reference/condition_variable/notify_all_at_thread_exit/)



```cpp
   void notify_all_at_thread_exit (condition_variable& cond,unique_lock<mutex> lck);
```

当调用线程退出时，等待在cond上的所有线程都被通知恢复执行。
 该函数还获得由lck管理的mutex对象上的锁的所有权，该对象在内部由函数存储，并在线程退出时解锁(仅在通知所有线程之前)，行为如下:一旦所有具有线程存储时间的对象都被销毁:`1 lck.unlock(); 2 cond.notify_all();`
 **注意**：
 *如果lock. mutex()没有被当前线程锁定，则调用此函数是未定义的行为。
 如果lock. mutex()与当前在同一条件变量中等待的所有其他线程使用的互斥对象不相同，则调用此函数。
 所提供的锁一直保持到线程退出。一旦调用这个函数，就不会有更多的线程可能获得相同的锁以等待cond。如果某个线程在这个条件变量上等待，它不应该尝试释放并重新获取锁，当它被错误地唤醒时。
 在典型的用例中，这个函数是由一个独立的线程调用的最后一个函数。*
 [示例17](https://github.com/jorionwen/threadtest/blob/master/condition_variable/example17.cpp)：



```c
  // notify_all_at_thread_exit
  #include <iostream>           // std::cout
  #include <thread>             // std::thread
  #include <mutex>              // std::mutex, std::unique_lock
  #include <condition_variable> // std::condition_variable

  std::mutex mtx;
  std::condition_variable cv;
  bool ready = false;

  void print_id (int id) {
    std::unique_lock<std::mutex> lck(mtx);
    while (!ready) cv.wait(lck);
    // ...
    std::cout << "thread " << id << '\n';
  }

  void go() {
    std::unique_lock<std::mutex> lck(mtx);
    std::notify_all_at_thread_exit(cv,std::move(lck));
    ready = true;
  }

  int main ()
  {
    std::thread threads[10];
    // spawn 10 threads:
    for (int i=0; i<10; ++i)
      threads[i] = std::thread(print_id,i);
    std::cout << "10 threads ready to race...\n";

    std::thread(go).detach();   // go!

    for (auto& th : threads) th.join();

    return 0;
  }
```

上面的例子ubuntu14.04 g++4.8.4 编译时报错notify_all_at_thread_exit 不是 std的成语函数; 原因：/usr/include/c++/4.8/condition_variable文件中没有对notify_all_at_thread_exit函数进行定义； vs2013中VC的include下的 condition_variable有定义，所以使用vs2013编译运行正常。

到这里condition_variable 头文件中的内容已经列完。



