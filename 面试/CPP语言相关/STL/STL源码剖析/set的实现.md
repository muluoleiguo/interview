### STL中set的实现？

STL中的容器可分为序列式容器（sequence）和关联式容器（associative），set属于关联式容器。

set的特性是，所有元素都会根据元素的值自动被排序（默认升序），set元素的键值就是实值，实值就是键值，set不允许有两个相同的键值

set不允许迭代器修改元素的值，其迭代器是一种constance iterators

标准的STL set以RB-tree（红黑树）作为底层机制，几乎所有的set操作行为都是转调用RB-tree的操作行为，这里补充一下红黑树的特性：

- 每个节点不是红色就是黑色
- 根结点为黑色
- 如果节点为红色，其子节点必为黑
- 任一节点至（NULL）树尾端的任何路径，所含的黑节点数量必相同

关于红黑树的具体操作过程，比较复杂读者可以翻阅《算法导论》详细了解。

举个例子：

```C++
#include <set>
#include <iostream>
using namespace std;


int main()
{
	int i;
	int ia[5] = { 1,2,3,4,5 };
	set<int> s(ia, ia + 5);
	cout << s.size() << endl; // 5
	cout << s.count(3) << endl; // 1
	cout << s.count(10) << endl; // 0

	s.insert(3); //再插入一个3
	cout << s.size() << endl; // 5
	cout << s.count(3) << endl; // 1

	s.erase(1);
	cout << s.size() << endl; // 4

	set<int>::iterator b = s.begin();
	set<int>::iterator e = s.end();
	for (; b != e; ++b)
		cout << *b << " "; // 2 3 4 5
	cout << endl;

	b = find(s.begin(), s.end(), 5);
	if (b != s.end())
		cout << "5 found" << endl; // 5 found

	b = s.find(2);
	if (b != s.end())
		cout << "2 found" << endl; // 2 found

	b = s.find(1);
	if (b == s.end())
		cout << "1 not found" << endl; // 1 not found
	return 0;
}
```

关联式容器尽量使用其自身提供的find()函数查找指定的元素，效率更高，因为STL提供的find()函数是一种顺序搜索算法。

> 《STL源码剖析》 侯捷 