### forward_list

既然 list 是双向链表，那么 C++ 里有没有单向链表呢？答案是肯定的。从 **C++11** 开始，前向列表 forward_list 成了标准的一部分。

我们先看一下它的内存布局：

![img](https://static001.geekbang.org/resource/image/ef/4e/ef23c4d60940c170629cf65771df084e.png)

大部分 C++ 容器都支持 insert 成员函数，语义是从指定的位置之前插入一个元素。对于 forward_list，这不是一件容易做到的事情。标准库提供了一个 insert_after 作为替代。此外，它跟 list 相比还缺了下面这些成员函数：

* back
* size
* push_back
* emplace_back
* pop_back

为什么会需要这么一个阉割版的 list 呢？原因是，在元素大小较小的情况下，forward_list 能节约的内存是非常可观的；在列表不长的情况下，不能反向查找也不是个大问题。提高内存利用率，往往就能提高程序性能，更不用说在内存可能不足时的情况了。

目前你只需要知道这个东西的存在就可以了。如果你觉得不需要用到它的话，也许你真的不需要它。