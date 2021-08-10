#### 关联容器

* set（集合）
* map（映射）
* multiset（多重集）
* multimap（多重映射）



跳出 C++ 的语境，map（映射）的更常见的名字是关联数组和字典 ，而在 JSON 里直接被称为对象（object）。

在 C++ 外这些容器常常是无序的；在 C++ 里**关联容器则被认为是有序**的。

名字带“multi”的允许键重复，不带的不允许键重复。set 和 multiset 只能用来存放键，而 map 和 multimap 则存放一个个键值对。

与序列容器相比，关联容器没有前、后的概念及相关的成员函数，但同样提供 insert、emplace 等成员函数。此外，关联容器都有 find、lower_bound、upper_bound 等查找函数，结果是一个迭代器：

* find(k) 可以找到**任何一个**等价于查找键 k 的元素（!(x < k || k < x)）
* lower_bound(k) 找到**第一个**不小于查找键 k 的元素（!(x < k)）
* upper_bound(k) 找到**第一个**大于查找键 k 的元素（k < x）

如果你需要在 multimap 里精确查找满足某个键的区间的话，建议使用 `equal_range`，可以一次性取得上下界（半开半闭）。

```c++
#include <tuple>
multimap<string, int>::iterator
  lower, upper;
std::tie(lower, upper) =
  mmp.equal_range("four");
```

如果在声明关联容器时没有提供比较类型的参数，缺省使用 less 来进行排序。如果键的类型提供了比较算符 < 的重载，我们不需要做任何额外的工作。否则，我们就需要对键类型进行 less 的特化，或者提供一个其他的函数对象类型。

对于自定义类型，我推荐尽量使用标准的 less 实现，通过重载 <（及其他标准比较运算符）对该类型的对象进行排序。存储在关联容器中的键一般应满足严格弱序关系（strict weak ordering；），即：

* 对于任何该类型的对象 x：!(x < x)（非自反）
* 对于任何该类型的对象 x 和 y：如果 x < y，则 !(y < x)（非对称）
* 对于任何该类型的对象 x、y 和 z：如果 x < y 并且 y < z，则 x < z（传递性）
* 对于任何该类型的对象 x、y 和 z：如果 x 和 y 不可比（!(x < y) 并且 !(y < x)）并且 y 和 z 不可比，则 x 和 z 不可比（不可比的传递性）

大部分情况下，类型是可以满足这些条件的，不过：

* 如果类型没有一般意义上的大小关系（如复数），我们一定要别扭地定义一个大小关系吗？
* 通过比较来进行查找、插入和删除，复杂度为对数 O(log(n))，有没有达到更好的性能的方法？

#### 无序关联容器

从 C++11 开始，每一个关联容器都有一个对应的无序关联容器，它们是：

* unordered_set
* unordered_map
* unordered_multiset
* unordered_multimap

都是以哈希表实现的。

![](http://img.blog.csdn.net/20160410123436394)

unordered\_set、unodered\_multiset结构：

![](http://img.blog.csdn.net/20160410123518692)

unordered\_map、unodered\_multimap结构：

![](http://img.blog.csdn.net/20160410123525739)

这些容器和关联容器非常相似，主要的区别就在于它们是“无序”的。这些容器不要求提供一个排序的函数对象，而要求一个可以计算哈希值的函数对象。你当然可以在声明容器对象时手动提供这样一个函数对象类型，但更常见的情况是，我们使用标准的 hash 函数对象及其特化。

```c++
#include <complex>        // std::complex
#include <iostream>       // std::cout/endl
#include <unordered_map>  // std::unordered_map
#include <unordered_set>  // std::unordered_set
#include "output_container.h"

using namespace std;

namespace std {

template <typename T>
struct hash<complex<T>> {
  size_t
  operator()(const complex<T>& v) const
    noexcept
  {
    hash<T> h;
    return h(v.real()) + h(v.imag());
  }
};

}  // namespace std

int main()
{
  unordered_set<int> s{
    1, 1, 2, 3, 5, 8, 13, 21
  };
  cout << s << endl;

  unordered_map<complex<double>,
                double>
    umc{{{1.0, 1.0}, 1.4142},
        {{3.0, 4.0}, 5.0}};
  cout << umc << endl;
}
```

输出可能是（顺序不能保证）：

```
{ 21, 5, 8, 3, 13, 2, 1 }
{ (3,4) => 5, (1,1) => 1.4142 }
```

请注意我们在 std 名空间中添加了特化，这是少数用户可以向 std 名空间添加内容的情况之一。正常情况下，向 std 名空间添加声明或定义是禁止的，属于未定义行为。

从实际的工程角度，无序关联容器的主要优点在于其性能。关联容器和 priority_queue 的插入和删除操作，以及关联容器的查找操作，其复杂度都是 O(log(n))，而无序关联容器的实现使用哈希表 ，可以达到平均 O(1)！但这取决于我们是否使用了一个好的哈希函数：在哈希函数选择不当的情况下，无序关联容器的插入、删除、查找性能可能成为最差情况的 O(n)，那就比关联容器糟糕得多了。