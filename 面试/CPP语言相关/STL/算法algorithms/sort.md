
```
template< class RandomIt, class Compare >
void sort( RandomIt first, RandomIt last, Compare comp );
```
## Compare comp
comparison function object (i.e. an object that satisfies the requirements of Compare) which **returns true if the first argument is ==less than==** (i.e. is ordered before) the second.

The signature of the comparison function should be equivalent to the following:


```
bool cmp(const Type1 &a, const Type2 &b);
```


While the signature does not need to have const &, the function must not modify the objects passed to it and must be able to accept all values of type (possibly const) Type1 and Type2 regardless of value category (thus, Type1 & is not allowed, nor is Type1 unless for Type1 a move is equivalent to a copy (since C++11)).
The types Type1 and Type2 must be such that an object of type RandomIt can be dereferenced and then implicitly converted to both of them.

Example

```

#include <algorithm>
#include <functional>
#include <array>
#include <iostream>
#include <string_view>
 
int main()
{
    std::array<int, 10> s = {5, 7, 4, 2, 8, 6, 1, 9, 0, 3};
 
    auto print = [&s](std::string_view const rem) {
        for (auto a : s) {
            std::cout << a << ' ';
        }
        std::cout << ": " << rem << '\n';
    };
 
    std::sort(s.begin(), s.end());
    print("sorted with the default operator<");
 
    std::sort(s.begin(), s.end(), std::greater<int>());
    print("sorted with the standard library compare function object");
 
    struct {
        bool operator()(int a, int b) const { return a < b; }
    } customLess;
    std::sort(s.begin(), s.end(), customLess);
    print("sorted with a custom function object");
 
    std::sort(s.begin(), s.end(), [](int a, int b) {
        return a > b;
    });
    print("sorted with a lambda expression");
}
```

Output:

```
0 1 2 3 4 5 6 7 8 9 : sorted with the default operator<
9 8 7 6 5 4 3 2 1 0 : sorted with the standard library compare function object
0 1 2 3 4 5 6 7 8 9 : sorted with a custom function object
9 8 7 6 5 4 3 2 1 0 : sorted with a lambda expression
```
