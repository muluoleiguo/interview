### priority_queue

priority_queue 也是一个容器适配器。上一讲没有和其他容器适配器一起讲的原因就在于它用到了比较函数对象（默认是 less）。它和 stack 相似，支持 push、pop、top 等有限的操作，但容器内的顺序既不是后进先出，也不是先进先出，而是（部分）排序的结果。在使用缺省的 less 作为其 Compare 模板参数时，最大的数值会出现在容器的“顶部”。如果需要最小的数值出现在容器顶部，则可以传递 greater 作为其 Compare 模板参数。

```c++

#include <functional>  // std::greater
#include <iostream>    // std::cout/endl
#include <memory>      // std::pair
#include <queue>       // std::priority_queue
#include <vector>      // std::vector
#include "output_container.h"

using namespace std;

int main()
{
  priority_queue<
    pair<int, int>,
    vector<pair<int, int>>,
    greater<pair<int, int>>>
    q;
  q.push({1, 1});
  q.push({2, 2});
  q.push({0, 3});
  q.push({9, 4});
  while (!q.empty()) {
    cout << q.top() << endl;
    q.pop();
  }
}
```

输出：

```
(0, 3)
(1, 1)
(2, 2)
(9, 4)
```

