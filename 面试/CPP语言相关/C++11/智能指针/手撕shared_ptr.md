* 智能指针类将一个计数器与类指向的对象相关联，引用计数跟踪该类有多少个对象共享同一指针。
* 每次创建类的新对象时，初始化指针并将引用计数置为1；
* 当对象作为另一对象的副本而创建时，拷贝构造函数拷贝指针并增加与之相应的引用计数；
* 对一个对象进行赋值时，赋值操作符减少左操作数所指对象的引用计数（如果引用计数为减至0，则删除对象），并增加右操作数所指对象的引用计数；
* 调用析构函数时，构造函数减少引用计数（如果引用计数减至0，则删除基础对象）。
* 所有的智能指针都会重载 -> 和 * 操作符。智能指针还有许多其他功能，比较有用的是自动销毁。


简单的实现了shared_ptr，主要包括如下成员函数：

1. 构造函数
2. 析构函数
3. 拷贝构造函数
4. 拷贝赋值函数
5. 移动构造
6. operator * ()
7. operator -> ()


    -> 和 . 优先级1 和() 一个级别的
    成员选择（指针）
    对象指针->成员名
    



这个实现不太对劲

```c++
template<typename T>
class SharedPtr {
private:
    T* m_ptr;
    int* m_count; //reference couting 指向引用计数的指针
public:
    SharedPtr(T* ptr = nullptr) : m_ptr(ptr) {
        if (m_ptr == nullptr) 
            m_count = new int(0);
        else 
            m_count = new int(1);                 
    }
    // 这种意外的情况真的可能发生吗？
    SharedPtr(const SharedPtr& ptr) {
        if (this != &ptr) {
            this->m_ptr = ptr.m_ptr;
            this->m_count = ptr.m_count;
            (*this->m_count)++;
        }
    }
    
    // 移动构造
    SharedPtr(SharedPtr&& rhs) {
        this->m_ptr = rhs.m_ptr;
        this->m_count = rhs.m_count;
        rhs.m_ptr = nullptr;
        rhs.m_count = nullptr;
    }
    // 移动赋值
    SharedPtr& operator=(SharedPtr&& rhs) {
        if (m_count != rhs.m_count)
            swap(this, rhs);
        return *this;
    }
    
    SharedPtr& operator=(const SharedPtr& rhs) {
        if (this->m_ptr == rhs.m_ptr) {
            return *this;
        }
        // 考虑初始化为nullptr的SharedPtr调用operator=
        // 因为this->m_count在堆上，不管不好
        // 左计数减1
        (*this->m_count)--;
        // nullptr 的 count 为 -1 ， 但是delete nullptr可以
        if (*this->m_count <= 0) {
            delete this->m_ptr;
            delete this->m_count;
        }
        // 右计数加1
        this->m_ptr = rhs.m_ptr;
        this->m_count = rhs.m_count;
        (*this->m_count)++;
        return *this;
    }
    
    ~SharedPtr() {
        (*this->m_count)--;
        if (*this->m_count <= 0) {
            delete this->m_ptr;
            delete this->m_count;
        }            
    }
    T& operator*() {
        assert(this->m_ptr != nullptr);
        return *(this->m_ptr);
    }
    
    T& operator->() {
        assert(this->m_ptr != nullptr);
        return this->m_ptr;
    }
    
    int useCount const() {
        return *this->m_count;
    }
    
    bool unique() const {
        return (*this->m_count) == 1;
    }
}
int main() {
	{

		SharedPtr<int> sp(nullptr);
		std::cout << sp.use_count() << std::endl;  //0
		SharedPtr<int> sp2(new int(10));
		sp = sp2;
		std::cout << sp.use_count() << std::endl;  //2

	}

	system("pause");
	return 0;
}
/*
0
operator=
operator= 过程释放
2
释放
*/


//int main() {
//	{
//		//只初始化了两次
//		SharedPtr<int> sp(new int(10));
//		SharedPtr<int> sp2(sp);         //SharedPtr(const SharedPtr& ptr)
//		std::cout << sp.use_count() << std::endl;   //2
//		SharedPtr<int> sp3(new int(20));
//		sp2 = sp3;
//		std::cout << sp.use_count() << std::endl;   //1
//		std::cout << sp3.use_count() << std::endl;  //2
//
//		SharedPtr<int> sp4(nullptr);
//		std::cout << sp4.use_count() << std::endl;  //0
//	}
//	system("pause");
//	return 0;
//}

/*
SharedPtr(const SharedPtr& ptr)
2
operator=
1
2
0
释放    //sp4
释放    //sp2 或者说 sp3
释放    //sp
*/

```
1. 初始化`SharedPtr<int> sp(new int(10))`;是什么方式
直接调用构造函数啦，这个傻缺

```

//初始化方式1
std::shared_ptr<int> sp1(new int(123));
 
//初始化方式2
std::shared_ptr<int> sp2;
sp2.reset(new int(123));
 
//初始化方式3
std::shared_ptr<int> sp3;
sp3 = std::make_shared<int>(123);
```


2. 调用析构函数的顺序是怎样的

主要思想：
（1）全局变量在main之前构造；
（2）局部静态变量在第一次执行到定义语句时构造；
（3）局部非静态变量每次进入作用域都会构造，每次退出作用域析构；
（3）全局变量和静态变量都在main之后析构，同时析构次序与构造次序相反

如果是局部非静态变量，则在建立对象的时候调用其构造函数，函数结束时调用析构函数，调用析构函数的顺序和调用构造函数的**顺序相反**（可以想象成入栈出栈的结构）。


```
class A {
public:
    A(int a) : _a(a) {
        std::cout<<"A()"<<_a<<std::endl;
    }

    A(const A& classA) {
        std::cout<<"A(const A& classA)"<<std::endl;
        this->_a = classA._a;
    }

    A& operator = (const A& classA) {
        std::cout<<"operator ="<<_a<<std::endl;
        if(&classA == this) return *this;
        this->_a = classA._a;
        return *this;

    }
    ~A() {
        std::cout<<"~A()"<<_a<<std::endl;
    }

    void SetA(int a) {
        _a = a;
    }
private:
    int _a;
};

void f() {
        A a = A(1);
        A b(a);
        b.SetA(2);
        A c = b;
        c.SetA(3);

        A d(-1);
        d = c;
        d.SetA(4);
}
int main() {
    f();
    system("pause");
	return 0;

}
```


    A()1
    A(const A& classA)
    A(const A& classA)
    A()-1
    operator =-1
    ~A()4
    ~A()3
    ~A()2
    ~A()1