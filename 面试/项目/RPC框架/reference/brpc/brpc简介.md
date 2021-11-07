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