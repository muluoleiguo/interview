## HTTP

**解析**
对于http请求的解析，使用HttpParser，主要是使用有限状态机 + 正则来进行字符串匹配。

### 初期设计思路

实际上是一个 map[string]Handler，是请求的url路径和该url路径对于的一个处理函数的映射关系。这个实现比较简单，有一些缺点：

1. 不支持参数设定，例如/user/:uid 这种泛型类型匹配
2. 无法很友好的支持REST模式，无法限制访问方法（POST，GET等）
3. 也不支持正则

**路由**

这里的设计参考了golang的mux包，

`mux` 是一个用来执行http请求的路由和分发的第三方扩展包。

`mux` 其名称来源于`HTTP request multiplexer`，类似于官方包`http.ServeMux`，`mux.Router`将会定义一个路由列表，其中每一个路由都会定义对应的请求url，及其处理方法。

`mux`有以下优势：（终究是搬起石头砸了自己的脚）

- 实现了标准的`http.Handler`接口，所以可以与`net/http`标准库结合使用，非常轻量；
- 可以根据请求的主机名、路径、路径前缀、协议、HTTP 首部、查询字符串和 HTTP 方法匹配处理器，还可以自定义匹配逻辑；
- 可以在主机名、路径和请求参数中使用变量，还可以为之指定一个正则表达式；
- 可以传入参数给指定的处理器让其构造出完整的 URL；
- 支持路由分组，方便管理和维护。



首先Router下有多个Route，每个Route下又有多个Matcher。

每个Matcher都对应了一条匹配条件，比如要求请求的方法为GET。当一个请求匹配了Route下的所有条件，即匹配成功这个Route，就可以得到该Route的处理函数来处理Request了。如果没有匹配的Route，那么就使用默认的处理函数，即返回404页面。

Matcher的匹配主要是通过正则表达式来进行，用户可以利用正则表达式来取得一些参数。

用户可以根据需要设置路由及匹配条件，目前支持URL、请求参数、请求头及请求方法的匹配，并且可以从中提取出参数。
一个请求必须满足所有条件才能匹配这个路由，得到它的Handler。

首先要设置相应的路由处理函数,该函数由用户实现，如果要实现静态资源访问，则需使用HttpServer的文件处理函数,该函数会将访问映射到用户指定的路径。



### 使用实例：



```go
r := mux.NewRouter()
//普通路由
r.HandleFunc("/", IndexHandler)
r.HandleFunc("/products", ProductsHandler)

func IndexHandler(w http.ResponseWriter, r *http.Request) {
	w.WriteHeader(http.StatusOK)
	fmt.Fprintf(w, "hello world")
}

func ProductsHandler(w http.ResponseWriter, r *http.Request) {
	w.WriteHeader(http.StatusOK)
	fmt.Fprintf(w, "hello, Products")
}
```



