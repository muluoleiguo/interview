### 注意：github上有空格可能导致超链接失效，建议下载后阅读



### 协议层次以及它们的服务类型

1. [计算机网络体系结构](protocol/计算机网络体系结构.md)

   1-1. [数据如何在各层之间传输【数据的封装过程】](protocol/数据如何在各层之间传输[数据的封装过程].md)

三种模型：

2. [OSI七层](protocol/OSI七层.md)

3. [五层参考模型](protocol/五层参考模型.md)

4. [TCP/IP四层](protocol/TCP IP 四层.md)

对比：

5. [OSI 和 TCP/IP 协议之间的对应关系](protocol/OSI 和 TCP IP 协议之间的对应关系.md)

6. [OSI 模型和 TCP/IP 模型异同比较](protocol/OSI 模型和 TCP IP 模型异同比较.md)

7. [为什么 TCP IP 去除了表示层和会话层](protocol/为什么 TCP IP 去除了表示层和会话层.md)

# TCP/IP

### 应用层

0. [应用层总纲](TCP IP/应用层/应用层总纲.md)

#### HTTP

1. [请你来说一说http协议](TCP IP/应用层/HTTP/请你来说一说http协议.md)

   * [http请求头部包含哪些信息](TCP IP/应用层/HTTP/http请求头部包含哪些信息.md)

   * [Keep-Alive 和非 Keep-Alive 区别，对服务器性能有影响吗](TCP IP/应用层/HTTP/Keep-Alive 和非 Keep-Alive 区别，对服务器性能有影响吗.md)

2. [HTTP 请求方法了解哪些](TCP IP/应用层/HTTP/HTTP 请求方法了解哪些.md)

3. [GET和POST的区别](TCP IP/应用层/HTTP/GET和POST的区别.md)

4. [请你说一说HTTP返回码](TCP IP/应用层/HTTP/请你说一说HTTP返回码.md)

5. [HTTP 为什么要用TCP而不用UDP](TCP IP/应用层/HTTP/HTTP 为什么要用TCP而不用UDP.md)

6. [怎么知道 HTTP 的报文长度](TCP IP/应用层/HTTP/怎么知道 HTTP 的报文长度.md)

7. [HTTP 是不保存状态的协议,如何保存用户状态](TCP IP/应用层/HTTP/HTTP 是不保存状态的协议,如何保存用户状态.md)

**不同版本**

8. [http1.0和1.1的区别](TCP IP/应用层/HTTP/http1.0和1.1的区别.md)

9. [HTTP2.0和HTTP1.X 区别](TCP IP/应用层/HTTP/HTTP2.0和HTTP1.X 区别.md)
10. [HTTP 3](TCP IP/应用层/HTTP/HTTP 3.md)

#### HTTPS

11. [HTTP 与 HTTPs 的工作方式[建立连接的过程]](TCP IP/应用层/HTTPS/HTTP 与 HTTPs 的工作方式[建立连接的过程].md)
12. [HTTP和HTTPS对比](TCP IP/应用层/HTTPS/HTTP和HTTPS对比.md)

13. [请你来说一下数字证书是什么，里面都包含那些内容](TCP IP/应用层/HTTPS/请你来说一下数字证书是什么，里面都包含那些内容.md)



#### DNS

15. [DNS 的作用和原理](TCP IP/应用层/DNS/DNS 的作用和原理.md)
16. [DNS 为什么用 UDP](TCP IP/应用层/DNS/DNS 为什么用 UDP.md)
17. [怎么实现 DNS 劫持](TCP IP/应用层/DNS/怎么实现 DNS 劫持.md)



20. **超高频考点**：[输入一个网站，到显示页面的过程](TCP IP/应用层/HTTP/输入一个网站，到显示页面的过程.md)
21. [如果你访问一个网站很慢，怎么排查和解决](TCP IP/应用层/如果你访问一个网站很慢，怎么排查和解决.md)

