# RPC

为了实现服务可重用或者说系统间的交互调用、解耦

### RPC 要解决的三个问题

RPC要达到的目标：远程调用时，要能够像本地调用一样方便，让调用者感知不到远程调用的逻辑。

- **Call ID映射**。我们怎么告诉远程机器我们要**调用哪个函数呢**？在本地调用中，函数体是直接通过函数指针来指定的，我们调用具体函数，编译器就自动帮我们调用它相应的函数指针。但是在远程调用中，是无法调用函数指针的，因为两个进程的地址空间是完全不一样。所以，在RPC中，**所有的函数都必须有自己的一个ID**。这个ID在所有进程中都是唯一确定的。客户端在做远程过程调用时，必须附上这个ID。然后我们还需要在客户端和服务端分别维护一个 {函数 <--> Call ID} 的对应表。两者的表不一定需要完全相同，但相同的函数对应的Call ID必须相同。当客户端需要进行远程调用时，它就查一下这个表，找出相应的Call ID，然后把它传给服务端，服务端也通过查表，来确定客户端需要调用的函数，然后执行相应函数的代码。
- **序列化和反序列化**。客户端怎么把参数值传给远程的函数呢？在本地调用中，我们只需要把参数压到栈里，然后让函数自己去栈里读就行。但是在远程过程调用时，客户端跟服务端是不同的进程，**不能通过内存来传递参数**。甚至有时候客户端和服务端使用的都**不是同一种语言**（比如服务端用C++，客户端用Java或者Python）。这时候就需要客户端把参数先转成一个字节流，传给服务端后，再把字节流转成自己能读取的格式。这个过程叫序列化和反序列化。同理，从服务端返回的值也需要序列化反序列化的过程。
- **网络传输**。远程调用往往是基于网络的，客户端和服务端是通过网络连接的。所有的数据都需要通过网络传输，因此就需要有一个网络传输层。网络传输层需要把Call ID和序列化后的参数字节流传给服务端，然后再把序列化后的调用结果传回客户端。只要能完成这两者的，都可以作为传输层使用。因此，它所使用的协议其实是不限的，能完成传输就行。尽管大部分RPC框架都使用TCP协议，但其实UDP也可以，而gRPC干脆就用了HTTP2。Java的Netty也属于这层的东西。

### 实现高可用RPC框架需要考虑到的问题

- 既然系统采用分布式架构，那一个服务势必会有多个实例，要解决**如何获取实例的问题**。所以需要一个服务注册中心，比如在Dubbo中，就可以使用Zookeeper作为注册中心，在调用时，从Zookeeper获取服务的实例列表，再从中选择一个进行调用；
- 如何选择实例呢？就要考虑负载均衡，例如dubbo提供了4种负载均衡策略；
- 如果每次都去注册中心查询列表，效率很低，那么就要加缓存；
- 客户端总不能每次调用完都等着服务端返回数据，所以就要支持异步调用；
- 服务端的接口修改了，老的接口还有人在用，这就需要版本控制；
- 服务端总不能每次接到请求都马上启动一个线程去处理，于是就需要线程池；

#### ` ProtoBuf ` 与  `json ` 的优缺点对比

+ `json` 没有类型，而 `protobuff ` 是具有类型的，更好的融入编程语言

+ `json`是文本传输，`protobuff` 是二进制传输，后者解析速度更加快，序列化数据非常简洁、紧凑。

#### 为什么需要序列化

序列化就是将一个结构对象转换成字节流的过程。那为什么需要序列化？

+ 为了跨网络传输
+ 持久化存储

即使不序列化，依然可以传输，即结构对象本身也可以跨网络传输和持久化存储。

- 序列化所做的工作除了将数据以二进制存入本地外，还要提供筛选数据，防止重复存储等功能。但是如果直接赋值内存中的数据，肯定达不到筛选数据，防止重复存储等功能。
- 跨平台、或者跨语言时，这个序列化方式就显得更重要了。例如，可以将 `java` 对象序列化成 `xml`、 `json` 形式。这样即使是 `python` 等非`java`语言都可以直接使用这个xml 或者json 对象得到自己需要的信息了

我对序列化的理解就是 **序列化使得对象信息更加普通化，可读化**（`xml`、`json`）。这样就可以使得别的进程，别的语言，别的平台都能够知道这个对象信息，从而保证了**对象信息的持久化**

#### [`RPC` 设计 ](https://www.yisu.com/zixun/8540.html)

![](image/Rpc框架.jpg)

+ 网络传输
+ 协议编码
+ 序列化与反序列化的设计与考量，比如兼容性。
+ 远程接口的代理实现
+ 服务负载均衡、容错

`Rpc` 的核心功能

![](image/Rpc核心.jpg)

下面分别介绍核心 RPC 框架的重要组成：

- `Client`：服务调用方。
- `Client Stub`：存放服务端地址信息，将客户端的请求参数数据信息打包成网络消息，再通过网络传输发送给服务端。
- `Server Stub`：接收客户端发送过来的请求消息并进行解包，然后再调用本地服务进行处理。
- `Server`：服务的真正提供者。
- `Network Service`：底层传输，可以是 TCP 或 HTTP。

在 `client/server stub` 部分进行编码与解码、序列化与反序列化。在 `server stub` 中一般会建立一个哈希表，`call Id` 与对应的函数建立映射关系。这个`call Id`可以是字符串，也是可以是数字，用来标识对应的函数。 

