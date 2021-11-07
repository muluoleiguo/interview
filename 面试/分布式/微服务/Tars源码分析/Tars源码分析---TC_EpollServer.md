# Tars源码分析---TC_EpollServer

#### 前言

这个系列的文章主要分析腾讯的开源RPC框架Tars。Tars除了提供RPC服务之外，还配套了一整套的运营管理平台。它同时支持C++，java等多种编程语言。本系列主要是分析tars的cpp部分实现，涉及的版本是github上最近release的1.40版。主要是在源代码层面进行分析，内容包括网络IO，服务路由等方面。本章将分析Tars上服务的网络IO所基于的TC_EpollServer组件。TC_EpollServer实现了底层的网络字符流接收，以及对外暴露Adapter和Handle接口，以实现插入式的协议解析，服务处理逻辑等等。因此，通过以TC_EpollServer为入口，我们可以粗粒度地对tars服务的实现框架进行了解。


#### 架构

![TC_EpollServer架构](https://img-blog.csdn.net/2018061519292052?watermark/2/text/aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L1N3YXJ0ejIwMTU=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70)

如上图是TC_EpollServer的设计架构。它的由两种线程池构成：

1. IO_Thread Pool
2. Handler Pool

##### IO_Thread Pool

IO_thread pool负责处理网络事件，包括建立连接，网络数据传输等。它是基于经典的Epoll+Non-Blocking+多线程模型。线程池中的线程分为两种：Acceptor线程，负责监听TCP连接；以及监听有效数据传输的IO读写事件的线程。在目前的实现中，TC_EpollServer分配一个线程为acceptor，其余线程负责监听连接上的IO事件：

```c++
int  TC_EpollServer::bind(TC_EpollServer::BindAdapterPtr &lsPtr)
{
    int iRet = 0;

    for(size_t i = 0; i < _netThreads.size(); ++i)
    {
        if(i == 0)
        {
            iRet = _netThreads[i]->bind(lsPtr);
        }
        else
        {
            //当网络线程中listeners没有监听socket时，list使用adapter中设置的最大连接数作为初始化
            _netThreads[i]->setListSize(lsPtr->getMaxConns());
        }
    }

    return iRet;
}
```

当然,Acceptor除了监听连接之外，还需要负责将新建立的连接分发到各个IO线程中：

```c++
void TC_EpollServer::addConnection(TC_EpollServer::NetThread::Connection * cPtr, int fd, int iType)
{
    TC_EpollServer::NetThread* netThread = getNetThreadOfFd(fd);

    if(iType == 0)
    {
        netThread->addTcpConnection(cPtr);
    }
    else
    {
        netThread->addUdpConnection(cPtr);
    }
}

```

Acceptor在选择线程时基本可以认为是以轮询的方式进行：

```c++
NetThread* getNetThreadOfFd(int fd)
{
    return _netThreads[fd % _netThreads.size()];
}
```

当然，每个线程还需要监听NOTIFY和CLOSE事件，这两个应该是对外提供管理接口的，和本章内容关系不大，就不讲了：

```c++
//void TC_EpollServer::NetThread::createEpoll(uint32_t iIndex)
        _epoller.add(_shutdown.getfd(), H64(ET_CLOSE), EPOLLIN);
        _epoller.add(_notify.getfd(), H64(ET_NOTIFY), EPOLLIN);
```

##### Handler Pool

这个比较简单，就是提供处理器，一般和业务强相关。因此tars为它提供了一系列虚函数，以实现动态插入

```c++
....
virtual void heartbeat() {}
virtual void handle(const tagRecvData &stRecvData) = 0;
....
```

由于和业务强相关，因此提供的是一个虚函数。业务通过继承这个类实现具体的服务。比如ServantHanfle就是通过继承它来实现服务的处理逻辑的。因此从TC_EpollServer的角度来看，它基本上还没开始做什么有意义的事情。后面我们分析ServantHandle时在具体讲handle。


##### Adapter

Adapter是一个核心的东西。它帮助tars完成了一些比较dirty的工作，比如向下负责包的完整性解析；向上负责和handle线程协调工作。只有Adapter认为接收到了一个完整的包，它才会将包传给handle进行处理。

每个监听连接的端口都对应一个Adapter：

```c++
//int  TC_EpollServer::NetThread::bind(BindAdapterPtr &lsPtr)
   _listeners[s.getfd()] = lsPtr;
```

这主要是为了使得该监听连接的端口建立的每个连接都使用该Adapter进行消息处理：

```c++
//bool TC_EpollServer::NetThread::accept(int fd)
Connection *cPtr = new Connection(_listeners[fd].get(), fd, (timeout < 2 ? 2 : timeout), cs.getfd(), ip, port);
```

因此，通过为每个端口设置不同的Adapter可以使多个监听端口分别实现不同通信协议的服务。

这里稍微提一下，Adapter并不存储网络传输过来的字符数据。网络传输的字符流数据被缓存在Connection结构中：

```c++
 ....
           /**
             * 接收数据buffer
             */
            string              _recvbuffer;

            /**
             * 发送数据buffer
             */
            std::vector<TC_Slice>  _sendbuffer;
            ...
```

Connection每次收到数据时都会调用它的

```c++
int TC_EpollServer::NetThread::Connection::parseProtocol(recv_queue::queue_type &o)
```

函数对数据进行解析。其中协议解析的具体算法是通过Adapter实现的：

```c++
_pBindAdapter->getProtocol()(*rbuf, ro);
_pBindAdapter->getHeaderFilterFunctor()((int)(TC_EpollServer::PACKET_LESS), _recvbuffer);
```

Adapter提供函数指针来完成解析算法的插入。因此我们可以通过动态地设置Adapter的函数指针来更改服务的通信协议。

当Connection接收到完整数据包，它就将数据插入到Adapter中：

```c++
//void TC_EpollServer::NetThread::Connection::insertRecvQueue(recv_queue::queue_type &vRecvData)
_pBindAdapter->insertRecvQueue(vRecvData);
```

该插入还会唤醒在这个Adapter上睡眠等待数据处理的Handle线程：

```c++
//void TC_EpollServer::BindAdapter::insertRecvQueue(const recv_queue::queue_type &vtRecvData, bool bPushBack)
 _handleGroup->monitor.notify();
```

这里可以看到的是一般每个Adapter都会配套一组handle对它进行处理。

到此还需要说一下，前面提到Adaptor负责的对数据包的完整性进行解析，但是并没有对数据包中的有效性数据进行更具体的解析。这个工作其实会交给handle来处理，这一点我们从ServantHandle可以看到。**事实上，Adapter解析后会将数据包封装成以下形式**：

```c++
               tagRecvData* recv = new tagRecvData();
                recv->buffer           = ro;
                recv->ip               = _ip;
                recv->port             = _port;
                recv->recvTimeStamp    = TNOWMS;
                recv->uid              = getId();
                recv->isOverload       = false;
                recv->isClosed         = false;
                recv->fd               = getfd();
```

主要是一些metadata，真正有效的数据存储在buffer中。buffer中的数据解析涉及到具体协议的序列化过程，Adapter交给了具体的handle来完成。

Adapter维护了一个tagRecvData队列，来接收完整的tagRecvData。当然，每个tagRecvData结构对应一个完整的数据包，最后交给Handle处理。

```c++
 recv_queue      _rbuffer;
```

**综上，Adapter负责协议(包头)解析，handle负责有效数据(包体数据)的解析(反序列化)**

#### 总结

本文简单介绍了Tars的TC_EpollServer，粒度较粗，主要是进行框架式的介绍。TC_EpollServer核心包含了IO线程池，Adapter和Handle线程池。本质上也是传统的EPOLL多路IO+多线程非阻塞，Reactor模式。不过在IO线程和Handle之间接入了Adapter适配器。

Adapter负责数据传输协议的解析，这使得TC_EpollServer在支持不同传输协议上具有了更大的灵活性。当然，它必然地还起到了IO线程和handle之间的数据桥梁的作用，数据桥梁中放的是一个个等待handle处理的tagRecvData，它代表一个完整的数据包。

handle负责具体的业务逻辑。由于数据包的包体和业务逻辑强相关，因此包体的解析就交给了handle来处理。一般包体中包含的是序列化数据，比如protobuf或者tar序列化。可以根据业务需求，更改handle，使用不同的序列化工具。这也带来了一定的灵活性。
