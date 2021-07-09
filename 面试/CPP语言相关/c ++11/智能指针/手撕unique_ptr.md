简单的实现了unique_ptr，主要包括如下成员函数：

1. 构造函数
2. 析构函数
3. 拷贝构造函数，禁用，不支持
4. 拷贝赋值函数，禁用，不支持
5. reset()：释放源资源，指向新资源
6. release()：返回资源，放弃对资源的管理
7. get()：返回资源，只是供外部使用，依然管理资源
8. operator bool ():是否持有资源
9. operator * ()
10. operator -> ()


```c++
template<typename T>
class UniquePtr
{
public:
	UniquePtr(T *pResource = NULL)
		: m_pResource(pResource)
	{

	}

	~UniquePtr()
	{
		del();
	}

public:
	void reset(T* pResource) // 先释放资源(如果持有), 再持有资源
	{
		del();
		m_pResource = pResource;
	}

	T* release() // 返回资源，资源的释放由调用方处理
	{
		T* pTemp = m_pResource;
		m_pResource = nullptr;
		return pTemp;
	}

	T* get() // 获取资源，调用方应该只使用不释放，否则会两次delete资源
	{
		return m_pResource;
	}

public:
	operator bool() const // 是否持有资源
	{
		return m_pResource != nullptr;
	}

	T& operator * ()
	{
		return *m_pResource;
	}

	T* operator -> ()
	{
		return m_pResource;
	}

private:
	void del()
	{
		if (nullptr == m_pResource) return;
		delete m_pResource;
		m_pResource = nullptr;
	}

private:
	UniquePtr(const UniquePtr &) = delete; // 禁用拷贝构造
	UniquePtr& operator = (const UniquePtr &) = delete; // 禁用拷贝赋值

private:
	T *m_pResource;
};

```
