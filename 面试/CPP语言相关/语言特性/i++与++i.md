### 请你来回答一下++ i和i++ 的区别

++ i先自增1，再返回，i ++先返回i,再自增1
### 请你来说一说 ++i和i ++的实现
1. ++ i 实现：

```c++
int&  int::operator++（）
{
*this +=1；
return *this；
}
```
2.  i ++ 实现：

```c++
const int  int::operator（int）
{
int oldValue = *this；
++（*this）；
return oldValue；
}
```


### i ++是否原子操作?并解释为什么?
i++的操作分三步：

1. 栈中取出i
2. i自增1
3. 将i存到栈

所以i++不是原子操作，上面的三个步骤中任何一个步骤同时操作，都可能导致i的值不正确自增