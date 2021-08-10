### deque

deque 的意思是 double-ended queue，双端队列。它主要是用来满足下面这个需求：

容器不仅可以从尾部自由地添加和删除元素，也可以从头部自由地添加和删除。

deque 的接口和 vector 相比，有如下的区别：

* deque 提供 push_front、emplace_front 和 pop_front 成员函数。
* deque 不提供 data、capacity 和 reserve 成员函数。

deque 的内存布局一般是这样的：

![img](https://static001.geekbang.org/resource/image/ae/a1/aea92e7e005d65fa56d6332bb7706fa1.png)

可以看到：

* 如果只从头、尾两个位置对 deque 进行增删操作的话，容器里的对象永远不需要移动。
* 容器里的元素只是部分连续的（因而没法提供 data 成员函数）。
* 由于元素的存储大部分仍然连续，它的遍历性能是比较高的。
* 由于每一段存储大小相等，deque 支持使用下标访问容器元素，大致相当于` index[i / chunk_size][i % chunk_size]`，也保持高效。

如果你需要一个经常在头尾增删元素的容器，那 deque 会是个合适的选择。