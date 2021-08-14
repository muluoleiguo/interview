### memcpy和memove

memcpy，strcpy这两个函数没有对内存重叠进行处理。使用这两个函数的时候只有程序员自己保证源地址与目标地址内存不重叠。或是使用memmov函数(对内存重叠进行了处理)进行拷贝内存。

？？不知道？？

似乎 memmove才会考虑内存覆盖的问题。

```c++
void *memcpy(void *dst, const void *src, size_t size)
{
    char *psrc;
    char *pdst;

    if (NULL == dst || NULL == src)
    {
        return NULL;
    }

    if ((src < dst) && (char *)src + size > (char *)dst) // 出现地址重叠的情况，自后向前拷贝
    {
        psrc = (char *)src + size - 1;
        pdst = (char *)dst + size - 1;
        while (size--)
        {
            *pdst-- = *psrc--;
        }
    }
    else
    {
        psrc = (char *)src;
        pdst = (char *)dst;
        while (size--)
        {
            *pdst++ = *psrc++;
        }
    }

    return dst;
}


```

### strcpy和memcpy

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

