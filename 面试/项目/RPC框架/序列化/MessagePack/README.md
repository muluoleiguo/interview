### MessagePack

它也是一种轻量级的数据交换格式，与 JSON 的不同之处在于它不是纯文本，而是二进制。

由于二进制这个特点，MessagePack 也得到了广泛的应用，著名的有 Redis

但 MessagePack 的设计理念和 JSON 是完全不同的，它没有定义 JSON 那样的数据结构，而是比较底层，只能对基本类型和标准容器序列化 / 反序列化，需要你自己去组织、整理要序列化的数据。

和 JSON for Modern C++ 一样，**msgpack-c** 也是仅头文件的库（head only），只要包含一个头文件“msgpack.hpp”就行了，不需要额外的编译链接选项.

我拿 vector 容器来举个例子，调用 pack() 函数序列化为 MessagePack 格式：

```c++
vector<int> v = {1,2,3,4,5};              // vector容器

msgpack::sbuffer sbuf;                    // 输出缓冲区
msgpack::pack(sbuf, v);                   // 序列化
```

从代码里你可以看到，它的用法不像 JSON 那么简单直观，**必须同时传递序列化的输出目标和被序列化的对象。**

输出目标 sbuffer 是个简单的缓冲区，你可以把它理解成是对字符串数组的封装，和vector很像，也可以用 data() 和 size() 方法获取内部的数据和长度。

```c++
cout << sbuf.size() << endl;            // 查看序列化后数据的长度
```



除了 sbuffer，你还可以选择另外的 zbuffer、fbuffer。它们是压缩输出和文件输出，和 sbuffer 只是格式不同，用法是相同的，所以后面我就都用 sbuffer 来举例说明。

MessagePack 反序列化的时候略微麻烦一些，要用到函数 unpack() 和两个核心类：object_handle 和 object。

函数 unpack() 反序列化数据，得到的是一个 object_handle，再调用 get()，就是 object：

```c++
auto handle = msgpack::unpack(          // 反序列化
            sbuf.data(), sbuf.size());  // 输入二进制数据
auto obj = handle.get();                // 得到反序列化对象
```

这个 object 是 MessagePack 对数据的封装，但你不能直接使用，必须知道数据的原始类型，才能转换还原：

```c++
vector<int> v2;                       // vector容器
obj.convert(v2);                      // 转换反序列化的数据

assert(std::equal(                    // 算法比较两个容器
      begin(v), end(v), begin(v2)));
```

因为 MessagePack 不能直接打包复杂数据，所以用起来就比 JSON 麻烦一些，你必须自己把数据逐个序列化，连在一起才行。

好在 MessagePack 又提供了一个 packer 类，可以实现串联的序列化操作，简化代码：

```c++
msgpack::sbuffer sbuf;                         // 输出缓冲区
msgpack::packer<decltype(sbuf)> packer(sbuf);  // 专门的序列化对象

packer.pack(10).pack("monado"s)                // 连续序列化多个数据
      .pack(vector<int>{1,2,3});
```

对于多个对象连续序列化后的数据，反序列化的时候可以用一个偏移量（offset）参数来同样连续操作：

```c++
for(decltype(sbuf.size()) offset = 0;          // 初始偏移量是0
    offset != sbuf.size();){                   // 直至反序列化结束

    auto handle = msgpack::unpack(            // 反序列化
            sbuf.data(), sbuf.size(), offset);  // 输入二进制数据和偏移量
    auto obj = handle.get();                  // 得到反序列化对象
}
```

但这样还是比较麻烦，能不能像 JSON 那样，直接对类型序列化和反序列化呢？

MessagePack 为此提供了一个特别的宏：MSGPACK_DEFINE，把它放进你的类定义里，就可以像标准类型一样被 MessagePack 处理。

下面定义了一个简单的 Book 类：

```c++
class Book final                       // 自定义类
{
public:
    int         id;
    string      title;
    set<string> tags;
public:
    MSGPACK_DEFINE(id, title, tags);   // 实现序列化功能的宏
};
```

它可以直接用于 pack() 和 unpack()，基本上和 JSON 差不多了：

```c++
Book book1 = {1, "1984", {"a","b"}};  // 自定义类

msgpack::sbuffer sbuf;                    // 输出缓冲区
msgpack::pack(sbuf, book1);              // 序列化

auto obj = msgpack::unpack(              // 反序列化
      sbuf.data(), sbuf.size()).get();   // 得到反序列化对象

Book book2;
obj.convert(book2);                      // 转换反序列化的数据

assert(book2.id == book1.id);
assert(book2.tags.size() == 2);
cout << book2.title << endl;
```

使用 MessagePack 的时候，你也要注意数据不完整的问题，必须要用 try-catch 来保护代码，捕获异常：

```c++
auto txt = ""s;                      // 空数据
try                                  // try保护代码
{
    auto handle = msgpack::unpack(   // 反序列化
        txt.data(), txt.size());
}
catch(std::exception& e)            // 捕获异常
{
    cout << e.what() << endl;
}
```

