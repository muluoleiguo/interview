# vector如何扩容

* 新增元素：Vector通过一个连续的数组存放元素，如果集合已满，在新增数据的时候，就要分配一块更大的内存，将原来的数据复制过来，释放之前的内存，在插入新增的元素；
* 对vector的任何操作，一旦引起空间重新配置，指向原vector的所有迭代器就都失效了 ；
* 初始时刻vector的capacity为0，塞入第一个元素后capacity增加为1；
* 不同的编译器实现的扩容方式不一样，VS2015中以1.5倍扩容，GCC以2倍扩容。

**总结**

1. vector在push_back以成倍增长可以在均摊后达到O(1)的事件复杂度，相对于增长指定大小的O(n)时间复杂度更好。
2. 为了防止申请内存的浪费，现在使用较多的有2倍与1.5倍的增长方式，而1.5倍的增长方式可以更好的实现对内存的重复利用，因为更好。

### vector

vector是表示可以改变大小的数组的序列容器。

就像数组一样，vector为它们的元素使用连续的存储位置，这意味着它们的元素也可以使用到其元素的常规指针上的偏移来访问，而且和数组一样高效。但是与数组不同的是，它们的大小可以动态地改变，它们的存储由容器自动处理。

在内部，vector使用一个动态分配的数组来存储它们的元素。这个数组可能需要重新分配，以便在插入新元素时增加大小，这意味着分配一个新数组并将所有元素移动到其中。就处理时间而言，这是一个相对昂贵的任务，因此每次将元素添加到容器时矢量都不会重新分配。

相反，vector容器可以分配一些额外的存储以适应可能的增长，并且因此容器可以具有比严格需要包含其元素（即，其大小）的存储更大的实际容量。库可以实现不同的策略的增长到内存使用和重新分配之间的平衡，但在任何情况下，再分配应仅在对数生长的间隔发生尺寸，使得在所述载体的末端各个元件的插入可以与提供分期常量时间复杂性。

因此，与数组相比，载体消耗更多的内存来交换管理存储和以有效方式动态增长的能力。

与其他动态序列容器（deques，lists和 forward\_lists ）相比，vector非常有效地访问其元素（就像数组一样），并相对有效地从元素末尾添加或移除元素。对于涉及插入或移除除了结尾之外的位置的元素的操作，它们执行比其他位置更差的操作，并且具有比列表和 forward\_lists 更不一致的迭代器和引用。

针对 vector 的各种常见操作的复杂度（效率）如下： 

* 随机访问 - 常数 O(1) 
* 在尾部增删元素 - 平摊（amortized）常数 O(1)}} 
* 增删元素 - 至 vector 尾部的线性距离 O(n)}}

```cpp
template < class T, class Alloc = allocator<T> > class vector;
```

