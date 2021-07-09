

HTTP 头部本质上是一个传递额外重要信息的key/value键值对。每行一对，key和value用冒号":"分隔，请求头部通知服务器有关于client端的请求信息，主要分为：
* 通用头部，
* 请求头部，
* 响应头部，
* 实体头部。


一个http**请求**报文由四部分组成：
1. 请求行（request line)、
2. 消息头部（header)、
3. 空行、
4. 请求正文。　

![image](https://pic2.zhimg.com/80/v2-c67ad0ca7157160071f1fcebb121b319_1440w.jpg)

请求头部常见的有：


 **User-Agent**：产生请求的浏览器类型 User-Agent: Mozilla/……

**Accept**：client端可识别的内容类型列表 Accept: text/plain

**Host**：请求的主机名，允许多个域名同处一个ip地址，即虚拟主机以及服务器所监听的端口号　Host: www.itbilu.com:80

其余见

https://leetcode-cn.com/leetbook/read/networks-interview-highlights/egdli7/