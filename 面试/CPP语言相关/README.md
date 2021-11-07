## 语言特性

#### 关键字

* static 
  * [static全局变量和普通全局变量](语言特性/static/static全局变量和普通全局变量.md)
  * [static修饰函数](语言特性/static/static修饰函数.md)
  * ?[static什么时候初始化](语言特性/static/static什么时候初始化.md)
  * [static在类中使用](语言特性/static/static在类中使用.md)
* volatile
  * [volatile作用](语言特性/volatile/volatile作用.md)
  * [const和volatile](语言特性/volatile/const和volatile.md)
* extern
  * [extern](语言特性/extern.md)
* const
  * [const作用](语言特性/const/const作用.md)
  * [const与define](语言特性/const/const与define.md)
* [mutable](语言特性/mutable.md)
* inline
  * [inline](语言特性/inline/inline.md)
  * [inline优缺点](语言特性/inline/inline优缺点.md)
  * [虚函数可以内联吗](语言特性/inline/虚函数可以内联吗.md)
  * [inline与typedef与define](语言特性/inline/inline与typedef与define.md)



#### 宏

* [宏的应用](语言特性/宏/宏的应用.md)



* [i++与++i](语言特性/i++与++i.md)
* [范围解析运算符](语言特性/范围解析运算符.md)
* [性能瓶颈](语言特性/性能瓶颈.md)



#### 模板

[模板函数和模板特化](语言特性/模板/模板函数和模板特化.md)

## STL

[traits](STL/traits.md)

[两级空间配置器](STL/两级空间配置器.md)

### 容器（containers）

#### **序列容器：**

* [string](STL/容器containers/string/string.md)

* [vector](STL/容器containers/vector/vector.md)

* [deque](STL/容器containers/deque/deque.md)

* [list](STL/容器containers/list/list.md)

* [forward_list](STL/容器containers/forward_list/forward_list.md)



**函数对象**：[less & hash](STL/容器containers/less & hash.md)



* [priority_queue](STL/容器containers/priority_queue/priority_queue.md)

#### [关联容器](STL/容器containers/关联式容器/README.md)

[hashtable的实现](STL/容器containers/关联式容器/hashtable的实现.md)

**C 数组的替代品**:[array](STL/容器containers/array/array.md)

### 容器适配器（adapter）

* [queue](STL/配接器adapters/queue/queue.md)

* [stack](STL/配接器adapters/stack/stack.md)



##### 为什么大部分容器都提供了 begin、end 等方法？

答：容器提供了 begin 和 end 方法，就意味着是可以迭代（遍历）的。大部分容器都可以从头到尾遍历，因而也就需要提供这两个方法。

##### 为什么容器没有继承一个公用的基类？

答：C++ 不是面向对象的语言，尤其在标准容器的设计上主要使用值语义，使用公共基类完全没有用处。



* [pair]()

* [tuple]()



#### STL源码剖析

[vector实现](STL/STL源码剖析/vector实现.md)

[slist的实现](STL/STL源码剖析/slist的实现.md)

[list的实现](STL/STL源码剖析/list的实现.md)

[deque的实现](STL/STL源码剖析/deque的实现.md)

[stack和queue实现](STL/STL源码剖析/stack和queue实现.md)

[heap实现](STL/STL源码剖析/heap实现.md)

[priority_queue的实现](STL/STL源码剖析/priority_queue的实现.md)

[set的实现](STL/STL源码剖析/set的实现.md)

[map的实现](STL/STL源码剖析/map的实现.md)





### 迭代器

[iterator](STL/迭代器iterator/iterator.md)



## C++11、14、17（某个点可能串起来）

#### 关键字：

1. [auto](C++11/关键字/auto.md)
2. [decltype](C++11/关键字/decltype.md)
3. [auto和decltype对比](C++11/关键字/auto和decltype对比.md)
4. [override & final](C++11/关键字/final.md):都**不是**关键字，放这里仅仅是方便
5. [default & delete](C++11/关键字/default & delete.md)
6. [explicit](C++11/关键字/explicit.md)
7. [using](C++11/关键字/using.md)



#### 常用技巧：

1. [委托构造](C++11/常用技巧/委托构造.md)
2. [成员初始化列表](C++11/常用技巧/成员初始化列表.md)
3. [类型别名](C++11/常用技巧/类型别名.md)
4. [for each循环](C++11/常用技巧/for each循环.md)
5. [可调用对象](C++11/常用技巧/可调用对象.md)
6. [lambda](C++11/常用技巧/lambda.md)
7. [类模板的模板参数推导](C++11/常用技巧/类模板的模板参数推导.md)
8. [结构化绑定](C++11/常用技巧/结构化绑定.md)
9. [列表初始化](C++11/常用技巧/列表初始化.md)
10. [统一初始化](C++11/常用技巧/统一初始化.md)
11. [类数据成员的默认初始化](C++11/常用技巧/类数据成员的默认初始化.md)
12. [静态断言](C++11/常用技巧/静态断言.md)
13. [类型转换](C++11/常用技巧/类型转换.md)
14. [可变参数模板](C++11/常用技巧/可变参数模板.md)
15. [智能指针](C++11/常用技巧/智能指针.md)





## 语言对比

#### [C与C++](语言对比/C与C++/C和C++.md)：

