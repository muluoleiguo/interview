### new：
指我们在C++里通常用到的运算符，比如`A* a = new A; ` 对于new来说，有new和::new之分，前者位于std

### operator new()：
指对new的重载形式，它是一个**函数**，并不是运算符。对于operator new来说，分为全局重载和类重载，全局重载是`void* ::operator new(size_t size)`，在类中重载形式 `void* A::operator new(size_t size)`。还要注意的是这里的`operator new()`完成的操作一般只是分配内存，事实上系统默认的全局`::operator new(size_t size)`也只是调用`malloc`分配内存，并且返回一个`void*`指针。而构造函数的调用(如果需要)是在new运算符中完成的

## new和operator new之间的关系

`A* a = new A`；我们知道这里分为两步：
1. 分配内存，
2. 调用A()构造对象。

事实上，分配内存这一操作就是由 `operator new(size_t)`来完成的，如果类A重载了`operator new`，那么将调用`A::operator new(size_t )`，如果没有重载，就调用`::operator new(size_t )`，全局new操作符由C++默认提供。
因此前面的两步也就是：
1. 调用operator new 
2. 调用构造函数。


（1）**new ：不能被重载，其行为总是一致的。**它先调用operator new分配内存，然后调用构造函数初始化那段内存。

new 操作符的执行过程：
1. 调用operator new分配内存 ；
2. 调用构造函数生成类对象；
3. 返回相应指针。

（2）`operator new`：要实现不同的内存分配行为，应该重载operator new，而不是new。

operator new就像operator + 一样，是可以重载的。如果类中没有重载operator new，那么调用的就是全局的::operator new来完成堆的分配。同理，operator new[]、operator delete、operator delete[]也是可以重载的。