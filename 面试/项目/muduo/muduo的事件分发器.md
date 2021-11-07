# muduo的事件分发器-Acceptor

从对muduo的架构解析中我们知道，Acceptor主要是处理服务器的连接事件和事件分发。因为它要处理连接事件，因此muduo把它放在了独立于服务器的事件处理器池(eventloopthreadpool)之外的  一个eventloop中。

![这里写图片描述](https://img-blog.csdn.net/20170223174713184?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvU3dhcnR6MjAxNQ==/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)

其中 **acceptSocket_**是产生事件的socket文件描述符。而 **acceptChannel**是对acceptSocket的封装。**因为事件不仅包含文件描述符，还包含对应这个事件的处理函数。**因此需要把文件描述符和这些处理函数封装在一起，Channel结构体就是事件封装结构体。后面我们还会详细介绍一下Channel。
   ***loop_**就是运行Acceptor事件的eventloop，它也是muduo中必需的一个eventloop(因为eventloop线程池可能为空)。
   **HandleRead()**函数就是Acceptor连接事件的读处理函数，因为Acceptor只负责处理读事件，因此只对相应的Channel设置读事件处理函数。 **HandleRead()**函数的核心工作是由NewConnectionCallback函数完成。 **NewConnectionCallback**函数可以由用户通过调用 setNewConnectionCallback函数设置。这里的Acceptor用户一般是Tcpserver。
  Acceptor在处理连接事件之前，必需的准备工作在listen函数中完成。从该函数的源代码我们可以了解需要完成的预备动作：

```c++
void Acceptor::listen()
{
  loop_->assertInLoopThread(); 
  listenning_ = true;
  acceptSocket_.listen();//设置套接口的状态为listen
  //将acceptChannel_设置为可处理读事件的状态
  acceptChannel_.enableReading(); 
}
```

后面介绍Channel时在具体介绍它的原理。
  以上基本上就是Acceptor的全部了，我们看一下Acceptor的构造函数：

```c++
Acceptor::Acceptor(EventLoop* loop, const InetAddress& listenAddr, bool reuseport)
  : loop_(loop),
    acceptSocket_(sockets::createNonblockingOrDie(listenAddr.family())),
    acceptChannel_(loop, acceptSocket_.fd()),
    listenning_(false),
    idleFd_(::open("/dev/null", O_RDONLY | O_CLOEXEC))
{
  assert(idleFd_ >= 0);
  acceptSocket_.setReuseAddr(true);
  acceptSocket_.setReusePort(reuseport);
  acceptSocket_.bindAddress(listenAddr);
  acceptChannel_.setReadCallback(
      boost::bind(&Acceptor::handleRead, this));
}
```

前三条初始化语句主要是设置套接口。最后一条是设置acceptChannel\_的读事件处理函数。而acceptChannel\_和acceptSocket\_关联在acceptChannel\_(loop, acceptSocket\_.fd())完成。因此通过构造函数完成了acceptChannel_的初始化。剩下的工作就是将acceptChannel\_和loop\_关联起来。由上面可以知道，这个工作主要是在listen()函数中完成。

  最后我们看一下Accptor是如何处理连接事件的：

```c++
void Acceptor::handleRead()
{
  loop_->assertInLoopThread();
  InetAddress peerAddr;
  //FIXME loop until no more
  //获取新建连接的描述符
  int connfd = acceptSocket_.accept(&peerAddr); 
  if (connfd >= 0)
  {
    // string hostport = peerAddr.toIpPort();
    // LOG_TRACE << "Accepts of " << hostport;
    if (newConnectionCallback_)
    {
      // 分发连接
      newConnectionCallback_(connfd, peerAddr);
    }
    else
    {
      sockets::close(connfd);
    }
  }
  else
  {
    LOG_SYSERR << "in Acceptor::handleRead";
    // Read the section named "The special problem of
    // accept()ing when you can't" in libev's doc.
    // By Marc Lehmann, author of libev.
    if (errno == EMFILE)
    {
      ::close(idleFd_);
      idleFd_ = ::accept(acceptSocket_.fd(), NULL, NULL);
      ::close(idleFd_);
      idleFd_ = ::open("/dev/null", O_RDONLY | O_CLOEXEC);
    }
  }
```

Acceptor连接事件处理器的核心任务包括

  1. 获取新连接的套接口
  2. 分发连接

其中获取连接有accept函数完成。
分发连接在newConnectionCallback_中完成。
   newConnectionCallback\_是由Acceptor所属的Tcpserver提供的。在muduo中，它是TcpServer的一个private函数：

![这里写图片描述](https://img-blog.csdn.net/20170223181526858?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvU3dhcnR6MjAxNQ==/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)

 下面我们具体看一下newConnection的实现：

```c++
void TcpServer::newConnection(int sockfd, const InetAddress& peerAddr) 
{
  loop_->assertInLoopThread();
  //从threadpool中选一个eventloop
  EventLoop* ioLoop = threadPool_->getNextLoop();
  char buf[64];
  snprintf(buf, sizeof buf, "-%s#%d", ipPort_.c_str(), nextConnId_);
  ++nextConnId_;
  string connName = name_ + buf;

  LOG_INFO << "TcpServer::newConnection [" << name_
           << "] - new connection [" << connName
           << "] from " << peerAddr.toIpPort();
  InetAddress localAddr(sockets::getLocalAddr(sockfd));
  // FIXME poll with zero timeout to double confirm the new connection
  // FIXME use make_shared if necessary
  //新建一个TcpConnection结构体管理这个连接
  TcpConnectionPtr conn(new TcpConnection(ioLoop,
                                          connName,
                                          sockfd,
                                          localAddr,
                                          peerAddr));
  connections_[connName] = conn;
 // 设置连接的一些属性
  conn->setConnectionCallback(connectionCallback_);
  conn->setMessageCallback(messageCallback_);
  conn->setWriteCompleteCallback(writeCompleteCallback_);
  conn->setCloseCallback(
      boost::bind(&TcpServer::removeConnection, this, _1)); // FIXME: unsafe
      //将连接加入到1中所选的eventloop中运行
  ioLoop->runInLoop(boost::bind(&TcpConnection::connectEstablished, conn)); 
}
```

可以看出，newConnection主要完成的工作为：
  1. 从threadpool中选择一个eventloop来运行这个连接
  2. 新建一个TcpConnection结构体管理这个连接
  3. 设置连接的一些属性
  4. 将连接加入到1中所选的eventloop中运行

由此可以看出，TcpConnection是粘结eventpool和Acceptor的主要结构。Accptor负责创建TcpConnection，然后将TcpConnection扔给eventloop去管理。每个TcpConnection代表的是Acceptor中新建的一个连接。它将连接的套接口和负责处理连接中的读写等事件处理器封装起来，这点和Acceptor有点相似，都是对文件描述符和事件处理器的封装，只不过相对于Acceptor中只负责读事件的处理不同，TcpConnection需要的事件处理器要多的多。

   下一篇我们详细看一下TcpConnection的实现。
