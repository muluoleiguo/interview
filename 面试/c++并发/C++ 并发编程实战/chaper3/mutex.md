## 3.2.1 C++中使用互斥量

C++中通过实例化`std::mutex`创建互斥量实例，通过成员函数lock()对互斥量上锁，unlock()进行解锁。

不过，实践中不推荐直接去调用成员函数，调用成员函数就意味着，必须在每个函数出口都要去调用unlock()，也包括异常的情况。C++标准库为互斥量提供了一个RAII语法的模板类`std::lock_guard`，在构造时就能提供已锁的互斥量，并在析构的时候进行解锁，从而保证了一个已锁互斥量能被正确解锁。



```c++
#include<list>
#include<mutex>
#include<algorithm>
std::list<int> some_list;// 1
std::mutex some_mutex;// 2
void add_to_list(int new_value)
{
  std::lock_guard<std::mutex> guard(some_mutex);// 3
  some_list.push_back(new_value);
}
bool list_contains(int value_to_find)
{
  std::lock_guard<std::mutex> guard(some_mutex);// 4
return std::find(some_list.begin(),some_list.end(),value_to_find)!= some_list.end();
}
```

清单3.1中有一个全局变量①，这个全局变量被一个全局的互斥量保护②。add_to_list()③和list_contains()④函数中使用`std::lock_guard<std::mutex>`，使得这两个函数中对数据的访问是互斥的：list_contains()不可能看到正在被add_to_list()修改的列表。



C++17中添加了一个新特性，称为模板类参数推导，这样类似`std::lock_guard`这样简单的模板类型的模板参数列表可以省略。③和④的代码可以简化成：

```c++
std::lock_guard guard(some_mutex);
```



但在大多数情况下，**互斥量通常会与需要保护的数据放在同一类中**，而不是定义成全局变量。这是面向对象设计的准则.



## 3.2.2 用代码来保护共享数据

使用互斥量来保护数据，并不是仅仅在每一个成员函数中都加入一个`std::lock_guard`对象那么简单；**一个指针或引用，也会让这种保护形同虚设。**

清单3.2 无意中传递了保护数据的引用

```c++
class some_data
{
int a;
  std::string b;
public:
void do_something();
};
class data_wrapper
{
private:
  some_data data;
  std::mutex m;
public:
template<typenameFunction>
void process_data(Function func)
{
    std::lock_guard<std::mutex> l(m);
    func(data);// 1 传递“保护”数据给用户函数
}
};
some_data* unprotected;
void malicious_function(some_data& protected_data)
{
  unprotected=&protected_data;
}
data_wrapper x;
void foo()
{
  x.process_data(malicious_function);// 2 传递一个恶意函数
  unprotected->do_something();// 3 在无保护的情况下访问保护数据
}
```

例子中process_data看起来没有任何问题，`std::lock_guard`对数据做了很好的保护，但调用用户提供的函数func①，就意味着foo能够绕过保护机制将函数`malicious_function`传递进去②，在没有锁定互斥量的情况下调用`do_something()`。

这段代码的问题在于根本没有保护，只是将所有可访问的数据结构代码标记为互斥。函数`foo()`中调用`unprotected->do_something()`的代码未能被标记为互斥。这种情况下，C++线程库无法提供任何帮助，只能由开发者使用正确的互斥锁来保护数据。

从乐观的角度上看，还是有方法可循的：切勿将受保护数据的指针或引用传递到互斥锁作用域之外，无论是函数返回值，还是存储在外部可见内存，亦或是以参数的形式传递到用户提供的函数中去。

虽然这是在使用互斥量保护共享数据时常犯的错误，但绝不仅仅是一个潜在的陷阱而已。下一节中，你将会看到，即便是使用了互斥量对数据进行了保护，条件竞争依旧可能存在。



## 3.2.4 死锁：问题描述及解决方案

C++标准库有办法解决这个问题，`std::lock`——可以一次性锁住多个(两个以上)的互斥量，并且没有副作用(死锁风险)。

一个互斥量可以在同一线程上多次上锁，标准库中`std::recursive_mutex`提供这样的功能

```c++
// 这里的std::lock()需要包含<mutex>头文件
class some_big_object;
void swap(some_big_object& lhs,some_big_object& rhs);
class X
{
private:
  some_big_object some_detail;
  std::mutex m;
public:
  X(some_big_object const& sd):some_detail(sd){}
friendvoid swap(X& lhs, X& rhs)
{
if(&lhs==&rhs)  //首先，检查参数是否是不同的实例
return;
    std::lock(lhs.m,rhs.m);// 1 std::lock要么将两个锁都锁住，要不一个都不锁。
    std::lock_guard<std::mutex> lock_a(lhs.m,std::adopt_lock);// 2
    std::lock_guard<std::mutex> lock_b(rhs.m,std::adopt_lock);// 3
    swap(lhs.some_detail,rhs.some_detail);
}
};
```

