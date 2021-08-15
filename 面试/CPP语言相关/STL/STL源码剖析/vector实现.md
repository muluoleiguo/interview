### STL中vector的实现

vector是一种序列式容器，其数据安排以及操作方式与array非常类似，两者的唯一差别就是对于空间运用的灵活性，众所周知，array占用的是静态空间，一旦配置了就不可以改变大小，如果遇到空间不足的情况还要自行创建更大的空间，并手动将数据拷贝到新的空间中，再把原来的空间释放。vector则使用灵活的动态空间配置，维护一块**连续的线性空间**，在空间不足时，可以自动扩展空间容纳新元素，做到按需供给。其在扩充空间的过程中仍然需要经历：**重新配置空间，移动数据，释放原空间**等操作。这里需要说明一下动态扩容的规则：以原大小的两倍配置另外一块较大的空间（或者旧长度+新增元素的个数），源码：

```C++
const size_type len  = old_size + max(old_size, n);


```

 Vector扩容倍数与平台有关，在Win +  VS 下是 1.5倍，在 Linux + GCC 下是 2 倍 

测试代码：

```C++
#include <iostream>
#include <vector>
using namespace std;

int main()
{
    //在Linux + GCC下
	vector<int> res(2,0);
	cout << res.capacity() <<endl; //2
	res.push_back(1);
	cout << res.capacity() <<endl;//4
	res.push_back(2);
	res.push_back(3);
    cout << res.capacity() <<endl;//8
	return 0;
    
    
    //在 win 10 + VS2019下
    vector<int> res(2,0);
	cout << res.capacity() <<endl; //2
	res.push_back(1);
	cout << res.capacity() <<endl;//3
	res.push_back(2);
	res.push_back(3);
    cout << res.capacity() <<endl;//6
    
    
}


```

运行上述代码，一开始配置了一块长度为2的空间，接下来插入一个数据，长度变为原来的两倍，为4，此时已占用的长度为3，再继续两个数据，此时长度变为8，可以清晰的看到空间的变化过程

需要注意的是，频繁对vector调用push_back()对性能是有影响的，这是因为每插入一个元素，如果空间够用的话还能直接插入，若空间不够用，则需要重新配置空间，移动数据，释放原空间等操作，对程序性能会造成一定的影响

> 《STL源码剖析》 侯捷 P115-128