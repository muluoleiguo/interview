既然是HTTP Request，先看看格式

我们的类里面私有成员变量如下

```c++
private:
    // 请求方法
    Method method_;
    // Http版本
    Version version_;
    // 路径
    std::string path_;
    // 参数
    std::unordered_map<std::string, std::string> querys_;
    // 头部字段
    std::unordered_map<std::string, std::string> headers_;
    // 接收时间
    Timestamp receive_time_;
```

现在看看HTTP请求报文对应的是什么

HTTP 的请求报文分为三个部分：**请求行、请求头、请求体**



## 1. 请求行

请求行（Request line）分为三个部分：**请求方法、请求地址和协议版本**

#### 1.1 请求方法 对应 Method method_;

HTTP/1.1 协议中共定义了八种方法（也叫“动作”）来以不同的方式操作指定的资源
其中，最常见的是 GET 和 POST 方法，如果是 RESTful 接口的话一般会用到 PUT、DELETE、GET、POST(RESTful API 的介绍有文章了，自己去看)

#### 1.2 请求地址 对应 string path_;

对应的URL地址,略

#### 1.3协议版本 对应 Version version_;

协议名称及版本号，略

## 2 请求头 对应 unordered_map<string, string> headers_;

请求头可用于传递一些附加信息，格式为：`键: 值`，注意冒号后面有一个空格：

#### Accept 

请求报文可通过一个“Accept”报文头属性告诉服务端 客户端接受什么类型的响应。 
如下报文头相当于告诉服务端，俺客户端能够接受的响应类型仅为纯文本数据啊，你丫别发其它什么图片啊，视频啊过来，那样我会歇菜的~~~

```
Accept:text/plain 
```

Accept属性的值可以为一个或多个MIME类型的值（描述消息内容类型的因特网标准， 消息能包含文本、图像、音频、视频以及其他应用程序专用的数据）

#### cookie

客户端的Cookie就是通过这个报文头属性传给服务端的哦！如下所示：

```
Cookie: $Version=1; Skin=new;jsessionid=5F4771183629C9834F8382E23 
```

服务端是怎么知道客户端的多个请求是隶属于一个Session呢？注意到后台的那个jsessionid = 5F4771183629C9834F8382E23木有？原来就是通过HTTP请求报文头的Cookie属性的jsessionid的值关联起来的！（当然也可以通过重写URL的方式将会话ID附带在每个URL的后面哦）。

#### Referer

表示这个请求是从哪个URL过来的，假如你通过google搜索出一个商家的广告页面，你对这个广告页面感兴趣，鼠标一点发送一个请求报文到商家的网站，这个请求报文的Referer报文头属性值就是http://www.google.com。

#### Cache-Control

对缓存进行控制，如一个请求希望响应返回的内容在客户端要被缓存一年，或不希望被缓存就可以通过这个报文头达到目的。

#### If-Modified-Since

用于确定某个资源是否被更改过，没有更改过就从缓存中读取

#### Content-Type

有application/x-www-form-unlencoded 等，下面会说有什么用

## 空行\r\n

每行分隔符号\r\n ,解决TCP黏包问题（可能吧我也不确定）
明确请求体和请求头之间有\r\n空行分隔


## 4.请求体

根据应用场景的不同，HTTP 请求的请求体有三种不同的形式

#### 第一种：

移动开发者常见的，请求体是任意类型的，服务器不会解析请求体，请求体的处理需要自己解析，如 POST JSON 的时候就是这类

#### 第二种：

在请求头中有  Content-Type: application/x-www-form-unlencoded\r\n

第二种和第三种都有固定的格式，是服务器端开发人员最先了解的两种。这里的格式要求就是 URL 中 Query String 的格式要求：多个键值对之间用&连接，键与值之间用=连接，且只能用 ASCII 字符，非 ASCII 字符需使用UrlEncode编码

请求体中 ： key1=value1&key2=value2....

#### 第三种：(这个不熟，将就看看就是)

Content-Type: mutipart/form-data;boundary={boundary}\r\n

第三种请求体被分成多个部分，**文件上传**时会被使用，这种格式最先是被用于邮件传输中，每个字段/文件都被 boundary（Content-Type中指定的）分成单独的段，每段以--加 boundary 开头，然后是该段的描述头，描述头之后空一行接内容，请求结束的标识为 boundary 后面加--

区分是否被当成文件的关键是 Content-Disposition 是否包含 filename，因为文件有不同的类型，所以还要使用 Content-Type 指示文件的类型，如果不知道是什么类型取值可以为 application/octet-stream 表示文件是一个二进制的文件，如果不是文件则 Content-Type 可以省略