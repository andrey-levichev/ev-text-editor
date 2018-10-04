#ifndef TEST_INCLUDED
#define TEST_INCLUDED

#include <foundation.h>
#include <console.h>
#include <file.h>

void printPlatformInfo();

// Test

class Test
{
public:
    Test() : _val(0)
    {
    }

    Test(int val) : _val(val)
    {
    }

    Test(const Test& other) : _val(other._val)
    {
    }

    Test(Test&& other) : _val(other._val)
    {
        other._val = 0;
    }

    Test& operator=(const Test& other)
    {
        _val = other._val;
        return *this;
    }

    Test& operator=(Test&& other)
    {
        _val = other._val;
        other._val = 0;
        return *this;
    }

    int val() const
    {
        return _val;
    }

    int& val()
    {
        return _val;
    }

    friend void swap(Test& left, Test& right)
    {
        swap(left._val, right._val);
    }

    friend bool operator==(const Test& left, const Test& right)
    {
        return left._val == right._val;
    }

    friend bool operator!=(const Test& left, const Test& right)
    {
        return left._val != right._val;
    }

    friend int hash(const Test& val)
    {
        return val._val;
    }

protected:
    int _val;
};

// Base

struct Base
{
    int val;

    virtual void f()
    {
        val = 123;
    }
};

// Derived

struct Derived : public Base
{
    virtual void f()
    {
        val = 456;
    }
};

#endif
