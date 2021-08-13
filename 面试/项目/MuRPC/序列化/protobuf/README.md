### ProtoBuffer

PB 也是一种二进制的数据格式，但毕竟是工业级产品，所以没有 JSON 和 MessagePack 那么“轻”，相关的东西比较多，要安装一个预处理器和开发库，编译时还要链接动态库（-lprotobuf）：

```sh
apt-get install protobuf-compiler
apt-get install libprotobuf-dev

g++ protobuf.cpp -std=c++14 -lprotobuf -o a.out
```

**PB 的另一个特点是数据有“模式”（schema）**，必须要先写一个 IDL（Interface Description Language）文件，在里面定义好数据结构，只有预先定义了的数据结构，才能被序列化和反序列化。

这个特点既有好处也有坏处：一方面，接口就是清晰明确的规范文档，沟通交流简单无歧义；而另一方面，就是缺乏灵活性，改接口会导致一连串的操作，有点繁琐。

下面是一个简单的 PB 定义：

```c++

syntax = "proto2";                    // 使用第2版

package sample;                        // 定义名字空间

message Vendor                        // 定义消息
{
    required uint32     id      = 1;  // required表示必须字段
    required string     name    = 2;  // 有int32/string等基本类型
    required bool       valid   = 3;  // 需要指定字段的序号，序列化时用
    optional string     tel     = 4;  // optional字段可以没有
}
```

有了接口定义文件，需要再用 protoc 工具生成对应的 C++ 源码，然后把源码文件加入自己的项目中，就可以使用了：

```shell
protoc --cpp_out=. sample.proto       // 生成C++代码
```

由于 PB 相关的资料实在太多了，这里我就只简单说一下重要的接口：

* 字段名会生成对应的 has/set 函数，检查是否存在和设置值；
* IsInitialized() 检查数据是否完整（required 字段必须有值）；
* DebugString() 输出数据的可读字符串描述；
* ByteSize() 返回序列化数据的长度；
* SerializeToString() 从对象序列化到字符串；
* ParseFromString() 从字符串反序列化到对象；
* SerializeToArray()/ParseFromArray() 序列化的目标是字节数组。

下面的代码示范了 PB 的用法：

```c++

using vendor_t = sample::Vendor;        // 类型别名

vendor_t v;                             // 声明一个PB对象
assert(!v.IsInitialized());            // required等字段未初始化

v.set_id(1);                            // 设置每个字段的值    
v.set_name("sony");
v.set_valid(true);

assert(v.IsInitialized());             // required等字段都设置了，数据完整
assert(v.has_id() && v.id() == 1); 
assert(v.has_name() && v.name() == "sony");
assert(v.has_valid() && v.valid());

cout << v.DebugString() << endl;       // 输出调试字符串

string enc;
v.SerializeToString(&enc);              // 序列化到字符串 

vendor_t v2; 
assert(!v2.IsInitialized());
v2.ParseFromString(enc);               // 反序列化
```

虽然业界很多大厂都在使用 PB，但我觉得它真不能算是最好的，IDL 定义和接口都太死板生硬，还只能用最基本的数据类型，不支持标准容器，在现代 C++ 里显得“不太合群”，用起来有点别扭。

不过它后面有 Google“撑腰”，而且最近几年又有 gRPC“助拳”，所以很多时候也不得不用。

PB 的另一个缺点是官方支持的编程语言太少，通用性较差，最常用的 proto2 只有 C++、Java 和 Python。后来的 proto3 增加了对 Go、Ruby 等的支持，但仍然不能和 JSON、MessagePack 相比。