![](http://img.blog.csdn.net/20160406151211233)

#### vector::vector

（1）empty容器构造函数（默认构造函数）
构造一个空的容器，没有元素。
（2）fill构造函数
用n个元素构造一个容器。每个元素都是val的副本（如果提供）。
（3）范围（range）构造器
使用与[ range，first，last]范围内的元素相同的顺序构造一个容器，其中的每个元素都是emplace -从该范围内相应的元素构造而成。
（4）复制（copy）构造函数（并用分配器复制）
按照相同的顺序构造一个包含x中每个元素的副本的容器。
（5）移动（move）构造函数（和分配器移动）
构造一个获取x元素的容器。
如果指定了alloc并且与x的分配器不同，那么元素将被移动。否则，没有构建元素（他们的所有权直接转移）。
x保持未指定但有效的状态。
（6）初始化列表构造函数
构造一个容器中的每个元件中的一个拷贝的IL，以相同的顺序。

```cpp
default (1)	
explicit vector (const allocator_type& alloc = allocator_type());
fill (2)	
explicit vector (size_type n);
         vector (size_type n, const value_type& val,
                 const allocator_type& alloc = allocator_type());
range (3)	
template <class InputIterator>
  vector (InputIterator first, InputIterator last,
          const allocator_type& alloc = allocator_type());
copy (4)	
vector (const vector& x);
vector (const vector& x, const allocator_type& alloc);
move (5)	
vector (vector&& x);
vector (vector&& x, const allocator_type& alloc);
initializer list (6)	
vector (initializer_list<value_type> il,
       const allocator_type& alloc = allocator_type());
```

Example

```cpp
#include <iostream>
#include <vector>

int main ()
{
    // constructors used in the same order as described above:
    std::vector<int> first;             // empty vector of ints
    std::vector<int> second(4, 100);    // four ints with value 100
    std::vector<int> third(second.begin(), second.end());// iterating through second
    std::vector<int> fourth(third);     // a copy of third

    // the iterator constructor can also be used to construct from arrays:
    int myints[] = {16,2,77,29};
    std::vector<int> fifth(myints, myints + sizeof(myints) / sizeof(int));

    std::cout << "The contents of fifth are:";
    for(std::vector<int>::iterator it = fifth.begin(); it != fifth.end(); ++it)
        std::cout << ' ' << *it;
    std::cout << '\n';

    return 0;
}
```

Output

```
The contents of fifth are: 16 2 77 29 
```

#### vector::~vector

销毁容器对象。这将在每个包含的元素上调用allocator_traits::destroy，并使用其分配器释放由矢量分配的所有存储容量。

```cpp
~vector();
```

#### vector::operator=

将新内容分配给容器，替换其当前内容，并相应地修改其大小。

```cpp
copy (1)	
vector& operator= (const vector& x);
move (2)	
vector& operator= (vector&& x);
initializer list (3)	
vector& operator= (initializer_list<value_type> il);
```

Example

```
#include <iostream>
#include <vector>

int main ()
{
  std::vector<int> foo (3,0);
  std::vector<int> bar (5,0);

  bar = foo;
  foo = std::vector<int>();

  std::cout << "Size of foo: " << int(foo.size()) << '\n';
  std::cout << "Size of bar: " << int(bar.size()) << '\n';
  return 0;
}
```

Output

```
Size of foo: 0
Size of bar: 3
```

#### vector::begin

#### vector::end

#### vector::rbegin

#### vector::rend

#### vector::cbegin

#### vector::cend

#### vector::rcbegin

#### vector::rcend

#### vector::size

返回vector中元素的数量。

这是vector中保存的实际对象的数量，不一定等于其存储容量。

```cpp
size_type size() const noexcept;
```

Example

```cpp
#include <iostream>
#include <vector>

int main ()
{
  std::vector<int> myints;
  std::cout << "0. size: " << myints.size() << '\n';

  for (int i=0; i<10; i++) myints.push_back(i);
  std::cout << "1. size: " << myints.size() << '\n';

  myints.insert (myints.end(),10,100);
  std::cout << "2. size: " << myints.size() << '\n';

  myints.pop_back();
  std::cout << "3. size: " << myints.size() << '\n';

  return 0;
}
```

Output

```
0. size: 0
1. size: 10
2. size: 20
3. size: 19
```

#### vector::max_size

返回该vector可容纳的元素的最大数量。由于已知的系统或库实现限制，

这是容器可以达到的最大潜在大小，但容器无法保证能够达到该大小：在达到该大小之前的任何时间，仍然无法分配存储。

```cpp
size_type max_size() const noexcept;
```

Example

```cpp
#include <iostream>
#include <vector>

int main ()
{
  std::vector<int> myvector;

  // set some content in the vector:
  for (int i=0; i<100; i++) myvector.push_back(i);

  std::cout << "size: " << myvector.size() << "\n";
  std::cout << "capacity: " << myvector.capacity() << "\n";
  std::cout << "max_size: " << myvector.max_size() << "\n";
  return 0;
}
```

A possible output for this program could be:

```
size: 100
capacity: 128
max_size: 1073741823
```

#### vector::resize

调整容器的大小，使其包含n个元素。

如果n小于当前的容器size，内容将被缩小到前n个元素，将其删除（并销毁它们）。

如果n大于当前容器size，则通过在末尾插入尽可能多的元素以达到大小n来扩展内容。如果指定了val，则新元素将初始化为val的副本，否则将进行值初始化。

如果n也大于当前的容器的capacity（容量），分配的存储空间将自动重新分配。

注意这个函数通过插入或者删除元素的内容来改变容器的实际内容。

```cpp
void resize (size_type n);
void resize (size_type n, const value_type& val);
```

Example

```cpp
#include <iostream>
#include <vector>

int main ()
{
  std::vector<int> myvector;

  // set some initial content:
  for (int i=1;i<10;i++) myvector.push_back(i);

  myvector.resize(5);
  myvector.resize(8,100);
  myvector.resize(12);

  std::cout << "myvector contains:";
  for (int i=0;i<myvector.size();i++)
    std::cout << ' ' << myvector[i];
  std::cout << '\n';

  return 0;
}
```

Output

```
myvector contains: 1 2 3 4 5 100 100 100 0 0 0 0
```

#### vector::capacity

返回当前为vector分配的存储空间的大小，用元素表示。这个capacity(容量)不一定等于vector的size。它可以相等或更大，额外的空间允许适应增长，而不需要重新分配每个插入。

```cpp
size_type capacity() const noexcept;
```

Example

```cpp
#include <iostream>
#include <vector>

int main ()
{
  std::vector<int> myvector;

  // set some content in the vector:
  for (int i=0; i<100; i++) myvector.push_back(i);

  std::cout << "size: " << (int) myvector.size() << '\n';
  std::cout << "capacity: " << (int) myvector.capacity() << '\n';
  std::cout << "max_size: " << (int) myvector.max_size() << '\n';
  return 0;
}
```

A possible output for this program could be:

```
size: 100
capacity: 128
max_size: 1073741823
```

#### vector::empty

返回vector是否为空（即，它的size是否为0）

```cpp
bool empty() const noexcept;
```

Example

```cpp
#include <iostream>
#include <vector>

int main ()
{
  std::vector<int> myvector;
  int sum (0);

  for (int i=1;i<=10;i++) myvector.push_back(i);

  while (!myvector.empty())
  {
     sum += myvector.back();
     myvector.pop_back();
  }

  std::cout << "total: " << sum << '\n';

  return 0;
}
```

Output

```
total: 55
```

#### vector::reserve

请求vector容量至少足以包含n个元素。

如果n大于当前vector容量，则该函数使容器重新分配其存储容量，从而将其容量增加到n（或更大）。

在所有其他情况下，函数调用不会导致重新分配，并且vector容量不受影响。

这个函数对vector大小没有影响，也不能改变它的元素。

```cpp
void reserve (size_type n);
```

Example

```cpp
#include <iostream>
#include <vector>

int main ()
{
  std::vector<int>::size_type sz;

  std::vector<int> foo;
  sz = foo.capacity();
  std::cout << "making foo grow:\n";
  for (int i=0; i<100; ++i) {
    foo.push_back(i);
    if (sz!=foo.capacity()) {
      sz = foo.capacity();
      std::cout << "capacity changed: " << sz << '\n';
    }
  }

  std::vector<int> bar;
  sz = bar.capacity();
  bar.reserve(100);   // this is the only difference with foo above
  std::cout << "making bar grow:\n";
  for (int i=0; i<100; ++i) {
    bar.push_back(i);
    if (sz!=bar.capacity()) {
      sz = bar.capacity();
      std::cout << "capacity changed: " << sz << '\n';
    }
  }
  return 0;
}
```

Possible output

```
making foo grow:
capacity changed: 1
capacity changed: 2
capacity changed: 4
capacity changed: 8
capacity changed: 16
capacity changed: 32
capacity changed: 64
capacity changed: 128
making bar grow:
capacity changed: 100
```

#### vector::shrink_to_fit

要求容器减小其capacity(容量)以适应其尺寸。

该请求是非绑定的，并且容器实现可以自由地进行优化，并且保持capacity大于其size的vector。 这可能导致重新分配，但对矢量大小没有影响，并且不能改变其元素。

```cpp
void shrink_to_fit();
```

Example

```cpp
#include <iostream>
#include <vector>

int main ()
{
  std::vector<int> myvector (100);
  std::cout << "1. capacity of myvector: " << myvector.capacity() << '\n';

  myvector.resize(10);
  std::cout << "2. capacity of myvector: " << myvector.capacity() << '\n';

  myvector.shrink_to_fit();
  std::cout << "3. capacity of myvector: " << myvector.capacity() << '\n';

  return 0;
}
```

Possible output

```
1. capacity of myvector: 100
2. capacity of myvector: 100
3. capacity of myvector: 10
```

#### vector::operator[]

#### vector::at

#### vector::front

#### vector::back

#### vector::data

#### vector::assign

将新内容分配给vector，替换其当前内容，并相应地修改其大小。

在范围版本（1）中，新内容是从第一个和最后一个范围内的每个元素按相同顺序构造的元素。

在填充版本（2）中，新内容是n个元素，每个元素都被初始化为一个val的副本。

在初始化列表版本（3）中，新内容是以相同顺序作为初始化列表传递的值的副本。

所述内部分配器被用于（通过其性状），以分配和解除分配存储器如果重新分配发生。它也习惯于摧毁所有现有的元素，并构建新的元素。

```cpp
range (1)	
template <class InputIterator>
  void assign (InputIterator first, InputIterator last);
fill (2)	
void assign (size_type n, const value_type& val);
initializer list (3)	
void assign (initializer_list<value_type> il);
```

Example

```cpp
#include <iostream>
#include <vector>

int main ()
{
  std::vector<int> first;
  std::vector<int> second;
  std::vector<int> third;

  first.assign (7,100);             // 7 ints with a value of 100

  std::vector<int>::iterator it;
  it=first.begin()+1;

  second.assign (it,first.end()-1); // the 5 central values of first

  int myints[] = {1776,7,4};
  third.assign (myints,myints+3);   // assigning from array.

  std::cout << "Size of first: " << int (first.size()) << '\n';
  std::cout << "Size of second: " << int (second.size()) << '\n';
  std::cout << "Size of third: " << int (third.size()) << '\n';
  return 0;
}
```

Output

```
Size of first: 7
Size of second: 5
Size of third: 3
```

补充：vector::assign 与 vector::operator= 的区别：

1. vector::assign 实现源码

```cpp
void assign(size_type __n, const _Tp& __val) { _M_fill_assign(__n, __val); }

template <class _Tp, class _Alloc>
void vector<_Tp, _Alloc>::_M_fill_assign(size_t __n, const value_type& __val) 
{
  if (__n > capacity()) {
    vector<_Tp, _Alloc> __tmp(__n, __val, get_allocator());
    __tmp.swap(*this);
  }
  else if (__n > size()) {
    fill(begin(), end(), __val);
    _M_finish = uninitialized_fill_n(_M_finish, __n - size(), __val);
  }
  else
    erase(fill_n(begin(), __n, __val), end());
}
```

2. vector::operator= 实现源码

```cpp
template <class _Tp, class _Alloc>
vector<_Tp,_Alloc>& 
vector<_Tp,_Alloc>::operator=(const vector<_Tp, _Alloc>& __x)
{
  if (&__x != this) {
    const size_type __xlen = __x.size();
    if (__xlen > capacity()) {
      iterator __tmp = _M_allocate_and_copy(__xlen, __x.begin(), __x.end());
      destroy(_M_start, _M_finish);
      _M_deallocate(_M_start, _M_end_of_storage - _M_start);
      _M_start = __tmp;
      _M_end_of_storage = _M_start + __xlen;
    }
    else if (size() >= __xlen) {
      iterator __i = copy(__x.begin(), __x.end(), begin());
      destroy(__i, _M_finish);
    }
    else {
      copy(__x.begin(), __x.begin() + size(), _M_start);
      uninitialized_copy(__x.begin() + size(), __x.end(), _M_finish);
    }
    _M_finish = _M_start + __xlen;
  }
  return *this;
}
```

#### vector::push_back

在vector的最后一个元素之后添加一个新元素。val的内容被复制（或移动）到新的元素。

这有效地将容器size增加了一个，如果新的矢量size超过了当前vector的capacity，则导致所分配的存储空间自动重新分配。

```cpp
void push_back (const value_type& val);
void push_back (value_type&& val);
```

Example

```cpp
#include <iostream>
#include <vector>

int main ()
{
  std::vector<int> myvector;
  int myint;

  std::cout << "Please enter some integers (enter 0 to end):\n";

  do {
    std::cin >> myint;
    myvector.push_back (myint);
  } while (myint);

  std::cout << "myvector stores " << int(myvector.size()) << " numbers.\n";

  return 0;
}
```

#### vector::pop_back

删除vector中的最后一个元素，有效地将容器size减少一个。

这破坏了被删除的元素。

```cpp
void pop_back();
```

Example

```cpp
#include <iostream>
#include <vector>

int main ()
{
  std::vector<int> myvector;
  int sum (0);
  myvector.push_back (100);
  myvector.push_back (200);
  myvector.push_back (300);

  while (!myvector.empty())
  {
    sum+=myvector.back();
    myvector.pop_back();
  }

  std::cout << "The elements of myvector add up to " << sum << '\n';

  return 0;
}
```

Output

```
The elements of myvector add up to 600
```

#### vector::insert

通过在指定位置的元素之前插入新元素来扩展该vector，通过插入元素的数量有效地增加容器大小。 这会导致分配的存储空间自动重新分配，只有在新的vector的size超过当前的vector的capacity的情况下。 

由于vector使用数组作为其基础存储，因此除了将元素插入到vector末尾之后，或vector的begin之前，其他位置会导致容器重新定位位置之后的所有元素到他们的新位置。与其他种类的序列容器（例如list或forward_list）执行相同操作的操作相比，这通常是低效的操作。

```cpp
single element (1)	
iterator insert (const_iterator position, const value_type& val);
fill (2)	
iterator insert (const_iterator position, size_type n, const value_type& val);
range (3)	
template <class InputIterator>
iterator insert (const_iterator position, InputIterator first, InputIterator last);
move (4)	
iterator insert (const_iterator position, value_type&& val);
initializer list (5)	
iterator insert (const_iterator position, initializer_list<value_type> il);
```

Example

```cpp
#include <iostream>
#include <vector>

int main ()
{
  std::vector<int> myvector (3,100);
  std::vector<int>::iterator it;

  it = myvector.begin();
  it = myvector.insert ( it , 200 );

  myvector.insert (it,2,300);

  // "it" no longer valid, get a new one:
  it = myvector.begin();

  std::vector<int> anothervector (2,400);
  myvector.insert (it+2,anothervector.begin(),anothervector.end());

  int myarray [] = { 501,502,503 };
  myvector.insert (myvector.begin(), myarray, myarray+3);

  std::cout << "myvector contains:";
  for (it=myvector.begin(); it<myvector.end(); it++)
    std::cout << ' ' << *it;
  std::cout << '\n';

  return 0;
}
```

Output

```
myvector contains: 501 502 503 300 300 400 400 200 100 100 100
```

补充：insert 迭代器野指针错误：

```cpp
int main()
{
	std::vector<int> v(5, 0);
	std::vector<int>::iterator vi;

  // 获取vector第一个元素的迭代器
	vi = v.begin();

  // push_back 插入元素之后可能会因为 push_back 的骚操作（创建一个新vector把旧vector的值复制到新vector），导致vector迭代器iterator的指针变成野指针，而导致insert出错
	v.push_back(10);

	v.insert(vi, 2, 300);

	return 0;
}
```

改正：应该把`vi = v.begin();`放到`v.push_back(10);`后面

#### vector::erase

从vector中删除单个元素（position）或一系列元素（[first，last））。

这有效地减少了被去除的元素的数量，从而破坏了容器的大小。

由于vector使用一个数组作为其底层存储，所以删除除vector结束位置之后，或vector的begin之前的元素外，将导致容器将段被擦除后的所有元素重新定位到新的位置。与其他种类的序列容器（例如list或forward_list）执行相同操作的操作相比，这通常是低效的操作。

```cpp
iterator erase (const_iterator position);
iterator erase (const_iterator first, const_iterator last);
```

Example

```cpp
#include <iostream>
#include <vector>

int main ()
{
  std::vector<int> myvector;

  // set some values (from 1 to 10)
  for (int i=1; i<=10; i++) myvector.push_back(i);

  // erase the 6th element
  myvector.erase (myvector.begin()+5);

  // erase the first 3 elements:
  myvector.erase (myvector.begin(),myvector.begin()+3);

  std::cout << "myvector contains:";
  for (unsigned i=0; i<myvector.size(); ++i)
    std::cout << ' ' << myvector[i];
  std::cout << '\n';

  return 0;
}
```

Output

```
myvector contains: 4 5 7 8 9 10
```

#### vector::swap

通过x的内容交换容器的内容，x是另一个相同类型的vector对象。尺寸可能不同。

在调用这个成员函数之后，这个容器中的元素是那些在调用之前在x中的元素，而x的元素是在这个元素中的元素。所有迭代器，引用和指针对交换对象保持有效。

请注意，非成员函数存在具有相同名称的交换，并使用与此成员函数相似的优化来重载该算法。

```cpp
void swap (vector& x);
```

Example

```cpp
#include <iostream>
#include <vector>

int main ()
{
  std::vector<int> foo (3,100);   // three ints with a value of 100
  std::vector<int> bar (5,200);   // five ints with a value of 200

  foo.swap(bar);

  std::cout << "foo contains:";
  for (unsigned i=0; i<foo.size(); i++)
    std::cout << ' ' << foo[i];
  std::cout << '\n';

  std::cout << "bar contains:";
  for (unsigned i=0; i<bar.size(); i++)
    std::cout << ' ' << bar[i];
  std::cout << '\n';

  return 0;
}
```

Output

```
foo contains: 200 200 200 200 200 
bar contains: 100 100 100 
```

#### vector::clear

从vector中删除所有的元素（被销毁），留下size为0的容器。

不保证重新分配，并且由于调用此函数， vector的capacity不保证发生变化。强制重新分配的典型替代方法是使用swap：`vector<T>().swap(x);   // clear x reallocating `

```cpp
void clear() noexcept;
```

Example

```cpp
#include <iostream>
#include <vector>

void printVector(const std::vector<int> &v)
{
	for (auto it = v.begin(); it != v.end(); ++it)
	{
		std::cout << *it << ' ';
	}
	std::cout << std::endl;
}

int main()
{
	std::vector<int> v1(5, 50);

	printVector(v1);
	std::cout << "v1 size  = " << v1.size() << std::endl;
	std::cout << "v1 capacity  = " << v1.capacity() << std::endl;

	v1.clear();

	printVector(v1);
	std::cout << "v1 size  = " << v1.size() << std::endl;
	std::cout << "v1 capacity  = " << v1.capacity() << std::endl;

	v1.push_back(11);
	v1.push_back(22);

	printVector(v1);
	std::cout << "v1 size  = " << v1.size() << std::endl;
	std::cout << "v1 capacity  = " << v1.capacity() << std::endl;

	return 0;
}
```

Output

```
50 50 50 50 50
v1 size  = 5
v1 capacity  = 5

v1 size  = 0
v1 capacity  = 5
11 22
v1 size  = 2
v1 capacity  = 5
```

#### vector::emplace

通过在position位置处插入新元素args来扩展容器。这个新元素是用args作为构建的参数来构建的。

这有效地增加了一个容器的大小。

分配存储空间的自动重新分配发生在新的vector的size超过当前向量容量的情况下。

由于vector使用数组作为其基础存储，因此除了将元素插入到vector末尾之后，或vector的begin之前，其他位置会导致容器重新定位位置之后的所有元素到他们的新位置。与其他种类的序列容器（例如list或forward_list）执行相同操作的操作相比，这通常是低效的操作。

该元素是通过调用allocator_traits::construct来转换args来创建的。插入一个类似的成员函数，将现有对象复制或移动到容器中。

```cpp
template <class... Args>
iterator emplace (const_iterator position, Args&&... args);
```

Example

```cpp
#include <iostream>
#include <vector>

int main ()
{
  std::vector<int> myvector = {10,20,30};

  auto it = myvector.emplace ( myvector.begin()+1, 100 );
  myvector.emplace ( it, 200 );
  myvector.emplace ( myvector.end(), 300 );

  std::cout << "myvector contains:";
  for (auto& x: myvector)
    std::cout << ' ' << x;
  std::cout << '\n';

  return 0;
}
```

Output

```
myvector contains: 10 200 100 20 30 300
```

#### vector::emplace_back

在vector的末尾插入一个新的元素，紧跟在当前的最后一个元素之后。这个新元素是用args作为构造函数的参数来构造的。

这有效地将容器大小增加了一个，如果新的矢量大小超过了当前的vector容量，则导致所分配的存储空间自动重新分配。

该元素是通过调用allocator_traits :: construct来转换args来创建的。

与push\_back相比，emplace\_back可以避免额外的复制和移动操作。

```cpp
template <class... Args>
  void emplace_back (Args&&... args);
```

Example

```cpp
#include <vector>
#include <string>
#include <iostream>
 
struct President
{
    std::string name;
    std::string country;
    int year;
 
    President(std::string p_name, std::string p_country, int p_year)
        : name(std::move(p_name)), country(std::move(p_country)), year(p_year)
    {
        std::cout << "I am being constructed.\n";
    }
    President(President&& other)
        : name(std::move(other.name)), country(std::move(other.country)), year(other.year)
    {
        std::cout << "I am being moved.\n";
    }
    President& operator=(const President& other) = default;
};
 
int main()
{
    std::vector<President> elections;
    std::cout << "emplace_back:\n";
    elections.emplace_back("Nelson Mandela", "South Africa", 1994);
 
    std::vector<President> reElections;
    std::cout << "\npush_back:\n";
    reElections.push_back(President("Franklin Delano Roosevelt", "the USA", 1936));
 
    std::cout << "\nContents:\n";
    for (President const& president: elections) {
        std::cout << president.name << " was elected president of "
                  << president.country << " in " << president.year << ".\n";
    }
    for (President const& president: reElections) {
        std::cout << president.name << " was re-elected president of "
                  << president.country << " in " << president.year << ".\n";
    }
}
```

Output

```
emplace_back:
I am being constructed.
 
push_back:
I am being constructed.
I am being moved.
 
Contents:
Nelson Mandela was elected president of South Africa in 1994.
Franklin Delano Roosevelt was re-elected president of the USA in 1936.
```

#### vector::get_allocator

返回与vector关联的构造器对象的副本。

```cpp
allocator_type get_allocator() const noexcept;
```

Example

```cpp
#include <iostream>
#include <vector>

int main ()
{
  std::vector<int> myvector;
  int * p;
  unsigned int i;

  // allocate an array with space for 5 elements using vector's allocator:
  p = myvector.get_allocator().allocate(5);

  // construct values in-place on the array:
  for (i=0; i<5; i++) myvector.get_allocator().construct(&p[i],i);

  std::cout << "The allocated array contains:";
  for (i=0; i<5; i++) std::cout << ' ' << p[i];
  std::cout << '\n';

  // destroy and deallocate:
  for (i=0; i<5; i++) myvector.get_allocator().destroy(&p[i]);
  myvector.get_allocator().deallocate(p,5);

  return 0;
}
```

Output

```
The allocated array contains: 0 1 2 3 4
```

注意：deallocate和destory的关系：

deallocate实现的源码：

	template <class T>
	inline void _deallocate(T* buffer)
	{
		::operator delete(buffer);    //为什么不用 delete [] ?  ,operator delete 区别于 delete 
		                             //operator delete  是一个底层操作符
	}

destory：

	template <class T>
	inline void _destory(T *ptr)
	{
		ptr->~T();
	}

destory负责调用类型的析构函数，销毁相应内存上的内容（但销毁后内存地址仍保留）

deallocate负责释放内存（此时相应内存中的值在此之前应调用destory销毁，将内存地址返回给系统，代表这部分地址使用引用-1）




#### relational operators (vector)

#### swap (vector)

#### vector <bool>