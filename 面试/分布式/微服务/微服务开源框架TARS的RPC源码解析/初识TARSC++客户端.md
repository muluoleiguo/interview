**导语**：微服务开源框架TARS的RPC调用包含客户端与服务端，《微服务开源框架TARS的RPC源码解析》系列文章将从初识客户端、客户端的同步及异步调用、初识服务端、服务端的工作流程四部分，以C++语言为载体，深入浅出地带你了解TARS RPC调用的原理。

#### 什么是TARS

TARS是腾讯使用十年的微服务开发框架，目前支持C++、Java、PHP、Node.js、Go语言。该开源项目为用户提供了涉及到开发、运维、以及测试的一整套微服务平台PaaS解决方案，帮助一个产品或者服务快速开发、部署、测试、上线。目前该框架应用在腾讯各大核心业务，基于该框架部署运行的服务节点规模达到数十万。
TARS的通信模型中包含客户端和服务端。客户端服务端之间主要是利用RPC进行通信。本系列文章分上下两篇，对RPC调用部分进行源码解析。本文是上篇，我们将以C++语言为载体，带大家了解一下TARS的客户端。

### 初识客户端

TARS的客户端最重要的类是Communicator，一个客户端只能声明出一个Communicator类实例，用户可以通过CommunicatorPtr& Application::getCommunicator()获取线程安全的Communicator类单例。Communicator类聚合了两个比较重要的类，一个是CommunicatorEpoll，负责网络线程的建立与通过ObjectProxyFactory生成ObjectProxy；另一个是ServantProxyFactory，生成不同的RPC服务句柄，即ServantProxy，用户通过ServantProxy调用RPC服务。下面简单介绍几个类的作用。

### Communicator

一个Communicator实例就是一个客户端，负责与服务端建立连接，生成RPC服务句柄，可以通过CommunicatorPtr& Application::getCommunicator()获取Communicator实例，用户最后不要自己声明定义新的Communicator实例。

### ServantProxy与ServantProxyFactory

ServantProxy就是一个服务代理，ServantProxy可以通过ServantProxyFactory工厂类生成，用户往往通过Communicator的template void stringToProxy()接口间接调用ServantProxyFactory的ServantPrx::element_type* getServantProxy()接口以获取服务代理，通过服务代理ServantProxy，用户就可以进行RPC调用了。ServantProxy内含多个服务实体ObjectProxy（详见下文第4小点），能够帮助用户在同一个服务代理内进行负载均衡。

### CommunicatorEpoll

CommunicatorEpoll类代表客户端的网络模块，内含TC_Epoller作为IO复用，能够同时处理不同主调线程（caller线程）的多个请求。CommunicatorEpoll内含服务实体工厂类ObjectProxyFactory（详见下文第4小点），意味着在同一网络线程中，能够产生不同服务的实体，能够完成不同的RPC服务调用。CommunicatorEpoll还聚合了异步调用处理线程AsyncProcThread，负责接收到异步的响应包之后，将响应包交给该线程处理。

### ObjectProxy与ObjectProxyFactory

ObjectProxy类是一个服务实体，注意与ServantProxy类是一个服务代理相区别，前者表示一个网络线程上的某个服务实体A，后者表示对所有网络线程上的某服务实体A的总代理，更详细的介绍可见下文。ObjectProxy通过ObjectProxyFactory生成，而ObjectProxyFactory类的实例是CommunicatorEpoll的成员变量，意味着一个网络线程CommunicatorEpoll能够产生各种各样的服务实体ObjectProxy，发起不同的RPC服务。ObjectProxy通过AdapterProxy来管理对服务端的连接。
好了，介绍完所有的类之后，先通过类图理一理他们之间的关系，这个类图在之后的文章中将会再次出现。

