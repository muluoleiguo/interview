#include <bits/stdc++.h>

template<typename T>
class SharedPtr {
private:
    T* m_ptr;
    int* m_count; //reference couting 指向引用计数的指针
public:
    SharedPtr(T* ptr = nullptr) : m_ptr(ptr) {
        if (m_ptr == nullptr)
            m_count = new int(0);
        else
            m_count = new int(1);
    }
    // 这种意外的情况真的可能发生吗？
    SharedPtr(const SharedPtr& ptr) {
        if (this != &ptr) {
            this->m_ptr = ptr.m_ptr;
            this->m_count = ptr.m_count;
            (*this->m_count)++;
        }
    }

    // 移动构造
    SharedPtr(SharedPtr&& rhs) {
        this->m_ptr = rhs.m_ptr;
        this->m_count = rhs.m_count;
        rhs.m_ptr = nullptr;
        rhs.m_count = nullptr;
    }
    // 移动赋值
    SharedPtr& operator=(SharedPtr&& rhs) {
        if (m_count != rhs.m_count)
            std::swap(this, rhs);
        return *this;
    }

    SharedPtr& operator=(const SharedPtr& rhs) {
        if (this->m_ptr == rhs.m_ptr) {
            return *this;
        }
        // 考虑初始化为nullptr的SharedPtr调用operator=
        // 因为this->m_count在堆上，不管不好
        // 左计数减1
        (*this->m_count)--;
        // nullptr 的 count 为 -1 ， 但是delete nullptr可以
        if (*this->m_count <= 0) {
            delete this->m_ptr;
            delete this->m_count;
        }
        // 右计数加1
        this->m_ptr = rhs.m_ptr;
        this->m_count = rhs.m_count;
        (*this->m_count)++;
        return *this;
    }

    ~SharedPtr() {
        (*this->m_count)--;
        if (*this->m_count <= 0) {
            delete this->m_ptr;
            delete this->m_count;
        }
    }
    T& operator*() {
        assert(this->m_ptr != nullptr);
        return *(this->m_ptr);
    }

    T& operator->() {
        assert(this->m_ptr != nullptr);
        return this->m_ptr;
    }

    int useCount() const {
        return *this->m_count;
    }

    bool unique() const {
        return (*this->m_count) == 1;
    }
};
int main() {
	{

		SharedPtr<int> sp(nullptr);
		std::cout << sp.useCount() << std::endl;  //0
		SharedPtr<int> sp2(new int(10));
		sp = sp2;
		std::cout << sp.useCount() << std::endl;  //2

	}

	system("pause");
	return 0;
}
