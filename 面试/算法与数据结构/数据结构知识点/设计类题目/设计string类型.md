```c++
class String {
private:
	char* m_data;
    int m_size;
public:
    // 默认构造函数
	String(const char* str = nullptr);
    // 拷贝构造函数
	String(const String& other);
	String(String&& other);
    // 字符串赋值函数
	String& operator=(const String& other);
	String& operator=(String&& other);
    // 析构函数
	~String();
	int length() { return strlen(data); }
	/*
	其它功能
	*/
};
// 构造函数
String::String(const char* str) {
    // 这一步判断是否为nullptr加分点
	if (str == nullptr) {
		m_data = new char[1];
        m_data[0] = '\0';
        m_size = 0;
	}
	else {
		m_size = strlen(str);
        m_data = new char[m_size + 1];
        strcpy(m_data, str);
	}
}
// 拷贝构造函数, 得分点：输入参数是const
String::String(const String& other) {
	m_size = other.m_size;
    m_data = new char[m_size + 1];
    strcpy(m_data, other.m_data);
}
// 移动构造函数
String::String(String&& other) {
	m_data = other.m_data;
	other.m_data = nullptr;
}
// 赋值，注意形参const ,注意返回引用
String& String::operator=(const String& other) {
	// 得分点1：检查自赋值
    if (this == &other)
        return *this;
    // 得分点2：释放原有的内存资源
    delete[] m_data;
    m_size = other.m_size;
    m_data = new char[m_size + 1];
    strcpy(m_data, other.m_data);
    return *this; // 返回本对象的引用
}
// 移动赋值
String& String::operator=(String&& other) {
	if (this == &other) 
        return *this;
	delete[] m_data;
	m_data = other.m_data;
	other.m_data = nullptr;
	return *this;
}
// 析构
String::~String() {	
	delete[] m_data;
}
```



需要注意的点：

delete[] nullptr会发生什么问题？试一试:一切正常

```c++
int main()
{
    int* p = new int[1];
    delete[] p;
    p = nullptr;
    delete[] p;
    return 0;
}
```

