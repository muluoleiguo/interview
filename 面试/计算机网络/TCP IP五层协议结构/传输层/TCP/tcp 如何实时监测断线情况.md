TCP正常的断开，通信双方（服务端和客户端）都是能知道的。但是非正常的断开，比如直接拔掉了网线，就只能靠如下两种方法，实现短时间内的检测。

1、心跳包机制

心跳包机制，是网游设计中的常用机制。从用户层面，自己发包去判断对方连线状态。可以根据情况，很灵活的使用。比如，20秒发送一个最小的数据包（也可以根据实际情况稍带一些其他数据）。如果发送没有回应，就判断对方掉线了。断开后立即关闭socket。

2、利用tcp_keepalive机制

利用TCP的机制，通过设置系统参数，从系统层面，监测tcp的连接状态。在配置socket属性时，使用 keepalive option，一旦有此配置，这些长时间无数据的链接会根据tcp的keepalive内核属性，在大于(tcp_keepalive_time)所对应的时间（单位为秒）之后，断开这些链接。

关于keep alive无论windows，还是linux，keepalive就三个参数：

* keepalive_probes: 探测次数
* keepalive_time: 探测的超时
* keepalive_intvl: 探测间隔

对于一个已经建立的tcp连接，如果在keepalive_time时间内双方没有任何的数据包传输，则开启keepalive功能的一端将发送 eepalive数据包，若没有收到应答，则每隔keepalive_intvl时间再发送该数据包，发送keepalive_probes次。一直没有收到应答，则发送rst包关闭连接。若收到应答，则将计时器清零。