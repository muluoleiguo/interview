### queue

它的特别点在于不是完整的实现，而是依赖于某个现有的容器，因而被称为容器适配器（container adaptor）。

我们先看一下队列 queue，先进先出（FIFO）的数据结构。

queue 缺省用 deque 来实现。它的接口跟 deque 比，有如下改变：

* 不能按下标访问元素
* 没有 begin、end 成员函数
* 用 emplace 替代了 emplace_back，用 push 替代了 push_back，用 pop 替代了 pop_front；没有其他的 push_…、pop\_…、emplace…、insert、erase 函数

它的实际内存布局当然是随底层的容器而定的。从概念上讲，它的结构可如下所示：

![img](https://static001.geekbang.org/resource/image/09/ff/090f23e3b4cdd8d297e4b970cbbf6cff.png)

鉴于 queue 不提供 begin 和 end 方法，无法无损遍历，我们只能用下面的代码约略展示一下其接口：

```c++

#include <iostream>
#include <queue>

int main()
{
  std::queue<int> q;
  q.push(1);
  q.push(2);
  q.push(3);
  while (!q.empty()) {
    std::cout << q.front()
              << std::endl;
    q.pop();
  }
}
```