![图（1-1）客户端类图](https://img-blog.csdnimg.cn/20200730151715793.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L1RBUlNGb3VuZGF0aW9u,size_16,color_FFFFFF,t_70)

TARS的客户端最重要的类是Communicator，一个客户端只能声明出一个Communicator类实例，用户可以通过CommunicatorPtr& Application::getCommunicator()获取线程安全的Communicator类单例。Communicator类聚合了两个比较重要的类，一个是CommunicatorEpoll，负责网络线程的建立与通过ObjectProxyFactory生成ObjectProxy；另一个是ServantProxyFactory，生成不同的RPC服务句柄，即ServantProxy，用户通过ServantProxy调用RPC服务。
根据用户配置，Communicator拥有n个网络线程，即n个CommunicatorEpoll。每个CommunicatorEpoll拥有一个ObjectProxyFactory类，每个ObjectProxyFactory可以生成一系列的不同服务的实体对象ObjectProxy，因此，假如Communicator拥有两个CommunicatorEpoll，并有foo与bar这两类不同的服务实体对象，那么如下图（1-2）所示，每个CommunicatorEpoll可以通过 ObjectProxyFactory创建两类ObjectProxy，这是TARS客户端的第一层负载均衡，每个线程都可以分担所有服务的RPC请求，因此，一个服务的阻塞可能会影响其他服务，因为网络线程是多个服务实体ObjectProxy所共享的。

![图（1-2）Communicator中的CommunicatorEpoll与ObjectProxy](https://img-blog.csdnimg.cn/20200730151817558.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L1RBUlNGb3VuZGF0aW9u,size_16,color_FFFFFF,t_70)

Communicator类下另一个比较重要的ServantProxyFactory类的作用是依据实际服务端的信息（如服务器的socket标志）与Communicator中客户端的信息（如网络线程数）而生成ServantProxy句柄，通过句柄调用RPC服务。举个例子，如下图（1-3）所示，Communicator实例通过ServantProxyFactory成员变量的getServantProxy()接口在构造fooServantProxy句柄的时候，会获取Communicator实例下的所有CommunicatorEpoll（即CommunicatorEpoll-1与CommunicatorEpoll-2）中的fooObjectProxy（即fooObjectProxy-1与fooObjectProxy-2），并作为构造fooServantProxy的参数。Communicator通过ServantProxyFactory能够获取foo与bar这两类ServantProxy，ServantProxy与相应的ObjectProxy存在相应的聚合关系：

![图（1-3）Communicator中的ServantProxyFactory与ObjectProxy](https://img-blog.csdnimg.cn/20200730151842958.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L1RBUlNGb3VuZGF0aW9u,size_16,color_FFFFFF,t_70)

另外，每个ObjectProxy都拥有一个EndpointManager，例如，fooObjectProxy 的EndpointManager管理fooObjectProxy 下面的所有fooAdapterProxy，每个AdapterProxy连接到一个提供相应foo服务的服务端物理机socket上。通过EndpointManager还可以以不同的负载均衡方式获取连接AdapterProxy。假如foo服务有两台物理机，bar服务有一台物理机，那么ObjectProxy，EndpointManager与AdapterProxy关系如下图（1-4）所示。上面提到，不同的网络线程CommunicatorEpoll均可以发起同一RPC请求，对于同一RPC服务，选取不同的ObjectProxy（或可认为选取不同的网络线程CommunicatorEpoll）是第一层的负载均衡，而对于同一个被选中的ObjectProxy，通过EndpointManager选择不同的socket连接AdapterProxy（假如ObjectProxy有大于1个的AdapterProxy，如图（1-4）的fooObjectProxy）是第二层的负载均衡。

![图（1-4）ObjectProxy与AdapterProxy的关系](https://img-blog.csdnimg.cn/20200730151903867.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L1RBUlNGb3VuZGF0aW9u,size_16,color_FFFFFF,t_70)

在客户端进行初始化时，必须建立上面介绍的关系，因此相应的类图如图（1-5）所示，通过类图可以看出各类的关系，以及初始化需要用到的函数。

![图（1-5）客户端初始化后建立的类图](https://img-blog.csdnimg.cn/20200730151927852.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L1RBUlNGb3VuZGF0aW9u,size_16,color_FFFFFF,t_70)

### 初始化代码

现在，通过代码跟踪来看看，在客户端初始化过程中，各个类是如何被初始化出来并建立上述的架构关系的。在简述之前，可以先看看函数的调用流程图，若看不清晰，可以将图片保存下来，用看图软件放大查看，强烈建议结合文章的代码解析以TARS源码一起查看，文章后面的所有代码流程图均如此。
接下来，将会按照函数调用流程图来一步一步分析客户端代理是如何被初始化出来的：

![图（1-6） 初始化函数调用过程图](https://img-blog.csdnimg.cn/20200730151950331.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L1RBUlNGb3VuZGF0aW9u,size_16,color_FFFFFF,t_70)

### 1.执行stringToProxy

在客户端程序中，一开始会执行下面的代码进行整个客户端代理的初始化：

```c++
Communicator comm;
HelloPrx prx;
comm.stringToProxy("TestApp.HelloServer.HelloObj@tcp -h 1.1.1.1 -p 20001" , prx);
```

先声明一个Communicator变量comm（其实不建议这么做）以及一个ServantProxy类的指针变量prx，在此处，服务为Hello，因此声明一个HelloPrx prx。注意一个客户端只能拥有一个Communicator。为了能够获得RPC的服务句柄，我们调用Communicator::stringToProxy()，并传入服务端的信息与prx变量，函数返回后，prx就是RPC服务的句柄。
进入Communicator::stringToProxy()函数中，我们通过Communicator::getServantProxy()来依据objectName与setName获取服务代理ServantProxy：

```c++
/**
* 生成代理
* @param T
* @param objectName
* @param setName 指定set调用的setid
* @param proxy
*/
template<class T> void stringToProxy(const string& objectName, T& proxy,const string& setName="")
{
    ServantProxy * pServantProxy = getServantProxy(objectName,setName);
    proxy = (typename T::element_type*)(pServantProxy);
}

```

### 2.执行Communicator的初始化函数

进入Communicator::getServantProxy()，首先会执行Communicator::initialize()来初始化Communicator，需要注意一点，Communicator:: initialize()只会被执行一次，下一次执行Communicator::getServantProxy()将不会再次执行Communicator:: initialize()函数：

```c++
void Communicator::initialize()
{
    TC_LockT<TC_ThreadRecMutex> lock(*this);
    if (_initialized) //已经被初始化则直接返回
        return;
    ......
｝
```

进入Communicator::initialize()函数中，在这里，将会new出上文介绍的与Communicator密切相关的类ServantProxyFactory与n个CommunicatorEpoll，n为客户端的网络线程数，最小为1，最大为MAX_CLIENT_THREAD_NUM：

```c++
void Communicator::initialize()
{
    ......
    _servantProxyFactory = new ServantProxyFactory(this);
    ......
    for(size_t i = 0; i < _clientThreadNum; ++i)
    {
        _communicatorEpoll[i] = new CommunicatorEpoll(this, i);
        _communicatorEpoll[i]->start(); //启动网络线程
    }
    ......
｝
```

在CommunicatorEpoll的构造函数中，ObjectProxyFactory被创建出来，这是构造图（1-2）关系的前提。除此之外，还可以看到获取相应配置，创建并启动若干个异步回调后的处理线程。创建完成后，调用CommunicatorEpoll::start()启动网络线程。至此，Communicator::initialize()顺利执行。通过下图回顾上面的过程：


![图（1-7）执行Communicator的初始化函数流程](https://img-blog.csdnimg.cn/20200730152028738.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L1RBUlNGb3VuZGF0aW9u,size_16,color_FFFFFF,t_70)

### 3.尝试获取ServantProxy

代码回到Communicator::getServantProxy()中 Communicator::getServantProxy()会执行ServantProxyFactory::getServantProxy()并返回相应的服务代理：

```c++
ServantProxy* Communicator::getServantProxy(const string& objectName, const string& setName)
{
    ……
    return _servantProxyFactory->getServantProxy(objectName,setName);
}
```

进入ServantProxyFactory::getServantProxy()，首先会加锁，从map<string, ServantPrx> _servantProxy中查找目标，若查找成功直接返回。若查找失败，TARS需要构造出相应的ServantProxy，ServantProxy的构造需要如图（1-3）所示的相对应的ObjectProxy作为构造函数的参数，由此可见，在ServantProxyFactory::getServantProxy()中有如下获取ObjectProxy指针数组的代码：

```c++
ObjectProxy ** ppObjectProxy = new ObjectProxy * [_comm->getClientThreadNum()];
assert(ppObjectProxy != NULL);
for(size_t i = 0; i < _comm->getClientThreadNum(); ++i)
{
    ppObjectProxy[i] = _comm->getCommunicatorEpoll(i)->getObjectProxy(name, setName);
}

```



### 4.获取ObjectProxy

代码来到ObjectProxyFactory::getObjectProxy()，同样，会首先加锁，再从map<string,ObjectProxy*> _objectProxys中查找是否已经拥有目标ObjectProxy，若查找成功直接返回。若查找失败，需要新建一个新的ObjectProxy，通过类图可知，ObjectProxy需要一个CommunicatorEpoll对象进行初始化，由此关联管理自己的CommunicatorEpoll，CommunicatorEpoll之后便可以通过getObjectProxy()接口获取属于自己的ObjectProxy。详细过程可见下图：
![图（1-8）获取ObjectProxy流程](https://img-blog.csdnimg.cn/20200730152049175.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L1RBUlNGb3VuZGF0aW9u,size_16,color_FFFFFF,t_70)

### 5.建立ObjectProxy与AdapterProxy的关系

新建ObjectProxy的过程同样非常值得关注，在ObjectProxy::ObjectProxy()中，关键代码是：

```c++
_endpointManger.reset(new EndpointManager(this, _communicatorEpoll->getCommunicator(), sObjectProxyName, pCommunicatorEpoll->isFirstNetThread(), setName));

```

每个ObjectProxy都有属于自己的EndpointManager负责管理到服务端的所有socket连接AdapterProxy，每个AdapterProxy连接到一个提供相应服务的服务端物理机socket上。通过EndpointManager还可以以不同的负载均衡方式获取与服务器的socket连接AdapterProxy。
ObjectProxy:: ObjectProxy()是图（1-6）或者图（1-8）中的略1，具体的代码流程如图（1-9）所示。ObjectProxy创建一个EndpointManager对象，在EndpointManager的初始化过程中，依据客户端提供的信息，直接创建连接到服务端物理机的TCP/UDP连接AdapterProxy或者从代理中获取服务端物理机socket列表后再创建TCP/UDP连接AdapterProxy。

![图（1-9）ObjectProxy::ObjectProxy()函数流程图](https://img-blog.csdnimg.cn/20200730152109736.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L1RBUlNGb3VuZGF0aW9u,size_16,color_FFFFFF,t_70)

按照图（1-9）的程序流程执行完成后，便会建立如图（2-3）所示的一个ObjectProxy对多个AdapterProxy的关系。
新建ObjectProxy之后，就可以调用其ObjectProxy::initialize()函数进行ObjectProxy对象的初始化了，当然，需要将ObjectProxy对象插入ObjectProxyFactory的成员变量_objectProxys与_vObjectProxys中，方便下次直接返回ObjectProxy对象。

### 6.继续完成ServantProxy的创建

退出层层的函数调用栈，代码再次回 ServantProxyFactory::getServantProxy()，此时，ServantProxyFactory已经获得相应的ObjectProxy数组ObjectProxy** ppObjectProxy，接着便可以调用：

```c++
ServantPrx sp = new ServantProxy(_comm, ppObjectProxy, _comm->getClientThreadNum());

```

进行ServantProxy的构造。构造完成便可以呈现出如图（2-1）的关系。在ServantProxy的构造函数中可以看到，ServantProxy在新建一个EndpointManagerThread变量，这是对外获取路由请求的类，是TARS为调用逻辑而提供的多种解决跨地区调用等问题的方案。同时可以看到：


```c++
for(size_t i = 0;i < _objectProxyNum; ++i)
{
   (*(_objectProxy + i))->setServantProxy(this);
}

```

建立了ServantProxy与ObjectProxy的相互关联关系。剩下的是读取配置文件，获取相应的信息。
构造ServantProxy变量完成后，ServantProxyFactory::getServantProxy()获取一些超时参数，赋值给ServantProxy变量，同时将其放进map<string, ServantPrx> _servantProxy中，方便下次直接查找获取。
ServantProxyFactory::getServantProxy()将刚刚构造的ServantProxy指针变量返回给调用他的Communicator::getServantProxy()，在Communicator::getServantProxy()中：

```c++
ServantProxy * Communicator::getServantProxy(const string& objectName,const string& setName)
{
    ……
    return _servantProxyFactory->getServantProxy(objectName,setName);
}

```

直接将返回值返回给调用起Communicator::getServantProxy()的Communicator::stringToProxy()。可以看到：

```c++
template<class T> void stringToProxy(const string& objectName, T& proxy,const string& setName="")
{
    ServantProxy * pServantProxy = getServantProxy(objectName,setName);
    proxy = (typename T::element_type*)(pServantProxy);
}

```

Communicator::stringToProxy()将返回值强制转换为客户端代码中与HelloPrx prx同样的类型HelloPrx。由于函数参数proxy就是prx的引用。那么实际就是将句柄prx成功初始化了，用户可以利用句柄prx进行RPC调用了。

## 同步调用

当我们获得一个ServantProxy句柄后，便可以进行RPC调用了。Tars提供四种RPC调用方式，分别是同步调用，异步调用，promise调用与协程调用。其中最简单最常见的RPC调用方式是同步调用，接下来，将简单分析Tars的同步调用。

现假设有一个MyDemo.StringServer.StringServantObj的服务，提供一个RPC接口是append，传入两个string类型的变量，返回两个string类型变量的拼接结果。而且假设有两台服务器，socket标识分别是192.168.106.129:34132与192.168.106.130:34132，设置客户端的网络线程数为3，那么执行如下代码：


```c++
Communicator _comm;
StringServantPrx _proxy;
_comm.stringToProxy("MyDemo.StringServer.StringServantObj@tcp -h 192.168.106.129 -p 34132", _proxy);
_comm.stringToProxy("MyDemo.StringServer.StringServantObj@tcp -h 192.168.106.130 -p 34132", _proxy);

```

经过上文关于客户端初始化的分析介绍可知，可以得出如下图所示的关系图：

![图（1-10）StringServant中ServantProxy，ObjectProxy与AdapterProxy的关系](https://img-blog.csdnimg.cn/20200730152148232.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L1RBUlNGb3VuZGF0aW9u,size_16,color_FFFFFF,t_70)

获取StringServantPrx _proxy后，直接调用：

```c++
string str1(abc-), str2(defg), rStr;
int retCode = _proxy->append(str1, str2, rStr);

```

成功进行RPC同步调用后，返回的结果是rStr = “abc-defg”。

同样，我们先看看与同步调用相关的类图，如下图所示：

![图（1-11） 客户端同步调用涉及的类图](https://img-blog.csdnimg.cn/20200730152208128.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L1RBUlNGb3VuZGF0aW9u,size_16,color_FFFFFF,t_70)

StringServantProxy是继承自ServantProxy的，StringServantProxy提供了RPC同步调用的接口Int32 append()，当用户发起同步调用_proxy->append(str1, str2, rStr)时，所进行的函数调用过程如下图所示。


![图（1-12）同步调用过程](https://img-blog.csdnimg.cn/20200730152238512.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L1RBUlNGb3VuZGF0aW9u,size_16,color_FFFFFF,t_70)

在函数StringServantProxy::append()中，程序会先构造ServantProxy::tars_invoke()所需要的参数，如请求包类型，RPC方法名，方法参数等，需要值得注意的是，传递参数中有一个ResponsePacket类型的变量，在同步调用中，最终的返回结果会放置在这个变量上。接下来便直接调用了ServantProxy::tars_invoke()方法：

```c++
tars_invoke(tars::TARSNORMAL, "append", _os.getByteBuffer(), context, _mStatus, rep);

```

在ServantProxy::tars_invoke()方法中，先创建一个ReqMessage变量msg，初始化msg变量，给变量赋值，如Tars版本号，数据包类型，服务名，RPC方法名，Tars的上下文容器，同步调用的超时时间（单位为毫秒）等。最后，调用ServantProxy::invoke()进行远程方法调用。

无论同步调用还是各种异步调用，ServantProxy::invoke()都是RPC调用的必经之地。在ServantProxy::invoke()中，继续填充传递进来的变量ReqMessage msg。此外，还需要获取调用者caller线程的线程私有数据ServantProxyThreadData，用来指导RPC调用。客户端的每个caller线程都有属于自己的维护调用上下文的线程私有数据，如hash属性，消息染色信息。最关键的还是每条caller线程与每条客户端网络线程CommunicatorEpoll进行信息交互的桥梁——**通信队列**ReqInfoQueue数组，数组中的每个ReqInfoQueue元素负责与一条网络线程进行交互，如图（1-13）所示，图中橙色阴影代表数组**ReqInfoQueue[]**，阴影内的圆柱体代表数组元素ReqInfoQueue。假如客户端create两条线程（下称caller线程）发起StringServant服务的RPC请求，且客户端网络线程数设置为2，那么两条caller线程各自有属于自己的线程私有数据请求队列数组ReqInfoQueue[]，数组里面的ReqInfoQueue元素便是该数组对应的caller线程与两条网络线程的通信桥梁，一条网络线程对应着数组里面的一个元素，通过网络线程ID进行数组索引。整个关系有点像生产者消费者模型，生产者Caller线程向自己的线程私有数据**ReqInfoQueue[]**中的第N个元素ReqInfoQueue[N] push请求包，消费者客户端第N个网络线程就会从这个队列中pop请求包。

![图（1-13）caller线程与网络线程的通信](https://img-blog.csdnimg.cn/20200730152257229.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L1RBUlNGb3VuZGF0aW9u,size_16,color_FFFFFF,t_70)

阅读代码可能会发现几个常量值，如MAX_CLIENT_THREAD_NUM=64，这是最大网络线程数，在图（1-13）中为单个请求队列数组ReqInfoQueue[]的最大size；MAX_CLIENT_NOTIFYEVENT_NUM=2048，在图（1-13）中，可以看作caller线程的最大数量，或者请求队列数组ReqInfoQueue[]的最大数量（反正两者一一对应，每个caller线程都有自己的线程私有数据ReqInfoQueue[]）。

接着依据caller线程的线程私有数据进行第一次的负载均衡——选取ObjectProxy（即选择网络线程CommunicatorEpoll）和与之相对应的ReqInfoQueue：

```c++
ObjectProxy * pObjProxy = NULL;
ReqInfoQueue * pReqQ = NULL;
//选择网络线程
selectNetThreadInfo(pSptd, pObjProxy, pReqQ);

```

在ServantProxy::selectNetThreadInfo()中，通过轮询的形式来选取ObjectProxy与ReqInfoQueue。

退出ServantProxy::selectNetThreadInfo()后，便得到ObjectProxy类型的pObjProxy及其对应的ReqInfoQueue类型的ReqInfoQueue，稍后通过pObjectProxy来发送RPC请求，请求信息会暂存在ReqInfoQueue中。

由于是同步调用，需要新建一个条件变量去监听RPC的完成，可见：

```c++
//同步调用 new 一个ReqMonitor
assert(msg->pMonitor == NULL);
if(msg->eType == ReqMessage::SYNC_CALL)
{
	msg->bMonitorFin = false;
	if(pSptd->_sched)
	{
    	msg->bCoroFlag = true;
    	msg->sched 	= pSptd->_sched;
    	msg->iCoroId   = pSptd->_sched->getCoroutineId();
	}
	else
	{
    	msg->pMonitor = new ReqMonitor;
	}
}

```

创建完条件变量，接下来往ReqInfoQueue中push_back()请求信息包msg，并通知pObjProxy所属的CommunicatorEpoll进行数据发送：

```c++
if(!pReqQ->push_back(msg,bEmpty))
{
	TLOGERROR("[TARS][ServantProxy::invoke msgQueue push_back error num:" << pSptd->_netSeq << "]" << endl);
	delete msg;
	msg = NULL;
	pObjProxy->getCommunicatorEpoll()->notify(pSptd->_reqQNo, pReqQ);
	throw TarsClientQueueException("client queue full");
}
 
pObjProxy->getCommunicatorEpoll()->notify(pSptd->_reqQNo, pReqQ);

```

来到CommunicatorEpoll::notify()中，往请求事件通知数组NotifyInfo _notify[]中添加请求事件，通知CommunicatorEpoll进行请求包的发送。注意了，这个函数的作用仅仅是通知网络线程准备发送数据，通过TC_Epoller::mod()或者TC_Epoller::add()触发一个EPOLLIN事件，从而促使阻塞在TC_Epoller::wait()（在CommunicatorEpoll::run()中阻塞）的网络线程CommunicatorEpoll被唤醒，并设置唤醒后的epoll_event中的联合体epoll_data变量为&_notify[iSeq].stFDInfo：

```c++
void CommunicatorEpoll::notify(size_t iSeq,ReqInfoQueue * msgQueue)
{
	assert(iSeq < MAX_CLIENT_NOTIFYEVENT_NUM);
 
	if(_notify[iSeq].bValid)
	{
    	_ep.mod(_notify[iSeq].notify.getfd(),(long long)&_notify[iSeq].stFDInfo, EPOLLIN);
    	assert(_notify[iSeq].stFDInfo.p == (void*)msgQueue);
	}
	else
	{
    	_notify[iSeq].stFDInfo.iType   = FDInfo::ET_C_NOTIFY;
    	_notify[iSeq].stFDInfo.p   	= (void*)msgQueue;
	    _notify[iSeq].stFDInfo.fd  	= _notify[iSeq].eventFd;
    	_notify[iSeq].stFDInfo.iSeq	= iSeq;
    	_notify[iSeq].notify.createSocket();
    	_notify[iSeq].bValid       	= true;
 
    	_ep.add(_notify[iSeq].notify.getfd(),(long long)&_notify[iSeq].stFDInfo, EPOLLIN);
	}
}
```

就是经过这么一个操作，网络线程就可以被唤醒，唤醒后通过epoll_event变量可获得&_notify[iSeq].stFDInfo。接下来的请求发送与响应的接收会在后面会详细介绍。

随后，代码再次回到ServantProxy::invoke()，阻塞于：

```c++
if(!msg->bMonitorFin)
{
	TC_ThreadLock::Lock lock(*(msg->pMonitor));
	//等待直到网络线程通知过来
	if(!msg->bMonitorFin)
   {
    	msg->pMonitor->wait();
	}
}
```

等待网络线程接收到数据后，对其进行唤醒。
接收到响应后，检查是否成功获取响应，是则直接退出函数即可，响应信息在传入的参数msg中：

```c++
if(msg->eStatus == ReqMessage::REQ_RSP && msg->response.iRet == TARSSERVERSUCCESS)
{
	snprintf(pSptd->_szHost, sizeof(pSptd->_szHost), "%s", msg->adapter->endpoint().desc().c_str());
	//成功
	return;
}
```

若接收失败，会抛出异常，并删除msg：

```c++
TarsException::throwException(ret, os.str());
```

若接收成功，退出ServantProxy::invoke()后，回到ServantProxy::tars_invoke()，获取ResponsePacket类型的响应信息，并删除msg包：

```c++
rsp = msg->response;
delete msg;
msg = NULL;
```

代码回到StringServantProxy::append()，此时经过同步调用，可以直接获取RPC返回值并回到客户端中。

### 网络线程发送请求

上面提到，当在ServantProxy::invoke()中，调用CommunicatorEpoll::notify()通知网络线程进行请求发送后，接下来，网络线程的具体执行流程如下图所示：

![图（1-14）网络线程发送请求包](https://img-blog.csdnimg.cn/20200730152329603.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L1RBUlNGb3VuZGF0aW9u,size_16,color_FFFFFF,t_70)

由于CommunicatorEpoll继承自TC_Thread，在上文1.2.2节中的第2小点的初始化CommunicatorEpoll之后便调用其CommunicatorEpoll::start()函数启动网络线程，网络线程在CommunicatorEpoll::run()中一直等待_ep.wait(iTimeout)。由于在上一节的描述中，在CommunicatorEpoll::notify()，caller线程发起了通知notify，网络线程在CommunicatorEpoll::run()就会调用CommunicatorEpoll::handle()处理通知：

```c++
void CommunicatorEpoll::run()
{
	......
    	try
    	{
        	int iTimeout = ((_waitTimeout < _timeoutCheckInterval) ? _waitTimeout : _timeoutCheckInterval);
 
        	int num = _ep.wait(iTimeout);
 
        	if(_terminate)
        	{
            	break;
        	}
 
        	//先处理epoll的网络事件
        	for (int i = 0; i < num; ++i)
        	{
            	//获取epoll_event变量的data，就是1.3.1节中提过的&_notify[iSeq].stFDInfo
            	const epoll_event& ev = _ep.get(i);
            	uint64_t data = ev.data.u64;
 
            	if(data == 0)
            	{
                	continue; //data非指针, 退出循环
            	}
            	handle((FDInfo*)data, ev.events);
        	}
    	}
	......
   
}

```

在CommunicatorEpoll::handle()中，通过传递进来的epoll_event中的data成员变量获取前面被选中的ObjectProxy并调用其ObjectProxy::invoke()函数：

```c++
void CommunicatorEpoll::handle(FDInfo * pFDInfo, uint32_t events)
{
	try
	{
    	assert(pFDInfo != NULL);
 
    	//队列有消息通知过来
    	if(FDInfo::ET_C_NOTIFY == pFDInfo->iType)
    	{
        	ReqInfoQueue * pInfoQueue=(ReqInfoQueue*)pFDInfo->p;
        	ReqMessage * msg = NULL;
 
        	try
        	{
            	while(pInfoQueue->pop_front(msg))
            	{
             	......
 
                	try
                	{
                    	msg->pObjectProxy->invoke(msg);
                	}
                	......
            	}
        	}
        	......
    	}
    	......
}

```

在ObjectProxy::invoke()中将进行第二次的负载均衡，像图（1-4）所示，每个ObjectProxy通过EndpointManager可以以不同的负载均衡方式对AdapterProxy进行选取选择：

```c++
void ObjectProxy::invoke(ReqMessage * msg)
{
	......
	//选择一个远程服务的Adapter来调用
	AdapterProxy * pAdapterProxy = NULL;
	bool bFirst = _endpointManger->selectAdapterProxy(msg, pAdapterProxy);
	......
}

```

在EndpointManager:: selectAdapterProxy()中，有多种负载均衡的方式来选取AdapterProxy，如getHashProxy()，getWeightedProxy()，getNextValidProxy()等。

获取AdapterProxy之后，便将选择到的AdapterProxy赋值给EndpointManager:: selectAdapterProxy()函数中的引用参数pAdapterProxy，随后执行：

```c++
void ObjectProxy::invoke(ReqMessage * msg)
{
   ......
	msg->adapter = pAdapterProxy;
	pAdapterProxy->invoke(msg);
}

```

调用pAdapterProxy将请求信息发送出去。而在AdapterProxy::invoke()中，AdapterProxy将调用Transceiver::sendRequset()进行请求的发送。
至此，对应同步调用的网络线程发送请求的工作就结束了，网络线程会回到CommunicatorEpoll::run()中，继续等待数据的收发。

网络线程接收响应
当网络线程CommunicatorEpoll接收到响应数据之后，如同之前发送请求那样， 在CommunicatorEpoll::run()中，程序获取活跃的epoll_event的变量，并将其中的epoll_data_t data传递给CommunicatorEpoll::handle()：

```c++
//先处理epoll的网络事件
for (int i = 0; i < num; ++i)
{
	const epoll_event& ev = _ep.get(i);
	uint64_t data = ev.data.u64;
 
	if(data == 0)
   {
   	continue; //data非指针, 退出循环
	}
	handle((FDInfo*)data, ev.events);
}

```

接下来的程序流程如下图所示：

![图（1-15）网络线程接收响应包](https://img-blog.csdnimg.cn/20200730152353147.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L1RBUlNGb3VuZGF0aW9u,size_16,color_FFFFFF,t_70)

在CommunicatorEpoll::handle()中，从epoll_data::data中获取Transceiver指针，并调用CommunicatorEpoll::handleInputImp()：

```c++
Transceiver *pTransceiver = (Transceiver*)pFDInfo->p;
//先收包
if (events & EPOLLIN)
{
	try
	{
handleInputImp(pTransceiver);
	}
	catch(exception & e)
	{
TLOGERROR("[TARS]CommunicatorEpoll::handle exp:"<<e.what()<<" ,line:"<<__LINE__<<endl);
	}
	catch(...)
	{
TLOGERROR("[TARS]CommunicatorEpoll::handle|"<<__LINE__<<endl);
	}
}

```

在CommunicatorEpoll::handleInputImp()中，除了对连接的判断外，主要做两件事，调用Transceiver::doResponse()以及AdapterProxy::finishInvoke(ResponsePacket&)，前者的工作是从socket连接中获取响应数据并判断接收的数据是否为一个完整的RPC响应包。后者的作用是将响应结果返回给客户端，同步调用的会唤醒阻塞等待在条件变量中的caller线程，异步调用的会在异步回调处理线程中执行回调函数。
在AdapterProxy::finishInvoke(ResponsePacket&)中，需要注意一点，假如是同步调用的，需要获取响应包rsp对应的ReqMessage信息，在Tars中，执行：

```c++
ReqMessage * msg = NULL;
// 获取响应包rsp对应的msg信息，并在超时队列中剔除该msg
bool retErase = _timeoutQueue->erase(rsp.iRequestId, msg);

```

在找回响应包对应的请求信息msg的同时，将其在超时队列中剔除出来。接着执行：

```c++
msg->eStatus = ReqMessage::REQ_RSP;
msg->response = rsp;
finishInvoke(msg);

```

程序调用另一个重载函数AdapterProxy::finishInvoke(ReqMessage*)，在AdapterProxy::finishInvoke(ReqMessage*)中，不同的RPC调用方式会执行不同的动作，例如同步调用会唤醒对应的caller线程：

```c++
//同步调用，唤醒ServantProxy线程
if(msg->eType == ReqMessage::SYNC_CALL)
{
	if(!msg->bCoroFlag)
	{
    	assert(msg->pMonitor);
 
    	TC_ThreadLock::Lock sync(*(msg->pMonitor));
    	msg->pMonitor->notify();
    	msg->bMonitorFin = true;
	}
	else
	{
    	msg->sched->put(msg->iCoroId);
	}
 
	return ;
}

```

至此，对应同步调用的网络线程接收响应的工作就结束了，网络线程会回到CommunicatorEpoll::run()中，继续等待数据的收发。
综上，客户端同步调用的过程如下图所示。

![图（1-16）同步调用图示](https://img-blog.csdnimg.cn/20200730152413739.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L1RBUlNGb3VuZGF0aW9u,size_16,color_FFFFFF,t_70)

## 异步调用

在Tars中，除了最常见的同步调用之外，还可以进行异步调用，异步调用可分三种：普通的异步调用，promise异步调用与协程异步调用，这里简单介绍普通的异步调用，看看其与上文介绍的同步调用有何异同。

异步调用不会阻塞整个客户端程序，调用完成（请求发送）之后，用户可以继续处理其他事情，等接收到响应之后，Tars会在异步处理线程当中执行用户实现好的回调函数。在这里，会用到《Effective C++》中条款35所介绍的“藉由Non-Virtual Interface手法实现Template Method模式”，用户需要继承一个XXXServantPrxCallback基类，并实现里面的虚函数，异步回调线程会在收到响应包之后回调这些虚函数，具体的异步调用客户端示例这里不作详细介绍，在Tars的Example中会找到相应的示例代码。


### 初始化

本文第一章已经详细介绍了客户端的初始化，这里再简单提一下，在第一章的“1.2.2初始化代码跟踪- 2.执行Communicator的初始化函数”中，已经提到说，在每一个网络线程CommunicatorEpoll的初始化过程中，会创建_asyncThreadNum条异步线程，等待异步调用的时候处理响应数据：


```c++
CommunicatorEpoll::CommunicatorEpoll(Communicator * pCommunicator,size_t netThreadSeq)
｛
 ......
   //异步线程数
	_asyncThreadNum = TC_Common::strto<size_t>(pCommunicator->getProperty("asyncthread", "3"));
 
	if(_asyncThreadNum == 0)
	{
    	_asyncThreadNum = 3;
	}
 
	if(_asyncThreadNum > MAX_CLIENT_ASYNCTHREAD_NUM)
	{
    	_asyncThreadNum = MAX_CLIENT_ASYNCTHREAD_NUM;
	}
 ......
	//异步队列的大小
	size_t iAsyncQueueCap = TC_Common::strto<size_t>(pCommunicator->getProperty("asyncqueuecap", "10000"));
	if(iAsyncQueueCap < 10000)
	{
    	iAsyncQueueCap = 10000;
	}
 ......
	//创建异步线程
	for(size_t i = 0; i < _asyncThreadNum; ++i)
	{
    	_asyncThread[i] = new AsyncProcThread(iAsyncQueueCap);
    	_asyncThread[i]->start();
	}
 ......
｝

```

在开始讲述异步调用与接收响应之前，先看看大致的调用过程，与图（1-16）的同步调用来个对比。

![图（1-17）客户端同步异步调用图示](https://img-blog.csdnimg.cn/2020073015244166.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L1RBUlNGb3VuZGF0aW9u,size_16,color_FFFFFF,t_70)

跟同步调用的示例一样，现在有一MyDemo.StringServer.StringServantObj的服务，提供一个RPC接口是append，传入两个string类型的变量，返回两个string类型变量的拼接结果。在执行tars2cpp而生成的文件中，定义了回调函数基类StringServantPrxCallback，用户需要public继承这个基类并实现自己的方法，例如：


```c++
class asyncClientCallback : public StringServantPrxCallback {
public:
  void callback_append(Int32 ret, const string& rStr) {
	cout <<  "append: async callback success and retCode is " << ret << " ,rStr is " << rStr << "\n";
  }
  void callback_append_exception(Int32 ret) {
	cout <<  "append: async callback fail and retCode is " << ret << "\n";
  }
};

```

然后，用户就可以通过proxy->async_append(new asyncClientCallback(), str1, str2)进行异步调用了，调用过程与上文的同步调用差不多，函数调用流程如下图所示，可以与图（1-12）进行比较，看看同步调用与异步调用的异同。

![图（1-18）异步调用过程](https://img-blog.csdnimg.cn/20200730152458725.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L1RBUlNGb3VuZGF0aW9u,size_16,color_FFFFFF,t_70)

在异步调用中，客户端发起异步调用_proxy->async_append(new asyncClientCallback(), str1, str2)后，在函数StringServantProxy::async_append()中，程序同样会先构造ServantProxy::tars_invoke_async()所需要的参数，如请求包类型，RPC方法名，方法参数等，与同步调用的一个区别是，还传递了承载回调函数的派生类实例。接下来便直接调用了ServantProxy::tars_invoke_async()方法：

```c++
tars_invoke_async(tars::TARSNORMAL,"append", _os.getByteBuffer(), context, _mStatus, callback)

```

在ServantProxy::tars_invoke_async()方法中，先创建一个ReqMessage变量msg，初始化msg变量，给变量赋值，如Tars版本号，数据包类型，服务名，RPC方法名，Tars的上下文容器，异步调用的超时时间（单位为毫秒）以及异步调用后的回调函数ServantProxyCallbackPtr callback（等待异步调用返回响应后回调里面的函数）等。最后，与同步调用一样，调用ServantProxy::invoke()进行远程方法调用。

在ServantProxy::invoke()中，继续填充传递进来的变量ReqMessage msg。此外，还需要获取调用者caller线程的线程私有数据ServantProxyThreadData，用来指导RPC调用。与同步调用一样，利用ServantProxy::selectNetThreadInfo()来轮询选取ObjectProxy（网络线程CommunicatorEpoll）与对应的ReqInfoQueue，详细可看同步调用中的介绍，注意区分客户端中的调用者caller线程与网络线程，以及之间的通信桥梁。

退出ServantProxy::selectNetThreadInfo()后，便得到ObjectProxy类型的pObjProxy及其对应的ReqInfoQueue类型的ReqInfoQueue，在异步调用中，不需要建立条件变量来阻塞进程，直接通过pObjectProxy来发送RPC请求，请求信息会暂存在ReqInfoQueue中：

```c++
if(!pReqQ->push_back(msg,bEmpty))
{
	TLOGERROR("[TARS][ServantProxy::invoke msgQueue push_back error num:" << pSptd->_netSeq << "]" << endl);
 
	delete msg;
	msg = NULL;
 
	pObjProxy->getCommunicatorEpoll()->notify(pSptd->_reqQNo, pReqQ);
 
	throw TarsClientQueueException("client queue full");
}
 
pObjProxy->getCommunicatorEpoll()->notify(pSptd->_reqQNo, pReqQ);

```

在之后，就不需要做任何的工作，退出层层函数调用，回到客户端中，程序可以继续执行其他动作。

### 接收响应与函数回调

异步调用的请求发送过程与同步调用的一致，都是在网络线程中通过ObjectProxy去调用AdapterProxy来发送数据。但是在接收到响应之后，通过图（1-15）可以看到，在函数AdapterProxy::finishInvoke(ReqMessage*)中，同步调用会通过msg->pMonitor->notify()唤醒客户端的caller线程来接收响应包，而在异步调用中，则是如图（1-19）所示，CommunicatorEpoll与AsyncProcThread的关系如图（1-20）所示。


![图（1-19）异步回调的收包处理](https://img-blog.csdnimg.cn/20200730152518744.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L1RBUlNGb3VuZGF0aW9u,size_16,color_FFFFFF,t_70)

![图（1-20）CommunicatorEpoll与AsyncProcThread](https://img-blog.csdnimg.cn/2020073015253918.png)

在函数AdapterProxy::finishInvoke(ReqMessage*)中，程序通过：

```c++
//异步回调，放入回调处理线程中
_objectProxy->getCommunicatorEpoll()->pushAsyncThreadQueue(msg);

```

将信息包msg（带响应信息）放到异步回调处理线程中，在CommunicatorEpoll::pushAsyncThreadQueue()中，通过轮询的方式选择异步回调处理线程处理接收到的响应包，异步处理线程数默认是3，最大是1024。

```c++
void CommunicatorEpoll::pushAsyncThreadQueue(ReqMessage * msg)
{
	//先不考虑每个线程队列数目不一致的情况
	_asyncThread[_asyncSeq]->push_back(msg);
	_asyncSeq ++;
 
	if(_asyncSeq == _asyncThreadNum)
	{
    	_asyncSeq = 0;
	}
}

```

选取之后，通过AsyncProcThread::push_back()，将msg包放在响应包队列AsyncProcThread::_msgQueue中，然后通过AsyncProcThread:: notify()函数通知本异步回调处理线程进行处理，AsyncProcThread:: notify()函数可以令阻塞在AsyncProcThread:: run()中的AsyncProcThread::timedWait()的异步处理线程被唤醒。

在AsyncProcThread::run()中，主要执行下面的程序进行函数回调：


```c++
if (_msgQueue->pop_front(msg))
{
 ......
 
	try
	{
    	ReqMessagePtr msgPtr = msg;
    	msg->callback->onDispatch(msgPtr);
	}
	catch (exception& e)
	{
    	TLOGERROR("[TARS][AsyncProcThread exception]:" << e.what() << endl);
	}
	catch (...)
	{
    	TLOGERROR("[TARS][AsyncProcThread exception.]" << endl);
	}
}

```

通过msg->callback，程序可以调用回调函数基类StringServantPrxCallback里面的onDispatch()函数。在StringServantPrxCallback:: onDispatch()中，分析此次响应所对应的RPC方法名，获取响应结果，并通过动态多态，执行用户所定义好的派生类的虚函数。通过ReqMessagePtr的引用计数，还可以将ReqNessage* msg删除掉，与同步调用不同，同步调用的msg的新建与删除都在caller线程中，而异步调用的msg在caller线程上构造，在异步回调处理线程中析构。


### 总结

TARS可以在考虑到易用性和高性能的同时快速构建系统并自动生成代码，帮助开发人员和企业以微服务的方式快速构建自己稳定可靠的分布式应用，从而令开发人员只关注业务逻辑，提高运营效率。多语言、敏捷研发、高可用和高效运营的特性使 TARS 成为企业级产品。