`std::adopt_lock `收养锁





C++17提供了`std::scoped_lock<>`一种新的RAII类型模板类型，与`std::lock_guard<>`的功能等价，这个新类型能接受不定数量的互斥量类型作为模板参数，以及相应的互斥量(数量和类型)作为构造参数。互斥量支持构造即上锁，与`std::lock`的用法相同，其解锁阶段是在析构中进行。swap()操作可以重写如下：

```c++
void swap(X& lhs, X& rhs)
{
if(&lhs==&rhs)
return;
  std::scoped_lock guard(lhs.m,rhs.m);// 1 C++17的另一个特性：自动推导模板参数。
  //等价于std::scoped_lock<std::mutex,std::mutex> guard(lhs.m,rhs.m);
  swap(lhs.some_detail,rhs.some_detail);
}
```





## 3.2.5 避免死锁的进阶指导

死锁通常是由于对锁的不当操作造成，但也不排除死锁出现在其他地方。无锁的情况下，仅需要每个`std::thread`对象调用join()，两个线程就能产生死锁。这种情况下，没有线程可以继续运行，因为他们正在互相等待。这种情况很常见，一个线程会等待另一个线程，其他线程同时也会等待第一个线程结束，所以三个或更多线程的互相等待也会发生死锁。

为了避免死锁，这里的指导意见为：**当机会来临时，不要拱手让人**。

**避免嵌套锁**

第一个建议往往是最简单的：一个线程已获得一个锁时，再别去获取第二个。因为每个线程只持有一个锁，锁上就不会产生死锁。即使互斥锁造成死锁的最常见原因，也可能会在其他方面受到死锁的困扰(比如：线程间的互相等待)。当你需要获取多个锁，使用一个`std::lock`来做这件事(对获取锁的操作上锁)，避免产生死锁。

**避免在持有锁时调用用户提供的代码**

第二个建议是次简单的：因为代码是用户提供的，你没有办法确定用户要做什么；用户程序可能做任何事情，包括获取锁。你在持有锁的情况下，调用用户提供的代码；如果用户代码要获取一个锁，就会违反第一个指导意见，并造成死锁(有时，这是无法避免的)。

**使用固定顺序获取锁**

当硬性条件要求你获取两个或两个以上的锁，并且不能使用`std::lock`单独操作来获取它们；那么最好在每个线程上，用固定的顺序获取它们(锁)

**使用锁的层次结构**

当代码试图对一个互斥量上锁，在该层锁已被低层持有时，上锁是不允许的。你可以在运行时对其进行检查，通过分配层数到每个互斥量上，以及记录被每个线程上锁的互斥量。下面的代码列表中将展示两个线程如何使用分层互斥。

**超越锁的延伸扩展**



## 3.2.6 std::unique_lock——灵活的锁

```c++
class some_big_object;
void swap(some_big_object& lhs,some_big_object& rhs);
class X
{
private:
  some_big_object some_detail;
  std::mutex m;
public:
  X(some_big_object const& sd):some_detail(sd){}
friendvoid swap(X& lhs, X& rhs)
{
if(&lhs==&rhs)
return;
    std::unique_lock<std::mutex> lock_a(lhs.m,std::defer_lock);// 1 表明互斥量应保持解锁状态
    std::unique_lock<std::mutex> lock_b(rhs.m,std::defer_lock);// 1 std::defer_lock 留下未上锁的互斥量
    std::lock(lock_a,lock_b);// 2 互斥量在这里上锁
    swap(lhs.some_detail,rhs.some_detail);
}
};
```

可以将`std::defer_lock`作为第二个参数传递进去unique_lock，表明互斥量应保持解锁状态。这样，就可以被`std::unique_lock`对象(不是互斥量)的lock()函数所获取，或传递`std::unique_lock`对象到`std::lock()`



## 3.2.7 不同域中互斥量所有权的传递

`std::unique_lock`是可移动，但不可赋值的类型

函数get_lock()锁住了互斥量，然后准备数据，返回锁的调用函数。

```c++
std::unique_lock<std::mutex> get_lock()
{
extern std::mutex some_mutex;
  std::unique_lock<std::mutex> lk(some_mutex);
  prepare_data();
return lk;// 1 
}
void process_data()
{
  std::unique_lock<std::mutex> lk(get_lock());// 2
  do_something();
}
```

lk在函数中被声明为自动变量，它不需要调用`std::move()`，可以直接返回①(编译器负责调用移动构造函数)。



process_data()函数直接转移`std::unique_lock`实例的所有权②



## 3.2.8 锁的粒度

3.2.3节中，已经对锁的粒度有所了解：锁的粒度是一个*摆手术语*(hand-waving term)，用来描述通过一个锁保护着的数据量大小。*一个细粒度锁*(a fine-grained lock)能够保护较小的数据量，*一个粗粒度锁*(a coarse-grained lock)能够保护较多的数据量。