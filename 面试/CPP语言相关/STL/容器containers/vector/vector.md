### vector

vector 应该是最常用的容器了。它的名字“向量”来源于数学术语，但在实际应用中，我们把它当成动态数组更为合适。它基本相当于 Java 的 ArrayList 和 Python 的 list。

和 string 相似，vector 的成员在内存里连续存放，同时 begin、end、front、back 成员函数指向的位置也和 string 一样，大致如下图所示：

![img](https://static001.geekbang.org/resource/image/24/10/247951f886561c30ced2eb7700f9d510.png)

除了容器类的共同点，vector 允许下面的操作（不完全列表）：

* 可以使用中括号的下标来访问其成员（同 string）
*  可以使用 data 来获得指向其内容的裸指针（同 string）
*  可以使用 capacity 来获得当前分配的存储空间的大小，以元素数量计（同 string）
*  可以使用 reserve 来改变所需的存储空间的大小，成功后 capacity() 会改变（同 string）
* 可以使用 resize 来改变其大小，成功后 size() 会改变（同 string）
* 可以使用 pop_back 来删除最后一个元素（同 string）
* 可以使用 push_back 在尾部插入一个元素（同 string）
* 可以使用 insert 在指定位置前插入一个元素（同 string）
* 可以使用 erase 在指定位置删除一个元素（同 string）
* 可以使用 emplace 在指定位置构造一个元素
* 可以使用 emplace_back 在尾部新构造一个元素

大家可以留意一下 push\_… 和 pop_… 成员函数。它们存在时，说明容器对指定位置的删除和插入性能较高。vector 适合在尾部操作，这是它的内存布局决定的。只有在尾部插入和删除时，其他元素才会不需要移动，除非内存空间不足导致需要重新分配内存空间。

当 push_back、insert、reserve、resize 等函数导致内存重分配时，或当 insert、erase 导致元素位置移动时，vector 会试图把元素“移动”到新的内存区域。vector 通常保证强异常安全性，如果元素类型没有提供一个**保证不抛异常的移动构造函数，**vector 通常会使用拷贝构造函数。因此，对于拷贝代价较高的自定义元素类型，我们应当定义移动构造函数，并标其为 noexcept，或只在容器中放置对象的智能指针。这就是为什么我之前需要在 smart_ptr 的实现中标上 noexcept 的原因。

下面的代码可以演示这一行为：

```c++

#include <iostream>
#include <vector>

using namespace std;

class Obj1 {
public:
  Obj1()
  {
    cout << "Obj1()\n";
  }
  Obj1(const Obj1&)
  {
    cout << "Obj1(const Obj1&)\n";
  }
  Obj1(Obj1&&)
  {
    cout << "Obj1(Obj1&&)\n";
  }
};

class Obj2 {
public:
  Obj2()
  {
    cout << "Obj2()\n";
  }
  Obj2(const Obj2&)
  {
    cout << "Obj2(const Obj2&)\n";
  }
  // 注意里的不同
  Obj2(Obj2&&) noexcept
  {
    cout << "Obj2(Obj2&&)\n";
  }
};

int main()
{
  vector<Obj1> v1;
  v1.reserve(2);
  v1.emplace_back();
  v1.emplace_back();
  v1.emplace_back();

  vector<Obj2> v2;
  v2.reserve(2);
  v2.emplace_back();
  v2.emplace_back();
  v2.emplace_back();
}
```

我们可以立即得到下面的输出：

```
Obj1()
Obj1()
Obj1()
Obj1(const Obj1&)
Obj1(const Obj1&)
Obj2()
Obj2()
Obj2()
Obj2(Obj2&&)
Obj2(Obj2&&)
```



Obj1 和 Obj2 的定义只差了一个 noexcept，但这个小小的差异就导致了 vector 是否会移动对象。这点非常重要。

C++11 开始提供的 emplace… 系列函数是为了提升容器的性能而设计的。你可以试试把 v1.emplace_back() 改成 v1.push_back(Obj1())。对于 vector 里的内容，结果是一样的；但使用 push_back 会额外生成临时对象，多一次（移动或拷贝）构造和析构。如果是移动的情况，那会有小幅性能损失；如果对象没有实现移动的话，那性能差异就可能比较大了。

现代处理器的体系架构使得对连续内存访问的速度比不连续的内存要快得多。因而，vector 的连续内存使用是它的一大优势所在。当你不知道该用什么容器时，缺省就使用 vector 吧。

vector 的一个主要缺陷是大小增长时导致的元素移动。如果可能，尽早使用 reserve 函数为 vector 保留所需的内存，这在 vector 预期会增长很大时能带来很大的性能提升。