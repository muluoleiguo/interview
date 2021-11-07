# muduo源码解析之EventLoopThread

回到TcpServer，我们记得里面有一个Eventloopthreadpool，根据名字，这是一个线程池，它主要用于管理所有的eventloop，每个eventloop对应一个线程。当新连接到来时，Acceptor事件分发器将连接分发到合适的线程中。线程里面核心是执行Eventloop类里面的loop函数，这个函数主要是执行epoll_wait和一些其他的处理函数。loop是一个while循环。muduo在eventloop上面包装了一层Eventloopthread。Eventloopthreadpool直接管理eventloopthread，eventloopthread再管理具体的thread和eventloop：
![这里写图片描述](https://img-blog.csdn.net/20170224150508946?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvU3dhcnR6MjAxNQ==/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)

**thread**数据成员是是对原生线程的封装，因为线程核心是运行eventloop，所以这里需要一个eventloop成员 **loop_**来管理这个事件循环。threadloop通过startloop开始开启一个线程，但记住，这时候新的线程还没有建立

```c++
EventLoop* EventLoopThread::startLoop()
{
  assert(!thread_.started());
  thread_.start();  // 启动线程，在这个线程里面执行的函数是: threadFunc

  {
    MutexLockGuard lock(mutex_);
    while (loop_ == NULL)
    {
      cond_.wait();
    }
  }

  return loop_;
}

```

到这里新的线程依旧没有建立，下面继续看一下thread类里面的start函数：

```c++
void Thread::start()
{
  assert(!started_);
  started_ = true;
  // FIXME: move(func_)
  detail::ThreadData* data = new detail::ThreadData(func_, name_, tid_);
  if (pthread_create(&pthreadId_, NULL, &detail::startThread, data))
  {
    started_ = false;
    delete data; // or no delete?
    LOG_SYSFATAL << "Failed in pthread_create";
  }
}

```

这里就会用pthread_create新建一个线程，线程里面将会运行data中的func函数，这个函数是thread类中的一个数据成员，我们关心的是func实在哪里被设置的呢。回头看一下thread的构造函数：

```c++
Thread::Thread(const ThreadFunc& func, const string& n)
  : started_(false),
    joined_(false),
    pthreadId_(0),
    tid_(new pid_t(0)),
    func_(func),//设置func
    name_(n)
{
  setDefaultName();
}
```

也就是说在新建thread的时候会确定新建线程执行的函数，我们回头看一下eventloopthread的数据成员thread的创建过程：

```c++
EventLoopThread::EventLoopThread(const ThreadInitCallback& cb,
                                 const string& name)
  : loop_(NULL),
    exiting_(false),
    thread_(boost::bind(&EventLoopThread::threadFunc, this), name), //创建thread
    mutex_(),
    cond_(mutex_),
    callback_(cb)
{
}

```

可以看到新建线程中执行的函数是eventloopthread中的threadFunc私有成员函数：

```c++
void EventLoopThread::threadFunc() // 这个函数运行在一个新的线程中
{
  EventLoop loop;  //在这里就可以通过loop的构造函数记录该线程的线程号，这样就可以使得每个loop和一个线程对应

  if (callback_)
  {
    callback_(&loop);
  }

  {
    MutexLockGuard lock(mutex_);
    loop_ = &loop;
    cond_.notify();
  }

  loop.loop();   //这里就会一直循环，
  //assert(exiting_);
  loop_ = NULL; //如果到达了这里，说明这个loop退出了
}
```

这里先调用callback_，然后调用eventloop中的loop()函数。到此，eventloop便和这个新建立的线程联系起来了。我们知道loop函数是一个while循环，因此线程将会一直执行loop函数，并在里面处理事情。这个loop函数显然可以访问eventloop结构中的数据成员和函数成员，因此我们通过改变这个eventloop结构中的具体数据成员和函数成员，就可以达到让线程在loop函数中为我们做定制的事情。所以我们通过eventloop结构接管了对这个线程的控制。
  另外我们可以看到函数中有条件变量的使用，这里的条件变量主要是作用于最开始时，Eventloopthread中的startloop的调用。在startloop中需要返回loop_指针，但是这个指针开始时是空的，到新线程中才创建具体的eventloop，而此时loop_指正才指向它。因此需要用条件变量，以等待新线程中创建好eventloop，否则返回的loop_将为空。这里的notify主要是通知调用startloop的线程，指向新线程的eventloop已经创建完毕了，并已经用loop_指正指向了它，可以将loop_从startloop函数返回了。
  我们还应该注意到，这个eventloop结构是在threadFunc中建立的。在eventloopthread的构造函数中直接将loop_指针指向null。之所以在这里建立eventloop主要是为了记录线程号。因为这个函数已经运行在新建线程中了，这样我们就可以获得新线程号，并把它记录在新建的loop结构中。这样我们就把每个eventloop和一个唯一的线程绑定起来了。后面我们通过控制这个eventloop就可以控制线程的执行。这里主要是可以把任务挂到eventloop的pendingFunctors_中让loop去完成。从loop函数的源码可以看到这点：

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
    pollReturnTime_ = poller_->poll(kPollTimeMs, &activeChannels_);
    ++iteration_;
    if (Logger::logLevel() <= Logger::TRACE)
    {
      printActiveChannels();
    }
    // TODO sort channel by priority
    eventHandling_ = true;
    for (ChannelList::iterator it = activeChannels_.begin();
        it != activeChannels_.end(); ++it)
    {
      currentActiveChannel_ = *it;
      currentActiveChannel_->handleEvent(pollReturnTime_);
    }
    currentActiveChannel_ = NULL;
    eventHandling_ = false;
    doPendingFunctors();  // 处理完了IO事件之后，处理等待在队列中的任务
  }

  LOG_TRACE << "EventLoop " << this << " stop looping";
  looping_ = false;
}
```

loop函数的核心是事件循环，这里我们关心的是最后的

```c++
doPendingFunctors();
```

它就是运行任务队列中的任务。
  综上所述，我们完成了把任务指发给特定的线程。完成的手段就是eventloop。线程和eventloop关系的建立过程用eventloopthread包装起来，这个过程只在线程建立的时候有用，其实主要是在服务器启动的时候，服务器通过一个eventloopthreadpool来管理所有的eventloopthread。线程建立以后，我们直接通过eventloop就可以控制具体的线程了。而所有的eventpool结构有eventloopthreadpool类中的loops_管理。所以后续我们分发任务时只需要在这个loops_找合适的eventloop，即合适的线程就可以了。这里的任务一般就是执行一个函数之类的。我们一般在这个函数中完成为eventloop添加Channel(事件处理器)等事情，使得事件循环可以监听和处理我们的事件。
  下一篇我们将解析muduo是怎么把任务加入到指定loop，即一个指定的线程中的。