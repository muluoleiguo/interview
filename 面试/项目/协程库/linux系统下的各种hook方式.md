一、linux系统下的各种hook方式
在计算机中,基本所有的软件程序都可以通过hook方式进行行为拦截,hook方式就是改变原始的执行流,

Linux平台上常见的拦截：

* 修改函数指针。
* 用户态动态库拦截。
* 内核态系统调用拦截。
* 堆栈式文件系统拦截。
* LSM(Linux Security Modules)

### 函数指针hook

C语言的一项强大的功能就是指针,指针代表一个地址,而函数指针就是指向一个函数地址的指针,通过函数指针来指向不同的函数地址控制执行流.

一般这类函数指针存在于软件运行的整个周期中,要实施这类hook首先就是找到关键的函数指针,之后就和普通的指针修改一样进行改变就OK.

函数指针的使用形式:

```c
//函数定义
func() {
....
}
//函数指针
fun = func
//函数指针调用
fun()

```

通过修改函数指针进行hook:

```c
origin_fun = fun;	//保存原始函数指针
f = hook_func;		//函数指针指向hook函数
hook_func() {
	...
	origin_fun();	//一般都会再次调用原始函数来完成实际功能
}

```

### 动态库劫持

Linux上的动态库劫持主要是基于LD_ PRELOAD环境变量，这个环境变量的主要作用是改变动态库的加载顺序，让用户有选择的载入不同动态库中的相同函数。但是使用不当就会引起严重的安全问题，我们可以通过它在主程序和动态连接库中加载别的动态函数，这就给我们提供了一个机会，向别人的程序注入恶意的代码。

> 为了安全起见，一般将LD_ PRELOAD环境变量禁用掉。

### Linux系统调用劫持 hook

系统调用劫持的目的是改变系统中原有的系统调用，用我们自己的程序替换原有的系统调用。**Linux内核中所有的系统调用都是放在一个叫做sys_ call _table的内核数组**中，**数组的值就表示这个系统调用服务程序的入口地址**。整个系统调用的流程如下：

![在这里插入图片描述](https://img-blog.csdnimg.cn/934790631fc643359b1b606ab1873d3f.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_Q1NETiBA6KW_5Lqs5YiA5a6i,size_34,color_FFFFFF,t_70,g_se,x_16)

当用户态发起一个系统调用时，会通过80软中断进入到syscall hander，进而进入全局的系统调用表sys_ call _table去查找具体的系统调用，那么**如果我们将这个数组中的地址改成我们自己的程序地址，就可以实现系统调用劫持。但是内核为了安全，对这种操作做了一些限制**：

* `sys_ call _table`的符号没有导出，不能直接获取。
* `sys_ call _table`所在的内存页是只读属性的，无法直接进行修改。

对于以上两个问题，解决方案如下（方法不止一种）：

?没搞明白

获取sys call table的地址 ：grep sys_call_table /boot/ -r

控制页表只读属性是由CR0寄存器的WP位控制的，只要将这个位清零就可以对只读页表进行修改

开始替换系统调用
本文实现的是对 ls这个命令对应的系统调用，系统调用号是 _ NR _getdents。

在linux内核中,比较常见并且关键的就是系统调用表,系统调用表实际上是指针数组,下标是系统调用号,应用层发起的所有活动基本上都绕不过系统调用,控制了系统调用表基本上就是控制了整个系统.

hook只需要两个信息:系统调用表的起始地址和系统调用号,之后就能像修改指针一样进行劫持.




## 4. 堆栈式文件系统

Linux通过vfs虚拟文件系统来统一抽象具体的磁盘文件系统，从上到下的IO栈形成了一个堆栈式。通过对内核源码的分析，以一次读操作为例，从上到下所执行的流程如下：

![在这里插入图片描述](https://img-blog.csdnimg.cn/f96d7fb43a4d4d6e82739f9f8a305632.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_Q1NETiBA6KW_5Lqs5YiA5a6i,size_34,color_FFFFFF,t_70,g_se,x_16)

内核中采用了很多c语言形式的面向对象，也就是函数指针的形式，例如read是vfs提供用户的接口，具体底下调用的是ext2的read操作。我们只要实现VFS提供的各种接口，就可以实现一个堆栈式文件系统。Linux内核中已经集成了一些堆栈式文件系统。例如Ubuntu在安装时会提醒你是否需要加密home目录，其实就是一个堆栈式的加密文件系统（eCryptfs），原理如下
![在这里插入图片描述](https://img-blog.csdnimg.cn/770e7ec550bd4b39b2d088eef99731db.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_Q1NETiBA6KW_5Lqs5YiA5a6i,size_34,color_FFFFFF,t_70,g_se,x_16)

实现了一个堆栈式文件系统，相当于所有的读写操作都会进入到我们的文件系统，可以拿到所有的数据，就可以进行做一些拦截过滤。

> 堆栈式文件系统，依赖于Mount,可能需要重启系统。

## 5. LSM

LSM是Linux Secrity Module的简称，即linux安全模块。是一种通用的Linux安全框架，具有效率高，简单易用等特点。原理如下：

![在这里插入图片描述](https://img-blog.csdnimg.cn/f3dfaf4887bc4139aeb33ee818bb3449.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_Q1NETiBA6KW_5Lqs5YiA5a6i,size_34,color_FFFFFF,t_70,g_se,x_16)

在内核中做了以下工作：

- 在特定的内核数据结构中加入安全域。
- 在内核源代码中不同的关键点插入对安全钩子函数的调用。
- 加入一个通用的安全系统调用。
- 提供了函数允许内核模块注册为安全模块或者注销。
- 将capabilities逻辑的大部分移植为一个可选的安全模块,具有可扩展性。