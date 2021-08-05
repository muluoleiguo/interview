一个为C++类和库共有的设计，同时也是为程序员提供很大便利的设计，也就是**获取资源即初始化**(RAII, Resource Acquisition Is Initialization)，



虽然C++线程库为多线程和并发处理提供了较全面的工具，但在某些平台上提供额外的工具。为了方便地访问那些工具的同时，又使用标准C++线程库，在C++线程库中提供一个`native_handle()`成员函数，允许通过使用平台相关API直接操作底层实现。（超标了了解就是）

一个简单的Hello, Concurrent World程序：

```c++
#include <iostream>
#include <thread>  // 1
void hello()  // 2
{
  std::cout << "Hello Concurrent World\n";
}
int main()
{
  std::thread t(hello);  // 3
  t.join();  // 4
}
```

①标准C++库中对多线程支持的声明在新的头文件中：管理线程的函数和类在`<thread>`中声明，而保护共享数据的函数和类在其他头文件中声明。

新的线程启动之后③，初始线程继续执行。如果它不等待新线程结束，它就将自顾自地继续运行到main()的结束，从而结束程序——有可能发生在新线程运行之前。这就是为什么在④这里调用`join()`的原因