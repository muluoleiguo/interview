# muduo中TcpConnection里IO事件的处理

muduo里面对IO事件的处理核心在TcpConnection里面。因为TcpConnection主要是负责TCP连接，因此有关TCP连接上所有的socket读写都发生在TcpConnection中。根据前一节对Buffer的介绍，在这里我们将会看到muduo是怎么使用Buffer的。

TcpConnection中有两个Buffer对象：

```c++
  Buffer inputBuffer_; 
  Buffer outputBuffer_; 
```



分别负责处理对数据的输入输出。从TcpConnection的构造函数中，我们可以看到TcpConnection的读写事件处理函数为：

```c++
void TcpConnection::handleRead(Timestamp receiveTime);
void TcpConnection::handleWrite();
```

两个函数的实现如下：

```c++
void TcpConnection::handleWrite()
{
  loop_->assertInLoopThread();
  if (channel_->isWriting())
  {
    ssize_t n = sockets::write(channel_->fd(),
                               outputBuffer_.peek(),
                               outputBuffer_.readableBytes()); //向chennel_->fd中写数据
    if (n > 0)
    {
      outputBuffer_.retrieve(n); //将已经写入channel_->fd的数据从outbuffer中除去
      if (outputBuffer_.readableBytes() == 0)  //如果outbuffer中已经没有数据了，则将该channel_的写事件从事件循环中去掉
      {
        channel_->disableWriting();
        if (writeCompleteCallback_)
        {
          loop_->queueInLoop(boost::bind(writeCompleteCallback_, shared_from_this()));
        }
        if (state_ == kDisconnecting)
        {
          shutdownInLoop();
        }
      }  // end of if (说明还有数据需要些写，不关闭写事件)
    }
    else
    {
      LOG_SYSERR << "TcpConnection::handleWrite";
      // if (state_ == kDisconnecting)
      // {
      //   shutdownInLoop();
      // }
    }
  }
  else
  {
    LOG_TRACE << "Connection fd = " << channel_->fd()
              << " is down, no more writing";
  }
}

void TcpConnection::handleRead(Timestamp receiveTime)
{
  loop_->assertInLoopThread();
  int savedErrno = 0;
  ssize_t n = inputBuffer_.readFd(channel_->fd(), &savedErrno);  //将fd中的数据读入到inputbuffer中
  //如果从fd中读到了数据，则调用messageCallback_，这个函数是可以通过用户来设置的
  if (n > 0)
  {
    messageCallback_(shared_from_this(), &inputBuffer_, receiveTime); 
  }
  //表示对端关闭了连接，调用handleclose
  else if (n == 0) // 
  {
    handleClose();
  }
  else
  {
    errno = savedErrno;
    LOG_SYSERR << "TcpConnection::handleRead";
    handleError();
  }
}

```

显然，不管是handleread还是handlewrite，他们都是先将数据放入到Buffer中，然后再通过Buffer进行进一步的操作的。因此，用户设置的函数都是直接操作Buffer的，用户程序不会直接接触到对socket的读写。比如messageCallback_函数，它就是基于inputbuffer的。至于发送数据的用户函数，一般是通过调用TcpConnection的send函数它们都间接调用sendInLoop，这个函数是线程安全的。下面看一下sendInLoop


```c++

void TcpConnection::sendInLoop(const void* data, size_t len)
{
  loop_->assertInLoopThread();
  ssize_t nwrote = 0;
  size_t remaining = len;
  bool faultError = false;
  if (state_ == kDisconnected)
  {
    LOG_WARN << "disconnected, give up writing";
    return;
  }
  // if no thing in output queue, try writing directly
  if (!channel_->isWriting() && outputBuffer_.readableBytes() == 0)
  {
    nwrote = sockets::write(channel_->fd(), data, len); //向fd中写数据
    if (nwrote >= 0) //写成功入了nwrote数据
    {
      remaining = len - nwrote; //记录还剩多少数据没有发送
      if (remaining == 0 && writeCompleteCallback_) //如果已经发送完毕
      {
        loop_->queueInLoop(boost::bind(writeCompleteCallback_, shared_from_this()));
      }
    }
    else // nwrote < 0 //说明写socket出错
    {
      nwrote = 0;
      if (errno != EWOULDBLOCK)
      {
        LOG_SYSERR << "TcpConnection::sendInLoop";
        if (errno == EPIPE || errno == ECONNRESET) // FIXME: any others?
        {
          faultError = true;
        }
      }
    }
  }

  assert(remaining <= len);
  //如果写socket操作正确返回，并且没有把数据全部发送出去
  if (!faultError && remaining > 0)
  {
    size_t oldLen = outputBuffer_.readableBytes(); 
    if (oldLen + remaining >= highWaterMark_
        && oldLen < highWaterMark_
        && highWaterMarkCallback_) //如果outputbuffer中国数据超过了警戒线，则调用highWaterMarkCallback_函数
    {
      loop_->queueInLoop(boost::bind(highWaterMarkCallback_, shared_from_this(), oldLen + remaining));
    }
    outputBuffer_.append(static_cast<const char*>(data)+nwrote, remaining); //将剩余的数据放入到outputbuffer中
    if (!channel_->isWriting()) //如果channel_没有在监听写事件，则注册监听事件，因为现在有数据等代写。
    {
      channel_->enableWriting(); // 缓冲区中还有数据，继续为当前的channel设置可写事件
    }
  }
}

```

根据源码注释，发送数据主要由两步完成，首先是尝试直接将数据发送出去，此时如果数据确实全部发送出去了，则正确返回，如果数据没有完全发送出去，则将剩余的数据写入outputbuffer中，等待下次写事件发生时在发送。用户调用这个函数时也是看不到原生socket的。对原生socket的读写全部由muduo封装起来了。
