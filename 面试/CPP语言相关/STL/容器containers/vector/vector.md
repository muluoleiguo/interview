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



### vector越界访问下标？

通过下标访问vector中的元素时不会做边界检查，即便下标越界。

也就是说，下标与first迭代器相加的结果超过了finish迭代器的位置，程序也不会报错，而是返回这个地址中存储的值。

如果想在访问vector中的元素时首先进行边界检查，可以使用vector中的**at函数**。通过使用at函数不但可以通过下标访问vector中的元素，而且在at函数内部会对下标进行边界检查。

### STL 中vector删除其中的元素，迭代器如何变化？为什么是两倍扩容？释放空间？

size()函数返回的是已用空间大小，capacity()返回的是总空间大小，capacity()-size()则是剩余的可用空间大小。当size()和capacity()相等，说明vector目前的空间已被用完，如果再添加新元素，则会引起vector空间的动态增长。

由于动态增长会引起重新分配内存空间、拷贝原空间、释放原空间，这些过程会降低程序效率。因此，可以使用reserve(n)预先分配一块较大的指定大小的内存空间，这样当指定大小的内存空间未使用完时，是不会重新分配内存空间的，这样便提升了效率。只有当n>capacity()时，调用reserve(n)才会改变vector容量。

 resize()成员函数只改变元素的数目，不改变vector的容量。

1、空的vector对象，size()和capacity()都为0

2、当空间大小不足时，新分配的空间大小为原空间大小的2倍。

3、使用reserve()预先分配一块内存后，在空间未满的情况下，不会引起重新分配，从而提升了效率。

4、当reserve()分配的空间比原空间小时，是不会引起重新分配的。

5、resize()函数只改变容器的元素数目，未改变容器大小。

6、用reserve(size_type)只是扩大capacity值，这些内存空间可能还是“野”的，如果此时使用“[ ]”来访问，则可能会越界。而resize(size_type new_size)会真正使容器具有new_size个对象。

 不同的编译器，vector有不同的扩容大小。在vs下是1.5倍，在GCC下是2倍；

空间和时间的权衡。简单来说， 空间分配的多，平摊时间复杂度低，但浪费空间也多。

使用k=2增长因子的问题在于，每次扩展的新尺寸必然刚好大于之前分配的总和，也就是说，之前分配的内存空间不可能被使用。这样对内存不友好。最好把增长因子设为(1,2)

 对比可以发现采用采用成倍方式扩容，可以保证常数的时间复杂度，而增加指定大小的容量只能达到O(n)的时间复杂度，因此，使用成倍的方式扩容。

 

如何释放空间：

由于vector的内存占用空间只增不减，比如你首先分配了10,000个字节，然后erase掉后面9,999个，留下一个有效元素，但是内存占用仍为10,000个。所有内存空间是在vector析构时候才能被系统回收。empty()用来检测容器是否为空的，clear()可以清空所有元素。但是即使clear()，vector所占用的内存空间依然如故，无法保证内存的回收。

如果需要空间动态缩小，可以考虑使用deque。如果vector，可以用swap()来帮助你释放内存。

~~~cpp
vector(Vec).swap(Vec);
 //将Vec的内存空洞清除；
 vector().swap(Vec);
 //清空Vec的内存；

~~~