### Rpc底层网络库的模型部分

本质上是个 多个 `EventLoop` 线程  + 线程池。前者用于处理IO事件，后者主要用于处理计算密集型。线程池可以自己选择，如果当前任务计算量比较大可以交由线程池进行计算，不大的可以直接在 `EventLoop` 线程中计算。

![](image/reacotr_2.jpg)



### 又到了力推brpc的时候了。

看之前建议先搞懂什么是RPC

然后去gayhub上下一个这个

[apache/incubator-brpcgithub.com](github.com/apache/incubator-brpc)

这个很长，不用全看。

学生的话，着重关注**task_group.cpp，bthread.cpp，futex.cpp，socket.cpp，channel.cpp**这些文件就好了，上面是重点。如果有能力建议再看下bvar的实现和bthread_t的使用。

以上，几乎涵盖了linux服务器端c++程序员所需要的全部知识。包括内存管理(RaII，一二级内存池，内存对齐)。事件模型(单轮询，多轮询，Reactor)，进程控制(创建销毁，M:N调度模型，Work_stealing工作方式，原地切换上下文)。网络编程(epoll)。进程同步(从这我第一次知道futex)，中间还介绍了许多优化方式，比如线程本地变量，比如怎么写wait free的程序，比如在写多读少的场景下转移竞争。

整个项目采用c++11编写。除了task_group的用户态切换(/狗头)

readme好好看一遍，讲了各类编程问题下的陷阱。而且里面介绍的熔断策略，负载均衡，雪崩预防策略很好读懂。也有很大的实用价值。

看完以后就可以去参加校招面试了，横扫国内互联网不成问题



然后从应用方面看，brpc目前被应用于百度公司内部各种核心业务上，据github上的资料，包括：高性能计算和模型训练和各种索引和排序服务。



那么b函数是如何“远过程调用”(RPC) a函数的呢？一般a函数对应的进程会开放一个网络端口，它接受某种协议（比如HTTP）的请求，然后把结果打包成对应的协议格式返回。b函数所在的进程则发起该请求，然后接收返回结果。

但是这种设计无意增加了代码开发者的工作量。因为本来就一个本地函数调用就能解决问题，现在却需要：编写a函数对应的服务、编写b访问网络的逻辑。这其中掺杂了网络、数据序列化等方面知识，开发难度直线上升。

大家开始想办法，如果我们能降低上述开发难度，让开发者不需要懂网络编程、不需要懂协议解析，就像写本地调用代码一样做开发就好了。于是rpc框架就被研发出来了，市面上的google出品的grpc、facebook出品的thrift以及本文介绍的百度出品的brpc就是这类产品。

## 易用性

​    以brpc为例，我们看一个远过程调用是如何被调用的

```c++
#include <brpc/channel.h>
#include "echo.pb.h"
  
    ……
    brpc::Channel channel;
    brpc::ChannelOptions options;
    // 设置超时、协议等信息
    ……
 
    example::EchoRequest request;
    example::EchoResponse response;
    
    // 设置参数
    request.set_message("hello world");
 
    // 设置调用桩
    example::EchoService_Stub stub(&channel);  
    brpc::Controller cntl;  
    // 发起调用
    stub.Echo(&cntl, &request, &response, NULL);
 
    // 检测并分析结果
    ……
```

可以见得，这段代码内容比较好的隐藏了网络知识——本地调用也存在超时和协议的概念。我们就像调用本地过程一样调用了Echo方法。

​    相应的远过程调用的远端——服务端代码如下

```c++
#include <brpc/server.h>
#include "echo.pb.h"
……
namespace example {
class EchoServiceImpl : public EchoService {
public:
    EchoServiceImpl() {};
    virtual ~EchoServiceImpl() {};
    virtual void Echo(google::protobuf::RpcController* cntl_base,
                      const EchoRequest* request,
                      EchoResponse* response,
                      google::protobuf::Closure* done) {
        brpc::ClosureGuard done_guard(done);
 
        brpc::Controller* cntl =
            static_cast<brpc::Controller*>(cntl_base);
 
        // Fill response.
        response->set_message(request->message());
    }
};
}  // namespace example
 
int main(int argc, char* argv[]) {
    // Parse gflags. We recommend you to use gflags as well.
    GFLAGS_NS::ParseCommandLineFlags(&argc, &argv, true);
 
    // Generally you only need one Server.
    brpc::Server server;
 
    // Instance of your service.
    example::EchoServiceImpl echo_service_impl;
 
    if (server.AddService(&echo_service_impl, 
                          brpc::SERVER_DOESNT_OWN_SERVICE) != 0) {
        LOG(ERROR) << "Fail to add service";
        return -1;
    }
 
    // Start the server.
    brpc::ServerOptions options;
    options.idle_timeout_sec = FLAGS_idle_timeout_s;
    if (server.Start(FLAGS_port, &options) != 0) {
        LOG(ERROR) << "Fail to start EchoServer";
        return -1;
    }
 
    // Wait until Ctrl-C is pressed, then Stop() and Join() the server.
    server.RunUntilAskedToQuit();
    return 0;
}
```

 我们关注于EchoServiceImpl的实现。它主要暴露了Echo方法，我们只要填充它的业务就行了，而main函数中的套路是固定的。

​    可以见得使用rpc框架大大降低了我们开发的难度。