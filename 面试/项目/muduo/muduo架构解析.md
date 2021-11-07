# muduo 架构解析

muduo是一个基于Reactor模式的C++网络库。它采用非阻塞I/O模型，基于事件驱动和回调。我们不仅可以通过muduo来学习linux服务端多线程编程，还可以通过它来学习C++11。
    Reactor是网络编程的一般范式。我们这里从reactor模式为出发点，根据Reactor模式的特点剖析muduo的架构设计。根据wiki的定义：

> The reactor design pattern is an event handling pattern for handling
> service requests delivered concurrently to a service handler by one or
> more inputs. The service handler then demultiplexes the incoming
> requests and dispatches them synchronously to the associated request
> handlers.

    我们可以知道，Reactor模式的基础是事件驱动，事件源可以有多个，并且会并发地产生事件。Reactor模式的核心是**一个事件分发器和多个事件处理器**，多个事件源向事件分发器发送事件，并要求事件分发器响应，reactor模式的设计难点也是在事件分发器，它必须能够有条不紊地把响应时间分派到合适的事件处理器中，保证事件处理的最小延迟。事件处理器主要是负责处理事件的业务逻辑，这是关系到具体事件的核心，因此和事件分发器不一样，它并不太具有一般性。

![这里写图片描述](https://img-blog.csdn.net/20170223162021284?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvU3dhcnR6MjAxNQ==/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)

    Reactor模式的特点可以很自然地应用到C/S架构中。在C/S架构的应用程序中，多个客户端会同时向服务端发送request请求。服务端接收请求，并根据请求内容处理请求，最后向客户端发送请求结果。这里，客户端就相当于事件源，服务端由事件分发器和事件处理器组成。分发器的任务主要是解析请求和将解析后的请求发送到具体的事件处理器中。

![从Reactor模式到C/S架构](https://img-blog.csdn.net/20170223162052222?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvU3dhcnR6MjAxNQ==/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)

    从技术的层面来说，怎么把“事件”这个概念放到“请求”上，也就是怎么样使得请求到来可以触发事件，是一个难点。从设计的层面上来说，怎么样分发事件使得响应延迟最小，并保持高可扩展性是难点(架构能够较好地适应各种事件的处理和事件数量的变化)。对于技术层面，linux上的解决方案是：epoll，select等。而设计层面，muduo提供了较好的解决方案。
    Muduo的基础设施是epoll，并在此基础上实现了one-thread-one-loop和thread-pool设计方案。也就是将事件处理器设置成线程池，每个线程对应一个事件处理器；因为事件处理器主要处理的是I/O事件，而且每个事件处理器可能会处理一个连接上的多个I/O事件，而不是处理完一个事件后直接断开，因此muduo选择每个事件处理器一个event-loop。这样，连接建立后，对于这条连接上的所有事件全权由它的事件处理器在event-loop中处理。
    我们可以根据上面的reactor架构图，简单地绘制出muduo的架构图：

![muduo架构图](https://img-blog.csdn.net/20170223162137832?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvU3dhcnR6MjAxNQ==/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)

    如图所示，客户端首先和服务端建立连接，如图橙色线所示。建立连接之后将这个连接分发到具体的Eventloopthread中(所有的eventloopthread由server中的一个eventloopthreadpool线程池管理)，这部分主要由server中的Acceptor完成。后续client就不再和Acceptor发生关系了。因此可以看出，建立连接之后，client直接和Eventloopthread关联，不再经过Acceptor。
    由于连接本身也是一个事件，因此Acceptor的工作是等待事件和分发事件，因此它也是在一个eventloop中。
    下面我们看一下Tcpserver类成员，其实类中的*loop_指向的其实就是Acceptor所在的eventloop。因此Acceptor的eventloop并不是存在于eventloopthreadpool中的。不过后面我们会看到，这个eventloop结构也是会传入到eventloopthreadpool结构中，由eventloopthreadpool的baseloop标识，这主要是为了管理server中的所有eventloop方便。

![这里写图片描述](https://img-blog.csdn.net/20170223162233063?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvU3dhcnR6MjAxNQ==/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)

    至此我们便介绍完了muduo中的Tcpserver架构。后面我们将从此展开，深入到具体细节中。比如怎么管理eventloopthread，建立好的连接怎么放入到eventloopthread中，以及eventloopthread怎么管理和客户端直接的I/O连接，eventloop怎么管理各个事件的处理逻辑等等。最后我们还将介绍muduo实现的一些架构之外的技术细节，比如缓冲区管理，日志系统，定时器管理等等。
