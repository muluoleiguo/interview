抽象连接Connection
为了方便管理，我们把TCP连接抽象成一个类Connection

该类的私有数据成员如下，该类拥有连接两端的地址，以及各个状态下的回调函数，比如当连接上有消息到来时就会调用message_arrival_cb_。同时每个连接都会有输入输出缓冲区，用来接收消息和发送消息，

```c++
class Connection : public std::enable_shared_from_this<Connection>
{
private:
    Looper* loop_;

    // 连接描述符
    const int conn_sockfd_;
    std::shared_ptr<EventBase> conn_eventbase_;

    // 服务器、客户端地址结构
    struct sockaddr_in local_addr_;
    struct sockaddr_in peer_addr_;

    // 连接建立回调
    Callback connection_established_cb_;
    // 消息到达
    MessageCallback message_arrival_cb_;
    // 答复完成
    Callback reply_complete_cb_;
    // 连接关闭
    Callback connection_close_cb_;
    // 结束自己生命的回调
    Callback suicide_cb_;

    // 输入输出缓冲区
    IOBuffer input_buffer_;
    IOBuffer output_buffer_;
};

```

