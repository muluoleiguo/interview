面试高频指数：★★★☆☆

std::move() 函数原型：


```c++
template <typename T>
typename remove_reference<T>::type&& move(T&& t)
{
	return static_cast<typename remove_reference<T>::type &&>(t);
}
```

说明：引用折叠原理

右值传递给上述函数的形参 T&& 依然是右值，即 T&& && 相当于 T&&。
左值传递给上述函数的形参 T&& 依然是左值，即 T&& & 相当于 T&。
小结：通过引用折叠原理可以知道，move() 函数的形参既可以是左值也可以是右值。

`remove_reference` 具体实现：


```c++
//原始的，最通用的版本
template <typename T> struct remove_reference{
    typedef T type;  //定义 T 的类型别名为 type
};
 
//部分版本特例化，将用于左值引用和右值引用
template <class T> struct remove_reference<T&> //左值引用
{ typedef T type; }
 
template <class T> struct remove_reference<T&&> //右值引用
{ typedef T type; }   
  
//举例如下,下列定义的a、b、c三个变量都是int类型
int i;
remove_refrence<decltype(42)>::type a;             //使用原版本，
remove_refrence<decltype(i)>::type  b;             //左值引用特例版本
remove_refrence<decltype(std::move(i))>::type  b; //右值引用特例版本
```

举例转化过程：

1. `std::move(var) => std::move(int&& &) `=> 折叠后 `std::move(int&)`

2. 此时：T 的类型为` int&，typename remove_reference<T>::type 为 int`，这里使用 remove_reference 的左值引用的特例化版本

3. 通过 static_cast 将 int& 强制转换为 int&&

```c++
int var = 10; 

//整个std::move被实例化如下
string&& move(int& t) 
{
    return static_cast<int&&>(t); 
}
```

### 总结：

std::move() 实现原理：

1. 利用引用折叠原理将右值经过 T&& 传递类型保持不变还是右值，而左值经过 T&& 变为普通的左值引用，以保证模板可以传递任意实参，且保持类型不变；
2. 然后通过 remove_refrence 移除引用，得到具体的类型 T；
3. 最后通过 static_cast<> 进行强制类型转换，返回 T&& 右值引用。

