#ifndef TEST_INCLUDED
#define TEST_INCLUDED

#include <foundation.h>

// Test

class Test
{
public:
    Test() : 
        _val(0)
    {
        Console::writeLine(STR("%p: Test::Test(): %d"), this, _val);
    }

    Test(int val) : 
        _val(val)
    {
        Console::writeLine(STR("%p: Test::Test(int): %d"), this, _val);
    }

    Test(const Test& other) : 
        _val(other._val)
    {
        Console::writeLine(STR("%p: Test::Test(const Test&): %d"), this, _val);
    }

    Test(Test&& other) :
        _val(other._val)
    {
        other._val = 0;
        Console::writeLine(STR("%p: Test::Test(Test&&): %d"), this, _val);
    }

    ~Test()
    {
        Console::writeLine(STR("%p: Test::~Test(): %d"), this, _val);
    }

    Test& operator=(const Test& other)
    {
        _val = other._val;
        Console::writeLine(STR("%p: Test::operator=(const Test&): %d"), this, _val);
        return *this;
    }

    Test& operator=(Test&& other)
    {
        _val = other._val;
        other._val = 0;
        Console::writeLine(STR("%p: Test::operator=(Test&&): %d"), this, _val);
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
    
private:
    int _val;
};

#endif

