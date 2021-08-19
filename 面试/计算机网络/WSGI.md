## 江湖外传

互联网飞速发展，在web开发的江湖中，许多脚本语言都参与进来分一杯羹。含着金汤匙出生的PHP自不必说，Ruby与Python也加入这场混战。诞生于日出之国的Ruby起初并未在江湖上掀起波浪，直到丹麦少年为其诞下一子——Ruby On Rails后，才母以子贵，为世人所熟知。

既生瑜，何生亮。Python与Ruby总是那么纠缠不清，在多个领域，Python声名显赫，为世人所青睐。唯独在Web开发领域难以望Ruby之项背。但这本身并不妨碍，Python在Web开发领域的不断的努力。从web.py、Flask、Django之中，可见一斑。今天，我们聚焦在这些框架背后共同使用的网关通信技术——WSGI。

## WSGI

WSGI即 **Web Server Gateway Interface**，看名字与CGI类似。没错，Python同样支持CGI接口编程，但如前文所述CGI之某些缺点，Python开发了自己的网关通信技术（当然Python本身也支持CGI）。

顾名思义，在职能上来说，WSGI和CGI确实是同样的东西，都是网关的一种实现，是连接Web服务器与Web应用之间的桥梁。**因此理解了前文我对CGI的描述，对于理解WSGI是大有裨益的**（和CGI重合的内容我就不提了）。当然就具体接口协议而言，他们是不同的。



> **WSGI不是库，也是框架。**

Django、Flask可以被称为使用（支持）了WSGI的Web框架。

## 应用皆回调

收益于Python这门脚本语言。我们在编写WSGI程序的时候，只将具体的处理逻辑def成函数，然后将该函数作为回调（callback）传入WSGI的server循环程序之中即可。此时该函数称之为WSGI Application。

看一下最简单的应用回调函数：

```python
def application(environ, start_response):
    start_response('200 OK', [('Content-Type', 'text/html')])
    return '<h1>Hello WSGI</h1>'
```

应用的回调函数都有两个参数，environ和start_response。

第一个参数environ就是一个字典，里面存着环境变量，里面的内容和CGI的环境变量基本一致。

第二个参数start_response是一个函数，用于输出HTTP的响应头，start_response一般有两个参数：第一个参数是就是状态码，比如200 OK，302 Found之类的。和CGI一样，**通常**情况下我们需要显示控制的HTTP头部的输出，就是**200**（正常返回）和**301/302**（重定向）这两种情况。因为类似404或者502之类的异常错误码，Web服务器肯定会自动抛出的。第二个参数则是HTTP首部的其他信息，是一个多个元组构成的list。每个首部字段，都放置到一个元组之中。

然后return出来的字符串，就是HTTP响应的实体部分了。看出来了吧，**这是和CGI的一大不同：**CGI是将所有响应内容（头部和实体）都作为标准输出，通过web服务器的重定向返回给客户端。而**WSGI的做法，头部响应和实体响应分离，并且不需要向标准输出去输出内容**。在WSGI中标准输出，就是普通的标准输出，会打印到终端控制台上，而不会作为内容返回。



## wsgiref

wsgiref模块是python官方库针对wsgi协议的一个参考实现，虽然与工业界标准或有差距，但用来学习基本够用。同时该模块实现了一个简单的wsgi协议的HTTP服务器。

```python
from wsgiref.simple_server import make_server
......

httpd = make_server('', 8000, application)
    print 'Serving HTTP on port %d' % port
    httpd.serve_forever()
```

*放心，如前文所言，这个print语句，只会输出到终端，作为log之用，不会发送给客户端哒*。

这段代码比较简单，调用了wsgiref.simple_server的make_server函数构造了一个HTTP Server的对象（明显是个工厂模式）。该函数接收3个固定参数（其实有5个参数，另外两个有默认值）：

1. ip；可以为空，表示0.0.0.0（0.0.0.0即绑定该服务器的任何ip地址）
2. 端口号；
3. 回调函数（上上文def的application函数）



接着，开始循环（httpd.serve_forever()）就启动了一个简单的HTTP Server啦。可以看出编写wsgi web应用的关键，就是回调函数的编写啦。

虽然前面说wsgiref基本够用，但是有个问题，就是不支持HTTPS啊。要弄个支持HTTPS的Web Server，就要用到一些第三方的组件了。

## eventlet

gevent和eventlet都可以完成，以eventlet举例：

```python
from eventlet import wsgi
import eventlet

wsgi.server(eventlet.wrap_ssl(eventlet.listen(('', 8443)),
                              certfile='cert.crt',
                              keyfile='private.key',
                              server_side=True),
            application)
```

其中certfile和keyfile是服务端的证书和私钥文件，可用openssl命令生成自签名的证书和私钥。关于eventlet的详细介绍不在本文讨论范围内，大家可以自行百度谷歌。我们需要聚焦的是，虽然使用了一个第三方库，启动server的代码与wsgiref模块不太相同，但是我们之前编写的application回调函数可以直接在这里使用。也就是说我们的回调是与具体server无关的，更换另一个支持wsgi协议的server，只是在启动的代码做些修改，而我们所关心的业务逻辑则丝毫无需改动。

## 做个简单的URL路由

之前CGI的文章中有指出，CGI程序只能交由Web服务器去配置出十分简单的路由功能。而受益于Python语言之利，WSGI程序自制一个URL路由的功能十分简单。

为了解耦，我们可以单写一个URL路由的脚本，作为配置文件之用。比如urls.py

```python
... 

urls = [
    (r'^$', index),
    (r'hello/?$', hello),
    (r'world/?$', world),
    (r'good/boy?$', boy)
]
```

r开头的字符串表示正则表达式，通过正则实际可以更为灵活的配置URL路由。另外需确保urls.py文件中import了index、hello、world、boy这些函数的定义。

然后在之前的启动脚本中，引入这个文件，并且解析路由：

```python
from urls import urls

def application(environ, start_response):
    path = environ.get('PATH_INFO', '').lstrip('/')
    for regex, callback in urls:
        match = re.search(regex, path)
        if match is not None:
            return callback(environ, start_response)
```

这样一个简单的URL路由功能就实现了。