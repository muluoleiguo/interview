1）基于TCP的socket：
1、服务器端程序：

1创建一个socket，用函数socket()

2绑定IP地址、端口等信息到socket上，用函数bind()

3设置允许的最大连接数，用函数listen()

4接收客户端上来的连接，用函数accept()

5收发数据，用函数send()和recv()，或者read()和write()

6关闭网络连接

2、客户端程序：

1创建一个socket，用函数socket()

2设置要连接的对方的IP地址和端口等属性

3连接服务器，用函数connect()

4收发数据，用函数send()和recv()，或read()和write()

5关闭网络连接
![image](https://uploadfiles.nowcoder.com/images/20190315/308571_1552654678444_69CF8398BCC9F204991E623723D022E7)
2）基于UDP的socket：

1、服务器端流程

1建立套接字文件描述符，使用函数socket()，生成套接字文件描述符。

2设置服务器地址和侦听端口，初始化要绑定的网络地址结构。

3绑定侦听端口，使用bind()函数，将套接字文件描述符和一个地址类型变量进行绑定。

4接收客户端的数据，使用recvfrom()函数接收客户端的网络数据。

5向客户端发送数据，使用sendto()函数向服务器主机发送数据。

6关闭套接字，使用close()函数释放资源。UDP协议的客户端流程

2、客户端流程

1建立套接字文件描述符，socket()。

2设置服务器地址和端口，struct sockaddr。

3向服务器发送数据，sendto()。

4接收服务器的数据，recvfrom()。

5关闭套接字，close()。
![image](https://uploadfiles.nowcoder.com/images/20190315/308571_1552654687053_263E6AFD1A48F8511D04B67EB12AA24C)

# 请你讲述一下Socket编程的send() recv() accept() socket()函数？
send函数用来向TCP连接的另一端发送数据。客户程序一般用send函数向服务器发送请求，而服务器则通常用send函数来向客户程序发送应答,send的作用是将要发送的数据拷贝到缓冲区，协议负责传输。
recv函数用来从TCP连接的另一端接收数据，当应用程序调用recv函数时，recv先等待s的发送缓冲中的数据被协议传送完毕，然后从缓冲区中读取接收到的内容给应用层。
accept函数用了接收一个连接，内核维护了半连接队列和一个已完成连接队列，当队列为空的时候，accept函数阻塞，不为空的时候accept函数从上边取下来一个已完成连接，返回一个文件描述符。

**socket编程的过程**：
![image](https://pic3.zhimg.com/80/v2-29a5c5900181463deb03c03ff8a15a7e_1440w.jpg)