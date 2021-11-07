# 将任务加入到Eventloopthread

根据我们分析新线程建立以后，会执行eventloop中的loop函数，而且loop主体是一个while循环，所以在不退出的情况下，新线程将会一直执行这个while循环。我们这里再看一下loop函数的代码：

```c++
void EventLoop::loop()
{
  assert(!looping_);
  assertInLoopThread();
  looping_ = true;
  quit_ = false;  // FIXME: what if someone calls quit() before loop() ?
  LOG_TRACE << "EventLoop " << this << " start looping";

  while (!quit_)
  {
    activeChannels_.clear();
    //监听并获取待处理的事件，把这些事件放在aciveChannels中
    pollReturnTime_ = poller_->poll(kPollTimeMs, &activeChannels_);
    ++iteration_;
    if (Logger::logLevel() <= Logger::TRACE)
    {
      printActiveChannels();
    }
    // TODO sort channel by priority
    eventHandling_ = true;
    //循环处理所有activeChannels_中的事件
    for (ChannelList::iterator it = activeChannels_.begin();
        it != activeChannels_.end(); ++it)
    {
      currentActiveChannel_ = *it;
      currentActiveChannel_->handleEvent(pollReturnTime_);
    }
    currentActiveChannel_ = NULL;
    eventHandling_ = false;
    //执行任务队列中待处理的其他任务
    doPendingFunctors();  
  }

  LOG_TRACE << "EventLoop " << this << " stop looping";
  looping_ = false;
}

```

从源代码中可以看到，loop函数就是不断循环往复在做三个工作
\1. 监听和获取待处理的事件
\2. 处理得到的待处理事件
\3. 处理任务队列中待处理的任务

1和2我们前面已经分析过了，这里主要看一下第三点。第三点由doPendingFunctors函数完成：

```c++
void EventLoop::doPendingFunctors()
{
  std::vector<Functor> functors;
  callingPendingFunctors_ = true;

  {
  MutexLockGuard lock(mutex_);
  functors.swap(pendingFunctors_);
  }

  for (size_t i = 0; i < functors.size(); ++i)
  {
    functors[i]();
  }
  callingPendingFunctors_ = false;
}

```

从代码中我们可以看到，doPendingFunctors就是逐个执行pendingFunctors_数组中的函数对象。前面的互斥锁主要是为了防止将任务加入队列中的线程和执行任务队列的线程对pendingFunctors_的操作造成不一致。
  到这里我们自然会想知道任务是怎么被加入到pendingFunctors_中的。其实这主要由runInLoop函数完成，它也是eventloop的一个成员函数：

```c++
void EventLoop::runInLoop(const Functor& cb)
{
//如果当前线程试图运行一个属于自己的任务，那就直接运行了
  if (isInLoopThread()) 
  {
    cb();
  }
  //如果当前线程试图让其他线程完成这个任务
  //则将任务加入到对应线程的任务队列中
  else
  {
    queueInLoop(cb);
  }
}
```

加入任务队列由queueInLoop函数完成

```c++
void EventLoop::queueInLoop(const Functor& cb)
{
  {
  MutexLockGuard lock(mutex_);
  pendingFunctors_.push_back(cb); //将任务加入到这个loop的任务队列中
  }
//下面这几行试比较容易困惑的地方
//在两个条件下需要唤醒IO线程
//1. 如果执行queueInLoop的线程不是这个loop指向的IO线程，则唤醒IO线
//   程(它可能被阻塞了)，保证加进队列的任务可以得到及时的执行
//2. 如果lopp指向的IO线程正在执行任务队列中的任务，则唤醒这个IO线
//   程。这里就比较难以理解了，为什么需要去唤醒正在
//   执行任务的IO线程呢，它又没有被阻塞；还有正在执行任务队列的线程
//   应该持有互斥锁，为什么还会执行到这里呢？其实主要是在执行任务队
//   中的任务时，这些任务可能本身会调用queueInLoop函数。这样就会使
//   这个被任务加进去的新任务得等到下一轮循环才能被重新执行，这里
//   可以根据doPendingFunctors中的functors.swap(pendingFunctors_)理解.
//   为了保证下一轮循环不在开始时阻塞于事件监听中(epoll_wait)，保证在
//   这轮任务队列执行完之后马上执行下一轮任务队列。wakeup的原理就是往一个wakeupfd中写入8个字节，使得epoll_wait迅速返回。
  if (!isInLoopThread() || callingPendingFunctors_)
  {
    wakeup();
  }
}
```

pendingFunctors_.push_back(cb)函数完成了将任务加入到任务队列中。这里互斥锁的使用功能如前面所述。其他部分根据源码解释可以很好理解这个函数的实现原理。
  到此就介绍了向eventloop中添加任务的原理以及loop中是如何完成任务的执行的。下篇我们看一下muduo中定时器的实现。
