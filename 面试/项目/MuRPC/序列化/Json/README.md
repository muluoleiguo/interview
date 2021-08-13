### JSON

JSON 是一种轻量级的数据交换格式，采用纯文本表示，所以是“human readable”，阅读和修改都很方便。

因为 JSON 本身就是个 KV 结构，很容易映射到类似 map 的关联数组操作方式。

不过我觉得，JSON 格式注重的是方便易用，在性能上没有太大的优势，所以一**般选择 JSON 来交换数据，通常都不会太在意性能**

推荐：[JSON for Modern C++](https://github.com/nlohmann/json)

仅需要包含一个头文件“json.hpp”

JSON for Modern C++ 使用一个 json 类来表示 JSON 数据，为了避免说的时候弄混，我给这个类起了个别名 json_t：

```c++
using json_t = nlohmann::json;
```

json_t 的序列化功能很简单，和标准容器 map 一样，用关联数组的“[]”来添加任意数据。

你不需要特别指定数据的类型，它会自动推导出恰当的类型。比如，连续多个“[]”就是嵌套对象，array、vector 或者花括号形式的初始化列表就是 JSON 数组，map 或者是花括号形式的 pair 就是 JSON 对象，非常自然：

```c++

json_t j;                                   // JSON对象

j["age"] = 23;                              // "age":23
j["name"] = "spiderman";                    // "name":"spiderman"
j["gear"]["suits"] = "2099";                // "gear":{"suits":"2099"}
j["jobs"] = {"superhero"};                  // "jobs":["superhero"]  

vector<int> v = {1,2,3};                   // vector容器
j["numbers"] = v;                          // "numbers":[1,2,3]

map<string, int> m =                       // map容器
    {{"one",1}, {"two", 2}};               // 初始化列表 
j["kv"] = m;                               // "kv":{"one":1,"two":2}
```

添加完之后，用成员函数 dump() 就可以序列化，得到它的 JSON 文本形式。默认的格式是紧凑输出，没有缩进，如果想要更容易阅读的话，可以加上指示缩进的参数：

```c++

cout << j.dump() << endl;         // 序列化，无缩进
cout << j.dump(2) << endl;        // 序列化，有缩进，2个空格
```

json_t 的反序列化功能同样也很简单，只要调用静态成员函数 parse() 就行，直接得到 JSON 对象，而且可以用 auto 自动推导类型：

```c++

string str = R"({                // JSON文本，原始字符串
    "name": "peter",
    "age" : 23,
    "married" : true
})";

auto j = json_t::parse(str);    // 从字符串反序列化
assert(j["age"] == 23);        // 验证序列化是否正确
assert(j["name"] == "peter");
```

json_t 使用异常来处理解析时可能发生的错误，如果你不能保证 JSON 数据的完整性，就要使用 try-catch 来保护代码，防止错误数据导致程序崩溃：

```c++

auto txt = "bad:data"s;        // 不是正确的JSON数据

try                             // try保护代码
{
    auto j = json_t::parse(txt);// 从字符串反序列化
}
catch(std::exception& e)        // 捕获异常
{
    cout << e.what() << endl;
}
```

JSON for Modern C++ 里还有很多高级用法，比如 SAX、BSON、自定义类型转换等。如果你需要这些功能，可以去看它的文档，里面写得都很详细。

