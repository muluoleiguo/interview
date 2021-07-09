//
// Created by jxq on 19-8-17.
//

#include <iostream>
#include <boost/smart_ptr.hpp>
#include <boost/weak_ptr.hpp>

using namespace std;
using namespace boost;

class BB;
class AA
{
public:
    AA() { cout << "AA::AA() called" << endl; }
    ~AA() { cout << "AA::~AA() called" << endl; }
    boost::weak_ptr<BB> m_bb_ptr;  //!
};

class BB
{
public:
    BB() { cout << "BB::BB() called" << endl; }
    ~BB() { cout << "BB::~BB() called" << endl; }
    boost::shared_ptr<AA> m_aa_ptr; //!
};
boost::shared_ptr<AA> ptr_a (new AA);

int main()
{
    {
        //boost::shared_ptr<AA> ptr_a (new AA);
        boost::shared_ptr<BB> ptr_b(new BB);
        cout << "ptr_a use_count: " << ptr_a.use_count() << endl;
        cout << "ptr_b use_count: " << ptr_b.use_count() << endl;
        //下面两句导致了AA与BB的循环引用，结果就是AA和BB对象都不会析构
        ptr_a->m_bb_ptr = ptr_b;    // b不变
        ptr_b->m_aa_ptr = ptr_a;    // a++
        cout << "ptr_a use_count: " << ptr_a.use_count() << endl;
        cout << "ptr_b use_count: " << ptr_b.use_count() << endl;
    }
    cout << "ptr_a use_count: " << ptr_a.use_count() << endl;
}