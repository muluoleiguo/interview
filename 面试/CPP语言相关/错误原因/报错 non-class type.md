
```
error: request for member 'push' in 's', which is of non-class type 'T_stack<int>()'|
```

因为我在main函数中使用了T_stack<int> s()导致的。

因为不带参数的构造函数，比如T_stack<T>()它可以是一个对象，也可是一个函数声明。但是c++编译器总是优先认为是一个函数声明，然后是对象。

我找了下有几种方法可以避免这种模糊性的。

1. 不使用()初始化对象，比如T_stack<int> s
2. ==使用花括号{}初始化对象==,比如T_stack<int> s{};
3. 使用赋值运算符和匿名默认构造的对象:T_stack<int> s = T_stack<int> {}。
