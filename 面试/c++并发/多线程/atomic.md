## 一、<atomic>头文件结构

<atomic>头文件：原子类型是封装了一个值的类型，它的访问保证不会导致数据的竞争，并且可以用于在不同的线程之间同步内存访问。
 这个头声明了两个c++类，原子和atomic_flag，它实现了自包含类中的原子类型的所有特性。header还声明了整个C样式类型和函数与C中的原子支持兼容。头文件中结构定义[查看](http://en.cppreference.com/w/cpp/header/atomic)。
 <atomic>头文件的结构：

| **class**                                                    |                                                       |
| ------------------------------------------------------------ | ----------------------------------------------------- |
| [**atomic**](http://www.cplusplus.com/reference/atomic/atomic/) | 用于bool、整数和指针类型的原子类模板和特殊化 (类模板) |
| [**atomic_flag**](http://www.cplusplus.com/reference/atomic/atomic_flag/) | 无锁布尔原子类型(类)                                  |

| **Types**                                                    |                                           |
| ------------------------------------------------------------ | ----------------------------------------- |
| [**memory_order**](http://www.cplusplus.com/reference/atomic/memory_order/) | 为给定的原子操作定义内存排序约束(typedef) |

| Typedefs                   |                                 |
| -------------------------- | ------------------------------- |
| std::atomic_bool           | std::atomic<bool>               |
| std::atomic_char           | std::atomic<char>               |
| std::atomic_schar          | std::atomic<signed char>        |
| std::atomic_uchar          | std::atomic<unsigned char>      |
| std::atomic_short          | std::atomic<short>              |
| std::atomic_ushort         | std::atomic<unsigned short>     |
| std::atomic_int            | std::atomic<int>                |
| std::atomic_uint           | std::atomic<unsigned int>       |
| std::atomic_long           | std::atomic<long>               |
| std::atomic_ulong          | std::atomic<unsigned long>      |
| std::atomic_llong          | std::atomic<long long>          |
| std::atomic_ullong         | std::atomic<unsigned long long> |
| std::atomic_char16_t       | std::atomic<char16_t>           |
| std::atomic_char32_t       | std::atomic<char32_t>           |
| std::atomic_wchar_t        | std::atomic<wchar_t>            |
| std::atomic_int_least8_t   | std::atomic<int_least8_t>       |
| std::atomic_uint_least8_t  | std::atomic<uint_least8_t>      |
| std::atomic_int_least16_t  | std::atomic<int_least16_t>      |
| std::atomic_uint_least16_t | std::atomic<uint_least16_t>     |
| std::atomic_int_least32_t  | std::atomic<int_least32_t>      |
| std::atomic_uint_least32_t | std::atomic<uint_least32_t>     |
| std::atomic_int_least64_t  | std::atomic<int_least64_t>      |
| std::atomic_uint_least64_t | std::atomic<uint_least64_t>     |
| std::atomic_int_fast8_t    | std::atomic<int_fast8_t>        |
| std::atomic_uint_fast8_t   | std::atomic<uint_fast8_t>       |
| std::atomic_int_fast16_t   | std::atomic<int_fast16_t>       |
| std::atomic_uint_fast16_t  | std::atomic<uint_fast16_t>      |
| std::atomic_int_fast32_t   | std::atomic<int_fast32_t>       |
| std::atomic_uint_fast32_t  | std::atomic<uint_fast32_t>      |
| std::atomic_int_fast64_t   | std::atomic<int_fast64_t>       |
| std::atomic_uint_fast64_t  | std::atomic<uint_fast64_t>      |
| std::atomic_intptr_t       | std::atomic<intptr_t>           |
| std::atomic_uintptr_t      | std::atomic<uintptr_t>          |
| std::atomic_size_t         | std::atomic<size_t>             |
| std::atomic_ptrdiff_t      | std::atomic<ptrdiff_t>          |
| std::atomic_intmax_t       | std::atomic<intmax_t>           |
| std::atomic_uintmax_t      | std::atomic<uintmax_t>          |

| **Functions**                                                |                                                              |
| ------------------------------------------------------------ | ------------------------------------------------------------ |
| [**kill_dependency**](http://www.cplusplus.com/reference/atomic/kill_dependency/) | 从[std::memory_order_consume](http://en.cppreference.com/w/cpp/atomic/memory_order)依赖树中删除指定对象 (模版函数) |
| [**atomic_thread_fence**](http://www.cplusplus.com/reference/atomic/atomic_thread_fence/) | 通用内存顺序依赖关系的隔离同步原语(函数)                     |
| [**atomic_signal_fence**](http://www.cplusplus.com/reference/atomic/atomic_signal_fence/) | 在同一线程中执行的线程和信号处理程序之间的隔离(函数)         |

| **Functions for atomic objects (C-style)**                   | description                                                  |
| ------------------------------------------------------------ | ------------------------------------------------------------ |
| [**atomic_is_lock_free**](http://www.cplusplus.com/reference/atomic/atomic_is_lock_free/) | 检查原子类型的操作是否免锁的(函数模板)                       |
| [**atomic_store**](http://www.cplusplus.com/reference/atomic/atomic_store/)  </br>  [**atomic_store_explicit**](http://www.cplusplus.com/reference/atomic/atomic_store_explicit/) | 使用非原子参数以原子方式替换原子对象的值(函数模板)           |
| [**atomic_load**](http://www.cplusplus.com/reference/atomic/atomic_load/)  </br>  [**atomic_load_explicit**](http://www.cplusplus.com/reference/atomic/atomic_load_explicit/) | 在原子对象中原子性地获取存储的值(函数模板)                   |
| [**atomic_exchange**](http://www.cplusplus.com/reference/atomic/atomic_exchange/)  </br>  [**atomic_exchange_explicit**](http://www.cplusplus.com/reference/atomic/atomic_exchange_explicit/) | 原子性地用非原子参数替换原子对象的值，并返回原子的旧值。(函数模板) |
| [**atomic_compare_exchange_weak**](http://www.cplusplus.com/reference/atomic/atomic_compare_exchange_weak/) </br> [**atomic_compare_exchange_weak_explicit**](http://www.cplusplus.com/reference/atomic/atomic_compare_exchange_weak_explicit/) </br> [**atomic_compare_exchange_strong**](http://www.cplusplus.com/reference/atomic/atomic_compare_exchange_strong/) </br> [**atomic_compare_exchange_strong_explicit**](http://www.cplusplus.com/reference/atomic/atomic_compare_exchange_strong_explicit/) | 原子地比较原子对象和非原子参数的值，如果不相等，则执行原子交换，如果没有，就load atomic |
| [**atomic_fetch_add**](http://www.cplusplus.com/reference/atomic/atomic_fetch_add/) </br> [**atomic_fetch_add_explicit**](http://www.cplusplus.com/reference/atomic/atomic_fetch_add_explicit/) | 向原子对象添加非原子值，并获取原子的前值(函数模板)           |
| [**atomic_fetch_sub**](http://www.cplusplus.com/reference/atomic/atomic_fetch_sub/) </br> [**atomic_fetch_sub_explicit**](http://www.cplusplus.com/reference/atomic/atomic_fetch_sub_explicit/) | 从原子对象中减去非原子值，并获得原子的前值(函数模板)         |
| [**atomic_fetch_and**](http://www.cplusplus.com/reference/atomic/atomic_fetch_and/) </br> [**atomic_fetch_and_explicit**](http://www.cplusplus.com/reference/atomic/atomic_fetch_and_explicit/) | 用逻辑结果和非原子参数替换原子对象，并获得原子的前值(函数模板) |
| [**atomic_fetch_or**](http://www.cplusplus.com/reference/atomic/atomic_fetch_or/) </br> [**atomic_fetch_or_explicit**](http://www.cplusplus.com/reference/atomic/atomic_fetch_or_explicit/) | 用逻辑或非原子参数替换原子对象，并获得原子的前值(函数模板)   |
| [**atomic_fetch_xor**](http://www.cplusplus.com/reference/atomic/atomic_fetch_xor/) [**atomic_fetch_xor_explicit**](http://www.cplusplus.com/reference/atomic/atomic_fetch_xor_explicit/) | 用逻辑XOR和非原子参数替换原子对象，并获得原子的前值(函数模板) |

| Functions for atomic flags (C-style)                         | description                                   |
| ------------------------------------------------------------ | --------------------------------------------- |
| [**atomic_flag_test_and_set**](http://www.cplusplus.com/reference/atomic/atomic_flag_test_and_set/)</br>[**atomic_flag_test_and_set_explicit**](http://www.cplusplus.com/reference/atomic/atomic_flag_test_and_set_explicit/) | 原子地将flag设置为true并返回其先前的值 (函数) |
| [**atomic_flag_clear**](http://www.cplusplus.com/reference/atomic/atomic_flag_clear/)</br>[**atomic_flag_clear_explicit**](http://www.cplusplus.com/reference/atomic/atomic_flag_clear_explicit/) | 原子地将flag设置成false（函数）               |

| Preprocessor macros                                          | description                            |
| ------------------------------------------------------------ | -------------------------------------- |
| [**ATOMIC_VAR_INIT**](http://www.cplusplus.com/reference/atomic/ATOMIC_VAR_INIT/) | 静态存储时间的原子变量的常量初始化(宏) |
| [**ATOMIC_FLAG_INIT**](http://www.cplusplus.com/reference/atomic/ATOMIC_FLAG_INIT/) | 初始化 std::atomic_flag为 false        |

**Macro constants**



```cpp
// lock-free property
#define ATOMIC_BOOL_LOCK_FREE /*unspecified*/
#define ATOMIC_CHAR_LOCK_FREE /*unspecified*/
#define ATOMIC_CHAR16_T_LOCK_FREE /*unspecified*/
#define ATOMIC_CHAR32_T_LOCK_FREE /*unspecified*/
#define ATOMIC_WCHAR_T_LOCK_FREE /*unspecified*/
#define ATOMIC_SHORT_LOCK_FREE /*unspecified*/
#define ATOMIC_INT_LOCK_FREE /*unspecified*/
#define ATOMIC_LONG_LOCK_FREE /*unspecified*/
#define ATOMIC_LLONG_LOCK_FREE /*unspecified*/
#define ATOMIC_POINTER_LOCK_FREE /*unspecified*/
```

上面是对<atomic>头文件结构的描述；下面具体按照上面的结构分析：

## 二 、[**std::atomic**](http://www.cplusplus.com/reference/atomic/atomic/)



```cpp
定义 ：  //类模版
template< class T > struct atomic;  (1)      (since C++11)
template<>  struct atomic<Integral>;  (2)   (since C++11)
template<>  struct atomic<bool>;   (3)  (since C++11)
template< class T >  struct atomic<T*>;  (4)     (since C++11) 指针特化
```

std::atomic模板的每个实例化和专门化都定义了一个原子类型。如果一个线程在另一个线程读取它时写入一个原子对象，那么行为就会被明确定义(参见关于数据竞争的详细信息的内存模型)。此外，对原子对象的访问可以建立线程间的同步，并按照std::memoryorder指定非原子性的内存访问。
 std::atomic可以用任何简单的可复制的t实例化。同时std::atomic是不可复制的，也不是可移动的。

### Member functions

1. [**(constructor)构造函数**](http://www.cplusplus.com/reference/atomic/atomic/atomic/)



```c
         atomic() noexcept = default;(1)             default        (since C++11)
         constexpr atomic( T desired ) noexcept;(2)  initialization (since C++11)
         atomic( const atomic& ) = delete;(3)       copy [deleted] (since C++11)
```

构造新的原子变量。
 1)将原子对象放在未初始化的状态中。一个未初始化的原子对象可以通过调用atomicinit来初始化。
 2)用desired 初始化对象。初始化不是原子性的。
 3)原子变量不是可复制的。
 [示例1](https://github.com/jorionwen/threadtest/blob/master/atomic/example1.cpp)：



```c
  // constructing atomics
  #include <iostream>       // std::cout
  #include <atomic>         // std::atomic, std::atomic_flag,   ATOMIC_FLAG_INIT
  #include <thread>         // std::thread, std::this_thread::yield
  #include <vector>         // std::vector

  std::atomic<bool> ready (false);
  std::atomic_flag winner = ATOMIC_FLAG_INIT;

  void count1m (int id) {
    while (!ready) { std::this_thread::yield(); }      // wait for the ready signal
    for (volatile int i=0; i<1000000; ++i) {}          // go!, count to 1 million
    if (!winner.test_and_set()) { std::cout << "thread #" << id << " won!\n"; }
  };

  int main ()
  {
    std::vector<std::thread> threads;
    std::cout << "spawning 10 threads that count to 1 million...\n";
    for (int i=1; i<=10; ++i) threads.push_back(std::thread(count1m,i));
    ready = true;
    for (auto& th : threads) th.join();
  
    return 0;
  }
```

1. [**operator=**](http://www.cplusplus.com/reference/atomic/atomic/operator%3D/)



```c
       T operator=( T desired ) noexcept;               set value (1)
       T operator=( T desired ) volatile noexcept;      set value (1)
       atomic& operator=( const atomic& ) = delete;          copy [deleted] (2)
       atomic& operator=( const atomic& ) volatile = delete; copy [deleted] (2)
```

1 用val替换存储的值。该操作是原子性的，并使用顺序一致性(memoryorderseqcst)。要使用不同的内存排序来修改值，请参见atomic::store。
 2 原子对象没有定义的复制赋值，但是注意它们是可以隐式地转换为类型T。
 注意：与大多数赋值运算符不同，原子类型的赋值运算符不会返回对它们的左参数的引用。它们返回一个存储值的副本。
 [示例2](https://github.com/jorionwen/threadtest/blob/master/atomic/example2.cpp)：



```c
   // atomic::operator=/operator T example:
  #include <iostream>       // std::cout
  #include <atomic>         // std::atomic
  #include <thread>         // std::thread, std::this_thread::yield
  
   std::atomic<int> foo(0);
  
   void set_foo(int x) {
   foo = x;
 }

  void print_foo() {
    while (foo==0) {             // wait while foo=0
      std::this_thread::yield();
    }
    std::cout << "foo: " << foo << '\n';
  }

  int main ()
  {
    std::thread first (print_foo);
    std::thread second (set_foo,10);
    first.join();
    second.join();
    return 0;
  }
```

### General atomic operations

1. [**std::atomic::is_lock_free**](http://www.cplusplus.com/reference/atomic/atomic/is_lock_free/)



```c
       bool is_lock_free() const noexcept;
       bool is_lock_free() const volatile noexcept;
```

一个无锁对象并不会导致其他线程在访问时被阻塞(可能使用某种类型的事务性内存)。该函数返回的值与相同类型的所有其他对象返回的值一致。
 检查这个类型的所有对象的原子操作是否都是无锁的。返回true表示lock_free.
 [示例3](https://github.com/jorionwen/threadtest/blob/master/atomic/example3.cpp)：



```c
  #include <iostream>
  #include <utility>
  #include <atomic>
   
  struct A { int a[100]; };
  struct B { int x, y; };
  int main()
  {
      std::cout << std::boolalpha
              << "std::atomic<A> is lock free? "
              << std::atomic<A>{}.is_lock_free() << '\n'
              << "std::atomic<B> is lock free? "
              << std::atomic<B>{}.is_lock_free() << '\n';
       return 0;
  }
```

1. [**std::atomic::store**](http://www.cplusplus.com/reference/atomic/atomic/store/)



```cpp
       void store (T val, memory_order sync = memory_order_seq_cst) volatile noexcept;
       void store (T val, memory_order sync = memory_order_seq_cst) noexcept;
```

用val替换包含的值。操作是原子的，按照同步所指定的内存顺序内存数序包括（std::memory_order_relaxed, std::memory_order_release 和std::memory_order_seq_cst）。
 参数sync的描述(后续会介绍memory_order)：
 [memory_order_relaxed](http://www.cplusplus.com/memory_order_relaxed)： 不同步副作用。
 [memory_order_release](http://www.cplusplus.com/memory_order_release)：同步下一个使用或者获取操作的副作用。
 [memory_order_seq_cst](http://www.cplusplus.com/memory_order_seq_cst)：同步所有与其他顺序一致操作的可见的副作用，并遵循一个完整的顺序。
 [示例4](https://github.com/jorionwen/threadtest/blob/master/atomic/example4.cpp)：



```c
  // atomic::load/store example
  #include <iostream>       // std::cout
  #include <atomic>         // std::atomic, std::memory_order_relaxed
  #include <thread>         // std::thread

  std::atomic<int> foo (0);

  void set_foo(int x) {
    foo.store(x,std::memory_order_relaxed);     // set value atomically
  }

  void print_foo() {
    int x;
    do {
      x = foo.load(std::memory_order_relaxed);  // get value atomically
    } while (x==0);
    std::cout << "foo: " << x << '\n';
  }

  int main ()
  {
    std::thread first (print_foo);
    std::thread second (set_foo,10);
    first.join();
    second.join();
    return 0;
  }
```

1. [**std::atomic::load**](http://www.cplusplus.com/reference/atomic/atomic/load/)



```c
       T load (memory_order sync = memory_order_seq_cst) const volatile noexcept;
       T load (memory_order sync = memory_order_seq_cst) const noexcept;
```

返回包含值。操作是原子的，按照同步所指定的内存顺序。指令必须是std::memory_order_relaxed, std::memory_order_consume, std::memory_order_acquire 和 std::memory_order_seq_cst)；否则，行为是没有定义的。
 sync指令描述：
 上文已经描述了std::memory_order_relaxed和 std::memory_order_seq_cst，这里只描述[memory_order_acquire](http://www.cplusplus.com/memory_order_acquire)和[memory_order_consume](http://www.cplusplus.com/memory_order_consume)。
 [memory_order_acquire](http://www.cplusplus.com/memory_order_acquire)：同步从最后一个Release或顺序一致的操作所有可见的副作用。
 [memory_order_consume](http://www.cplusplus.com/memory_order_consume)：同步与最后一个Release或顺序一致的操作所产生的依赖关系的可见的副作用。
 [示例5](https://github.com/jorionwen/threadtest/blob/master/atomic/example5.cpp)：



```c
  // atomic::load/store example
  #include <iostream>       // std::cout
  #include <atomic>         // std::atomic, std::memory_order_relaxed
  #include <thread>         // std::thread

  std::atomic<int> foo (0);

  void set_foo(int x) {
    foo.store(x,std::memory_order_relaxed);     // set value atomically
  }

  void print_foo() {
    int x;
    do {
      x = foo.load(std::memory_order_relaxed);  // get value atomically
    } while (x==0);
    std::cout << "foo: " << x << '\n';
  }

  int main ()
  {
    std::thread first (print_foo);
    std::thread second (set_foo,10);
    first.join();
    second.join();
    return 0;
  }
```

1. [**std::atomic::operator T**](http://www.cplusplus.com/reference/atomic/atomic/operator%20T/)



```c
       operator T() const volatile noexcept;
       operator T() const noexcept;
```

这是一个类型转换的操作符:这个表达式期望它包含的类型(T)的值，调用这个成员函数，访问包含的值。
 该操作是原子的，并使用顺序一致性(memory_order_seq_cst)。要检索具有不同内存顺序的值，相当于std::atomic::load。
 [示例6](https://github.com/jorionwen/threadtest/blob/master/atomic/example6.cpp)：



```c
  // atomic::operator=/operator T example:
  #include <iostream>       // std::cout
  #include <atomic>         // std::atomic
  #include <thread>         // std::thread, std::this_thread::yield

  std::atomic<int> foo(0);
  std::atomic<int> bar(0);

  void set_foo(int x) {
    foo = x;
  }
  void copy_foo_to_bar () {
    while (foo==0) std::this_thread::yield();
    bar = static_cast<int>(foo);
  }
  void print_bar() {
    while (bar==0) std::this_thread::yield();
    std::cout << "bar: " << bar << '\n';
  }

  int main ()
  {
    std::thread first (print_bar);
    std::thread second (set_foo,10);
    std::thread third (copy_foo_to_bar);
    first.join();
    second.join();
    third.join();
    return 0;
  }
```

1. [**std::atomic::exchange**](http://www.cplusplus.com/reference/atomic/atomic/exchange/)



```c
       T exchange (T val, memory_order sync = memory_order_seq_cst) volatile noexcept;
       T exchange (T val, memory_order sync = memory_order_seq_cst) noexcept;
```

访问和修改包含的值
 用val替换所包含的值，并返回它之前的值。整个操作是原子性的(一个原子的读-修改-写操作):在读取(返回)值和被该函数修改的那一刻之间，值不会受到其他线程的影响。
 sync指令描述：
 上文已经描述了std::memory_order_relaxed和 std::memory_order_seq_cst 和memory_order_consume 和 memory_order_acquire，这里只描述[memory_order_acq_rel](http://www.cplusplus.com/memory_order_acq_rel)。
 [memory_order_acq_rel](http://www.cplusplus.com/memory_order_acq_rel)：读取作为一个获取操作，并作为一个发布操作写入。
 [示例 7](https://github.com/jorionwen/threadtest/blob/master/atomic/example7.cpp)：



```c
  // atomic::exchange example
  #include <iostream>       // std::cout
  #include <atomic>         // std::atomic
  #include <thread>         // std::thread
  #include <vector>         // std::vector

  std::atomic<bool> ready (false);
  std::atomic<bool> winner (false);

  void count1m (int id) {
    while (!ready) {}                  // wait for the ready signal
    for (int i=0; i<1000000; ++i) {}   // go!, count to 1 million
    if (!winner.exchange(true)) { std::cout << "thread #" << id << " won!\n"; }
  };

  int main ()
  {
    std::vector<std::thread> threads;
    std::cout << "spawning 10 threads that count to 1 million...\n";
    for (int i=1; i<=10; ++i) threads.push_back(std::thread(count1m,i));
    ready = true;
    for (auto& th : threads) th.join();

    return 0;
  }
```

1. [**std::atomic::compare_exchange_weak**](http://www.cplusplus.com/reference/atomic/atomic/compare_exchange_weak/) 和[**std::atomic::compare_exchange_strong**](http://www.cplusplus.com/reference/atomic/atomic/compare_exchange_strong/)



```c
       bool compare_exchange_weak (T& expected, T val,
           memory_order sync = memory_order_seq_cst) volatile noexcept;  (1)
       bool compare_exchange_weak (T& expected, T val,
           memory_order sync = memory_order_seq_cst) noexcept;           (1)

       bool compare_exchange_weak (T& expected, T val,
           memory_order success, memory_order failure) volatile noexcept;  (2)
       bool compare_exchange_weak (T& expected, T val,
           memory_order success, memory_order failure) noexcept;           (2)
        
       bool compare_exchange_strong (T& expected, T val,
           memory_order sync = memory_order_seq_cst) volatile noexcept;  (1)
       bool compare_exchange_strong (T& expected, T val,
           memory_order sync = memory_order_seq_cst) noexcept;           (1)

       bool compare_exchange_strong (T& expected, T val,
           memory_order success, memory_order failure) volatile noexcept; (2)
       bool compare_exchange_strong (T& expected, T val,
           memory_order success, memory_order failure) noexcept;          (2)
```

\* 比较原子对象的包含值与预期的内容:
 --1 如果是真的，它会用val替换包含的值(比如存储)。
 --2 如果是假的，它会用所包含的值替换预期,因此调用该函数之后，如果被该原子对象封装的值与参数 expected 所指定的值不相等，expected 中的内容就是原子对象的旧值。

\* 函数总是访问包含的值来读取它，如果这个比较是真的，那么它也会替换它。但是整个操作都是原子性的:在读取值和被替换的时刻之间，它的值不能被其他线程修改。

\* 在第(2)种情况下，内存序（Memory Order）的选择取决于比较操作结果，如果比较结果为 true(即原子对象的值等于 expected)，则选择参数 success 指定的内存序，否则选择参数 failure 所指定的内存序。

** 注意：
 这个函数直接的比较物理内容所包含的价值与预期的内容,这可能导致得到使用operator==比较的结果是一个失败的结果,因为对象底层的物理内容中可能存在位对齐或其他逻辑表示相同但是物理表示不同的值(比如 true 和 2 或 3，它们在逻辑上都表示"真"，但在物理上两者的表示并不相同)。
 不像 compare_exchange_strong，这个弱版本允许(spuriously 地)返回 false(即原子对象所封装的值与参数 expected 的物理内容相同，但却仍然返回 false)，即使在预期的实际情况与所包含的对象相比较时也是如此。对于某些循环算法来说，这可能是可接受的行为，并且可能会在某些平台上带来显著的性能提升。在这些虚假的失败中，函数返回false，而不修改预期。
 对于非循环算法来说， compare_exchange_strong通常是首选。
 [示例8](https://github.com/jorionwen/threadtest/blob/master/atomic/example8.cpp)：



```c
  // atomic::compare_exchange_weak example:
  #include <iostream>       // std::cout
  #include <atomic>         // std::atomic
  #include <thread>         // std::thread
  #include <vector>         // std::vector

  // a simple global linked list:
  struct Node { int value; Node* next; };
  std::atomic<Node*> list_head (nullptr);

  void append (int val) {     // append an element to the list
    Node* oldHead = list_head;
    Node* newNode = new Node {val,oldHead};

    // what follows is equivalent to: list_head = newNode, but in a thread-safe way:
    while (!list_head.compare_exchange_weak(oldHead,newNode))
      newNode->next = oldHead;
  }

  int main ()
  {
    // spawn 10 threads to fill the linked list:
    std::vector<std::thread> threads;
    for (int i=0; i<10; ++i) threads.push_back(std::thread(append,i));
    for (auto& th : threads) th.join();

    // print contents:
    for (Node* it = list_head; it!=nullptr; it=it->next)
      std::cout << ' ' << it->value;
    std::cout << '\n';

    // cleanup:
    Node* it; while (it=list_head) {list_head=it->next; delete it;}

    return 0;
  }
```

compare_exchange_strong 跟 compare_exchange_week 不同的是：
 与compare_exchange_weak 不同, strong版本的 compare-and-exchange 操作不允许(spuriously 地)返回 false，即原子对象所封装的值与参数 expected 的物理内容相同，比较操作一定会为 true。不过在某些平台下，如果算法本身需要循环操作来做检查， compare_exchange_weak 的性能会更好。

### 特定的操作支持(整形和指针)

1. [**std::atomic::fetch_add**](http://www.cplusplus.com/reference/atomic/atomic/fetch_add/)/[**std::atomic::fetch_sub**](http://www.cplusplus.com/reference/atomic/atomic/fetch_sub/)



```c
if T is integral (1)    
       T fetch_add (T val, memory_order sync = memory_order_seq_cst) volatile noexcept;
       T fetch_add (T val, memory_order sync = memory_order_seq_cst) noexcept;
       T fetch_sub (T val, memory_order sync = memory_order_seq_cst) volatile noexcept;
       T fetch_sub (T val, memory_order sync = memory_order_seq_cst) noexcept;
if T is pointer (2) 
       T fetch_add (ptrdiff_t val, memory_order sync = memory_order_seq_cst) volatile noexcept;
       T fetch_add (ptrdiff_t val, memory_order sync = memory_order_seq_cst) noexcept;
       T fetch_sub (ptrdiff_t val, memory_order sync = memory_order_seq_cst) volatile noexcept;
       T fetch_sub (ptrdiff_t val, memory_order sync = memory_order_seq_cst) noexcept;
```

*将val加或者减去到包含的值并返回在操作之前的值。
 *整个操作是原子的(一个原子的读-修改-写操作):当在这个函数被修改的时候,读取的(返回)值被读取，值不受其他线程的影响。
 *这个成员函数是对整数(1)和指针(2)类型(除了bool除外)的原子专门化中定义。
 *如果第二个参数使用默认值，则该函数等价于原子::运算符+ =。
 [示例9](https://github.com/jorionwen/threadtest/blob/master/atomic/example9.cpp)：



```c
  #include <iostream>
  #include <thread>
  #include <atomic>
 
  std::atomic<long long> data;
  void do_work()
{
    data.fetch_add(1, std::memory_order_relaxed);
}
 
  int main()
{
    std::thread th1(do_work);
    std::thread th2(do_work);
    std::thread th3(do_work);
    std::thread th4(do_work);
    std::thread th5(do_work);
 
    th1.join();
    th2.join();
    th3.join();
    th4.join();
    th5.join();
 
    std::cout << "Result:" << data << '\n';
    return 0;
}
```

1. [**std::atomic::fetch_and**](http://www.cplusplus.com/reference/atomic/atomic/fetch_and/)/[**std::atomic::fetch_or**](http://www.cplusplus.com/reference/atomic/atomic/fetch_or/)/[**std::atomic::fetch_xor**](http://www.cplusplus.com/reference/atomic/atomic/fetch_xor/)



```c
       T fetch_and (T val, memory_order sync = memory_order_seq_cst) volatile noexcept;
       T fetch_and (T val, memory_order sync = memory_order_seq_cst) noexcept;
       T fetch_or (T val, memory_order sync = memory_order_seq_cst) volatile noexcept;
       T fetch_or (T val, memory_order sync = memory_order_seq_cst) noexcept;
       T fetch_xor (T val, memory_order sync = memory_order_seq_cst) volatile noexcept;
       T fetch_xor (T val, memory_order sync = memory_order_seq_cst) noexcept;
```

\* 读取包含的值并替换调该值和val执行一个位和操作的结果。
 \* 整个操作是原子的(一个原子的读-修改-写操作):当在这个函数被修改的时候,读取的(返回)值被读取，值不受其他线程的影响。
 \* 这个成员函数只在原子专门化中定义为整数类型(除了bool)。
 \* 如果第二个参数使用默认值，则该函数等价于原子::operator& =。

1. [**std::atomic::operator++**](http://www.cplusplus.com/reference/atomic/atomic/operatorplusplus/)/[**std::atomic::operator--**](http://www.cplusplus.com/reference/atomic/atomic/operatorminusminus/)
    pre-increment (1)



```cpp
       T operator++() volatile noexcept;
       T operator++() noexcept;
       T operator--() volatile noexcept;
       T operator--() noexcept;
 post-increment (2) 
       T operator++ (int) volatile noexcept;
       T operator++ (int) noexcept;
       T operator-- (int) volatile noexcept;
       T operator-- (int) noexcept;
```

\* 包含值的值进行增加或减少1，操作(1)返回所得到的包含值，操作(2)返回之前的值。
 \* 整个操作是原子的(一个原子的读-修改-写操作):当在这个函数被修改的时候,读取的(返回)值被读取，值不受其他线程的影响。
 \* 函数只在原子专门化中定义为整数和指针类型(除了bool)。
 \* 这个函数的行为就像调用`std::stomic::fetch_add(1)`,`memory_order_seq_cst`作为参数。

1. [**atomic::operator (comp. assign.)**](http://www.cplusplus.com/reference/atomic/atomic/operatororequal/)



```c
if T is integral (1)    
       T operator+= (T val) volatile noexcept;
       T operator+= (T val) noexcept;
       T operator-= (T val) volatile noexcept;
       T operator-= (T val) noexcept;
       T operator&= (T val) volatile noexcept;
       T operator&= (T val) noexcept;
       T operator|= (T val) volatile noexcept;
       T operator|= (T val) noexcept;
       T operator^= (T val) volatile noexcept;
       T operator^= (T val) noexcept;
if T is pointer (2) 
       T operator+= (ptrdiff_t val) volatile noexcept;
       T operator+= (ptrdiff_t val) noexcept;
       T operator-= (ptrdiff_t val) volatile noexcept;
       T operator-= (ptrdiff_t val) noexcept;
```

\* 对于整形(1)和指针(2)类型的原子专门化是支持复合赋值的;每一个函数都访问包含的值，应用合适的操作符，并在操作之前返回包含值的值;所有这些操作都不会受到其他线程的影响。
 \* 这些函数的行为就像使用memory_order_seq_cst调用std::stomic::fetch_ *函数一样:



![img](https:////upload-images.jianshu.io/upload_images/3414858-7a9738cc7bd1f4d5.png?imageMogr2/auto-orient/strip|imageView2/2/w/544/format/webp)

## 二 、[**atomic_flag**](http://www.cplusplus.com/reference/atomic/atomic_flag/)

atomic_flag是一个原子布尔类型。不同于std::atomic的所有专门化，它保证是lock_free。不像std::stomic< bool >，std::atomic_flag不提供负载或存储操作。
 [示例10](https://github.com/jorionwen/threadtest/blob/master/atomic/example10.cpp)：



```c
#include <thread>
#include <vector>
#include <iostream>
#include <atomic>
 
std::atomic_flag lock = ATOMIC_FLAG_INIT;
 
void f(int n)
{
    for (int cnt = 0; cnt < 100; ++cnt) {
        while (lock.test_and_set(std::memory_order_acquire))  // acquire lock
             ; // spin
        std::cout << "Output from thread " << n << '\n';
        lock.clear(std::memory_order_release);               // release lock
    }
}
 
int main()
{
    std::vector<std::thread> v;
    for (int n = 0; n < 10; ++n) {
        v.emplace_back(f, n);
    }
    for (auto& t : v) {
        t.join();
    }
    return 0;
}
```

1. [**构造函数(std::atomic_flag::atomic_flag)**](http://www.cplusplus.com/reference/atomic/atomic_flag/atomic_flag/)



```c
        atomic_flag() noexcept = default;
        atomic_flag (const atomic_flag&T) = delete;
```

\* atomic_flag在构建(或设置或清除)上处于一个未指定的状态，除非它被显式地初始化为ATOMIC_FLAG_INIT。
 \* ATOMIC_FLAG_INIT初始化是通过简单地调用默认构造函数或其他方法来实现的，这取决于特定的库实现。
 \* atomic_flag值不能复制/移动。
 *** 注意： `std::atomic_flag::operator=` 不可赋值，其赋值操作符被删除。。
 [示例11](https://github.com/jorionwen/threadtest/blob/master/atomic/example11.cpp)：



```c
  // constructing atomics: atomic<bool> vs atomic_flag
  #include <iostream>       // std::cout
  #include <atomic>         // std::atomic, std::atomic_flag, ATOMIC_FLAG_INIT
  #include <thread>         // std::thread, std::this_thread::yield
  #include <vector>         // std::vector

  std::atomic<bool> ready (false);               // can be checked without being set
  std::atomic_flag winner = ATOMIC_FLAG_INIT;    // always set when checked

  void count1m (int id) {
      while (!ready) { std::this_thread::yield(); }      // wait for the ready signal
      for (int i=0; i<1000000; ++i) {}                   // go!, count to 1 million
      if (!winner.test_and_set()) { std::cout << "thread #" << id << " won!\n"; }
  };

  int main ()
  {
      std::vector<std::thread> threads;
      std::cout << "spawning 10 threads that count to 1 million...\n";
      for (int i=1; i<=10; ++i) threads.push_back(std::thread(count1m,i));
      ready = true;
      for (auto& th : threads) th.join();
  
      return 0;
  }
```

1. [**std::atomic_flag::test_and_set**](http://www.cplusplus.com/reference/atomic/atomic_flag/test_and_set/)



```c
       bool test_and_set (memory_order sync = memory_order_seq_cst) volatile noexcept;
       bool test_and_set (memory_order sync = memory_order_seq_cst) noexcept;
```

\* 设置atomic_flag并返回是否在调用之前已经设置的。
 \* * 整个操作是原子的(一个原子的读-修改-写操作):当在这个函数被修改的时候,读取的(返回)值被读取，值不受其他线程的影响。
 [示例12](https://github.com/jorionwen/threadtest/blob/master/atomic/example12.cpp)：



```c
  // atomic_flag as a spinning lock
  #include <iostream>       // std::cout
  #include <atomic>         // std::atomic_flag
  #include <thread>         // std::thread
  #include <vector>         // std::vector
  #include <sstream>        // std::stringstream

  std::atomic_flag lock_stream = ATOMIC_FLAG_INIT;
  std::stringstream stream;

  void append_number(int x) {
    while (lock_stream.test_and_set()) {}
    stream << "thread #" << x << '\n';
    lock_stream.clear();
}

  int main ()
{
    std::vector<std::thread> threads;
    for (int i=1; i<=10; ++i)   threads.push_back(std::thread(append_number,i));
    for (auto& th : threads) th.join();

    std::cout << stream.str();
    return 0;
}
```

1. [**std::atomic_flag::clear**](http://www.cplusplus.com/reference/atomic/atomic_flag/clear/)



```c
       void clear (memory_order sync = memory_order_seq_cst) volatile noexcept;
       void clear (memory_order sync = memory_order_seq_cst) noexcept;
```

清除atomic_flag(即把atomic_flag 设为假)。
 清除atomic_flag使下一次调用成员atomic_flag::test_and_set对象返回false。
 操作是原子的，按照sync所指定的内存顺序。
 [示例13](https://github.com/jorionwen/threadtest/blob/master/atomic/example13.cpp)：



```c
  // atomic_flag as a spinning lock
  #include <iostream>       // std::cout
  #include <atomic>         // std::atomic_flag
  #include <thread>         // std::thread
  #include <vector>         // std::vector
  #include <sstream>        // std::stringstream

  std::atomic_flag lock_stream = ATOMIC_FLAG_INIT;
  std::stringstream stream;

  void append_number(int x) {
    while (lock_stream.test_and_set()) {}
    stream << "thread #" << x << '\n';
    lock_stream.clear();
}

  int main ()
{
    std::vector<std::thread> threads;
    for (int i=1; i<=10; ++i) threads.push_back(std::thread(append_number,i));
    for (auto& th : threads) th.join();

    std::cout << stream.str();
    return 0;
}
```

## 三 、[**memory_order**](http://www.cplusplus.com/reference/atomic/memory_order/)

作为用于执行原子操作的函数的参数，用于指定如何同步不同线程上的其他操作。也可参见[网址](http://en.cppreference.com/w/cpp/atomic/memory_order)。



~~~c
定义：
```c
typedef enum memory_order {
    memory_order_relaxed,   // relaxed
    memory_order_consume,   // consume
    memory_order_acquire,   // acquire
    memory_order_release,   // release
    memory_order_acq_rel,   // acquire/release
    memory_order_seq_cst    // sequentially consistent
} memory_order;
~~~

\* 当多个线程访问原子对象时，所有原子操作都会对一个原子对象定义良好的行为:在任何其他原子操作能够访问该对象之前，每个原子操作都是完全在对象上执行的。这保证了这些对象上没有数据竞争，而这正是定义原子性的特性。

\* 但是，每个线程可能在内存位置上执行操作，而不是原子对象本身:这些操作可能会对其他线程产生可见的副作用。这种类型的参数允许指定操作的内存顺序，以确定这些(可能非原子)可见的副作用是如何在线程间同步的，使用原子操作作为同步点:

- *memory_order_relaxed*
   该操作在某一时刻被命令进行原子化。这是最宽松的内存顺序，无法保证对不同线程的内存访问是如何根据原子操作进行排序的。
   标记为memory_order_relaxed的原子操作不是同步操作;它们不会在并发内存访问中强制执行顺序。它们只保证原子性和修改顺序的一致性。
   [示例14](https://github.com/jorionwen/threadtest/blob/master/atomic/example14.cpp)：



```c
  #include <vector>
  #include <iostream>
  #include <thread>
  #include <atomic>
 
  std::atomic<int> cnt = {0};
 
  void f()
  {
    for (int n = 0; n < 1000; ++n) {
        cnt.fetch_add(1, std::memory_order_relaxed);
    }
  }
 
  int main()
  {
    std::vector<std::thread> v;
    for (int n = 0; n < 10; ++n) {
        v.emplace_back(f);
    }
    for (auto& t : v) {
        t.join();
    }
    std::cout << "Final counter value is " << cnt << '\n';
    return 0;
  }
```

- *memory_order_consume*
   如果在这个对释放操作进行依赖（并且对加载线程有明显的副作用）的释放线程上 所有访问内存  已经发生，那么操作将被命令执行。
   带有这个内存顺序的加载操作在受影响的内存位置上执行获取操作:当前线程依赖于当前装载的值，在此负载之前，不需要读取或写入。在当前线程中可以看到相同的原子变量。在大多数平台上，这只会影响编译器优化(参见下面的Release-Consume)。
- *memory_order_acquire*
   在释放线程(对加载线程有明显的副作用)的情况下，操作被命令进行一次。
   带有这个内存顺序的加载操作在受影响的内存位置上执行获取操作:在此负载之前，在当前线程中不可以重新排序。所有在其他线程中都可以在当前线程中看到相同的原子变量(参见下面的Release-Acquire)
- *memory_order_release*
   \* 该操作被命令在消费或获取操作之前发生，作为对内存的其他访问的同步点，可能对加载线程有可见的副作用。
   \* 具有这种内存顺序的存储操作执行释放操作:在当前线程中，在该存储之后，不可以在当前线程中重新排序。当前线程中所有的写操作都可以在其他线程中看到，这些线程获得相同的原子变量(参见下面的Release-Acquire)，并且在使用相同原子的其他线程中，将对原子变量的依赖变为可见(参见下面的Release-Consume)。
- *memory_order_acq_rel*
   \* 该操作加载并存储释放(如上所述，用于memory_order_acquire 、memory_order_release)。
- *memory_order_seq_cst*
   \* 这个操作是按顺序一致的方式排序的:所有使用这个内存顺序的操作都是在所有可能对其他线程都有可见副作用的内存中执行的。
   \* 这是最严格的内存顺序，在非原子内存访问的情况下，保证了线程交互中最不意外的副作用。
   \* 对于消费和获取负载，顺序一致的存储操作被认为是发布操作。

## 四、Functions

1. [**std::kill_dependency**](http://www.cplusplus.com/reference/atomic/kill_dependency/)
    定义：



```c
      template <class T>
                    T kill_dependency (T y) noexcept;
```

\* 返回y的值而不需要依赖项。
 \* 使用memory_order_consume 作为内存顺序的原子操作，要求编译器检查通过访问存储的内存位置所带来的依赖关系。同步这样的依赖关系可能会导致某些硬件设置被设置，并迫使编译器放弃涉及这些内存位置的某些潜在优化。
 \* 调用此函数指示编译器，任何依赖于y的依赖项都不应被传递到返回值，而不需要同步。

1. [**atomic_thread_fence**](http://www.cplusplus.com/reference/atomic/atomic_thread_fence/)
    "C"风格导出的函数：



```c
       extern "C" void atomic_thread_fence (memory_order sync) noexcept;
```

建立一个多线程的隔离:对这个函数的调用的时间点变成一个获取或者一个释放(或者两者都是)的同步点。
 在调用这个函数之前发生的释放线程的所有可见的副作用都是同步的，在调用这个函数在获取线程之前。
 调用该函数与加载或存储原子操作具有相同的效果，但不涉及原子值。

1. [**atomic_signal_fence**](http://www.cplusplus.com/reference/atomic/atomic_signal_fence/)
    “C”风格导出的函数：



```c
       extern "C" void atomic_signal_fence (memory_order sync) noexcept;
```

建立一个单线程的隔离:对这个函数的调用点在一个线程内变成一个获取或一个释放点(或者两者)。
 这个函数相当于atomic_thread_fence ，但是没有因为调用而发生线程间同步。该函数的作用是对编译器进行指令，以阻止它进行优化，包括将写操作移动到一个释放栅栏或在获取栅栏之前的读操作。

## 五、 Functions for atomic objects (C-style)

1. [**atomic_is_lock_free**](http://www.cplusplus.com/reference/atomic/atomic_is_lock_free/)



```c
template (1)    
       template <class T> bool atomic_is_lock_free (const volatile atomic<T>* obj) noexcept;
       template <class T> bool atomic_is_lock_free (const atomic<T>* obj) noexcept;
overloads (2)   
       bool atomic_is_lock_free (const volatile A* obj) noexcept;
       bool atomic_is_lock_free (const A* obj) noexcept;
```

是否是lock_free。
 注意：
 除了std::atomicflag的所有原子类型都可以使用互斥锁或其他锁定操作实现，而不是使用无锁的原子CPU指令。原子类型也被允许有时是无锁的，例如，如果在给定的体系结构中，只有对齐的内存访问是自然的，那么相同类型的不一致的对象就必须使用锁。
 C++标准推荐(但不需要)无锁的原子操作也是无地址的，也就是说，适合于使用共享内存的进程之间的通信。
 示例15 (同[示例3](https://github.com/jorionwen/threadtest/blob/master/atomic/example3.cpp))：



```c
  #include <iostream>
  #include <utility>
  #include <atomic>
 
  struct A { int a[100]; };
  struct B { int x, y; };
  int main()
{
    std::atomic<A> a;
    std::atomic<B> b;
    std::cout << std::boolalpha
              << "std::atomic<A> is lock free? "
              << std::atomic_is_lock_free(&a) << '\n'
              << "std::atomic<B> is lock free? "
              << std::atomic_is_lock_free(&b) << '\n';
    return 0;
}
```

1. [**std::atomic_init**](http://www.cplusplus.com/reference/atomic/atomic_init/)



```c
template (1)    
       template <class T> void atomic_init (volatile atomic<T>* obj, T val) noexcept;
       template <class T> void atomic_init (atomic<T>* obj, T val) noexcept;
overloads (2)   
       void atomic_init (volatile A* obj, T val) noexcept;
       void atomic_init (A* obj, T val) noexcept;
```

用val的一个包含值初始化obj,该函数不是原子性的:来自另一个线程的并发访问，即使是通过原子操作，也是一种数据竞争。
 如果obj不是默认构造，那么这个行为就没有定义。
 如果这个函数在同一个obj上被调用两次，那么这个行为是没有定义的。

1. [**std::atomic_store**](http://www.cplusplus.com/reference/atomic/atomic_store/)和[**std::atomic_store_explicit**](http://www.cplusplus.com/reference/atomic/atomic_store_explicit/)



```c
template (1)    
       template <class T> void atomic_store (volatile atomic<T>* obj, T val) noexcept;
       template <class T> void atomic_store (atomic<T>* obj, T val) noexcept;
       template <class T>
         void atomic_store_explicit (volatile atomic<T>* obj, T val, memory_order sync) noexcept;
       template <class T>
         void atomic_store_explicit (atomic<T>* obj, T val, memory_order sync) noexcept;
overloads (2)   
       void atomic_store (volatile A* obj, T val) noexcept;
       void atomic_store (A* obj, T val) noexcept;
       void atomic_store_explicit (volatile A* obj, T val, memory_order sync) noexcept;
       void atomic_store_explicit (A* obj, T val, memory_order sync) noexcept;
```

用val替换obj中包含的值。该操作是原子性的，atomic_store 使用顺序一致性(memory_order_seq_cst), atomic_store_explicit 显示制定内存顺序。参见stomic的等价函数atomic::store和 atomic::operator=。

1. [**std::atomic_load**](http://www.cplusplus.com/reference/atomic/atomic_load/)和[**std::atomic_load_explicit**](http://www.cplusplus.com/reference/atomic/atomic_load_explicit/)



```c
template (1)    
        template <class T> T atomic_load (const volatile atomic<T>* obj) noexcept;
        template <class T> T atomic_load (const atomic<T>* obj) noexcept;
        template <class T>
          T atomic_load_explicit (const volatile atomic<T>* obj, memory_order sync) noexcept;
        template <class T>
          T atomic_load_explicit (const atomic<T>* obj, memory_order sync) noexcept;
overloads (2)   
        T atomic_load (const volatile A* obj) noexcept;
        T atomic_load (const A* obj) noexcept;
        T atomic_load_explicit (const volatile A* obj, memory_order sync) noexcept;
        T atomic_load_explicit (const A* obj, memory_order sync) noexcept;
```

同上面函数，返回obj中包含的值。该操作是原子性的，并使用顺序一致性(memory_order_seq_cst)。要制定内存数序使用显示调用函数atomic_load_explicit。

1. [**std::atomic_exchange**](http://www.cplusplus.com/reference/atomic/atomic_exchange/)和[**std::atomic_exchange_explicit**](http://www.cplusplus.com/reference/atomic/atomic_exchange_explicit/)



```c
template (1)    
        template <class T> T atomic_exchange (volatile atomic<T>* obj, T val) noexcept;
        template <class T> T atomic_exchange (atomic<T>* obj, T val) noexcept;
        template <class T>
          T atomic_exchange_explicit(volatile atomic<T>* obj, T val, memory_order sync) noexcept;
        template <class T>
          T atomic_exchange_explicit(atomic<T>* obj, T val, memory_order sync) noexcept;
overloads (2)   
        T atomic_exchange (volatile A* obj, T val) noexcept;
        T atomic_exchange (A* obj, T val) noexcept;
        T atomic_exchange_explicit(volatile A* obj, T val, memory_order sync) noexcept;
        T atomic_exchange_explicit(A* obj, T val, memory_order sync) noexcept;
```

用val替换obj中包含的值，并返回obj之前的值。
 整个操作是原子性的(一个原子的读-修改-写操作):在读取(返回)值和被该函数修改的瞬间之间，obj的值不会受到其他线程的影响。要制定内存数序使用显示调用函数atomic_exchange_explicit。
 注意： 这个显示代码网站上有错误，本文已经改正。

1. [**atomic_compare_exchange_weak**](http://www.cplusplus.com/reference/atomic/atomic_compare_exchange_weak/)和[**atomic_compare_exchange_weak_explicit**](http://www.cplusplus.com/reference/atomic/atomic_compare_exchange_weak_explicit/)



```c
template (1)    
        template <class T>
        bool atomic_compare_exchange_weak (volatile atomic<T>* obj, T* expected, T val) noexcept;
        template <class T>
          bool atomic_compare_exchange_weak (atomic<T>* obj, T* expected, T val) noexcept;
        template <class T>
          bool atomic_compare_exchange_weak_explicit (volatile atomic<T>* obj,
              T* expected, T val, memory_order success, memory_order failure) noexcept;
        template <class T>
          bool atomic_compare_exchange_weak_explicit (atomic<T>* obj,
              T* expected, T val, memory_order success, memory_order failure) noexcept;
overloads (2)   
        bool atomic_compare_exchange_weak (volatile A* obj, T* expected, T val) noexcept;
        bool atomic_compare_exchange_weak (A* obj, T* expected, T val) noexcept;
        bool atomic_compare_exchange_weak_explicit (volatile A* obj,
            T* expected, T val, memory_order success, memory_order failure) noexcept;
        bool atomic_compare_exchange_weak_explicit (A* obj,
            T* expected, T val, memory_order success, memory_order failure) noexcept;
```

将obj中包含的值与预期的内容进行比较:
 -如果是真的，它会用val替换包含的值。
 -如果是假的，它将用所包含的值替换预期值

函数总是访问包含的值来读取它，如果这个比较是真的，那么它也会替换它。但是整个操作都是原子性的:在读取值和被替换的时刻之间，它的值不能被其他线程修改。

** 注意：
 这个函数直接的比较物理内容所包含的价值与预期的内容,这可能导致得到使用operator==比较的结果是一个失败的结果,因为对象底层的物理内容中可能存在位对齐或其他逻辑表示相同但是物理表示不同的值(比如 true 和 2 或 3，它们在逻辑上都表示"真"，但在物理上两者的表示并不相同)。
 不像 compare_exchange_strong，这个弱版本允许(spuriously 地)返回 false(即原子对象所封装的值与参数 expected 的物理内容相同，但却仍然返回 false)，即使在预期的实际情况与所包含的对象相比较时也是如此。对于某些循环算法来说，这可能是可接受的行为，并且可能会在某些平台上带来显著的性能提升。在这些虚假的失败中，函数返回false，而不修改预期。
 对于非循环算法来说， compare_exchange_strong通常是首选。

1. [**atomic_compare_exchange_strong**](http://www.cplusplus.com/reference/atomic/atomic_compare_exchange_strong/)和[**atomic_compare_exchange_strong_explicit**](http://www.cplusplus.com/reference/atomic/atomic_compare_exchange_strong_explicit/)



```c
template (1)    
        template <class T>
            bool atomic_compare_exchange_strong (volatile atomic<T>* obj, T* expected, T val) noexcept;
        template <class T>
            bool atomic_compare_exchange_strong (atomic<T>* obj, T* expected, T val) noexcept;
        template <class T>
            bool atomic_compare_exchange_strong_explicit (volatile atomic<T>* obj,
                  T* expected, T val, memory_order success, memory_order failure) noexcept;
        template <class T>
            bool atomic_compare_exchange_strong_explicit (atomic<T>* obj,
                  T* expected, T val, memory_order success, memory_order failure) noexcept;
overloads (2)   
        bool atomic_compare_exchange_strong (volatile A* obj, T* expected, T val) noexcept;
        bool atomic_compare_exchange_strong (A* obj, T* expected, T val) noexcept;
        bool atomic_compare_exchange_strong_explicit (volatile A* obj,
            T* expected, T val, memory_order success, memory_order failure) noexcept;
        bool atomic_compare_exchange_strong_explicit (A* obj,
            T* expected, T val, memory_order success, memory_order failure) noexcept;
```

将obj中包含的值与预期的值进行比较:
 -如果是真的，它会用val替换包含的值。
 -如果是假的，它将用所包含的值替换预期值。
 函数总是访问包含的值来读取它，如果这个比较是真的，那么它也会替换它。但是整个操作都是原子性的:在读取值和被替换的时刻之间，它的值不能被其他线程修改。
 ** 注意：
 compare_exchange_strong 跟 compare_exchange_week 不同的是：
 与compare_exchange_weak 不同, strong版本的 compare-and-exchange 操作不允许(spuriously 地)返回 false，即原子对象所封装的值与参数 expected 的物理内容相同，比较操作一定会为 true。不过在某些平台下，如果算法本身需要循环操作来做检查， compare_exchange_weak 的性能会更好。

1. [**atomic_fetch_add**](http://www.cplusplus.com/reference/atomic/atomic_fetch_add/)和[**atomic_fetch_add_explicit**](http://www.cplusplus.com/reference/atomic/atomic_fetch_add_explicit/)



```c
template (integral) (1) 
        template <class T> T atomic_fetch_add (volatile atomic<T>* obj, T val) noexcept;
        template <class T> T atomic_fetch_add (atomic<T>* obj, T val) noexcept;
        template <class T>
          T atomic_fetch_add_explicit (volatile atomic<T>* obj, T val, memory_order sync) noexcept;
        template <class T>
          T atomic_fetch_add_explicit (atomic<T>* obj, T val, memory_order sync) noexcept;
template (pointer) (2)  
        template <class U> U* atomic_fetch_add (volatile atomic<U*>* obj, ptrdiff_t val) noexcept;
        template <class U> U* atomic_fetch_add (atomic<U*>* obj, ptrdiff_t val) noexcept;
        template <class U>
          U* atomic_fetch_add_explicit (volatile atomic<U*>* obj, ptrdiff_t val, memory_order sync) noexcept;
        template <class U>
          U* atomic_fetch_add_explicit (atomic<U*>* obj, ptrdiff_t val, memory_order sync) noexcept;
overloads (3)   
        T atomic_fetch_add (volatile A* obj, M val) noexcept;
        T atomic_fetch_add (A* obj, M val) noexcept;
        T atomic_fetch_add_explicit (volatile A* obj, M val, memory_order sync) noexcept;
        T atomic_fetch_add_explicit (A* obj, M val, memory_order sync) noexcept;
```

将val添加到obj中所包含的值。整个操作都是原子性的:在读取(返回)值和被该函数修改的时刻之间，值不能被修改。要制定内存数序使用显示调用函数atomic_fetch_add_explicit 。

1. [**atomic_fetch_sub**](http://www.cplusplus.com/reference/atomic/atomic_fetch_sub/)和[**atomic_fetch_sub_explicit**](http://www.cplusplus.com/reference/atomic/atomic_fetch_sub_explicit/)



```c
template (integral) (1) 
        template <class T> T atomic_fetch_sub (volatile atomic<T>* obj, T val) noexcept;
        template <class T> T atomic_fetch_sub (atomic<T>* obj, T val) noexcept;
        template <class T>
          T atomic_fetch_sub_explicit (volatile atomic<T>* obj, T val, memory_order sync) noexcept;
        template <class T>
          T atomic_fetch_sub_explicit (atomic<T>* obj, T val, memory_order sync) noexcept;
template (pointer) (2)  
        template <class U> U* atomic_fetch_sub (volatile atomic<U*>* obj, ptrdiff_t val) noexcept;
        template <class U> U* atomic_fetch_sub (atomic<U*>* obj, ptrdiff_t val) noexcept;
        template <class U>
          U* atomic_fetch_sub_explicit (volatile atomic<U*>* obj, ptrdiff_t val, memory_order sync) noexcept;
        template <class U>
          U* atomic_fetch_sub_explicit (atomic<U*>* obj, ptrdiff_t val, memory_order sync) noexcept;
overloads (3)   
        T atomic_fetch_sub (volatile A* obj, M val) noexcept;
        T atomic_fetch_sub (A* obj, M val) noexcept;
        T atomic_fetch_sub_explicit (volatile A* obj, M val, memory_order sync) noexcept;
        T atomic_fetch_sub_explicit (A* obj, M val, memory_order sync) noexcept;
```

从obj中所包含的值减去val。整个操作都是原子性的:在读取(返回)值和被该函数修改的时刻之间，值不能被修改。要制定内存数序使用显示调用函数atomic_fetch_sub_explicit 。

1. [**atomic_fetch_and**](http://www.cplusplus.com/reference/atomic/atomic_fetch_and/)和[**atomic_fetch_and_explicit**](http://www.cplusplus.com/reference/atomic/atomic_fetch_and_explicit/)



```c
template (integral) (1) 
        template <class T> T atomic_fetch_and (volatile atomic<T>* obj, T val) noexcept;
        template <class T> T atomic_fetch_and (atomic<T>* obj, T val) noexcept;
        template <class T>
          T atomic_fetch_and_explicit (volatile atomic<T>* obj,T val, memory_order sync) noexcept;
        template <class T>
          T atomic_fetch_and_explicit (atomic<T>* obj,T val, memory_order sync) noexcept;
overloads (2)   
        T atomic_fetch_and (volatile A* obj, T val) noexcept;
        T atomic_fetch_and (A* obj, T val) noexcept;
        T atomic_fetch_and_explicit (volatile A* obj, T val, memory_order sync) noexcept;
        T atomic_fetch_and_explicit (A* obj, T val, memory_order sync) noexcept;
```

读取obj中包含的值，并通过在读取值和val之间执行一个位操作“与”操作来替换它。
 整个操作是原子性的(一个原子的读-修改-写操作):在读取(返回)值和被该函数修改的那一刻之间，值不会受到其他线程的影响。要制定内存数序使用显示调用函数atomic_fetch_and_explicit 。

1. [**atomic_fetch_or**](http://www.cplusplus.com/reference/atomic/atomic_fetch_or/)和[**atomic_fetch_or_explicit**](http://www.cplusplus.com/reference/atomic/atomic_fetch_or_explicit/)



```c
template (integral) (1) 
        template <class T> T atomic_fetch_or (volatile atomic<T>* obj, T val) noexcept;
        template <class T> T atomic_fetch_or (atomic<T>* obj, T val) noexcept;
        template <class T>
          T atomic_fetch_or_explicit (volatile atomic<T>* obj, T val, memory_order sync) noexcept;
        template <class T>
          T atomic_fetch_or_explicit (atomic<T>* obj,T val, memory_order sync) noexcept;
overloads (2)   
        T atomic_fetch_or (volatile A* obj, T val) noexcept;
        T atomic_fetch_or (A* obj, T val) noexcept;
        T atomic_fetch_or_explicit (volatile A* obj, T val, memory_order sync) noexcept;
        T atomic_fetch_or_explicit (A* obj, T val, memory_order sync) noexcept;
```

读取obj中包含的值，并通过在读取值和val之间执行一个位操作“或”操作来替换它。
 整个操作是原子性的(一个原子的读-修改-写操作):在读取(返回)值和被该函数修改的那一刻之间，值不会受到其他线程的影响。要制定内存数序使用显示调用函数atomic_fetch_or_explicit

1. [**atomic_fetch_xor**](http://www.cplusplus.com/reference/atomic/atomic_fetch_xor/)和[**atomic_fetch_xor_explicit**](http://www.cplusplus.com/reference/atomic/atomic_fetch_xor_explicit/)



```c
template (integral) (1) 
        template <class T> T atomic_fetch_xor (volatile atomic<T>* obj, T val) noexcept;
        template <class T> T atomic_fetch_xor (atomic<T>* obj, T val) noexcept;
        template <class T>
          T atomic_fetch_xor_explicit (volatile atomic<T>* obj,T val, memory_order sync) noexcept;
        template <class T>
          T atomic_fetch_xor_explicit (atomic<T>* obj,T val, memory_order sync) noexcept;
overloads (2)   
        T atomic_fetch_xor (volatile A* obj, T val) noexcept;
        T atomic_fetch_xor (A* obj, T val) noexcept;
        T atomic_fetch_xor_explicit (volatile A* obj, T val, memory_order sync) noexcept;
        T atomic_fetch_xor_explicit (A* obj, T val, memory_order sync) noexcept;
```

读取obj中包含的值，并通过在读取值和val之间执行一个位操作“异或”操作来替换它。
 整个操作是原子性的(一个原子的读-修改-写操作):在读取(返回)值和被该函数修改的那一刻之间，值不会受到其他线程的影响。要制定内存数序使用显示调用函数atomic_fetch_xor_explicit

1. [**atomic_flag_test_and_set**](http://www.cplusplus.com/reference/atomic/atomic_flag_test_and_set/)和[**atomic_flag_test_and_set_explicit**](http://www.cplusplus.com/reference/atomic/atomic_flag_test_and_set_explicit/)



```c
        bool atomic_flag_test_and_set (volatile atomic_flag* obj) noexcept;
        bool atomic_flag_test_and_set (atomic_flag* obj) noexcept;
        bool atomic_flag_test_and_set (volatile atomic_flag* obj, memory_order sync) noexcept;
        bool atomic_flag_test_and_set (atomic_flag* obj, memory_order sync) noexcept;
```

设置obj指向的原子标志，并返回调用之前的值。
 整个操作是原子性的(一个原子的读-修改-写操作):在读取(返回)值和被该函数修改的瞬间之间，obj的值不会受到其他线程的影响。要制定内存数序使用显示调用函数atomic_flag_test_and_set

1. [**atomic_flag_clear**](http://www.cplusplus.com/reference/atomic/atomic_flag_clear/)和[**atomic_flag_clear_explicit**](http://www.cplusplus.com/reference/atomic/atomic_flag_clear_explicit/)



```c
        void atomic_flag_clear (volatile atomic_flag* obj) noexcept;
        void atomic_flag_clear (atomic_flag* obj) noexcept;
        void atomic_flag_clear_explicit(volatile atomic_flag* obj, memory_order sync) noexcept;
        void atomic_flag_clear_explicit(atomic_flag* obj, memory_order sync) noexcept;
```

清除obj，将它的标志值设置为false。该操作是原子性的。要制定内存数序使用显示调用函数atomic_flag_clear_explicit。

## 六、Macro functions

1. ATOMIC_VAR_INIT
    ATOMIC_VAR_INIT(val)    //初始化 std::atomic 对象。
    这个宏的存在是为了与C实现兼容，在其中，它被用作构造函数(默认构造的)原子对象;在C++中，这个初始化可以由初始化构造函数直接执行。
2. ATOMIC_FLAG_INIT
    ATOMIC_FLAG_INIT    //初始化 std::atomic_flag 对象。
    这个宏的定义用来将类型std::atomic_flag的对象初始化到clear的状态。



