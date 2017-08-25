#ifndef TEST_INCLUDED
#define TEST_INCLUDED

#include <foundation.h>
#include <console.h>
#include <file.h>

void printPlatformInfo();

// Dog

class Dog
{
public:
    Dog() :
        _val(0)
    {
        Console::writeLineFormatted(STR("%p: Dog::Dog(): %d"), this, _val);
    }

    Dog(int val) :
        _val(val)
    {
        Console::writeLineFormatted(STR("%p: Dog::Dog(int): %d"), this, _val);
    }

    Dog(const Dog& other) :
        _val(other._val)
    {
        Console::writeLineFormatted(STR("%p: Dog::Dog(const Dog&): %d"), this, _val);
    }

    Dog(Dog&& other) :
        _val(other._val)
    {
        other._val = 0;
        Console::writeLineFormatted(STR("%p: Dog::Dog(Dog&&): %d"), this, _val);
    }

    ~Dog()
    {
        Console::writeLineFormatted(STR("%p: Dog::~Dog(): %d"), this, _val);
    }

    Dog& operator=(const Dog& other)
    {
        _val = other._val;
        Console::writeLineFormatted(STR("%p: Dog::operator=(const Dog&): %d"), this, _val);
        return *this;
    }

    Dog& operator=(Dog&& other)
    {
        _val = other._val;
        other._val = 0;
        Console::writeLineFormatted(STR("%p: Dog::operator=(Dog&&): %d"), this, _val);
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

    friend void swap(Dog& left, Dog& right)
    {
        swap(left._val, right._val);
    }

    friend bool operator==(const Dog& left, const Dog& right)
    {
        return left._val == right._val;
    }

    friend bool operator!=(const Dog& left, const Dog& right)
    {
        return left._val != right._val;
    }

    friend int hash(const Dog& val)
    {
        return val._val;
    }

protected:
    int _val;
};

// Cat

class Cat
{
public:
    Cat() :
        _val(0)
    {
    }

    Cat(int val) :
        _val(val)
    {
    }

    Cat(const Cat& other) :
        _val(other._val)
    {
    }

    Cat(Cat&& other) :
        _val(other._val)
    {
        other._val = 0;
    }

    Cat& operator=(const Cat& other)
    {
        _val = other._val;
        return *this;
    }

    Cat& operator=(Cat&& other)
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

    friend void swap(Cat& left, Cat& right)
    {
        swap(left._val, right._val);
    }

    friend bool operator==(const Cat& left, const Cat& right)
    {
        return left._val == right._val;
    }

    friend bool operator!=(const Cat& left, const Cat& right)
    {
        return left._val != right._val;
    }

    friend int hash(const Cat& val)
    {
        return val._val;
    }

protected:
    int _val;
};

#endif

