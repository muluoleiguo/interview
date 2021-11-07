```c++
#include <iostream>
using namespace std;
template<typename T>
class SharedPtr {
private:
    int* m_cnt;
    T* m_ptr;
public:
    SharedPtr(T* ptr) : m_cnt(new int(0)) {
        if (ptr != nullptr) {
            m_ptr = ptr;
            *m_cnt = 1;
        }        
    }
    // 拷贝构造
    SharedPtr(SharedPtr& rhs) {
        *(rhs.m_cnt)++;
        this->m_cnt = rhs.m_cnt;
        this->m_ptr = rhs.m_ptr;
    }
    
    // 移动构造
    SharedPtr(SharedPtr&& rhs) {
        this->m_cnt = rhs.m_cnt;
        this->m_ptr = rhs.m_ptr;
        rhs.m_ptr = nullptr;
    }
    
    // 拷贝赋值
    SharedPtr& operator=(SharedPtr& rhs) {
        if (rhs == *this)
            return *this;
        this->m_ptr = rhs.m_ptr;
        *(rhs.m_cnt)++;
        this->m_cnt = rhs.m_cnt;
        return *this;
    }
    // 移动赋值
    SharedPtr& operator=(SharedPtr&& rhs) {
        if (rhs == *this)
            return *this;
        this->m_cnt = rhs->m_cnt;
        this->m_ptr = rhs.m_ptr;
        rhs.m_ptr = nullptr;
        return *this;
    }
    
    //析构函数
    ~SharedPtr() {
        *(this->m_cnt)--;
        free();
    }
    
    // 清理资源
    void free() {
        if (*(this->m_cnt) <= 0) {
            delete this->m_ptr;
            delete this->m_cnt;
        }
    }
    
    // 解引用重载操作符号
    T operator->() {
        return *(this->m_ptr);
    }
    
    // 重载*
    T* operator*() {
        return m_ptr;
    }
    // 返回引用计数的数量
    int useCount() {
        return *(this->m_cnt);
    }   
};


int main() {
    //int a;
    //cin >> a;
    
    // 测试析构和引用计数
    {
        SharedPtr<int> sp1(new int(3));
        cout << sp1.useCount();
    }
    
    // 测试拷贝构造
    {
        SharedPtr<int> sp2(new int (4));
        SharedPtr<int> sp3(sp2);
        cout << sp2.useCount();
    }
    
    // 测试移动构造
    {
        SharedPtr<int> sp4(new int (5));
        SharedPtr<int> sp5(move(sp4));
        cout << sp5.useCount();
    }
    
    // 测试拷贝赋值和移动赋值
    {
        SharedPtr<int> sp6(new int (6));
        SharedPtr<int> sp7 = sp6;
        cout << sp7.useCount();
        SharedPtr<int> sp8 = move(sp7);
        cout << sp8.useCount();
    }
      
    // cout << "Hello World!" << endl;
}
```

