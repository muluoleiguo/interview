
# 请你来说一说STL迭代器删除元素
这个主要考察的是迭代器失效的问题。
1. 对于序列容器vector,deque来说，使用erase(itertor)后，删除当前的iterator会使后面所有元素的iterator都失效，这是因为vector，deque使用了连续分配的内存，删除一个元素导致后面所有的元素会向前移动一个位置。不过erase方法可以返回下一个有效的iterator。使用方式如下,例如
 
```c++
vector<int> val = { 1,2,3,4,5,6 };
vector<int>::iterator iter;
for (iter = val.begin(); iter != val.end(); )
{
 if (3 == *iter)
 iter = val.erase(iter); //返回下一个有效的迭代器，无需+1
 else
 ++iter;
}
```

2. 对于关联容器(如map，set，multimap，multiset），使用了erase(iterator)后，仅仅会使当前元素的迭代器失效，但是其结构是红黑树，删除当前元素的，不会影响到下一个元素的迭代器，所以在调用erase之前，记录下一个元素的迭代器即可。
使用方式如下例子：

```c++
set<int> valset = { 1,2,3,4,5,6 };
set<int>::iterator iter;
for (iter = valset.begin(); iter != valset.end(); )
{
 if (3 == *iter)
 valset.erase(iter++);
 else
 ++iter;
}
```



3. 对于list来说，它使用了不连续分配的内存，并且它的erase方法也会返回下一个有效的iterator，因此上面两种正确的方法都可以使用。


# 请你来说一下STL中迭代器的作用，有指针为何还要迭代器

1、迭代器
Iterator（迭代器）模式又称Cursor（游标）模式，用于提供一种方法顺序访问一个聚合对象中各个元素, 而又不需暴露该对象的内部表示。或者这样说可能更容易理解：Iterator模式是运用于聚合对象的一种模式，通过运用该模式，使得我们可以在不知道对象内部表示的情况下，按照一定顺序（由iterator提供的方法）访问聚合对象中的各个元素。

由于Iterator模式的以上特性：与聚合对象耦合，在一定程度上限制了它的广泛运用，一般仅用于底层聚合支持类，如STL的list、vector、stack等容器类及ostream_iterator等扩展iterator。

2、迭代器和指针的区别

迭代器不是指针，是类模板，表现的像指针。他只是模拟了指针的一些功能，通过重载了指针的一些操作符，->、*、++、--等。迭代器封装了指针，是一个“可遍历STL（ Standard Template Library）容器内全部或部分元素”的对象， 本质是封装了原生指针，是指针概念的一种提升（lift），提供了比指针更高级的行为，相当于一种智能指针，他可以根据不同类型的数据结构来实现不同的 ++，--等操作。

迭代器返回的是对象引用而不是对象的值，所以cout只能输出迭代器使用*取值后的值而不能直接输出其自身。

3、迭代器产生原因

Iterator类的访问方式就是把不同集合类的访问逻辑抽象出来，使得不用暴露集合内部的结构而达到循环遍历集合的效果。