### 初识 constexpr

我们先来看一些例子：

```c++
int sqr(int n)
{
  return n * n;
}

int main()
{
  int a[sqr(3)];
}
```

想一想，这个代码合法吗？

看过之后，再想想这个代码如何？

```c

int sqr(int n)
{
  return n * n;
}

int main()
{
  const int n = sqr(3);
  int a[n];
}
```

还有这个？

```c++

#include <array>

int sqr(int n)
{
  return n * n;
}

int main()
{
  std::array<int, sqr(3)> a;
}
```

