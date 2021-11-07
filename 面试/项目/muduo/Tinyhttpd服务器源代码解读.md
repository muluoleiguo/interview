# Tinyhttpd服务器源代码解读(理解CGI)

Tinyhttpd服务器是一个轻量级的HTTP服务器。代码简短易读，源代码只有500多行，是一个学习unix服务器编程的极佳入门。它的一个github地址为：[Tinyhttpd](https://github.com/EZLippi/Tinyhttpd)。

Tinyhttpd服务器只有一个核心的c源码文件：httpd.c. 其中包含12个主要的函数。 分别为    

```c
void accept_request(int);//对有效请求的响应函数
void bad_request(int);//对无效请求的响应函数
void cat(int, FILE *);//将一个文件中的内容全部放到socket中，其实socket本身就是一个特殊的文件描述符，因此可以简单看成是文件之间的复制
void cannot_execute(int);//通知客户端，该cgi脚本不能运行
void error_die(const char *);//错误处理函数，其实就是打印错误信息，然后结束程序
void execute_cgi(int, const char *, const char *, const char *);//运行CGI脚本的程序
int get_line(int, char *, int);//从socket中获取一行数据，至始至终都把socket当成一个普通文件的文件描述符
void headers(int, const char *);//这个是向客户端发送http文件头的函数
void not_found(int);//404响应函数
void serve_file(int, const char *);//将文件直接发送到client端
int startup(u_short *);//socket初始化函数，他将返回服务器端的socket文件描述符
void unimplemented(int);//tinyhttp只支持“GET”和“POST”方法，如果发现method等于其他方法，则执行此函数
```

我们当然是先从main函数看起，他是服务器进程的源头，也是服务器进程的主循环所在。

```c
int main(void)
{
 int server_sock = -1;
 u_short port = 0;
 int client_sock = -1;
 struct sockaddr_in client_name;
 int client_name_len = sizeof(client_name);
 pthread_t newthread;
 
 server_sock = startup(&port);
 printf("httpd running on port %d\n", port);
 
 while (1)
 {
  client_sock = accept(server_sock,
                       (struct sockaddr *)&client_name,
                       &client_name_len);
  if (client_sock == -1)
   error_die("accept");
 /* accept_request(client_sock); */
 if (pthread_create(&newthread , NULL, accept_request, client_sock) != 0)
   perror("pthread_create");
 }
 
 close(server_sock);
 
 return(0);
}
```

前面都是很简单的，无非就是初始化socket，绑定地址和端口，然后用listen请求接收服务。前面这些都是在startup函数中完成。初始化完成之后，就重新回到main函数，然后进入主循环，主循环之后，服务器等待客户端的请求。如果客户端没有请求到来，则服务器进程处于阻塞状态。当服务器端接收到客户端的请求时，服务器端的main进程用
pthread_create(&newthread , NULL, accept_request, client_sock)

函数开启一个新的线程。该进程将执行accept_request函数，并一client_sock为参数。其中client_sock是发送请求的客户端的socket，通过这个socket文件描述符就可以读客户端发送过来的信息，就像读普通文件一样。废话少说，虾米那看一下accept_request函数的具体实现。它是整个服务器的核心之一。也是通过tinyhttpd学习HTTP服务器编程的主要。

下面是accept_request函数代码

```c
void accept_request(int client)
{
 char buf[1024];
 int numchars;
 char method[255];
 char url[255];
 char path[512];
 size_t i, j;
 struct stat st;
 int cgi = 0;      /* becomes true if server decides this is a CGI
                    * program */
 char *query_string = NULL;
 
 numchars = get_line(client, buf, sizeof(buf));
//printf("%s\n",buf);
 i = 0; j = 0;
 while (!ISspace(buf[j]) && (i < sizeof(method) - 1))
 {
  method[i] = buf[j];
  i++; j++;
 }
 method[i] = '\0';
 
// printf("%s\n",method);
 
 if (strcasecmp(method, "GET") && strcasecmp(method, "POST"))
 {
  unimplemented(client);
  return;
 }
 
 if (strcasecmp(method, "POST") == 0)
  cgi = 1;
 
 i = 0;
 while (ISspace(buf[j]) && (j < sizeof(buf)))
  j++;
 while (!ISspace(buf[j]) && (i < sizeof(url) - 1) && (j < sizeof(buf)))
 {
  url[i] = buf[j];
  i++; j++;
 }
 url[i] = '\0';
 
// printf("%s\n",url);
//exit(1);
 if (strcasecmp(method, "GET") == 0)
 {
  query_string = url;
  while ((*query_string != '?') && (*query_string != '\0'))
   query_string++;
  if (*query_string == '?')
  {
   cgi = 1;
   *query_string = '\0';
   query_string++;
  }
 }
//printf("%s\n",query_string);
 sprintf(path, "htdocs%s", url);// after this function, path = htdoc/, cause url = \
 if (path[strlen(path) - 1] == '/')
  strcat(path, "index.html"); // after this function, path = htdoc/index.html
 if (stat(path, &st) == -1) {//can not find htdoc/index.html
  while ((numchars > 0) && strcmp("\n", buf))  /* read & discard headers */
   numchars = get_line(client, buf, sizeof(buf));
  not_found(client);
 }
 else
 {
  if ((st.st_mode & S_IFMT) == S_IFDIR)// the attributes of the file
   strcat(path, "/index.html");
  if ((st.st_mode & S_IXUSR) ||
      (st.st_mode & S_IXGRP) ||
      (st.st_mode & S_IXOTH)    )
   cgi = 1;
  if (!cgi)
   serve_file(client, path);
  else
   execute_cgi(client, path, method, query_string);
 }
 
 close(client);
}
```

该函数首先通过`get_line(int,char*,int)`函数读取客户端socket中的一行，也就是客户端发送的http请求中的第一行。我们可以通过打印信息了解这一行的具体内容。其实根据HTTP协议可以知道此行的形式为：GET / HTTP1.1 或者POST / HTTP1.1 。 所以通过这一行，我们可以知道请求的方式(method)。需要注意TinyHttpd只识别GET和POST两种格式的请求，这也是为什么后面需要对method进行合法性判断。
获取到该请求的method之后，**就需要知道客户端请求的文件在服务器端的路径，并判断该请求是否包含有参数**，**如果请求包含有参数则需要传递给后端的CGI脚本对参数进行处理，以根据参数返回相应的页面**。后面那一部分就是一个组合文件路径以及对文件的属性进行判断的过程。然后根据文件判断是否该执行脚本来相应请求。(比如如果是jsp文件，那就需要运行CGI来相应请求，如果是静态的html文件，则服务器直接将文件写到client  socket即可。

所以下面是两个主要的函数，分别是服务器端直接将文件返回给客户端和服务器端通过CGI后，再将文件返回到客户端。这两个函数分别是serve_file(cilent)和execute_cgi(client,path,mechod,query_string)。第一个函数比较简单，先来看它


```c
void serve_file(int client, const char *filename)
{
 FILE *resource = NULL;
 int numchars = 1;
 char buf[1024];
 
 buf[0] = 'A'; buf[1] = '\0';
 while ((numchars > 0) && strcmp("\n", buf))  /* read & discard headers */
  numchars = get_line(client, buf, sizeof(buf));
 
 resource = fopen(filename, "r");
 if (resource == NULL)
  not_found(client);
 else
 {
  headers(client, filename);
  cat(client, resource);
 }
 fclose(resource);
}
```

serve_file的参数很简单，**客户端的socket**和**请求的文件的文件名**。
servefile主要分为两部分，一个是headers(), 一个是cat()。header主要是向客户端发送http头，cat则是通过client socket向服务器端写文件。

下面是execute_cgi函数，这应该是tinyhttp中最难的地方了吧。

```c
void execute_cgi(int client, const char *path,
                 const char *method, const char *query_string)
{
 char buf[1024];
 int cgi_output[2];
 int cgi_input[2];
 pid_t pid;
 int status;
 int i;
 char c;
 int numchars = 1;
 int content_length = -1;
 
 buf[0] = 'A'; buf[1] = '\0';
 if (strcasecmp(method, "GET") == 0)
  while ((numchars > 0) && strcmp("\n", buf))  /* read & discard headers */
   numchars = get_line(client, buf, sizeof(buf));
 else    /* POST */
 {
  numchars = get_line(client, buf, sizeof(buf));
  while ((numchars > 0) && strcmp("\n", buf))
  {
   buf[15] = '\0';
   if (strcasecmp(buf, "Content-Length:") == 0)
    content_length = atoi(&(buf[16]));
   numchars = get_line(client, buf, sizeof(buf));
  }
  if (content_length == -1) {
   bad_request(client);
   return;
  }
 }
 
 sprintf(buf, "HTTP/1.0 200 OK\r\n");
 send(client, buf, strlen(buf), 0);
 
 if (pipe(cgi_output) < 0) {
  cannot_execute(client);
  return;
 }
 if (pipe(cgi_input) < 0) {
  cannot_execute(client);
  return;
 }
 
 if ( (pid = fork()) < 0 ) {
  cannot_execute(client);
  return;
 }
 if (pid == 0)  /* child: CGI script */
 {
  char meth_env[255];
  char query_env[255];
  char length_env[255];
 
  dup2(cgi_output[1], 1);
  dup2(cgi_input[0], 0);
  close(cgi_output[0]);
  close(cgi_input[1]);
  sprintf(meth_env, "REQUEST_METHOD=%s", method);
  putenv(meth_env);
  if (strcasecmp(method, "GET") == 0) {
   sprintf(query_env, "QUERY_STRING=%s", query_string);
   putenv(query_env);
  }
  else {   /* POST */
   sprintf(length_env, "CONTENT_LENGTH=%d", content_length);
   putenv(length_env);
  }
  execl(path, path, NULL);
  exit(0);
 } else {    /* parent */
  close(cgi_output[1]);
  close(cgi_input[0]);
  if (strcasecmp(method, "POST") == 0)
   for (i = 0; i < content_length; i++) {
    recv(client, &c, 1, 0);
    write(cgi_input[1], &c, 1);
   }
  while (read(cgi_output[0], &c, 1) > 0)
   send(client, &c, 1, 0);
 
  close(cgi_output[0]);
  close(cgi_input[1]);
  waitpid(pid, &status, 0);
 }
}
```

这个函数的主要功能是开**启一个新的进程运行CGI程序**，以处理客户端的请求。**其中CGI程序和父进程主要通过管道通信**。父进程，也就是main函数中的accept_request线程，向其子进程，即CGI程序传送客户端的请求参数，CGI通过一定的处理后，通过管道向父进程传递最终需要传递给客户端的数据，最后main进程的accept_request线程向客户端传递最终的数据。

tinyhttp是一个非常简单的http服务器，用c语言编写。是我进入linux后台开发的起点。

