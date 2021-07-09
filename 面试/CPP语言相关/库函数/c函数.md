# 请你说一说strcpy和strlen
strcpy是字符串拷贝函数，原型：

```
char *strcpy(char* dest, const char *src);
```


从src逐字节拷贝到dest，直到遇到'\0'结束，因为没有指定长度，可能会导致拷贝越界，造成缓冲区溢出漏洞,安全版本是strncpy函数。
strlen函数是计算字符串长度的函数，返回从开始到'\0'之间的字符个数。

c++随机数 rand()产生，使用srand(种子)设置随机种子
* 产生0~n的随机数：

```
rand()%(n+1)
```
* 产生n~m的随机数

```
rand()%(m-n+1)+n
```

* 产生浮点数

```
rand()
```


# strcpy 和strncpy 的代码和区别？（这里你要说本质区别，就是strcpy不安全，有可能cpy越界）
strcpy() 函数用来复制字符串，其原型为：

char *strcpy(char *dest, const char *src);

【参数】dest 为目标字符串指针，src 为源字符串指针。

注意：src 和 dest 所指的内存区域不能重叠，且dest 必须有足够的空间放置 src 所包含的字符串（包含结束符NULL）。

【返回值】成功执行后返回目标数组指针 dest。

strcpy() 把src所指的由NULL结束的字符串复制到dest 所指的数组中，返回指向 dest 字符串的起始地址。

注意：如果参数 dest 所指的内存空间不够大，可能会造成缓冲溢出(buffer Overflow)的错误情况，在编写程序时请特别留意，或者用strncpy()来取代。

strncpy()用来复制字符串的前n个字符，其原型为：

char * strncpy(char *dest, const char *src, size_t n);

【参数说明】dest 为目标字符串指针，src 为源字符串指针。

strncpy()会将字符串src前n个字符拷贝到字符串dest。

不像strcpy()，strncpy()不会向dest追加结束标记'\0'.

如果src的前n个字节不含NULL字符，则结果不会以NULL字符结束。

如果src的长度小于n个字节，则以NULL填充dest直到复制完n个字节。

src和dest所指内存区域不可以重叠且dest必须有足够的空间来容纳src的字符串。

注意：src 和 dest 所指的内存区域不能重叠，且dest 必须有足够的空间放置n个字符。

【返回值】返回指向dest的指针（该指向dest的最后一个元素）

# strcpy

已知strcpy的原型是char * strcpy(char *strDest, const char *strSrc);

(1)实现strcpy函数

(2)解释为什么要返回char*

(3)假如考虑strDest和strSrc内存重叠的情况，strcpy该怎么实现

#### 一、strcpy的代码实现

```c++
char * strcpy(char *strDest, const char *strSrc) 
{
    assert((strDest != NULL) || (strSrc != NULL));  
 char *address = strDest;					
 while((*strDest++ = *strSrc++)!='\0');		
 return address;
}
```

注意：strDest和strSrc是形参作用域和生命周期仅限于函数体内，虽然*strDest++ = *strSrc++ 一直加加导致指向结束符'\0',但实参指针的地址依然未变。

**[1]const修饰**

源字符串参数用const修饰，防止修改源字符串。

**[2]空指针检查**

断言assert是仅在Debug版本起作用的宏，用于检查“不应该发生”的情况，在运行过程中，若assert的参数为假,那么程序会发生中止。如果程序在assert处终止了，并不是说含有该assert的函数有错误，而是调用者出了差错。

#### 二、为什么要返回char *？
返回strDest的原始值使函数能够支持链式表达式。

链式表达式的形式如：


```
int l=strlen(strcpy(strA,strB));
```
#### 三、假如考虑dst和src内存重叠的情况，strcpy该怎么实现

```c++
char s[10]="hello";

strcpy(s, s+1); //应返回ello，

//strcpy(s+1, s); //应返回hhello，但实际会报错，因为dst与src重叠了，把'\0'覆盖了
```


所谓重叠，就是src未处理的部分已经被dst给覆盖了，只有一种情况：

```
src<dst<=src+strlen(src)-1
```
![image](https://pic3.zhimg.com/80/v2-a157d07599c3ec2e797a25df3262a416_1440w.jpg)
(1）src+count<=dst或dst+count<=src即不重叠，这时正常从低字节依次赋值

(2）dst+count>src >dst 这时部分重叠，但不影响赋值拷贝，也可以从低字节开始

(3）src+count>dst>src 这时也是部分重叠，但影响拷贝，应该从高字节逆序开始。

```c++
char * strcpy(char *strDest, const char *strSrc)
{
     assert((strDest!=NULL)||(strSrc != NULL));
     char *address = strDest;
     int size = strlen(strDest)+1;
     if((strDest >strSrc) &&(strDest <= strSrc + size -1)) //内存重叠，从尾往前复制
        {
            strSrc = strSrc + size -1;
            strDest = strDest +size -1;
     while(size--)
        {
            *strDest-- = *strSrc--;
        }
    }
     else//正常情况，从头往后复制
    {
     while(size--)
        {
            *strDest++ = *strSrc++;
        }
    }
 return address;
}
```

# strcpy和memcpy
已知strcpy函数的原型是：char* strcpy(char* dest, const char* src);

memcpy提供了一般内存的复制。即memcpy对于需要复制的内容没有限制，因此用途更广。

void * memcpy ( void * destination, const void * source, size_t num );

strcpy和memcpy主要有以下3方面的区别。

(1)复制的内容不同。strcpy只能复制字符串，而memcpy可以复制任意内容，例如字符数组、整型、结构体、类等。

(2)复制的方法不同。strcpy不需要指定长度，它遇到被复制字符的串结束符"\0"才结束，所以容易溢出。memcpy则是根据其第3个参数决定复制的长度。

(3)用途不同。通常在复制字符串时用strcpy，而需要复制其他类型数据时则一般用memcpy

```c++
void *memcpy(void *pvTo, const void *pvFrom, size_t size)
{
    assert((pvTo != NULL) && (pvFrom != NULL));
    char *pbTo = (char *)pvTo;//因为memcpy的size是byte字节
    char *pbFrom = (char *)pvFrom;
    while(size-- > 0)
    {
        *pbTo ++ = *pbFrom ++;
    }
    return pvTo;
}
```
# memcpy和memove
memcpy，strcpy这两个函数没有对内存重叠进行处理。使用这两个函数的时候只有程序员自己保证源地址与目标地址内存不重叠。或是使用memmov函数(对内存重叠进行了处理)进行拷贝内存。