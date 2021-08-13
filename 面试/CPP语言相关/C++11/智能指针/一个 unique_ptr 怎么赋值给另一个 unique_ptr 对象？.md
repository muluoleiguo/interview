借助 std::move() 可以实现将一个 unique\_ptr 对象赋值给另一个 unique\_ptr 对象，其目的是实现所有权的转移。



```
// A 作为一个类 
std::unique_ptr<A> ptr1(new A());
std::unique_ptr<A> ptr2 = std::move(ptr1);
```
