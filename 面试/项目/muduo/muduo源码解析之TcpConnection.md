在上一篇对Tcpserver的连接事件处理器Acceptor的解析中知道Acceptor为每个连接创建一个TcpConnection结构，通过将这个结构传递给Eventpool实现eventpool和Acceptor的关联。因此TcpConnection是连接后面eventpool的桥梁。本篇将解析TcpConnection类。
  首先看一下TcpConnection的类数据成员：

![这里写图片描述](https://img-blog.csdn.net/20170223232001836?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvU3dhcnR6MjAxNQ==/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)

毫无疑问，因为需要监视I/O事件，因此必然存在一个socket文件描述符，这里的 socket\_数据成员就是了。而根据前面的分析可以知道，muduo中必须通过Channel将文件描述符和对应的事件处理函数封装，然后再将Channel交给eventpool完成事件处理器在eventpool中的安装。这里解释一下为什么需要将文件描述符和事件处理函数封装在一起。主要是因为如果不将事件处理函数和文件描述符放在一起，后面当文件描述符上面有事件发生时，就不知道怎么去找到事件处理函数了。而把他们都放在一个Channel里面后，事件发生时直接去找相应的Channel就可以了。TcpConnection中的 channel\_就是对socket\_进行封装的Channel。
  在TcpConnection的构造函数中我们可以清楚地看到对channel\_设置事件处理函数：

```c++
TcpConnection::TcpConnection(EventLoop* loop,
                             const string& nameArg,
                             int sockfd,
                             const InetAddress& localAddr,
                             const InetAddress& peerAddr)
  : loop_(CHECK_NOTNULL(loop)),
    name_(nameArg),
    state_(kConnecting),
    socket_(new Socket(sockfd)),
    channel_(new Channel(loop, sockfd)),
    localAddr_(localAddr),
    peerAddr_(peerAddr),
    highWaterMark_(64*1024*1024),
    reading_(true)
{
  channel_->setReadCallback(
      boost::bind(&TcpConnection::handleRead, this, _1));
  channel_->setWriteCallback(
      boost::bind(&TcpConnection::handleWrite, this));
  channel_->setCloseCallback(
      boost::bind(&TcpConnection::handleClose, this));
  channel_->setErrorCallback(
      boost::bind(&TcpConnection::handleError, this));
  LOG_DEBUG << "TcpConnection::ctor[" <<  name_ << "] at " << this
            << " fd=" << sockfd;
  socket_->setKeepAlive(true);
}

```

这里主要是四个事件处理函数的设置。事件处理器的具体实现我们后面再详细描述，这里主要还是了解代码框架。下面我们继续看TcpConnection的数据成员。
  TcpConnection核心是处理IO事件。作为一个健壮的服务器，肯定是不能直接将需要发送的所有数据写入TCP的内核缓冲区中(不能保证内核有足够的缓冲区)，也不能允许用户程序完全把TCP内核缓冲区中的数据全部读到应用程序自己设置的缓存中(不能保证用户程序设置的缓冲区容量能容纳所有读进来的数据)，因此是必须在TCP内核缓冲的上面在封装一层数据缓冲区的，这就是inputBuffer\_和outputBuffer\_的作用了。后面我们还会详细介绍它的实现。这些功能主要是用于TcpConnection的事件处理函数，所以到时候我们详细分析它的事件处理函数就可以了。
  通过上面的分析知道，当TcpServer创建TcpConnection时，TcpConnection已经把它和eventpool的桥梁Channel结构初始化完毕了，剩下的就是怎么把这个Channel加入到Eventloop中。这个任务由Tcpserver的newConnection函数(记住TcpConnection结构也是在这个函数中创建的)中的下面这行代码完成：

```c++
ioLoop->runInLoop(boost::bind(&TcpConnection::connectEstablished, conn));
```

这行代码的作用我们现在可以简单理解为执行TcpConnection中的connectEstablished函数。我们看一下connectEstablished的实现：

```c++
void TcpConnection::connectEstablished()
{
  loop_->assertInLoopThread();
  assert(state_ == kConnecting);
  setState(kConnected);
  channel_->tie(shared_from_this());
  channel_->enableReading();  // enable_reading之后在表示把这个事件添加到了event_loop中了

  connectionCallback_(shared_from_this());
}
```

好了，我们看到了熟悉的channel\_->enableReading()，记住这行代码表示将该Channel加入到了Eventloop中。可能会问为什么这里没有看到Eventloop结构呢，其实这个Eventloop结构已经存在了channel\_中(在Channel的构造函数中可以看到)，它是Channel中的一个数据成员。到此为止，我们将TcpConnection通过channel和eventpool连接起来了(回顾一下Acceptor，这个过程很相似)。至于Channel是如何和eventloop连接的，下一篇我们会看Channel的实现，就会知晓答案。


