一般在面试时往往要求实现出String类的构造函数、拷贝构造函数、赋值函数以及析构函数


```c++

class String
{
public:
     String(const char *str = NULL);// 普通构造函数
     String(const String &other);    // 拷贝构造函数
     ~ String(void);    // 析构函数
     String & operate =(const String &other);// 赋值函数
private:
     char *m_data;// 用于保存字符串
}; 

//普通构造函数
String::String(const char *str)
{
        if(str==NULL)
        {
                m_data = new char[1]; 
                *m_data = '\0';
        }    
        else
        {
         int length = strlen(str);
         m_data = new char[length+1]; 
         strcpy(m_data, str);
        }
} 
// String的析构函数
String::~String(void)
{
        delete [] m_data; // 或delete m_data;
}
//拷贝构造函数
String::String(const String &other) 　　　// 输入参数为const型
{     
        int length = strlen(other.m_data);
        m_data = new char[length+1]; 　　　　
        strcpy(m_data, other.m_data);    
} 
//赋值函数
String & String::operate =(const String &other) // 输入参数为const型
{     
        if(this == &other)                  　　//检查自赋值
                return *this;   
        delete [] m_data;            　　　　//释放原有的内存资源
        int length = strlen( other.m_data );      
        m_data = new char[length+1]; 　
        strcpy( m_data, other.m_data );   
        return *this;     　　　　　　　　//返回本对象的引用  

}
```
总结
在这个类中包括了指针类成员变量m_data，当类中包括指针类成员变量时，一定要重载其拷贝构造函数、赋值函数和析构函数。