22. [为什么 fidder，charles 能抓到你的包[抓取数据包的过程]](TCP IP/应用层/为什么 fidder，charles 能抓到你的包[抓取数据包的过程].md)
23. [URI（统一资源标识符）和 URL（统一资源定位符）之间的区别](TCP IP/应用层/URI（统一资源标识符）和 URL（统一资源定位符）之间的区别.md)







### 传输层

0. [传输层总纲](TCP IP/传输层/传输层总纲.md)

#### TCP

**连接建立与断开**

1. [三次握手和四次挥手机制](TCP IP/传输层/TCP/三次握手 四次挥手/三次握手和四次挥手机制.md)
   * [为什么连接的时候是三次握手，关闭的时候却是四次握手？](TCP IP/传输层/TCP/三次握手 四次挥手/为什么连接的时候是三次握手，关闭的时候却是四次握手？.md)

**三次握手：**

2. [请问tcp握手为什么两次不可以](TCP IP/传输层/TCP/三次握手 四次挥手/三次握手/请问tcp握手为什么两次不可以.md)

3. [为什么不用四次](TCP IP/传输层/TCP/三次握手 四次挥手/三次握手/为什么不用四次.md)

4. [第 2 次握手传回了 ACK，为什么还要传回 SYN](TCP IP/传输层/TCP/三次握手 四次挥手/三次握手/第 2 次握手传回了 ACK，为什么还要传回 SYN.md)
5. [为什么要三次握手](TCP IP/传输层/TCP/三次握手 四次挥手/三次握手/为什么要三次握手.md)
6. [如果三次握手的时候每次握手信息对方没有收到会怎么样](TCP IP/传输层/TCP/三次握手 四次挥手/三次握手/如果三次握手的时候每次握手信息对方没有收到会怎么样.md)



**四次挥手：**

7. [请你说一说TCP的四次挥手的原因](TCP IP/传输层/TCP/三次握手 四次挥手/四次挥手/请你说一说TCP的四次挥手的原因.md)

8. [CLOSE-WAIT 和 TIME-WAIT 的状态和意义](TCP IP/传输层/TCP/三次握手 四次挥手/四次挥手/CLOSE-WAIT 和 TIME-WAIT 的状态和意义.md)

9. [TIME-WAIT状态如何产生，产生原因，如何避免](TCP IP/传输层/TCP/三次握手 四次挥手/四次挥手/TIME-WAIT状态如何产生，产生原因，如何避免.md)
10. [TIME-WAIT 为什么是 2MSL](TCP IP/传输层/TCP/三次握手 四次挥手/四次挥手/TIME-WAIT 为什么是 2MSL.md)
11. [有很多 CLOSE-WAIT 怎么解决](TCP IP/传输层/TCP/三次握手 四次挥手/四次挥手/有很多 CLOSE-WAIT 怎么解决.md)



**数据传输流量控制和拥塞控制**

20. [TCP 流量控制与拥塞控制](TCP IP/传输层/TCP/control/TCP 流量控制与拥塞控制.md)
21. [TCP 拥塞控制采用的四种算法](TCP IP/传输层/TCP/control/TCP 拥塞控制采用的四种算法.md)
    * [请你说一说TCP拥塞控制？以及达到什么情况的时候开始减慢增长的速度？](TCP IP/传输层/TCP/control/请你说一说TCP拥塞控制？以及达到什么情况的时候开始减慢增长的速度？.md)
22. [滑动窗口协议](TCP IP/传输层/TCP/control/滑动窗口协议.md)
    * [如果接收方滑动窗口满了，发送方会怎么做](TCP IP/传输层/TCP/control/如果接收方滑动窗口满了，发送方会怎么做.md)



**TCP协议：**

30. [TCP状态转移](TCP IP/传输层/TCP/TCP状态转移.md)
31. [TCP如何保证其可靠性](TCP IP/传输层/TCP/TCP如何保证其可靠性.md)

