#include <Foundation.hpp>

void testString()
{
    // String()

    {
        String s;
        ASSERT(s.chars() == nullptr);
        ASSERT(s.empty());
        ASSERT(s.capacity() == 0);
    }

    // String(int count, char_t c)

    ASSERT_EXCEPTION(Exception, String(-1));

    {
        String s(0, 'a');
        ASSERT(s.chars() == nullptr);
        ASSERT(s.empty());
        ASSERT(s.capacity() == 0);
    }

    {
        String s(1, 'a');
        ASSERT(s == STR("a"));
        ASSERT(s.length() == 1);
        ASSERT(s.capacity() == 2);
    }

    // String(const String& other)

    {
        String s1, s2(s1);
        ASSERT(s2.chars() == nullptr);
        ASSERT(s2.empty());
        ASSERT(s2.capacity() == 0);
    }

    {
        String s1(1), s2(s1);
        ASSERT(s2.chars() == nullptr);
        ASSERT(s2.empty());
        ASSERT(s2.capacity() == 0);
    }

    {
        String s1(STR("a")), s2(s1);
        ASSERT(s2 == STR("a"));
        ASSERT(s2.length() == 1);
        ASSERT(s2.capacity() == 2);
    }

    // String(const String& other, int pos, int len)

    {
        String s(STR("a"));
        ASSERT_EXCEPTION(Exception, String(s, -1, 0));
        ASSERT_EXCEPTION(Exception, String(s, 2, 0));
        ASSERT_EXCEPTION(Exception, String(s, 0, -1));
        ASSERT_EXCEPTION(Exception, String(s, 0, 2));
    }

    {
        String s1, s2(s1, 0, 0);
        ASSERT(s2.chars() == nullptr);
        ASSERT(s2.empty());
        ASSERT(s2.capacity() == 0);
    }

    {
        String s1(1), s2(s1, 0, 0);
        ASSERT(s2.chars() == nullptr);
        ASSERT(s2.empty());
        ASSERT(s2.capacity() == 0);
    }

    {
        String s1(STR("a")), s2(s1, 0, 0);
        ASSERT(s2.chars() == nullptr);
        ASSERT(s2.empty());
        ASSERT(s2.capacity() == 0);
    }

    {
        String s1(STR("a")), s2(s1, 1, 0);
        ASSERT(s2.chars() == nullptr);
        ASSERT(s2.empty());
        ASSERT(s2.capacity() == 0);
    }

    {
        String s1(STR("a")), s2(s1, 0, 1);
        ASSERT(s2 == STR("a"));
        ASSERT(s2.length() == 1);
        ASSERT(s2.capacity() == 2);
    }

    // String(const char_t* chars)

    {
        const char* p = nullptr;
        ASSERT_EXCEPTION(Exception, String s(p));
    }

    {
        String s(STR(""));
        ASSERT(s.chars() == nullptr);
        ASSERT(s.empty());
        ASSERT(s.capacity() == 0);
    }

    {
        String s(STR("a"));
        ASSERT(s == STR("a"));
        ASSERT(s.length() == 1);
        ASSERT(s.capacity() == 2);
    }

    // String(const char_t* chars, int pos, int len)

    ASSERT_EXCEPTION(Exception, String(nullptr, 0, 0));
    ASSERT_EXCEPTION(Exception, String(STR("a"), -1, 0));
    ASSERT_EXCEPTION(Exception, String(STR("a"), 0, -1));

    {
        String s(STR("a"), 0, 0);
        ASSERT(s.chars() == nullptr);
        ASSERT(s.empty());
        ASSERT(s.capacity() == 0);
    }

    {
        String s(STR("a"), 1, 0);
        ASSERT(s.chars() == nullptr);
        ASSERT(s.empty());
        ASSERT(s.capacity() == 0);
    }

    {
        String s(STR("a"), 0, 1);
        ASSERT(s == STR("a"));
        ASSERT(s.length() == 1);
        ASSERT(s.capacity() == 2);
    }

    // String::String(char_t* chars)

    {
        char* p = nullptr;
        ASSERT_EXCEPTION(Exception, String(p));
    }
    
    {
        char* p = Memory::allocateArray<char_t>(1);
        STRCPY(p, "");
        String s(p);
        ASSERT(s.chars() == p);
        ASSERT(s == STR(""));
        ASSERT(s.length() == 0);
        ASSERT(s.capacity() == 1);
    }

    {
        char* p = Memory::allocateArray<char_t>(2);
        STRCPY(p, "a");
        String s(p);
        ASSERT(s.chars() == p);
        ASSERT(s == STR("a"));
        ASSERT(s.length() == 1);
        ASSERT(s.capacity() == 2);
    }

    // String::String(int capacity)

    ASSERT_EXCEPTION(Exception, String(-1));

    {
        String s(0);
        ASSERT(s.chars() == nullptr);
        ASSERT(s.empty());
        ASSERT(s.capacity() == 0);
    }

    {
        String s(1);
        ASSERT(s == STR(""));
        ASSERT(s.length() == 0);
        ASSERT(s.capacity() == 1);
    }

    {
        String s(2);
        ASSERT(s == STR(""));
        ASSERT(s.length() == 0);
        ASSERT(s.capacity() == 2);
    }
}

void testFoundation()
{
    testString();
}
