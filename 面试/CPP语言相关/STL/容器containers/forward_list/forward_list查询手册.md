### forward\_list

forward_list（单向链表）是序列容器，允许在序列中的任何地方进行恒定的时间插入和擦除操作。

forward\_list（单向链表）被实现为单链表; 单链表可以将它们包含的每个元素存储在不同和不相关的存储位置中。通过关联到序列中下一个元素的链接的每个元素来保留排序。forward\_list容器和列表

之间的主要设计区别容器是第一个内部只保留一个到下一个元素的链接，而后者每个元素保留两个链接：一个指向下一个元素，一个指向前一个元素，允许在两个方向上有效的迭代，但是每个元素消耗额外的存储空间并且插入和移除元件的时间开销略高。因此，forward_list对象比列表对象更有效率，尽管它们只能向前迭代。

与其他基本的标准序列容器（array，vector和deque），forward_list通常在插入，提取和移动容器内任何位置的元素方面效果更好，因此也适用于密集使用这些元素的算法，如排序算法。

的主要缺点修饰符Modifiers S和列表相比这些其它序列容器s是说，他们缺乏可以通过位置的元素的直接访问; 例如，要访问forward_list中的第六个元素，必须从开始位置迭代到该位置，这需要在这些位置之间的线性时间。它们还消耗一些额外的内存来保持与每个元素相关联的链接信息（这可能是大型小元素列表的重要因素）。

该修饰符Modifiersclass模板的设计考虑到效率：按照设计，它与简单的手写C型单链表一样高效，实际上是唯一的标准容器，为了效率的考虑故意缺少尺寸成员函数：由于其性质作为一个链表，具有一个需要一定时间的大小的成员将需要它保持一个内部计数器的大小（如列表所示）。这会消耗一些额外的存储空间，并使插入和删除操作效率稍低。要获取forward_list对象的大小，可以使用距离算法的开始和结束，这是一个需要线性时间的操作。

![](http://img.blog.csdn.net/20160407212133266)

#### forward\_list::forward\_list

```cpp
default (1)	
explicit forward_list (const allocator_type& alloc = allocator_type());
fill (2)	
explicit forward_list (size_type n);
explicit forward_list (size_type n, const value_type& val,
                        const allocator_type& alloc = allocator_type());
range (3)	
template <class InputIterator>
  forward_list (InputIterator first, InputIterator last,
                const allocator_type& alloc = allocator_type());
copy (4)	
forward_list (const forward_list& fwdlst);
forward_list (const forward_list& fwdlst, const allocator_type& alloc);
move (5)	
forward_list (forward_list&& fwdlst);
forward_list (forward_list&& fwdlst, const allocator_type& alloc);
initializer list (6)	
forward_list (initializer_list<value_type> il,
              const allocator_type& alloc = allocator_type());
```

Example

```cpp
#include <iostream>
#include <forward_list>

int main ()
{
  // constructors used in the same order as described above:

  std::forward_list<int> first;                      // default: empty
  std::forward_list<int> second (3,77);              // fill: 3 seventy-sevens
  std::forward_list<int> third (second.begin(), second.end()); // range initialization
  std::forward_list<int> fourth (third);            // copy constructor
  std::forward_list<int> fifth (std::move(fourth));  // move ctor. (fourth wasted)
  std::forward_list<int> sixth = {3, 52, 25, 90};    // initializer_list constructor

  std::cout << "first:" ; for (int& x: first)  std::cout << " " << x; std::cout << '\n';
  std::cout << "second:"; for (int& x: second) std::cout << " " << x; std::cout << '\n';
  std::cout << "third:";  for (int& x: third)  std::cout << " " << x; std::cout << '\n';
  std::cout << "fourth:"; for (int& x: fourth) std::cout << " " << x; std::cout << '\n';
  std::cout << "fifth:";  for (int& x: fifth)  std::cout << " " << x; std::cout << '\n';
  std::cout << "sixth:";  for (int& x: sixth)  std::cout << " " << x; std::cout << '\n';

  return 0;
}
```

Possible output

```
forward_list constructor examples:
first:
second: 77 77 77
third: 77 77 77
fourth:
fifth: 77 77 77
sixth: 3 52 25 90
```

#### forward\_list::~forward\_list

#### forward\_list::before\_begin

返回指向容器中第一个元素之前的位置的迭代器。

返回的迭代器不应被解除引用：它是为了用作成员函数的参数emplace\_after，insert\_after，erase\_after或splice\_after，指定序列，其中执行该动作的位置的开始位置。

```cpp
      iterator before_begin() noexcept;
const_iterator before_begin() const noexcept;
```

Example

```cpp
#include <iostream>
#include <forward_list>

int main ()
{
  std::forward_list<int> mylist = {20, 30, 40, 50};

  mylist.insert_after ( mylist.before_begin(), 11 );

  std::cout << "mylist contains:";
  for ( int& x: mylist ) std::cout << ' ' << x;
  std::cout << '\n';

  return 0;
}
```

Output

```
mylist contains: 11 20 30 40 50
```

#### forward\_list::cbefore\_begin

返回指向容器中第一个元素之前的位置的const_iterator。

一个常量性是指向常量内容的迭代器。这个迭代器可以增加和减少（除非它本身也是const），就像forward\_list::before\_begin返回的迭代器一样，但不能用来修改它指向的内容。

返回的价值不得解除引用。

```cpp
const_iterator cbefore_begin() const noexcept;
```

Example

```cpp
#include <iostream>
#include <forward_list>

int main ()
{
  std::forward_list<int> mylist = {77, 2, 16};

  mylist.insert_after ( mylist.cbefore_begin(), 19 );

  std::cout << "mylist contains:";
  for ( int& x: mylist ) std::cout << ' ' << x;
  std::cout << '\n';

  return 0;
}
```

Output

```
mylist contains: 19 77 2 16
```

