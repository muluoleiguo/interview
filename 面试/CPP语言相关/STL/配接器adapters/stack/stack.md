### stack

类似地，栈 stack 是后进先出（LIFO）的数据结构。

stack 缺省也是用 deque 来实现，但它的概念和 vector 更相似。它的接口跟 vector 比，有如下改变：

* 不能按下标访问元素
* 没有 begin、end 成员函数
* back 成了 top，没有 front
* 用 emplace 替代了 emplace_back，用 push 替代了 push_back，用 pop 替代了 pop_back；没有其他的 push_…、pop\_…、emplace…、insert、erase 函数

一般图形表示法会把 stack 表示成一个竖起的 vector：

![img](https://static001.geekbang.org/resource/image/68/47/68026091ff5eacac00c49d75151fff47.png)

这里有一个小细节需要注意。stack 跟我们前面讨论内存管理时的栈有一个区别：在这里下面是低地址，向上则地址增大；而我们讨论内存管理时，高地址在下面，向上则地址减小，方向正好相反。提这一点，是希望你在有需要检查栈结构时不会因此而发生混淆；在使用 stack 时，这个区别通常无关紧要。

示例代码：

```c++

#include <iostream>
#include <stack>

int main()
{
  std::stack<int> s;
  s.push(1);
  s.push(2);
  s.push(3);
  while (!s.empty()) {
    std::cout << s.top()
              << std::endl;
    s.pop();
  }
}
```



##### 为什么 stack（或 queue）的 pop 函数返回类型为 void，而不是直接返回容器的 top（或 front）成员？

这是 C++98 里、还没有移动语义时的设计。如果 pop 返回元素，而元素拷贝时发生异常的话，那这个元素就丢失了。因而容器设计成有分离的 top（或 front）和 pop 成员函数，分别执行访问和弹出的操作。

有一种可能的设计是把接口改成 void pop(T&)，这增加了 T 必须支持默认构造和赋值的要求，在单线程为主的年代没有明显的好处，反而带来了对 T 的额外要求。