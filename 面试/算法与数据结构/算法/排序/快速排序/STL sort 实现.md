### 并非所有容器都使用sort算法
既然问的是STL的sort算法实现，那么先确认一个问题，哪些STL容器需要用到sort算法？
* 首先，关系型容器拥有自动排序功能，因为底层采用RB-Tree，所以不需要用到sort算法。

* 其次，序列式容器中的stack、queue和priority-queue都有特定的出入口，不允许用户对元素排序。

* 剩下的vector、deque，适用sort算法。

实现逻辑STL的sort算法，数据量大时采用QuickSort快排算法，分段归并排序。一旦分段后的数据量小于某个门槛（16），为避免QuickSort快排的递归调用带来过大的额外负荷，就改用Insertion Sort插入排序。如果递归层次过深，还会改用HeapSort堆排序。
![image](https://pic1.zhimg.com/v2-8fa032e195365f77fb6b980a4ed71958_b.jpg)
结合快速排序-插入排序-堆排序 三种排序算法。