32. [TCP 最大连接数限制](TCP IP/传输层/TCP/TCP 最大连接数限制.md)
33. [TCP 粘包问题](TCP IP/传输层/TCP/TCP 粘包问题.md)
34. [TCP 协议中的定时器](TCP IP/传输层/TCP/TCP 协议中的定时器.md)
35. [TCP 如何实时监测断线情况](TCP IP/传输层/TCP/TCP 如何实时监测断线情况.md)
36. [TCP 的停止等待协议是什么](TCP IP/传输层/TCP/TCP 的停止等待协议是什么.md)
37. [TCP 超时重传的原理](TCP IP/传输层/TCP/TCP 超时重传的原理.md)
38. [TCP 报文包含哪些信息](TCP IP/传输层/TCP/TCP 报文包含哪些信息.md)
39. [为什么服务端易受到 SYN 攻击](TCP IP/传输层/TCP/为什么服务端易受到 SYN 攻击.md)
40. [SYN FLOOD 是什么](TCP IP/传输层/TCP/SYN FLOOD 是什么.md)
41. [请你说一下TCP怎么保证可靠性](TCP IP/传输层/TCP/请你说一下TCP怎么保证可靠性.md)
42. [标志位](TCP IP/传输层/TCP/标志位.md)
43. [高并发服务器客户端主动关闭连接和服务端主动关闭连接的区别](TCP IP/传输层/TCP/高并发服务器客户端主动关闭连接和服务端主动关闭连接的区别.md)



#### UDP

50. [UDP 为什么是不可靠的？bind 和 connect 对于 UDP 的作用是什么](TCP IP/传输层/UDP/UDP 为什么是不可靠的？bind 和 connect 对于 UDP 的作用是什么.md)
51. [请你来介绍一下udp的connect函数](TCP IP/传输层/UDP/请你来介绍一下udp的connect函数.md)
52. [如何实现可靠地UDP传输](TCP IP/传输层/UDP/如何实现可靠地UDP传输.md)

53. [UDP可靠性设计](TCP IP/传输层/UDP/UDP可靠性设计.md)

**区别：**

[TCP和UDP的区别和各自适用的场景](TCP IP/传输层/TCP和UDP的区别和各自适用的场景.md)

[TCP UDP端口扫描的实现方式](TCP IP/传输层/TCP UDP端口扫描的实现方式.md)



### [TCP半连接队列和全连接队列](TCP IP/传输层/TCP半连接队列和全连接队列.md)







### 网络层

0. [网络层总纲](TCP IP/网络层/网络层总纲.md)



1. [IP 协议的定义和作用](TCP IP/网络层/IP 协议的定义和作用.md)
2. [请你说一说IP地址作用，以及MAC地址作用](TCP IP/网络层/请你说一说IP地址作用，以及MAC地址作用.md)
3. [域名和 IP 的关系，一个 IP 可以对应多个域名吗](TCP IP/网络层/域名和 IP 的关系，一个 IP 可以对应多个域名吗.md)
4. [ARP](TCP IP/网络层/ARP.md)
5. [TTL 是什么？有什么作用](TCP IP/网络层/TTL 是什么？有什么作用.md)
6. [请你说说传递到IP层怎么知道报文该给哪个应用程序，它怎么区分UDP报文还是TCP报文](TCP IP/网络层/请你说说传递到IP层怎么知道报文该给哪个应用程序，它怎么区分UDP报文还是TCP报文.md)
7. [运输层协议和网络层协议的区别](TCP IP/网络层/运输层协议和网络层协议的区别.md)





**路由：**

* [路由器的分组转发流程](TCP IP/网络层/router/路由器的分组转发流程.md)

* [路由器和交换机的区别](TCP IP/网络层/router/路由器和交换机的区别.md)



**IPv4 & IPv6：**

* [IPV4 地址不够如何解决](TCP IP/网络层/IP/IPV4 地址不够如何解决.md)

