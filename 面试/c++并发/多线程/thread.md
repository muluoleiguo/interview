# [多线程](http://www.cplusplus.com/Freference/Fmultithreading/F)

C++ 中关于并发多线程的部分，主要包含 <thread>、<mutex>、<atomic>、<condition_varible>、<future>五个部分。

- <atomic>：该头文主要声明了两个类, std::atomic 和 std::atomic_flag，另外还声明了一套 C 风格的原子类型和与 C 兼容的原子操作的函数。
- <thread>：该头文件主要声明了 std::thread 类，另外 std::this_thread 命名空间也在该头文件中。
- <mutex>：该头文件主要声明了与互斥量(mutex)相关的类，包括 std::mutex 系列类，std::lock_guard, std::unique_lock, 以及其他的类型和函数。
- <condition_variable>：该头文件主要声明了与条件变量相关的类，包括 std::condition_variable 和 std::condition_variable_any。
- <future>：该头文件主要声明了 std::promise, std::package_task 两个 Provider 类，以及 std::future 和 std::shared_future 两个 Future 类，另外还有一些与之相关的类型和函数，std::async() 函数就声明在此头文件中。

## 1、 [**thread**](http://www.cplusplus.com/reference/multithreading/)

本节讲thread头文件中的内容，练习代码[地址](https://github.com/Fjorionwen/Fthreadtest/Ftree/Fmaster/Fthread);
 <thread> 头文件中声明：thread线程和命名空间this_thread； thread包含如下：

| (一)、Member types                                           |                                          |
| ------------------------------------------------------------ | ---------------------------------------- |
| [**id**](http://www.cplusplus.com/Freference/Fthread/Fthread/Fid/F) | Thread id (public member type )          |
| [**native_handle_type**](http://www.cplusplus.com/Freference/Fthread/Fthread/Fnative_handle_type/F) | Native handle type (public member type ) |

std::thread::id是线程调用get_id和this_thread::get_id的返回值；thread::id默认构造函数的结果是一个non-joinable的值；通常用来和其他线程 thread::get_id的结果做比较。
 std::thread::native_handle_type本地句柄类型，如果库实现支持它，这个成员类型只存在于类线程中。是thread类成员函数thread::native_handle的返回值。
 定义:  `typedef /* implementation-defined */ native_handle_type;`

| (二)、Member functions                                       |                                                              |
| ------------------------------------------------------------ | ------------------------------------------------------------ |
| [**(constructor)**](http://www.cplusplus.com/Freference/Fthread/Fthread/Fthread/F) | Construct thread (public member function )                   |
| [**(destructor)**](http://www.cplusplus.com/Freference/Fthread/Fthread/F~thread/F) | Thread destructor (public member function )                  |
| [**operator=**](http://www.cplusplus.com/Freference/Fthread/Fthread/Foperator%3D/F) | Move-assign thread (public member function )                 |
| [**get_id**](http://www.cplusplus.com/Freference/Fthread/Fthread/Fget_id/F) | Get thread id (public member function )                      |
| [**joinable**](http://www.cplusplus.com/Freference/Fthread/Fthread/Fjoinable/F) | Check if joinable (public member function )                  |
| [**join**](http://www.cplusplus.com/Freference/Fthread/Fthread/Fjoin/F) | Join thread (public member function )                        |
| [**detach**](http://www.cplusplus.com/Freference/Fthread/Fthread/Fdetach/F) | Detach thread (public member function )                      |
| [**swap**](http://www.cplusplus.com/Freference/Fthread/Fthread/Fswap/F) | Swap threads (public member function )                       |
| [**native_handle**](http://www.cplusplus.com/Freference/Fthread/Fthread/Fnative_handle/F) | Get native handle (public member function )                  |
| [**hardware_concurrency [static\]**](http://www.cplusplus.com/Freference/Fthread/Fthread/Fhardware_concurrency/F) | Detect hardware concurrency (public static member function ) |

[示例 1](https://github.com/Fjorionwen/Fthreadtest/Fblob/Fmaster/Fthread/Fexample1.cpp):



```c
// thread example
#include  <iostream>        // std::cout
#include  <thread>        // std::thread
void foo()  { 
    std::cout << "foo is called" << std::endl;
}
void bar(int x) {
     std::cout << "bar is called" << std::endl;
}

int main()
{
     std::thread first (foo);    // spawn new thread that calls foo()
     std::thread second (bar,0);  // spawn new thread that calls bar(0)                                                 
     std::cout << "main, foo and bar now execute concurrently...\n";
     // synchronize threads:
     first.join();                // pauses until first finishes
     second.join();              // pauses until second finishes
     std::cout << "foo and bar completed.\n";
     return 0;
}
```

- 构造函数[**(constructor)**](http://www.cplusplus.com/Freference/Fthread/Fthread/Fthread/F)



```cpp
default (1)              thread() noexcept;   
initialization (2)       template <class Fn, class... Args>  
                           explicit thread (Fn&& fn, Args&&... args);
copy [deleted] (3)     thread (const thread&) = delete;
move (4)                 thread (thread&& x) noexcept;
```

(1)默认构造函数
 构造一个不表示任何执行线程的线程对象。
 (2)初始化的构造函数 模版函数
 构建一个线程对象，该对象表示一个新的可接合线程。新的执行线程调用fn传递args作为参数(使用其lvalue或rvalue引用的衰变副本)。此构建的完成开始同步调用fn副本的。
 (3) 拷贝构造   不允许拷贝构造
 (4) 移动构造   构造线程获取x线程，这个操作不会影响移动线程的执行，它只会传输它的处理程序。完成x将不再表示一个线程。
 [示例2](https://github.com/Fjorionwen/Fthreadtest/Fblob/Fmaster/Fthread/Fexample2.cpp)：



```c
  // constructing threads
  #include <iostream>       // std::cout
  #include <atomic>         // std::atomic
  #include <thread>         // std::thread
  #include <vector>         // std::vector

  std::atomic<int> global_counter (0);

  void increase_global (int n) { for (int i=0; i<n; ++i) ++global_counter; }

  void increase_reference (std::atomic<int>& variable, int n) { for (int i=0;   i<n; ++i) ++variable; }

  struct C : std::atomic<int> {
    C() : std::atomic<int>(0) {}
    void increase_member (int n) { for (int i=0; i<n; ++i) fetch_add(1); }
  };

  int main ()
  {
    std::vector<std::thread> threads;

    std::cout << "increase global counter with 10 threads...\n";
    for (int i=1; i<=10; ++i)
      threads.push_back(std::thread(increase_global,1000));

    std::cout << "increase counter (foo) with 10 threads using   reference...\n";
    std::atomic<int> foo(0);
    for (int i=1; i<=10; ++i)
    {
        threads.push_back(std::thread(increase_reference,std::ref(foo),1000));
    }

    std::cout << "increase counter (bar) with 10 threads using member...\n";
    C bar;
    for (int i=1; i<=10; ++i)
    {
      threads.push_back(std::thread(&C::increase_member,std::ref(bar),1000))  ;
    }

    std::cout << "synchronizing all threads...\n";
    for (auto& th : threads) th.join();

    std::cout << "global_counter: " << global_counter << '\n';
    std::cout << "foo: " << foo << '\n';
    std::cout << "bar: " << bar << '\n';

    return 0;
  }
```

- 析构函数[**(destructor)**](http://www.cplusplus.com/Freference/Fthread/Fthread/F~thread/F)
   std::thread::~thread破坏了线程对象。如果在销毁时线程是可接合的，则调用终止()。
- std::thread::operator=：
   move (1)                thread& operator= (thread&& rhs) noexcept；
   copy [deleted] (2)  thread& operator= (const thread&) = delete;
   thread不允许拷贝；如果对象当前不是joinable的，它将获得由rhs(如果有的话)表示的执行线程。如果是joinable，则调用终止()。赋值“=”运算符通过右值表达式，复制后的thread对象不再是一个线程。
   [示例3](https://github.com/Fjorionwen/Fthreadtest/Fblob/Fmaster/Fthread/Fexample3.cpp)：



```c
  // example for thread::operator=
  #include <iostream>       // std::cout
  #include <thread>         // std::thread, std::this_thread::sleep_for
  #include <chrono>         // std::chrono::seconds
 
  void pause_thread(int n) 
  {
    std::this_thread::sleep_for (std::chrono::seconds(n));
    std::cout << "pause of " << n << " seconds ended\n";
  }

  int main() 
  {
    std::thread threads[5];                         // default-constructed threads

    std::cout << "Spawning 5 threads...\n";
    for (int i=0; i<5; ++i)
      threads[i] = std::thread(pause_thread,i+1);   // move-assign threads

    std::cout << "Done spawning threads. Now waiting for them to join:\n";
    for (int i=0; i<5; ++i)
      threads[i].join();

    std::cout << "All threads joined!\n";

    return 0;
  }
```

- std::thread::get_id
   如果线程对象是joinable，函数将返回唯一标识线程的值。
   如果线程对象不可joinable，函数将返回成员类型线程的默认构造对象:id。
   [示例4](https://github.com/Fjorionwen/Fthreadtest/Fblob/Fmaster/Fthread/Fexample4.cpp)：



```c
  // thread::get_id / this_thread::get_id
  #include <iostream>       // std::cout
  #include <thread>         // std::thread, std::thread::id,   std::this_thread::get_id
  #include <chrono>         // std::chrono::seconds
 
  std::thread::id main_thread_id = std::this_thread::get_id();

  void is_main_thread() {
    if ( main_thread_id == std::this_thread::get_id() )
      std::cout << "This is the main thread.\n";
    else
      std::cout << "This is not the main thread.\n";
  }

  int main() 
  {
    is_main_thread();
    std::thread th (is_main_thread);
    th.join();
  }
```

- std::thread::joinable
   返回线程对象是否可joinable。
   如果线程对象表示执行的线程，则是可joinable。
   在这些情况下，一个线程对象是不可连接的:

  - 如果是默认构造。
  - 如果它已经被移动(或者构造另一个线程对象，或者分配给它)。
  - 如果它的成员加入或分离被调用。

  [示例5](https://github.com/Fjorionwen/Fthreadtest/Fblob/Fmaster/Fthread/Fexample5.cpp)：



```c
  // example for thread::joinable
  #include <iostream>       // std::cout
  #include <thread>         // std::thread
 
  void mythread() 
  {
    // do stuff...
  }
 
  int main() 
  {
    std::thread foo;
    std::thread bar(mythread);

    std::cout << "Joinable after construction:\n" << std::boolalpha;
    std::cout << "foo: " << foo.joinable() << '\n';
    std::cout << "bar: " << bar.joinable() << '\n';

    if (foo.joinable()) foo.join();
    if (bar.joinable()) bar.join();

    std::cout << "Joinable after joining:\n" << std::boolalpha;
    std::cout << "foo: " << foo.joinable() << '\n';
    std::cout << "bar: " << bar.joinable() << '\n';

    return 0;
  }
```

- std::thread::join
   join 函数在线程执行完成的时候返回；此函数在函数返回时与线程中所有操作的完成是同步的；调用join直到join被构造函数调用返回间，阻塞调用的线程；在调用此函数之后，线程对象变得不可连接，可以安全地销毁。
   [示例6](https://github.com/Fjorionwen/Fthreadtest/Fblob/Fmaster/Fthread/Fexample6.cpp)：



```c
  // example for thread::join
  #include <iostream>       // std::cout
  #include <thread>         // std::thread, std::this_thread::sleep_for
  #include <chrono>         // std::chrono::seconds
 
  void pause_thread(int n) 
  {
    std::this_thread::sleep_for (std::chrono::seconds(n));
    std::cout << "pause of " << n << " seconds ended\n";
  }
 
  int main() 
  {
    std::cout << "Spawning 3 threads...\n";
    std::thread t1 (pause_thread,1);
    std::thread t2 (pause_thread,2);
    std::thread t3 (pause_thread,3);
    std::cout << "Done spawning threads. Now waiting for them to join:\n";
    t1.join();
    t2.join();
    t3.join();
    std::cout << "All threads joined!\n";

    return 0;
  }
```

- std::thread::detach
   detach分离出调用线程对象所代表的线程，允许它们彼此独立地执行;这两个线程在任何方式上都不阻塞或同步；注意，当一个结束执行时，它的资源被释放。在调用此函数之后，线程对象变得不可连接，可以安全地销毁。
   [示例7](https://github.com/Fjorionwen/Fthreadtest/Fblob/Fmaster/Fthread/Fexample7.cpp)：



```c
  #include <iostream>       // std::cout
  #include <thread>         // std::thread, std::this_thread::sleep_for
  #include <chrono>         // std::chrono::seconds
 
  void pause_thread(int n) 
  {
    std::this_thread::sleep_for (std::chrono::seconds(n));
    std::cout << "pause of " << n << " seconds ended\n";
  }
   
  int main() 
  {
    std::cout << "Spawning and detaching 3 threads...\n";
    std::thread (pause_thread,1).detach();
    std::thread (pause_thread,2).detach();
    std::thread (pause_thread,3).detach();
    std::cout << "Done spawning threads.\n";

    std::cout << "(the main thread will now pause for 5 seconds)\n";
    // give the detached threads time to finish (but not guaranteed!):
    pause_thread(5);
    return 0;
  }
```

- std::thread::swap
   void swap (thread& x) noexcept;  // 与X交换对象状态。
- std::thread::native_handle
   获取本地处理函数；如果库实现支持，这个成员函数只存在于类线程中；如果存在，它将返回用于访问与线程关联的特定于实现的信息的值。
- std::thread::hardware_concurrency
   static unsigned hardware_concurrency() noexcept;  //函数定义
   检测硬件并发，返回硬件线程上下文的数量。对这个值的解释是看具体的系统和实现，可能不是精确的，只是一个近似值。请注意，这并不需要匹配系统中可用的处理器或内核的实际数目:一个系统可以支持每个处理单元的多个线程，或者限制对程序的资源的访问。如果此值没有计算或被定义好，则函数返回0。
- std::swap (thread)
   std::swap 跟前面提到的成员函数有所不同，他不是成员函数。函数定义：
   void swap (thread& x, thread& y) noexcept;
   交换线程对象x和y的状态；就像x.swap(y)被调用。

