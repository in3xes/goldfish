/*
 * test.cc
 *
 *  Created on: May 23, 2012
 *  Author: doslab
 */

#include <c++/begin_include.h>
#include <linux/kernel.h>
#include <c++/end_include.h>


class TestClass
{
private:
        int n;
public:
        TestClass(int);
       ~TestClass();
        void print_version();

}; 

void TestClass::print_version()
{
    printk("Printing object number: %d\n", n);
}

TestClass::TestClass(int a)
{
    this->n = a;
    printk("Constructing TestClass with object number :%d\n", n);
}

TestClass::~TestClass()
{
    printk("Destructing TestClass with object number :%d\n", n);
}


extern "C"
{

int init_test()
{
    int n;
    TestClass **t;

    n = 5;
    t = new TestClass* [n];
    for(int i=0; i<n; i++) {
       t[i] = new TestClass(i); 
       t[i]->print_version();
    }
 
    for(int i=0; i<n; i++)
        delete t[i]; 
    delete t;
    return 0;  
}

}

