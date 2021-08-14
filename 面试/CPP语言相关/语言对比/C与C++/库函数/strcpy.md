面试高频指数：★★☆☆☆

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

# 

### strcpy 函数的缺陷：

strcpy 函数不检查目的缓冲区的大小边界，而是将源字符串逐一的全部赋值给目的字符串地址起始的一块连续的内存空间，同时加上字符串终止符，会导致其他变量被覆盖。


```c++
#include <iostream>
#include <cstring>
using namespace std;

int main()
{
    int var = 0x11112222;
    char arr[10];
    cout << "Address : var " << &var << endl;
    cout << "Address : arr " << &arr << endl;
    strcpy(arr, "hello world!");
    cout << "var:" << hex << var << endl; // 将变量 var 以 16 进制输出
    cout << "arr:" << arr << endl;
    return 0;
}

/*
Address : var 0x23fe4c
Address : arr 0x23fe42
var:11002164
arr:hello world!
*/
```

说明：从上述代码中可以看出，变量 var 的后六位被字符串` "hello world!" `的 `"d!\0"` 这三个字符改变，这三个字符对应的 ascii 码的十六进制为：`\0`(0x00)，`!`(0x21)，`d`(0x64)。
原因：变量 arr 只分配的 10 个内存空间，通过上述程序中的地址可以看出 arr 和 var 在内存中是连续存放的，但是在调用 strcpy 函数进行拷贝时，源字符串 "hello world!" 所占的内存空间为 13，因此在拷贝的过程中会占用 var 的内存空间，导致 var的后六位被覆盖。

### strcpy 和strncpy 的代码和区别？（这里你要说本质区别，就是strcpy不安全，有可能cpy越界）

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
