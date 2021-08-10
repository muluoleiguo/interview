## STL

### 容器（containers）

#### **序列容器：**

[string](STL/容器containers/string/string.md)

[vector](STL/容器containers/vector/vector.md)

[deque](STL/容器containers/deque/deque.md)

[list](STL/容器containers/list/list.md)

[forward_list](STL/容器containers/forward_list/forward_list.md)



**函数对象**：[less & hash](STL/容器containers/less & hash.md)



[priority_queue](STL/容器containers/priority_queue/priority_queue.md)

#### [关联容器](STL/容器containers/关联式容器/README.md)



**C 数组的替代品**:[array](STL/容器containers/array/array.md)

### 容器适配器（adapter）

[queue](STL/配接器adapters/queue/queue.md)

[stack](STL/配接器adapters/stack/stack.md)



##### 为什么大部分容器都提供了 begin、end 等方法？

答：容器提供了 begin 和 end 方法，就意味着是可以迭代（遍历）的。大部分容器都可以从头到尾遍历，因而也就需要提供这两个方法。

##### 为什么容器没有继承一个公用的基类？

答：C++ 不是面向对象的语言，尤其在标准容器的设计上主要使用值语义，使用公共基类完全没有用处。



[pair]()

[tuple]()





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



#### 常用技巧：

1. [委托构造](C++11/常用技巧/委托构造.md)
2. [成员变量初始化](C++11/常用技巧/成员变量初始化.md)
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





## 语言对比

#### C与C++：

1. [结构体、联合体(以及匿名)](语言对比/C与C++/结构体、联合体(以及匿名).md)
2. 



## 右值

[右值和移动究竟解决了什么问题](右值/右值和移动究竟解决了什么问题.md)

## 异常

1. [异常：用还是不用，这是个问题](异常/异常：用还是不用，这是个问题.md)



