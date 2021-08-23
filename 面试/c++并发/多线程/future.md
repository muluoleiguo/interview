# 一、<future>头文件描述及结构

<future>头文件功能允许对特定提供者设置的值进行异步访问，可能在不同的线程中。
 这些提供程序(要么是promise 对象，要么是packaged_task对象，或者是对异步的调用async)与future对象共享共享状态:提供者使共享状态就绪的点与future对象访问共享状态的点同步。
 <future> 头文件结构：

| **type**    | **Classes**                                                  | **description**                                              |
| ----------- | ------------------------------------------------------------ | ------------------------------------------------------------ |
| Providers   | [**promise**](http://www.cplusplus.com/reference/future/promise/)   [**packaged_task**](http://www.cplusplus.com/reference/future/packaged_task/) | 为异步检索存储一个值(类模板)   打包一个函数，用于存储异步检索的返回值(类模板) |
| Futures     | [**future**](http://www.cplusplus.com/reference/future/future/)   [**shared_future**](http://www.cplusplus.com/reference/future/shared_future/) | 等待一个异步设置的值(类模板)   等待一个异步设置的值(可能被其他future所引用)(类模板) |
| Other types | [**future_error**](http://www.cplusplus.com/reference/future/future_error/)   [**future_errc**](http://www.cplusplus.com/reference/future/future_errc/)   [**future_status**](http://www.cplusplus.com/reference/future/future_status/)   [**launch**](http://www.cplusplus.com/reference/future/launch/) | 报告与futures或 promises有关的错误 (类)   识别future的错误代码(枚举)   用于定时的操作的返回值(枚举)   指定std::async的启动策略(枚举) |

| **type**        | **Functions**                                                | **description**                                              |
| --------------- | ------------------------------------------------------------ | ------------------------------------------------------------ |
| Providers       | [**async**](http://www.cplusplus.com/reference/future/async/) | 异步地运行一个函数(可能在一个新线程中)并返回一个保持结果的std::future(函数模板) |
| Other functions | [**future_category**](http://www.cplusplus.com/reference/future/future_category/) | 识别future的错误类别 (函数)                                  |

# **二、<future>头文件Classes**

## 1、 Providers

### 1、 [**promise**](http://www.cplusplus.com/reference/future/promise/)



```cpp
template <class T>  promise;   
template <class R&> promise<R&>;     // specialization : T is a reference type (R&)
template <>         promise<void>;   // specialization : T is void
```

promise是一个存储由一个(可能是另一个线程)future对象检索的类型T的值的对象，可以提供一个同步点。在构造上，承诺对象与一个新的共享状态相关联，它们可以存储类型T的值或从std::exception.中派生的异常。
 这个共享状态可以通过调用成员get_future关联到一个future对象。调用后，两个对象共享相同的共享状态:
 1、 promise对象是异步提供程序，它将为共享状态设置一个值。
 2、 future对象是一个异步返回对象，它可以检索共享状态的值，等待它在必要时就绪。
 共享状态的生命周期至少持续到最后一个对象释放它或被销毁。因此，它可以使promise对象存活下来，如果把它与future对象联系在一起的话，它就能在第一个地方得到它。
 两个promise的特化定义在<future>头文件中，它们以与非专门化模板相同的方式操作，除非参数是set_value和set_value_at_thread_exit成员函数。
 [示例1](https://github.com/jorionwen/threadtest/blob/master/future/example1.cpp)：



```cpp
// promise example
#include <iostream>       // std::cout
#include <functional>     // std::ref
#include <thread>         // std::thread
#include <future>         // std::promise, std::future

void print_int (std::future<int>& fut) {
  int x = fut.get();
  std::cout << "value: " << x << '\n';
}

int main ()
{
  std::promise<int> prom;                      // create promise

  std::future<int> fut = prom.get_future();    // engagement with future

  std::thread th1 (print_int, std::ref(fut));  // send future to new thread

  prom.set_value (10);                         // fulfill promise
                                               // (synchronizes with getting the future)
  th1.join();
  return 0;
}
```

#### Member functions

1. [**(constructor)构造函数**](http://www.cplusplus.com/reference/future/promise/promise/)



```jsx
       default (1)         promise();
       with allocator (2)   template <class Alloc> promise (allocator_arg_t aa, 
                                                                  const Alloc& alloc);
       copy [deleted] (3)   promise (const promise&) = delete;
       move (4)           promise (promise&& x) noexcept;
```

(1)默认构造函数
 对象被初始化，以访问一个新的空共享状态。
 (2)构造器和分配器
 与(1)相同，但是使用alloc为共享状态分配内存。
 aa： std::allocator_arg价值。这个常量值只是用于显式地选择这个构造函数重载。
 alloc：分配器对象。容器保存并使用这个分配器的内部副本，并使用它为共享状态分配存储。
 (3)拷贝构造函数(删除)
 承诺对象不能被复制。
 (4)构造函数
 构造的对象获得x的共享状态(如果有的话)。x是没有共享状态的。同一类型的另一个承诺对象(具有相同的模板参数T)。
 函数执行异常情况:
 强保证:在抛出异常时没有任何效果。
 根据库的实现，这个成员函数可能会在某些条件(例如bad_alloc)上抛出异常(例如在没有分配内存的情况下)。
 移动构造函数(4)从不抛出异常(无抛出保证)。
 [示例2](https://github.com/jorionwen/threadtest/blob/master/future/example2.cpp)：



```cpp
  // promise constructors
  #include <iostream>       // std::cout
  #include <functional>     // std::ref
  #include <memory>         // std::allocator, std::allocator_arg
  #include <thread>         // std::thread
  #include <future>         // std::promise, std::future

  void print_int (std::future<int>& fut) {
      int x = fut.get();
      std::cout << "value: " << x << '\n';
  }

  int main ()
  {
      std::promise<int> foo;
      std::promise<int> bar = std::promise<int>(std::allocator_arg,std::allocator<int>());

      std::future<int> fut = bar.get_future();

      std::thread th (print_int, std::ref(fut));

      bar.set_value (20);

      th.join();
      return 0;
  }
```

1. [**(destructor)析构函数**](http://www.cplusplus.com/reference/future/promise/~promise/)
    析构promise。
    放弃共享状态(如果有的话)，并破坏promise对象。
    注意，如果其他(future)对象与相同的共享状态相关联，共享状态本身不会被破坏(直到所有这些对象也释放它)。
    如果对象在它的共享状态(通过显式设置值或引用)之前被销毁，那么共享状态将自动为包含future_error类型的异常做好准备(有一个故障时的错误条件)。
2. [**std::promise::operator=**](http://www.cplusplus.com/reference/future/promise/operator%3D/)
    move (1)           promise& operator= (promise&& rhs) noexcept;
    copy [deleted] (2)   promise& operator= (const promise&) = delete;
    Move-assign承诺
    获得rhs(如果有的话)的共享状态。
    在调用(如果有的话)之前与对象相关联的共享状态(如果promise对象被摧毁了)。
    rhs没有共享状态:任何影响其共享状态的操作都将抛出future_error和no_state错误条件。
    promise对象不能拷贝。
    [示例3](https://github.com/jorionwen/threadtest/blob/master/future/example3.cpp)：



```c
 // promise::operator=
 #include <iostream>       // std::cout
 #include <thread>         // std::thread
 #include <future>         // std::promise, std::future

 std::promise<int> prom;

 void print_global_promise () {
     std::future<int> fut = prom.get_future();
     int x = fut.get();
     std::cout << "value: " << x << '\n';
 }

 int main ()
 {
     std::thread th1 (print_global_promise);
     prom.set_value (10);
     th1.join();

     prom = std::promise<int>();    // reset, by move-assigning a new promise

     std::thread th2 (print_global_promise);
     prom.set_value (20);
     th2.join();

     return 0;
 }
```

1. [**std::promise::get_future**](http://www.cplusplus.com/reference/future/promise/get_future/)
    future<T> get_future();    // 定义
    得到future对象。
    返回关联着共享状态的future对象。
    返回的future对象可以通过promise对象在准备就绪时访问共享状态的值或异常。
    每个promise共享状态只能检索到一个future对象。
    在调用此函数之后，预期将在某个点(通过设置值或异常)使其共享状态就绪(通过设置值或异常)，否则它将自动为销毁所准备就绪，包含了future_error类型的异常(有一个破碎的承诺错误条件)。
    [示例4](https://github.com/jorionwen/threadtest/blob/master/future/example4.cpp)：



```cpp
  // promise example
  #include <iostream>       // std::cout
  #include <functional>     // std::ref
  #include <thread>         // std::thread
  #include <future>         // std::promise, std::future

  void print_int (std::future<int>& fut) {
    int x = fut.get();
    std::cout << "value: " << x << '\n';
  }

  int main ()
  {
    std::promise<int> prom;                      // create promise

    std::future<int> fut = prom.get_future();    // engagement with future

    std::thread th1 (print_int, std::ref(fut));  // send future to new thread

    prom.set_value (10);                         // fulfill promise
                                               // (synchronizes with getting the future)
    th1.join();
    return 0;
  }
```

1. [**std::promise::set_value**](http://www.cplusplus.com/reference/future/promise/set_value/)
    generic template(1) void set_value (const T& val);
    void set_value (T&& val);
    specializations(2) void promise<R&>::set_value (R& val); //when T is a reference type(R&)
    void promise<void>::set_value (void); //when T is void
    存储val作为共享状态的值，它已经准备好了。
    如果一个与相同共享状态相关联的未来对象正在等待对未来的调用::get，它将打开并返回val。
    void专门化的成员只是使共享状态就绪，没有设置任何值。
    [示例5](https://github.com/jorionwen/threadtest/blob/master/future/example5.cpp)：



```cpp
#include <thread>
#include <future>
#include <cctype>
#include <vector>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <sstream>
int main()
{
    std::istringstream iss_numbers{"3 4 1 42 23 -23 93 2 -289 93"};
    std::istringstream iss_letters{" a 23 b,e a2 k k?a;si,ksa c"};

    std::vector<int> numbers;
    std::vector<char> letters;
    std::promise<void> numbers_promise, letters_promise;

    auto numbers_ready = numbers_promise.get_future();
    auto letter_ready = letters_promise.get_future();
    
    std::thread value_reader([&]
    {
        // I/O operations.
        std::copy(std::istream_iterator<int>{iss_numbers},
                  std::istream_iterator<int>{},
                  std::back_inserter(numbers));
 
        //Notify for numbers.
        numbers_promise.set_value();
 
        std::copy_if(std::istreambuf_iterator<char>{iss_letters},
                     std::istreambuf_iterator<char>{},
                     std::back_inserter(letters),
                     ::isalpha);
 
        //Notify for letters.
        letters_promise.set_value();
    });

    numbers_ready.wait();
 
    std::sort(numbers.begin(), numbers.end());
    if (letter_ready.wait_for(std::chrono::seconds(1)) ==
            std::future_status::timeout)
    {
        //output the numbers while letters are being obtained.
        for (int num : numbers) std::cout << num << ' ';
        numbers.clear(); //Numbers were already printed.
    }
 
    letter_ready.wait();
    std::sort(letters.begin(), letters.end());
 
    //If numbers were already printed, it does nothing.
    for (int num : numbers) std::cout << num << ' ';
    std::cout << '\n';
 
    for (char let : letters) std::cout << let << ' ';
    std::cout << '\n';
 
    value_reader.join();
    return 0;
 }
```

1. **std::promise::set_exception**

   void set_exception (exception_ptr p);  //定义

   设置异常

   变成准备状态的共享状态中存储异常指针p，。

   如果与同一共享状态相关联的future对象当前正在等待vfuture::get，它将抛出异常对象，并抛出异常对象。

   此成员函数在以下条件下抛出异常：根据库的实现，这个成员函数也可能会抛出异常来报告其他情况。

   ![img](https:////upload-images.jianshu.io/upload_images/3414858-8d33eabf670a9607.png?imageMogr2/auto-orient/strip|imageView2/2/w/516/format/webp)

   示例6

   ：



```cpp
  // promise::set_exception
  #include <iostream>       // std::cin, std::cout, std::ios
  #include <functional>     // std::ref
  #include <thread>         // std::thread
  #include <future>         // std::promise, std::future
  #include <exception>      // std::exception, std::current_exception

  void get_int (std::promise<int>& prom) {
    int x;
    std::cout << "Please, enter an integer value: ";
    std::cin.exceptions (std::ios::failbit);   // throw on failbit
    try {
      std::cin >> x;                           // sets failbit if input is not int
      prom.set_value(x);
    }
    catch (std::exception&) {
      prom.set_exception(std::current_exception());
    }
  }

  void print_int (std::future<int>& fut) {
    try {
      int x = fut.get();
      std::cout << "value: " << x << '\n';
    }
    catch (std::exception& e) {
      std::cout << "[exception caught: " << e.what() << "]\n";
    }
  }
  
  int main ()
  {
    std::promise<int> prom;
    std::future<int> fut = prom.get_future();

    std::thread th1 (print_int, std::ref(fut));
    std::thread th2 (get_int, std::ref(prom));

    th1.join();
    th2.join();
    return 0;
  }
```

1. **std::promise::set_value_at_thread_exit**

   generic template (1) void set_value_at_thread_exit (const T& val);

   void set_value_at_thread_exit (T&& val);

   specializations (2)   void promise<R&>::set_value_at_thread_exit (R& val);     // when T is a reference type (R&)

   void promise<void>::set_value_at_thread_exit (void);     // when T is void

   在线程退出时设置值

   存储val作为共享状态的值，而不立即准备。相反，它将在线程退出时自动完成，因为所有线程存储时间的对象都被销毁了。

   如果与同一共享状态相关联的future对象正在等待调用future:get，它会一直阻塞，直到线程结束。一旦线程结束，它将打开并返回val。

   注意，调用此函数已经设置了共享状态的值，并且任何尝试在此调用和线程末尾修改此值的调用，都会将future_error与promise_already_心满意足作为错误条件。

   此成员函数在以下条件下抛出异常：

   ![img](https:////upload-images.jianshu.io/upload_images/3414858-cf17c2f6b7785bcd.png?imageMogr2/auto-orient/strip|imageView2/2/w/518/format/webp)

   示例7

   ：



```c
  #include <iostream>
  #include <future>
  #include <thread>
 
  int main()
  {
     // using namespace std::chrono_literals;
      std::promise<int> p;
      std::future<int> f = p.get_future();
      std::thread([&p] {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            p.set_value_at_thread_exit(9);
      }).detach();
 
      std::cout << "Waiting..." << std::flush;
      f.wait();
      std::cout << "Done!\nResult is: " << f.get() << '\n';
      return 0;
  }
```

1. **std::promise::set_exception_at_thread_exit**

   void set_exception_at_thread_exit (exception_ptr p);

   在线程退出时设置异常

   在共享状态中存储异常指针，而不立即准备。相反，它将在线程退出时自动完成，因为所有线程存储时间的对象都被销毁了。

   如果与同一共享状态相关联的future对象正在等待调用future:get，它会一直阻塞，直到线程结束。一旦线程结束，它就会抛出异常对象p。

   注意，调用这个函数已经在共享状态中设置了一个值，并且在这个调用和线程结束之间修改这个值的任何调用都会将future_error与promise_already_心满意足作为错误条件。

   此成员函数在以下条件下抛出异常：

   ![img](https:////upload-images.jianshu.io/upload_images/3414858-cf17c2f6b7785bcd.png?imageMogr2/auto-orient/strip|imageView2/2/w/518/format/webp)

2. [**std::promise::swap**](http://www.cplusplus.com/reference/future/promise/swap/)
    void swap (promise& x) noexcept;
    与x交换共享状态(如果有的话)。

#### Non-member function overloads

1. [**std::swap (promise)**](http://www.cplusplus.com/reference/future/promise/swap-free/)
    template <class T>
    void swap (promise<T>& x, promise<T>& y) noexcept;
    交换x和y之间的共享状态。这是一个类似重载的交换，就像x.swap(y)被调用。

#### Non-member class specializations

1. **std::uses_allocator<promise>**

   template <class T, class Alloc>

   struct uses_allocator<promise<T>,Alloc>;

   为保证使用分配器

   uses_allocator的这种特性专门化通知承诺对象可以用配置器构建，即使类没有任何allocator_type成员。它在< future >中定义，与相同的行为如下:

   template <class T, class Alloc>

   struct uses_allocator<promise<T>,Alloc> : true_type {};

   在里在说下 std::uses_allocator

   template <class T, class Alloc> struct uses_allocator;

   Uses allocator

   ![img](https:////upload-images.jianshu.io/upload_images/3414858-ba1eedaf43ed965c.png?imageMogr2/auto-orient/strip|imageView2/2/w/269/format/webp)

   识别T是否接受来自Alloc的分配器可转换的特性类。

   它从 integral_constant继承为true_type或false_type，这取决于T是否接受这样的分配器。

   这个模板的非专门化定义继承自true_type，如果T::allocator_type存在和is_敞篷车< Alloc,T::allocator_type >::值不是假的。

   不填写这一要求但仍然使用分配器的类将专门化该模板，以从true_type派生出:

   1、 构造函数的最后一个参数有类型Alloc。

   2、 构造函数的第一个参数有类型allocator_arg_t，第二个参数有类型Alloc。

   示例8

   ：



```c
  // uses_allocator example
  #include <iostream>
  #include <memory>
  #include <vector>

  int main() {
    typedef std::vector<int> Container;
    typedef std::allocator<int> Allocator;

    if (std::uses_allocator<Container,Allocator>::value) {
      Allocator alloc;
      Container foo (5,10,alloc);
      for (auto x:foo) std::cout << x << ' ';
    }
    std::cout << '\n';
    return 0;
  }
```

### 2、[**packaged_task**](http://www.cplusplus.com/reference/future/packaged_task/)

包装的任务
 packaged_task封装可调用的元素，并允许异步检索其结果。
 它类似于std::function，但是将其结果自动传输到一个future对象。
 对象包含两个元素:
 1、一个存储的任务，它是一些可调用的对象(例如函数指针、指向成员或函数对象的指针)，其调用签名将在Args中进行类型的参数。返回一个类型为Ret的值。
 2、一个共享状态，它可以存储调用存储任务(类型为Ret)的结果，并通过未来异步访问。
 共享状态通过调用成员get_future关联到一个future对象。调用后，两个对象共享相同的共享状态:
 1、 packaged_task对象是异步提供程序，并期望通过调用存储任务在某个点上设置共享状态。
 2、 future对象是一个异步返回对象，可以检索共享状态的值，等待它在必要时就绪。
 共享状态的生命周期至少持续到最后一个对象释放它或被销毁。因此，它可以在packaged_task对象中生存，该对象在与将来相关联的情况下，首先获得它。
 [示例9](https://github.com/jorionwen/threadtest/blob/master/future/example9.cpp)：



```cpp
// packaged_task example
#include <iostream>     // std::cout
#include <future>       // std::packaged_task, std::future
#include <chrono>       // std::chrono::seconds
#include <thread>       // std::thread, std::this_thread::sleep_for

// count down taking a second for each value:
int countdown (int from, int to) {
  for (int i=from; i!=to; --i) {
    std::cout << i << '\n';
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  std::cout << "Lift off!\n";
  return from-to;
}

int main ()
{
  std::packaged_task<int(int,int)> tsk (countdown);   // set up packaged_task
  std::future<int> ret = tsk.get_future();            // get future

  std::thread th (std::move(tsk),10,0);   // spawn thread to count down from 10 to 0

  // ...

  int value = ret.get();                  // wait for the task to finish and get result

  std::cout << "The countdown lasted for " << value << " seconds.\n";

  th.join();

  return 0;
}
```

#### Member functions

1. [**(constructor)构造函数**](http://www.cplusplus.com/reference/future/packaged_task/packaged_task/)
    default (1)         packaged_task() noexcept;
    initialization (2)   template <class Fn>
    explicit packaged_task (Fn&& fn);
    with allocator (3)   template <class Fn, class Alloc>
    explicit packaged_task (allocator_arg_t aa, const Alloc& alloc, Fn&& fn);
    copy [deleted] (4)   packaged_task (packaged_task&) = delete;
    move (5)             packaged_task (packaged_task&& x) noexcept;
    (1)默认构造函数
    对象被初始化，没有共享状态，也没有存储任务。
    (2)初始化的构造函数
    对象有一个共享状态，它的存储任务初始化为fn。
    (3)带有分配器的初始化构造函数
    与(2)相同，但是使用alloc为内部数据结构分配内存。
    (4)拷贝构造函数(删除)
    无法复制packaged_task对象(删除构造函数)。
    (5)构造函数
    构造的对象获得x的共享状态(如果有的话)，并且它的存储任务被移动。
    x是没有共享状态的。
    参数：
    1、 fn：  一个可调用的指针指向函数、指向成员的指针或任何形式的可构造函数对象(即。一个类定义操作符()的对象，包括闭包和函数对象。这个函数的返回类型和参数应该与packaged_task的模板参数(Ret和Args…)相对应。存储任务被设置为fn(内部初始化为std::forward < fn >(fn))。
    2、 aa： std::allocator_arg价值。这个常量值只是用于显式地选择这个构造函数重载。
    3、 alloc： 容器保存并使用这个分配器的内部副本，并使用它为内部数据结构分配存储。
    4、 x：同一类型的另一个packaged_task对象(具有相同的模板参数、Ret和Args)。
    [示例10](https://github.com/jorionwen/threadtest/blob/master/future/example10.cpp):



```c
// packaged_task construction / assignment
#include <iostream>     // std::cout
#include <utility>      // std::move
#include <future>       // std::packaged_task, std::future
#include <thread>       // std::thread

  int main ()
  {
    std::packaged_task<int(int)> foo;                          // default-constructed
    std::packaged_task<int(int)> bar ([](int x){return x*2;}); // initialized

    foo = std::move(bar);                                      // move-assignment

    std::future<int> ret = foo.get_future();  // get future
    std::thread(std::move(foo),10).detach();  // spawn thread and call task
    // ...
    int value = ret.get();                    // wait for the task to finish and get result
    std::cout << "The double of 10 is " << value << ".\n";

    return 0;
  }
```

1. [**(destructor)析构函数**](http://www.cplusplus.com/reference/future/packaged_task/~packaged_task/)
    放弃共享状态(如果有的话)并销毁packaged_task对象。
    注意，如果其他(future)对象与相同的共享状态相关联，共享状态本身不会被破坏(直到所有这些对象也释放它)。
    如果对象在已经准备好(通过调用存储任务)之前被销毁，那么共享状态就会自动被准备好，包含一个future_error类型的异常(有一个破碎的承诺错误条件)。
2. [**std::packaged_task::operator=**](http://www.cplusplus.com/reference/future/packaged_task/operator%3D/)
    move (1)             packaged_task& operator= (packaged_task&& rhs) noexcept;
    copy [deleted] (2)   packaged_task& operator= (const packaged_task&) = delete;
    获得rhs的共享状态和存储任务。
    在调用(如果有的话)之前，与对象关联的共享状态(如果有的话)(就像packaged_task被销毁一样)。
    rhs没有共享状态:任何访问其共享状态的操作都将抛出future_error和no_state错误条件。
    示例同上面。
3. [**std::packaged_task::valid**](http://www.cplusplus.com/reference/future/packaged_task/valid/)
    bool valid() const noexcept;
    检查有效的共享状态
    返回packaged_task当前是否与共享状态相关联。
    对于默认构造的packaged_task对象，此函数将返回false(除非被分配或与有效的packaged_task交换)。
    [示例11](https://github.com/jorionwen/threadtest/blob/master/future/example11.cpp):



```c
// packaged_task::get_future
#include <iostream>     // std::cout
#include <utility>      // std::move
#include <future>       // std::packaged_task, std::future
#include <thread>       // std::thread

  // function that launches an int(int) packaged_task in a new thread:
  std::future<int> launcher (std::packaged_task<int(int)>& tsk, int arg) {
      if (tsk.valid()) {
        std::future<int> ret = tsk.get_future();
      std::thread (std::move(tsk),arg).detach();
      return ret;
    }
    else return std::future<int>();
  }

  int main ()
  {
    std::packaged_task<int(int)> tsk ([](int x){return x*2;});
    std::future<int> fut = launcher (tsk,25);
    std::cout << "The double of 25 is " << fut.get() << ".\n";
    return 0;
  }
```

1. [**std::packaged_task::get_future**](http://www.cplusplus.com/reference/future/packaged_task/get_future/)
    future<Ret> get_future();
    返回与对象的共享状态相关联的future对象。
    一旦调用了它的存储任务,返回可以通过packaged_task访问共享状态下的值或异常的future对象。
    每个packaged_task共享状态只能检索一个未来对象。
    调用此函数之后，packaged_task将在某个点(通过调用它的存储任务)使其共享状态，否则，它会自动为销毁而准备就绪，包含一个future_error类型的异常(有一个破碎的承诺错误条件)。
    示例 同上。

2. **std::packaged_task::operator()**

   void operator()(Args... args);

   调用存储任务，将args作为其参数转发。

   ——如果对存储任务的调用成功完成，它返回的值(如果有的话)存储在共享状态中。

   ——如果对存储任务的调用抛出异常，则捕获异常并存储在共享状态中。

   在这两种情况下，共享状态都已准备好(这将解除当前等待它的任何线程)。

   可以通过调用get来访问共享状态，该对象将通过之前调用成员get_future返回的对象进行访问。

   存储任务通常在构造上指定。调用的效果取决于它的类型:

   ——如果存储任务是一个函数指针或函数对象，则调用它将参数转发给调用。

   ——如果存储任务是一个指向非静态成员函数的指针,它被称为使用第一个参数作为对象的成员被称为(这可以是一个对象,引用,或者一个指针),和其余参数转发为成员函数作为参数。

   ——如果它是指向非静态数据成员的指针，则应该用一个参数调用它，并且在共享状态中存储的函数将引用其参数的成员(参数可能是对象、引用或指向它的指针)。

   此成员函数在以下条件下抛出异常：

   ![img](https:////upload-images.jianshu.io/upload_images/3414858-a547a9b98d88ae82.png?imageMogr2/auto-orient/strip|imageView2/2/w/622/format/webp)

3. [**std::packaged_task::make_ready_at_thread_exit**](http://www.cplusplus.com/reference/future/packaged_task/make_ready_at_thread_exit/)
    void make_ready_at_thread_exit (args... args);
    调用存储任务并在线程退出时准备就绪
    调用存储任务，将args作为其参数转发，就像调用它的operate()成员一样，但是在线程退出时使共享状态就绪，而不是在调用完成时立即执行。
    如果与同一共享状态相关联的future对象正在等待调用future:get，它会一直阻塞，直到线程结束。一旦线程结束，对未来的调用::获取unblock和返回或抛出异常。
    注意，调用此函数已经设置了共享状态的值，并且任何尝试在此调用和线程末尾修改此值的调用，都会将future_error与promise_already_心满意足作为错误条件。
    函数异常情况 同上。
    [示例12](https://github.com/jorionwen/threadtest/blob/master/future/example12.cpp)：



```c
  #include <future>
  #include <iostream>
  #include <chrono>
  #include <thread>
  #include <functional>
  #include <utility>
  void worker(std::future<void>& output)
  {
      std::packaged_task<void(bool&)> my_task{ [](bool& done) { done=true; } };
 
      auto result = my_task.get_future();
      bool done = false;
 
      my_task.make_ready_at_thread_exit(done); // execute task right away
      std::cout << "worker: done = " << std::boolalpha << done << std::endl;
   
      auto status = result.wait_for(std::chrono::seconds(0));
      if (status == std::future_status::timeout)
          std::cout << "worker: result is not ready yet" << std::endl;
 
      output = std::move(result);
  }
 
  int main()
  {
      std::future<void> result;
      std::thread{worker, std::ref(result)}.join();
      auto status = result.wait_for(std::chrono::seconds(0));
      if (status == std::future_status::ready)
          std::cout << "main: result is ready" << std::endl;
  }
```

1. [**std::packaged_task::reset**](http://www.cplusplus.com/reference/future/packaged_task/reset/)
    在保留相同的存储任务时，重新设置具有新共享状态的对象。
    这样就可以再次调用存储的任务。
    在调用(如果有的话)之前，与对象关联的共享状态(如果有的话)(就像packaged_task被销毁一样)。
    在内部，该函数的行为就好像是对新构造的packaged_task(以及它的存储任务)进行了分配.
    [示例13](https://github.com/jorionwen/threadtest/blob/master/future/example13.cpp)：



```c
// packaged_task::get_future
#include <iostream>     // std::cout
#include <utility>      // std::move
#include <future>       // std::packaged_task, std::future
#include <thread>       // std::thread

  // a simple task:
  int triple (int x) { return x*3; }

  int main ()
  {
    std::packaged_task<int(int)> tsk (triple); // package task

    std::future<int> fut = tsk.get_future();
    tsk(33);
    std::cout << "The triple of 33 is " << fut.get() << ".\n";

    // re-use same task object:
    tsk.reset();
    fut = tsk.get_future();
    std::thread(std::move(tsk),99).detach();
    std::cout << "Thre triple of 99 is " << fut.get() << ".\n";

    return 0;
  }
```

1. [**std::packaged_task::swap**](http://www.cplusplus.com/reference/future/packaged_task/swap/)
    void swap (packaged_task& x) noexcept;
    与x交换共享状态和存储任务。

#### Non-member function overloads

1. [**std::swap (packaged_task)**](http://www.cplusplus.com/reference/future/packaged_task/swap-free/)
    template <class Ret, class... Args>
    void swap (packaged_task<Ret(Args...)>& x, packaged_task<Ret(args...)>& y) noexcept;
    交换x和y之间的共享状态和存储任务。这是一个过载的交换，就像x.swap(y)被调用。

#### Non-member class specializations

1. [**std::uses_allocator**](http://www.cplusplus.com/reference/future/packaged_task/uses_allocator/)
    template <class Ret, class Alloc>
    struct uses_allocator<packaged_task<Ret>,Alloc>;
    使用分配器packaged_task
    uses_allocator的这种特性专门化通知了packaged_task对象可以与配置器一起构建，即使类中没有任何allocator_type成员。它在< future >中定义，与相同的行为如下:
    template <class Ret, class Alloc>
    struct uses_allocator<packaged_task<Ret>,Alloc> : true_type {};
    uses_allocator 这个的说明同上面promise的std::uses_allocator<promise>里面的解释。

## 2、Futures

### 1、 [**future**](http://www.cplusplus.com/reference/future/future/)



```cpp
template <class T>  future;
template <class R&> future<R&>;     // specialization : T is a reference type (R&)
template <>         future<void>;   // specialization : T is void
```

future是一个对象，它可以从某个提供者对象或函数中检索值，如果在不同的线程中，可以正确地同步此访问。
 “有效”futures是与共享状态相关联的future对象，并通过调用下列函数之一来构建:
 1、 async
 2、 promise::get_future
 3、 packaged_task::get_future
 future对象只在有效的时候有用。默认构建的future对象是无效的(除非被分配到一个有效的未来)。
 在一个有效的future对象上调用future::get线程就会阻塞住，直到提供者使共享状态就绪(通过设置值或异常)。这样，两个线程就可以同步，等待另一个线程来设置值。
 共享状态的生命周期至少持续到最后一个对象释放它或被销毁。因此，如果关联了future，共享状态可以在第一个地方(如果有的话)中存活。
 定义中有两个特化，一个是对引用特化，一个是对void特化。
 [示例14](https://github.com/jorionwen/threadtest/blob/master/future/example14.cpp)：



```c
// future example
#include <iostream>       // std::cout
#include <future>         // std::async, std::future
#include <chrono>         // std::chrono::milliseconds

// a non-optimized way of checking for prime numbers:
bool is_prime (int x) {
  for (int i=2; i<x; ++i) if (x%i==0) return false;
  return true;
}

int main ()
{
  // call function asynchronously:
  std::future<bool> fut = std::async (is_prime,444444443); 

  // do something while waiting for function to set future:
  std::cout << "checking, please wait";
  std::chrono::milliseconds span (100);
  while (fut.wait_for(span)==std::future_status::timeout)
    std::cout << '.' << std::flush;

  bool x = fut.get();     // retrieve return value

  std::cout << "\n444444443 " << (x?"is":"is not") << " prime.\n";

  return 0;
}
```

#### **Member functions**

1. [**(constructor)构造函数**](http://www.cplusplus.com/reference/future/future/future/)
    default (1)           future() noexcept;
    copy [deleted] (2)   future (const future&) = delete;
    move (3)           future (future&& x) noexcept;
    (1)默认构造函数
    构造一个空的未来:对象没有共享状态，因此是无效的，但它可以被分配给另一个未来值。
    (2)拷贝构造函数(删除)
    未来的对象不能被复制(参见shared_future为可复制的未来类)。
    (3)构造函数
    构造的对象获得x的共享状态(如果有的话)。x就变成没有共享状态的(它不再有效)。
    具有有效共享状态的future只能由某些提供者函数(如async)最初构建，像 async, promise::get_future 或 packaged_task::get_future。
    示例 同前面。
2. [**(destructor)析构函数**](http://www.cplusplus.com/reference/future/future/~future/)
    如果对象是有效的(即它可以访问共享状态），使它与它的共享状态不再关联。如果它是与共享状态相关联的唯一对象，那么共享状态本身也会被破坏。
3. [**std::future::operator=**](http://www.cplusplus.com/reference/future/future/operator%3D/)
    move (1)           future& operator= (future&& rhs) noexcept;
    copy [deleted] (2)   future& operator= (const future&) = delete;
    获得rhs(如果有的话)的共享状态。
    如果对象是有效的(即它可以在调用之前访问共享状态），它与共享状态是分离的。如果它是与这个共享状态相关联的唯一对象，那么前共享状态本身也会被破坏。
    在移动后，rhs没有共享状态(就像默认构建的那样):它不再有效。
    future对象不能被复制(2)。
    示例 同上。
4. [**std::future::share**](http://www.cplusplus.com/reference/future/future/share/)
    shared_future<T> share();
    返回一个shared_future对象，该对象获取future对象的共享状态。future对象(* this)没有共享状态(如默认构建的)，不再有效。
    [示例15](https://github.com/jorionwen/threadtest/blob/master/future/example15.cpp)：



```c
// future::share
#include <iostream>       // std::cout
#include <future>         // std::async, std::future, std::shared_future

  int get_value() { return 10; }

  int main ()
  {
    std::future<int> fut = std::async (get_value);
    std::shared_future<int> shfut = fut.share();

    // shared futures can be accessed multiple times:
    std::cout << "value: " << shfut.get() << '\n';
    std::cout << "its double: " << shfut.get()*2 << '\n';

    return 0;
  }
```

1. [**std::future::get**](http://www.cplusplus.com/reference/future/future/get/)
    generic template (1)      T get();
    reference specialization (2) R& future<R&>::get();       // when T is a reference type (R&)
    void specialization (3)    void future<void>::get();   // when T is void
    当共享状态就绪时，返回存储在共享状态(或抛出异常)的值。
    如果共享状态还未就绪(即。提供者还没有设置其值或异常)，该函数阻塞调用线程，并等待它就绪。
    一旦共享状态就绪，函数将打开并返回共享状态(或抛出异常)。这使得以后的对象不再有效:这个成员函数将在每个future共享状态的大多数情况下被调用。
    所有可见的副作用在提供者使共享状态就绪并返回该函数之间同步。
    void专门化的成员(3)不返回任何值，但是仍然等待共享状态准备好并释放它。
    [示例16](https://github.com/jorionwen/threadtest/blob/master/future/example16.cpp)：



```c
// future::get
#include <iostream>       // std::cout, std::ios
#include <future>         // std::async, std::future
#include <exception>      // std::exception

  int get_int() {
    std::cin.exceptions (std::ios::failbit);   // throw on failbit set
    int x;
    std::cin >> x;                             // sets failbit if invalid
    return x;
  }

  int main ()
  {
    std::future<int> fut = std::async (get_int);
    std::cout << "Please, enter an integer value: ";

    try {
      int x = fut.get();
      std::cout << "You entered: " << x << '\n';
    }
    catch (std::exception&) {
      std::cout << "[exception caught]";
    }

    return 0;
  }
```

1. [**std::future::valid**](http://www.cplusplus.com/reference/future/future/valid/)
    返回当前future对象是否与共享状态相关联的。
    对于默认构造的future对象，这个函数将返回false(除非有一个有效的future)。
    只能通过某些提供者函数(如async, promise::get_future或packaged_task::get_future)初始构建与有效共享状态的期货。
    一旦共享状态的值被future::get，调用此函数返回false(除非被分配到新的有效的future)。
    [示例17](https://github.com/jorionwen/threadtest/blob/master/future/example17.cpp)：



```c
// future::valid
#include <iostream>       // std::cout
#include <future>         // std::async, std::future
#include <utility>        // std::move

  int get_value() { return 10; }

  int main ()
  {
    std::future<int> foo,bar;
    foo = std::async (get_value);
    bar = std::move(foo);

    if (foo.valid())
      std::cout << "foo's value: " << foo.get() << '\n';
    else
      std::cout << "foo is not valid\n";

    if (bar.valid())
      std::cout << "bar's value: " << bar.get() << '\n';
    else
      std::cout << "bar is not valid\n";

    return 0;
  }
```

1. [**std::future::wait**](http://www.cplusplus.com/reference/future/future/wait/)
    等待共享状态就绪。
    如果共享状态还未就绪(即。提供者还没有设置其值或异常），该函数阻塞调用线程，并等待它就绪。
    一旦共享状态就绪，函数就会打开并返回，而不读取其值，也不会抛出它的设置异常(如果有的话)。
    所有可见的副作用在提供者使共享状态就绪并返回该函数之间同步。
    [示例18](https://github.com/jorionwen/threadtest/blob/master/future/example18.cpp)：



```c
// future::wait
#include <iostream>       // std::cout
#include <future>         // std::async, std::future
#include <chrono>         // std::chrono::milliseconds

  // a non-optimized way of checking for prime numbers:
  bool is_prime (int x) {
    for (int i=2; i<x; ++i) if (x%i==0) return false;
    return true;
  }

  int main ()
  {
    // call function asynchronously:
    std::future<bool> fut = std::async (is_prime,194232491); 
  
    std::cout << "checking...\n";
    fut.wait();

    std::cout << "\n194232491 ";
    if (fut.get())      // guaranteed to be ready (and not block) after wait returns
      std::cout << "is prime.\n";
    else
      std::cout << "is not prime.\n";

    return 0;
  }
```

1. [**std::future::wait_for**](http://www.cplusplus.com/reference/future/future/wait_for/)
    template <class Rep, class Period>
    future_status wait_for (const chrono::duration<Rep,Period>& rel_time) const;
    在指定的时间rel_time内等待共享状态就绪。
    如果共享状态还未就绪(即提供者还没有设置其值或异常），该函数阻塞调用线程，并等待直到它准备就绪，或者直到rel_time已经运行，无论第一次发生什么。
    当函数返回时，因为它的共享状态已经准备好了，在共享状态上的值或异常设置为不被读取，但是，所有可见的副作用都是在提供者使共享状态就绪并返回该函数之间同步。
    如果共享状态包含一个递延函数(如async返回的future对象)，则函数不会阻塞，立即返回值为future_status::deferred。
    类型的值future_status指示什么导致函数返回：
    future_status::ready： 该共享状态是准备就绪：生产者已设定的值或异常。
    future_status::timeout： 该函数等待rel_time，而没有共享状态就绪。
    future_status::deferred：该共享状态包含延迟功能。
    [示例19](https://github.com/jorionwen/threadtest/blob/master/future/example19.cpp)：



```c
// future::wait_for
#include <iostream>       // std::cout
#include <future>         // std::async, std::future
#include <chrono>         // std::chrono::milliseconds

  // a non-optimized way of checking for prime numbers:
  bool is_prime (int x) {
    for (int i=2; i<x; ++i) if (x%i==0) return false;
    return true;
  }

  int main ()
  {
    // call function asynchronously:
    std::future<bool> fut = std::async (is_prime,700020007); 

    // do something while waiting for function to set future:
    std::cout << "checking, please wait";
    std::chrono::milliseconds span (100);
    while (fut.wait_for(span)==std::future_status::timeout)
      std::cout << '.';

    bool x = fut.get();

   std::cout << "\n700020007 " << (x?"is":"is not") << " prime.\n";

    return 0;
  }
```

1. [**std::future::wait_until**](http://www.cplusplus.com/reference/future/future/wait_until/)
    template <class Clock, class Duration>
    future_status wait_until (const chrono::time_point<Clock,Duration>& abs_time) const;
    等待共享状态就绪，最多到abs_time。
    如果共享状态还未就绪(即提供者还没有设置它的值或异常)，该函数阻塞调用线程，并等待直到它准备好，无论哪个第一次出现。
    当函数返回时，因为它的共享状态已经准备好了，在共享状态上的值或异常设置为不被读取，但是，所有可见的副作用都是在提供者使共享状态就绪并返回该函数之间同步。
    如果共享状态包含一个递延函数(如async返回的future对象)，则函数不会阻塞，立即返回值为future_status::deferred。
    类型的值future_status指示什么导致函数返回：
    future_status::ready： 该共享状态是准备就绪：生产者已设定的值或异常。
    future_status::timeout： 该函数等待rel_time，而没有共享状态就绪。
    future_status::deferred：该共享状态包含延迟功能。

### 2、[**shared_future**](http://www.cplusplus.com/reference/future/shared_future/)



```cpp
template <class T>  shared_future;
template <class R&> shared_future<R&>;   // specialization : T is a reference type (R&)
template <>         shared_future<void>; // specialization : T is void
```

shared_future对象的行为就像一个未来的对象，除非它可以被复制，而且不止一个shared_future可以共享共享状态的结束。它们还允许将共享状态中的值一次性检索多次。
 shared_future对象可以隐式地从future的对象(查看其构造函数)转换，或者通过调用future(共享)来显式地获得。在这两种情况下，它所获得的future对象将其与共享状态的关联转移到shared_future并使其本身无效。
 共享状态的生命周期至少持续到与它关联的最后一个对象被破坏。从shared_future检索值(与成员获取)不释放其对共享状态的所有权(与期货不同)。因此，如果关联到shared_future对象，共享状态可以在第一个位置(如果有的话)中存活。

#### **Member functions**

1. [**(constructor)构造函数**](http://www.cplusplus.com/reference/future/shared_future/shared_future/)
    default (1)           shared_future() noexcept;
    copy (2)             shared_future (const shared_future& x);
    move (3)             shared_future (shared_future&& x) noexcept;
    move from future (4) shared_future (future<T>&& x) noexcept;
    (1)默认构造函数
    构造一个空的shared_future:对象没有共享状态，因此它是无效的，但是它可以被赋值一个有效值。
    (2)拷贝构造函数
    构建的shared_future具有与x相同的共享状态，并共享所有权。
    (3)(4)构造函数
    构造的对象获得x的共享状态(如果有的话)。x是没有共享状态的(它不再有效)。
    通过使用move构造函数(4)或调用成员future:share，可以从未来的对象中获得具有有效共享状态的future。
2. [**(destructor)析构函数**](http://www.cplusplus.com/reference/future/shared_future/~shared_future/)
    破坏shared_future对象。
    如果对象是有效的(即可以访问共享状态），它与它分离。如果它是与共享状态相关联的唯一对象，那么共享状态本身也会被破坏。
3. [**std::shared_future::operator=**](http://www.cplusplus.com/reference/future/shared_future/operator%3D/)
    move (1) shared_future& operator= (shared_future&& rhs) noexcept;
    copy (2) shared_future& operator= (const shared_future& rhs);
    (1)move-assignment
    对象获得rhs(如果有的话)的共享状态。rhs没有共享状态(就像默认构建的那样):它不再有效。
    (2)拷贝赋值
    对象与rhs(如果有的话)相同的共享状态相关联，现在共享所有权。rhs没有被修改。
    如果在调用之前对象是有效的(即它可以访问共享状态），它与共享状态是分离的。如果它是与这个共享状态相关联的唯一对象，那么前共享状态本身也会被破坏。
4. [**std::shared_future::get**](http://www.cplusplus.com/reference/future/shared_future/get/)
    generic template (1) const T& get() const;
    reference specialization (2) R& shared_future<R&>::get() const;     // when T is a
    reference type (R&)  void specialization (3) void shared_future<void>::get() const; // when T is void
    当共享状态就绪时，返回对存储在共享状态(或抛出异常)的值的引用。
    如果共享状态还未就绪(即提供者还没有设置其值或异常），该函数阻塞调用线程，并等待它就绪。
    一旦共享状态就绪，函数就会启动并返回(或抛出)，但不会释放它的共享状态(不像future::get)，允许其他shared_future对象也访问存储的值(或同一对象的新访问)。
    所有可见的副作用在提供者使共享状态就绪并返回该函数的时候同步，但是注意，在这个事件中返回的潜在的多个函数之间不存在同步(在多个shared_future对象等待相同的共享状态的情况下)。
    void专门化的成员(3)不返回任何值，但是仍然等待共享状态在返回/抛出之前准备就绪。
5. [**std::shared_future::valid**](http://www.cplusplus.com/reference/future/shared_future/valid/)
    返回shared_future对象当前是否与共享状态相关联。
    对于默认构造的shared_future对象，这个函数将返回false(除非指定有效的shared_future)。
    共享futures从future的对象(在构建或分配它们)中获得有效的共享状态。与future对象不同，它们保留相同的共享状态关联和有效性，直到销毁(或分配)。
6. [**std::shared_future::wait**](http://www.cplusplus.com/reference/future/shared_future/wait/)
    等待共享状态就绪。
    如果共享状态还未就绪(即提供者还没有设置其值或异常），该函数阻塞调用线程，并等待它就绪。
    一旦共享状态就绪，函数就会打开并返回，而不读取其值，也不会抛出它的设置异常(如果有的话)。
    所有可见的副作用在提供者使共享状态就绪并返回该函数的时候同步，但是注意，在这个事件中返回的潜在的多个函数之间不存在同步(在多个shared_future对象等待相同的共享状态的情况下)。
7. [**std::shared_future::wait_for**](http://www.cplusplus.com/reference/future/shared_future/wait_for/)
    template <class Rep, class Period>
    future_status wait_for (const chrono::duration<Rep,Period>& rel_time) const;
    等待指定的rel_time时间共享状态就绪。
    如果共享状态还未就绪(即提供者还没有设置其值或异常），该函数阻塞调用线程，并等待直到它准备就绪，或者直到rel_time时间到，无论第一次发生什么。
    当函数返回时，因为它的共享状态已经准备好了，在共享状态上的值或异常设置为不被读取，但是，所有可见的副作用都是在提供者使共享状态就绪并返回该函数之间同步。
    如果共享状态包含一个递延函数，则函数不会阻塞，立即返回值为future_status::deferred。
    同 future::wait_for的异常情况
8. [**std::shared_future::wait_until**](http://www.cplusplus.com/reference/future/shared_future/wait_until/)
    template <class Clock, class Duration>
    future_status wait_until (const chrono::time_point<Clock,Duration>& abs_time) const;
    等待共享状态就绪，最多只能等待abs_time。
    如果共享状态还未就绪(即提供者还没有设置它的值或异常)，该函数阻塞调用线程，并等待直到它准备好，无论哪个第一次出现。
    当函数返回时，因为它的共享状态已经准备好了，在共享状态上的值或异常设置为不被读取，但是，所有可见的副作用都是在提供者使共享状态就绪并返回该函数之间同步。
    如果共享状态包含一个递延函数，则函数不会阻塞，立即返回值为future_status::deferred。
    同 future::wait_for的异常情况。

## 3、 Other types

1. **std::future_error**

   class future_error : public logic_error;

   ![img](https:////upload-images.jianshu.io/upload_images/3414858-7b915b99ef7b681b.png?imageMogr2/auto-orient/strip|imageView2/2/w/393/format/webp)

   这个类定义抛出的对象的类型，以便对将来可能访问未来共享状态的库中的其他对象或其他元素报告无效的操作。

   这个类继承了logic_error，它添加了一个error_code作为成员代码(并定义了一个专门的成员)。

   1、 std::future_error::future_error   构造一个future_error异常对象以ec作为错误代码

   future_error (error_code ec);   //定义

   2、 std::future_error::code   返回与异常关联的error_code对象。

   const error_code& code() const noexcept;

   3、 std::future_error::what  返回描述异常的消息。包括由code(). message()返回的字符串。

   const char* what() const noexcept;

   示例20

   ：



```c
// future_error::what example:
#include <iostream>     // std::cout
#include <future>       // std::promise, std::future_error

  int main ()
  {
    std::promise<int> prom;

    try {
      prom.get_future();
      prom.get_future();   // throws std::future_error
    }
    catch (std::future_error& e) {
      std::cout << "future_error caught: " << e.what() << '\n';
      //std::cout << "future_error caught: " << e.code().message() << '\n';
    }

    return 0;
  }
```

1. [**std::future_errc**](http://www.cplusplus.com/reference/future/future_errc/)
    enum class future_errc;   //定义
    这个enum类定义了未来类别的错误条件。枚举类型future_errc的值可用于创建error_condition对象，以与future_error的代码成员返回的值进行比较。
    broken_promise （值: 0）：  该 promise 对象用哪个future分享其共享状态在设定值或异常之前被销毁。
    future_already_retrieved（值: 1）： 一个 future对象已从此提供程序检索。
    promise_already_satisfied（值: 2）： 该 promise 对象已经设置了一个值或异常
    no_state（值: 3）： 操作尝试访问共享状态，没有一个对象的共享状态。
    [示例21](https://github.com/jorionwen/threadtest/blob/master/future/example21.cpp)：



```c
// std::future_errc example:
#include <iostream>     // std::cerr
#include <future>       // std::promise, std::future_error, std::future_errc

  int main ()
  {
    std::promise<int> prom;

    try {
      prom.get_future();
      prom.get_future();   // throws std::future_error with future_already_retrieved
    }
    catch (std::future_error& e) {
      if (e.code() == std::make_error_condition(std::future_errc::future_already_retrieved))
      std::cerr << "[future already retrieved]\n";
      else std::cerr << "[unknown exception]\n";
    }

    return 0;
  }
```

1. [**std::future_status**](http://www.cplusplus.com/reference/future/future_status/)
    enum class future_status;
    这个enum类定义了furure和shared_future的成员函数wait_for和wait_until的可能返回值。
    future_status::ready : 由于共享状态已准备就绪，因此返回该函数。
    future_status::timeout : 该函数返回，因为指定的时间已用尽。
    future_status::deferred :  该函数返回，因为共享状态包含延迟函数（参见的std::sync）
2. [**std::launch**](http://www.cplusplus.com/reference/future/launch/)
    enum class launch;
    这个enum类类型是一个位掩模类型，它定义了对异步的调用的启动策略。
    指定的等价int值应该表示单个位，允许多个标签组合在一个位掩码中。当将此类型的多个值组合在一起时，该函数将自动选择一个(根据它们特定的库实现)。
    库实现可以为不同的启动策略定义这种类型的附加标签，这些策略可以通过不同的调用描述对任务交互的限制。
    它可以是下列值的任意组合:
    launch::async :异步:该函数被一个新线程异步调用，并与共享状态的访问点同步它的返回。
    launch::deferred :递延:函数在访问共享状态时调用。
    [示例22](https://github.com/jorionwen/threadtest/blob/master/future/example22.cpp)：



```c
  // launch::async vs launch::deferred
  #include <iostream>     // std::cout
  #include <future>       // std::async, std::future, std::launch
  #include <chrono>       // std::chrono::milliseconds
  #include <thread>       // std::this_thread::sleep_for

  void print_ten (char c, int ms) {
    for (int i=0; i<10; ++i) {
      std::this_thread::sleep_for (std::chrono::milliseconds(ms));
      std::cout << c;
    }
  }

  int main ()
  {
    std::cout << "with launch::async:\n";
    std::future<void> foo = std::async (std::launch::async,print_ten,'*',100);
    std::future<void> bar = std::async (std::launch::async,print_ten,'@',200);
    // async "get" (wait for foo and bar to be ready):
    foo.get();
    bar.get();
    std::cout << "\n\n";

    std::cout << "with launch::deferred:\n";
    foo = std::async (std::launch::deferred,print_ten,'*',100);
    bar = std::async (std::launch::deferred,print_ten,'@',200);
    // deferred "get" (perform the actual calls):
    foo.get();
    bar.get();
    std::cout << '\n';

    return 0;
  }
```

# 三、 <future>头文件Functions

## 1、 Providers

### 1、 [**std::async**](http://www.cplusplus.com/reference/future/async/)



```ruby
unspecified policy (1)  template <class Fn, class... Args>
                                      future<typename result_of<Fn(Args...)>::type>
                                         async (Fn&& fn, Args&&... args);
specific policy (2)    template <class Fn, class... Args>
                                      future<typename result_of<Fn(Args...)>::type>
                                         async (launch policy, Fn&& fn, Args&&... args);
```

异步调用函数
 在某个点调用fn(以args为参数)，无需等待fn的执行完成。
 fn返回的值可以通过future对象访问(通过调用它的成员future:get)。
 第二个版本(2)让调用者选择一个特定的启动策略，而第一个版本(1)使用自动选择，就像调用(2)launch::async|launch::deferred 作为策略。
 函数在共享状态中临时存储，线程处理程序使用或复制fn和args(作为递延函数)，而不准备就绪。一旦fn的执行完成，共享状态包含fn返回的值，并准备就绪。
 参数 policy：
 launch::async：启动一个新的线程来调用fn(好像一个线程对象是用fn和args作为参数构建的，并访问返回的future共享状态连接它
 launch::deferred：对fn的调用被推迟，直到被访问的将来的共享状态被访问(等待或得到)。此时，调用fn并不再考虑函数的递延。当这个调用返回时，返回的future共享状态已经准备好了。
 launch::async|launch::deferred：函数自动地选择策略(在某个点)。这取决于系统和库的实现，这通常会优化系统当前的并发可用性。
 参数 fn：指向函数的指针，指向成员的指针，或任何形式的可构造函数对象(例如。一个类定义操作符()的对象，包括闭包和函数对象）。fn的返回值存储为异步返回的future对象检索的共享状态。如果fn抛出，则在共享状态中设置异常，以被future对象检索。
 参数 args：参数传递给fn(如果有的话)。它们的类型应该是可构造的。如果fn是一个成员指针，则第一个参数应该是定义该成员的对象(或引用，或指向它的指针)。
 [示例23](https://github.com/jorionwen/threadtest/blob/master/future/example23.cpp)：



```c
// async example
#include <iostream>       // std::cout
#include <future>         // std::async, std::future

  // a non-optimized way of checking for prime numbers:
  bool is_prime (int x) {
    std::cout << "Calculating. Please, wait...\n";
    for (int i=2; i<x; ++i) if (x%i==0) return false;
    return true;
  }

  int main ()
  {
    // call is_prime(313222313) asynchronously:
    std::future<bool> fut = std::async (is_prime,313222313);

    std::cout << "Checking whether 313222313 is prime.\n";
    // ...

    bool ret = fut.get();      // waits for is_prime to return

    if (ret) std::cout << "It is prime!\n";
    else std::cout << "It is not prime.\n";

    return 0;
  }
```

## 2、 Other functions

## 1、 [**std::future_category**](http://www.cplusplus.com/reference/future/future_category/)



```cpp
 const error_category& future_category() noexcept;
```

返回对具有以下特征的error_category类型的静态对象的引用:
 1、它的名称成员函数返回一个指向字符序列“future”的指针
 2、default_error_condition成员函数按照基本error_category类指定。
 描述与enum类型future_errc对应的错误的error_condition对象与此类别相关联。这些对应关系中的一个取决于操作系统和特定的库实现。
 [示例24](https://github.com/jorionwen/threadtest/blob/master/future/example24.cpp)：



```c
// std::future_category example:
#include <iostream>     // std::cerr
#include <future>       // std::promise, std::future_error, std::future_category

  int main ()
  {
    std::promise<int> prom;

    try {
      prom.get_future();
      prom.get_future();   // throws a std::future_error of the future category
    }
    catch (std::future_error& e) {
      if (e.code().category() == std::future_category())
      std::cerr << "future_error of the future category thrown\n";
    }

    return 0;
  }
```

# 完结



