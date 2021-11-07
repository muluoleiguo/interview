### 事件分发

让我们先理清一下事件分发的概念。在linux系统中，信奉着一切皆文件的思想，对于我们网络库使用的socket套接字，也是用文件描述符来表示。每个tcp socket在内核中都有一个接收缓冲区和发送缓冲区，客户机有消息到来实际上是存放到接收缓冲区中，而服务器要发送消息也是放在发送缓冲区等待内核发送而已。

现在就可引入事件的概念了，我们要朝客户发送消息需要什么条件呢？需要发送缓冲区有足够空间；同理，要读取客户发过来的消息就需要接收缓冲区有内容。因此，我们需要关注的是发送缓冲区有足够空间，这称为**可写事件**，关注接收缓冲区有内容，这称为**可读事件**。

事件分发的意思就是当发生了不同的事件，分发给不同的处理。比如发生了可读事件，我就把内容读出来并根据内容生成相应的回答，当发生了可写事件，就把这个回答答复回去。

### EventBase类

这里就可以开始介绍EventBase类了，这个类抽象的就是所谓的事件分发，它只负责一个文件描述符上的事件分发，也就是我们说的socket，我们设置要关注的事件，以及对应的事件处理函数，当发生事件时就根据事件分发给不同的函数回调。该类的部分接口如下：


```c++
class EventBase
{
public:
    using Callback = std::function<void()>;
    using ReadCallback = std::function<void(Timestamp)>;

    // 传入参数为文件描述符
    EventBase(int fd);
    ~EventBase();

    // 关注可读事件
    void EnableReadEvents();    
    // 关注可写事件
    void EnableWriteEvents();   
    // 取消关注相应事件
    void DisableReadEvents(); 
    void DisableWriteEvents();  

    // 设置相应的事件处理函数
    void SetReadCallback(ReadCallback&& cb);;
    void SetWriteCallback(Callback&& cb);
    void SetErrorCallback(Callback&& cb);
    void SetCloseCallback(Callback&& cb);
    
  	// 分发事件
    void HandleEvent();
};

```

该类的私有数据成员如下，我们需要设置的就是该socket的文件描述符，关注的事件，以及相应的处理函数，**当有事件发生时，发生的事件类型会写到revents_中，然后就可以根据这个来决定执行那个处理函数了。**

```c++
    using Callback = std::function<void()>;
    using ReadCallback = std::function<void(Timestamp)>;
private:
    // 文件描述符
    const int fd_;
    // 关注的事件
    int events_;

    // 返回的活跃事件
    int revents_;

    // 事件处理函数
    ReadCallback read_callback_;
    Callback write_callback_;
    Callback error_callback_;
    Callback close_callback_;
```