* [网络地址转换 NAT](TCP IP/网络层/IP/网络地址转换 NAT.md)





**ICMP：**

* [ICMP 协议概念和作用](TCP IP/网络层/ICMP/ICMP 协议概念 作用.md)

* [ICMP的应用](TCP IP/网络层/ICMP/ICMP的应用.md)

* [Ping的原理与工作过程](TCP IP/网络层/ICMP/Ping的原理与工作过程.md)

* [两台电脑连起来后 ping 不通，你觉得可能存在哪些问题](TCP IP/网络层/ICMP/两台电脑连起来后 ping 不通，你觉得可能存在哪些问题？.md)

### 数据链路层

* [数据链路层总纲](TCP IP/数据链路层/数据链路层总纲.md)

* [数据链路层上的三个基本问题](TCP IP/数据链路层/数据链路层上的三个基本问题.md)

* [请你说说TCP IP数据链路层的交互过程](TCP IP/数据链路层/请你说说TCP IP数据链路层的交互过程.md)

* [为什么有了 IP 地址还需要 MAC 地址](TCP IP/数据链路层/为什么有了 IP 地址还需要 MAC 地址.md)

* [为什么有了 MAC 地址还需要 IP 地址](TCP IP/数据链路层/为什么有了 MAC 地址还需要 IP 地址.md)

* [MAC 地址和 IP 地址分别有什么作用](TCP IP/数据链路层/MAC 地址和 IP 地址分别有什么作用.md)

* [私网地址和公网地址之间进行转换：同一个局域网内的两个私网地址，经过转换之后外面看到的一样吗](TCP IP/数据链路层/私网地址和公网地址之间进行转换：同一个局域网内的两个私网地址，经过转换之后外面看到的一样吗.md)

* [以太网中的 CSMA CD 协议](TCP IP/数据链路层/以太网中的 CSMA CD 协议.md)

* [PPP 协议](TCP IP/数据链路层/PPP 协议.md)

* [为什么 PPP 协议不使用序号和确认机制](TCP IP/数据链路层/为什么 PPP 协议不使用序号和确认机制.md)



### 物理层



[物理层主要做什么事情](TCP IP/物理层/物理层主要做什么事情.md)

[主机之间的通信方式](TCP IP/物理层/主机之间的通信方式.md)

[通道复用技术](TCP IP/物理层/通道复用技术.md)

[几种常用的宽带接入技术](TCP IP/物理层/几种常用的宽带接入技术.md)



### 网络安全

[对称加密和非对称的区别，非对称加密有哪些](TCP IP/网络安全/对称加密和非对称的区别，非对称加密有哪些.md)

[安全攻击有哪些](TCP IP/网络安全/安全攻击有哪些.md)

[RSA 和 AES 算法有什么区别](TCP IP/网络安全/RSA 和 AES 算法有什么区别.md)

[DDoS 有哪些，如何防范](TCP IP/网络安全/DDoS 有哪些，如何防范.md)

[ARP 攻击](TCP IP/网络安全/ARP 攻击.md)

[AES 的过程](TCP IP/网络安全/AES 的过程.md)





# socket编程



[总览](socket/总览.md)

[socket() 套接字有哪些](socket/socket() 套接字有哪些.md)

[请你说一下阻塞，非阻塞，同步，异步](socket/请你说一下阻塞，非阻塞，同步，异步.md)

[请你说说select，epoll的区别，原理，性能，限制都说一说](socket/请你说说select，epoll的区别，原理，性能，限制都说一说.md)

[请你来说一下socket编程中服务器端和客户端主要用到哪些函数](socket/请你来说一下socket编程中服务器端和客户端主要用到哪些函数.md)



[socket阻塞与非阻塞情况下的recv、send、read、write返回值](socket/socket阻塞与非阻塞情况下的recv、send、read、write返回值)



[请你来说一下reactor模型组成](socket/请你来说一下reactor模型组成.md)
