# muduo源码分析之事件处理器-Channel

前面两篇对Acceptor和TcpConnection的分析中，我们知道他们最终都是通过Channel和Eventpool简历连接，而且真正的事件处理函数也都是封装在Channel类中的。所以这里我把Channel看成事件处理器。
  除了事件处理函数之外，Channel也是必须将可能产生事件的文件描述符封装在其中的。这里的文件描述符可以是file descriptor，可以是socket，还可以是timefd，signalfd。文件描述符是最终需要加入到linux的epoll或者select等IO复用设施上的，显然这些设施是封装在了Eventpool中，这个我们在以后会介绍到。根据前面两篇，我们知道最终影响到Channel事件处理器加入eventpool是Channel中的下面函数

```c++
  void enableReading() { events_ |= kReadEvent; update(); }
  void disableReading() { events_ &= ~kReadEvent; update(); }
  void enableWriting() { events_ |= kWriteEvent; update(); }
  void disableWriting() { events_ &= ~kWriteEvent; update(); }
```

可以知道enable类函数将相应的Channel事件处理器加入到eventpool中，disable类函数将相应的事件处理器从eventloop中删除。具体的加入动作都落入到update中，其中的event_标志位主要用于标记事件类型，熟悉epoll的话应该比较简单，下面我们看一下update函数的实现：

```c++
void Channel::update()
{
  addedToLoop_ = true; //标记这个Channel已经加入到了evenloop中
  loop_->updateChannel(this);//将Channel加入到eventloop中
}

```

任务由eventloop中的updateChannel接力：

```c++
void EventLoop::updateChannel(Channel* channel)
{
  assert(channel->ownerLoop() == this);
  assertInLoopThread();
  poller_->updateChannel(channel);
}

```

这里的poller是eventloop中的一个数据成员，它是对linux平台上的IO复用设施的一个封装

![这里写图片描述](https://img-blog.csdn.net/20170224103354379?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvU3dhcnR6MjAxNQ==/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)

所以最终还是将任务传给了最底层的平台设施完成，这里用到了C++的多态，上图中的EPollPoller是对epoll的封装，这就落实到了具体的设施上了。下面我们以epoll为例看看poller_->updateChannel(channel)的实现：

```c++
void EPollPoller::updateChannel(Channel* channel)
{
  Poller::assertInLoopThread();
  const int index = channel->index();
  LOG_TRACE << "fd = " << channel->fd()
    << " events = " << channel->events() << " index = " << index;
  if (index == kNew || index == kDeleted) //说明这个Channel目前没有被epoll监听
  {
    // a new one, add with EPOLL_CTL_ADD
    int fd = channel->fd(); //获取Channel中的文件描述符
    if (index == kNew) //如果这个Channel是最新的，也就是之前没有被加进来过
    {
      assert(channels_.find(fd) == channels_.end());
      channels_[fd] = channel; //则设置channels_[fd]
    } 
    else // index == kDeleted //说明这个channel以前被加进来过，此时说明channels数组中已经存储了对应的channel
    {
      assert(channels_.find(fd) != channels_.end());
      assert(channels_[fd] == channel);
    }

    channel->set_index(kAdded);//设置channel的index为kAdded
    update(EPOLL_CTL_ADD, channel); //将监听事件的描述符加入到epoll中
  }
  else //如果index==kAdded，说明这个channel已经加进来了，并且epoll已经在监听这个channel中的描述符了
  {
    // update existing one with EPOLL_CTL_MOD/DEL
    int fd = channel->fd();
    (void)fd;
    assert(channels_.find(fd) != channels_.end());
    assert(channels_[fd] == channel);
    assert(index == kAdded);
    if (channel->isNoneEvent()) //如果channel中已经没有想监听的事件，则将这个channel的fd从epoll中删除
    {
      update(EPOLL_CTL_DEL, channel);
      channel->set_index(kDeleted);
    }
    else //另一种调用这个函数的情况是，修改监听事件的类型
    {
      update(EPOLL_CTL_MOD, channel);
    }
  }
}
```

由注释我们可以比较清楚地了解这个函数的功能。
  源码中我们可以知道往poller中update事件最后还需要由一个类中的update函数完成：

```c++
void EPollPoller::update(int operation, Channel* channel)
{
  struct epoll_event event;
  bzero(&event, sizeof event);
  event.events = channel->events();
  event.data.ptr = channel;
  int fd = channel->fd();
  LOG_TRACE << "epoll_ctl op = " << operationToString(operation)
    << " fd = " << fd << " event = { " << channel->eventsToString() << " }";
  if (::epoll_ctl(epollfd_, operation, fd, &event) < 0)
  {
    if (operation == EPOLL_CTL_DEL)
    {
      LOG_SYSERR << "epoll_ctl op =" << operationToString(operation) << " fd =" << fd;
    }
    else
    {
      LOG_SYSFATAL << "epoll_ctl op =" << operationToString(operation) << " fd =" << fd;
    }
  }
}
```

到这里就很熟悉了。这里主要用的是channel的fd和event_数据成员。这样就完成了在epoll中更新事件，删除的过程也是一样的。
  至此，我们从Acceptor，TcpConnection到Channel，清楚了将事件处理器加入到eventpool中的过程。具体地说，是加入到eventpool类中的成员Poller中的过程。当时到目前未知，一切都是静止地，无论是channel，eventloop，它们都还是一个个类，我们只是完成了一些对eventloop静态的设置，但是还没有让eventloop转起来。下面我们将从eventloop类出发，看muduo是怎么让它loop起来的。
