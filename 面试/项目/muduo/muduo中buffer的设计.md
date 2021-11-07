# muduo中buffer的设计

Tcpconnection必须要有outputbuffer：

想像一个场景：应用程序想向TCP连接发送100k数据，但是在write系统调用中，操作系统只接受80k数据。为了避免阻塞，我们肯定不想在这里阻塞。因为不知道要等多久。程序应该尽快交出控制权，返回eventloop，此时，剩下的20k数据怎么办？

对于应用程序而言，它只管生成数据，不应该关心到底数据是一次性发送还是分成多次发送。这些问题应该由网络库关心。应用程序只需要直接调用Tcpconnection::send()就可以，网络库负责接管剩下的20k数据，把它保存在TcpConnection的buffer中，然后注册POLLOUT事件，一旦POLLOUT事件发生就可以直接将剩下的数据发送出去。当然，第二次调用也不能确定可以写完20k数据。如果还有剩余数据，网络库继续关注POLLOUT事件。如果buffer中已经没有数据可发送了，则取消对POLLOUT事件的关注，避免造成busy loop。

如果程序又写入50k数据，而这时候Tcpconnection缓冲区中还有20k数据等待发送，那么网络库不应该直接调用write，而应该把这50k的数据append到原来的20k数据之后，等socke可写时再一起写入。

如果outbuffer中还有待发送的数据，而程序又想关闭连接，那么这时候网络库不能立即关闭，而要等待缓冲区数据发送完毕再关闭。

综上，要让程序在write操作上不阻塞，网络库必须要给每个tcp connection配置output buffer。

TcpConnection必须要有input buffer

Tcp是一个无边界的字节流协议。接收方必须处理收到的数据尚不构成一条完整的消息和一次收到两条消息的数据等情况。而且网络库在读取socket可读事件时，必须一次性把socket里的数据读完。否则会反复出发POLLIN事件，因为我们的epoll loop采用的是epoll level triger。

为了应对TCP字节流协议带来的数据接受的不完整性，收到的数据必须先放到input buffer中，等构成一条完整的消息再通知程序的业务逻辑。

所以在Tcp Connection中必须要给每个tcp connection设置input buffer。

所有muduo中的IO都是带缓冲的。你不会自己去调用read或者write函数，只会操作tcpConnection的input buffer和output buffer。更确切地说，是在onMessage回调里读取input buffer；调用TcpConnection::send来间接操作output buffer。一般不会直接操作output buffer。


**Buffer源码分析**

**1.Buffer数据结构**

Buffer的数据成员如下：

```c++
 private:
  std::vector<char> buffer_;
  size_t readerIndex_;
  size_t writerIndex_;
```

buffer_是存放数据的vector容器，它是可以自动增长的。readerindex_和writeIindex_主要标识了Buffer中数据的分布情况，这两个变量的含义可用下图直观表示：

![这里写图片描述](https://img-blog.csdn.net/20170227204738415?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvU3dhcnR6MjAxNQ==/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)

两个index把vector分成三个部分：prependable，readable，writable
readable表示缓存中等待发送的数据所在位置，writable表示缓存中空闲的位置，可往里面写新的数据。predendable是一个固定大小的区域，我们可以通过往这个区域添加数据来为待发送的数据添加额外信息头。

muduo buffer里面有两个常量：

```c++
  static const size_t kCheapPrepend = 8;
  static const size_t kInitialSize = 1024;
```

其中kInitialSize = readable + writable；(这是初始状态，要记住Buffer中的buffer_是可变长度的)。

**2.buffer操作**
buffer的核心操作是下面的两个函数：

```c++
  void append(const char* /*restrict*/ data, size_t len)
  {
    ensureWritableBytes(len);
    std::copy(data, data+len, beginWrite());
    hasWritten(len);
  }
  void retrieve(size_t len)
  {
    assert(len <= readableBytes());
    if (len < readableBytes())
    {
      readerIndex_ += len;
    }
    else
    {
      retrieveAll();
    }
  }
```

显然，读时，readIndex右移。因为离readindex越近，则说明这些数据应该越早被发送，因为它们放在Buffer的时间越久。而写时，writeIndex右移，说明是往writable区域添加数据，添加之后readable区域的数据增加，因此writeIndex右移。

这里读可以理解为TCP连接从Buffer中读取数据，写表示有用户(网络库使用者)向Buffer中写数据。

从前面对Buffer操作中可以看到，经过若干次读写，prependable区域的数据会变大(因为readindex右移)。此时我们可能需要对buffer_进行调整，不能简单resize它。因为可能前面的predendable区域有很多空间不用，具体的方法是readindex左移，同时相应的数据往左边复制：


```c++
  void makeSpace(size_t len)
  {
    if (writableBytes() + prependableBytes() < len + kCheapPrepend) //需要重新分配空间
    {
      // FIXME: move readable data
      buffer_.resize(writerIndex_+len);
    }
    else //不需要重新分配空间，只需要调整readindex和writeindex和复制一下相关的数据到左边即可
    {
      // move readable data to the front, make space inside buffer
      assert(kCheapPrepend < readerIndex_);
      size_t readable = readableBytes();
      std::copy(begin()+readerIndex_,
                begin()+writerIndex_,
                begin()+kCheapPrepend);
      readerIndex_ = kCheapPrepend;
      writerIndex_ = readerIndex_ + readable;
      assert(readable == readableBytes());
    }
  }
```