1. [结构体、联合体(以及匿名)](语言对比/C与C++/结构体、联合体(以及匿名).md)
2. [bool类型](语言对比/C与C++/bool类型.md)
3. [memcpy](语言对比/C与C++/库函数/memcpy.md)
4. [strcpy](语言对比/C与C++/库函数/strcpy.md)
5. [strlen](语言对比/C与C++/库函数/strlen.md)
6. [类型安全](语言对比/C与C++/类型安全.md)

#### Java和C++

1. [Java和C++](语言对比/Java和C++.md)

#### Python和C++

1. [Python和C++](语言对比/Python和C++.md)



## 指针和引用

#### 指针

1. [指针和数组](指针和引用/指针和数组.md)
   * [数组名当做指针用](指针和引用/数组名当做指针用.md)
2. [野指针和悬空指针](指针和引用/野指针和悬空指针.md)
3. [函数指针](指针和引用/函数指针.md)
4. [nullptr](指针和引用/nullptr.md)
5. [对象指针](指针和引用/对象指针.md)
6. [this指针](指针和引用/this指针.md)
7. [指针和引用](指针和引用/指针和引用.md)



## 右值

0. [右值和移动究竟解决了什么问题](右值/右值和移动究竟解决了什么问题.md)
1. [右值和左值](右值/右值和左值.md)
2. [右值引用](右值/右值引用.md)
3. [转移语意](右值/转移语意.md)
4. [完美转发](右值/完美转发.md)
5. [move实现原理](右值/move实现原理.md)

## [异常](异常/异常.md)

1. [异常：用还是不用，这是个问题](异常/异常：用还是不用，这是个问题.md)



## 内存管理

#### 编译链接

0. [程序编译过程](内存管理/编译链接/程序编译过程.md)
1. [#include](内存管理/编译链接/#include.md)
2. [动态链接与静态链接](内存管理/编译链接/动态链接与静态链接.md)
3. [main前](内存管理/编译链接/main前.md)
4. [模板的编译与链接](内存管理/编译链接/模板的编译与链接.md)
5. [各个平台相关](内存管理/编译链接/各个平台相关.md)

#### 内存对齐

1. [结构体大小](内存管理/类和结构体/结构体大小.md)
2. [类的大小](内存管理/类和结构体/类的大小.md)
3. [内存对齐原因](内存管理/类和结构体/内存对齐.md)

#### 运行时内存

1. [内存泄露](内存管理/运行时内存/内存泄露.md)
2. [内存溢出OOM](内存管理/运行时内存/内存溢出.md)
3. [防止内存泄露](内存管理/运行时内存/防止内存泄漏.md)
4. [检测内存泄漏](内存管理/运行时内存/检测内存泄漏.md)
5. new和delete和malloc和free
   * [malloc](内存管理/new和delete和malloc和free/malloc.md)
   * [new和operator new()](内存管理/new和delete和malloc和free/new和operator new().md)
   * [new和malloc](内存管理/new和delete和malloc和free/new和malloc.md)
   * [三种new](内存管理/new和delete和malloc和free/三种new.md)
   * [delete](内存管理/new和delete和malloc和free/delete.md)
   
6. 虚拟地址空间
   * [虚拟地址空间](内存管理/虚拟地址空间/虚拟地址空间.md)
7. [变量区别](内存管理/变量.md)



## 类与对象

#### 面向对象

1. [什么是面向对象编程](类与对象/面向对象/什么是面向对象编程.md)
   * [public和protected和private](类与对象/面向对象/public和protected和private.md)
2. [重载、重写、隐藏](类与对象/面向对象/重载、重写、隐藏的区别.md)
3. [多态](类与对象/面向对象/多态.md)
   * [RTTI](类与对象/面向对象/RTTI.md)

   * 虚函数
     * [虚函数机制](类与对象/虚机制/虚函数机制.md)
     * [多继承虚函数机制](类与对象/虚机制/多继承虚函数机制.md)
     * [纯虚函数](类与对象/虚机制/纯虚函数.md)
     * [构造、析构要虚不](类与对象/虚机制/构造析构和虚.md)
     * [虚函数效率分析](类与对象/虚机制/虚函数效率分析.md)
     * [虚函数与内联](类与对象/虚机制/虚函数与内联.md)
     * [哪些函数不能是虚函数](类与对象/虚机制/哪些函数不能是虚函数.md)
4. [多重继承](类与对象/面向对象/多重继承.md)

   * [多继承](类与对象/面向对象/多继承.md)
   * [虚基类](类与对象/虚机制/虚基类.md)
   * [自适应偏移](类与对象/面向对象/自适应偏移.md)
5. [如何让类不能被继承](类与对象/面向对象/如何让类不能被继承.md)
6. [友元](类与对象/面向对象/友元.md)
7. [返回类型协变](类与对象/面向对象/返回类型协变.md)

#### 构造

1. [对象构造过程](类与对象/构造/对象构造过程.md)
2. [生成默认构造函数](类与对象/构造/生成默认构造函数.md)
3. [拷贝构造函数](类与对象/构造/拷贝构造函数.md)
4. [禁止类被实例化](类与对象/构造/禁止类被实例化.md)
5. [禁止拷贝](类与对象/构造/禁止拷贝.md)

#### 析构

1. [对象析构过程](类与对象/析构/对象析构过程.md)

#### 内存管理

1. [深拷贝和浅拷贝](类与对象/内存管理/深拷贝和浅拷贝.md)
2. [只能在栈上或堆上生成对象](类与对象/内存管理/只能在栈上或堆上生成对象.md)
3. [空类](类与对象/内存管理/空类.md)
4. [简单对象](类与对象/内存管理/简单对象.md)

