#include <foundation.h>
#include <test.h>

void testString()
{
    // String()

    {
        String s;
        ASSERT(s.chars() == nullptr);
        ASSERT(s.length() == 0);
        ASSERT(s.capacity() == 0);
    }

    // String(int count, char_t ch)

    ASSERT_EXCEPTION(Exception, String(-1));

    {
        String s(0, 'a');
        ASSERT(s.chars() == nullptr);
        ASSERT(s.length() == 0);
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
        ASSERT(s2.length() == 0);
        ASSERT(s2.capacity() == 0);
    }

    {
        String s1(1), s2(s1);
        ASSERT(s2.chars() == nullptr);
        ASSERT(s2.length() == 0);
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
        ASSERT(s2.length() == 0);
        ASSERT(s2.capacity() == 0);
    }

    {
        String s1(1), s2(s1, 0, 0);
        ASSERT(s2.chars() == nullptr);
        ASSERT(s2.length() == 0);
        ASSERT(s2.capacity() == 0);
    }

    {
        String s1(STR("a")), s2(s1, 0, 0);
        ASSERT(s2.chars() == nullptr);
        ASSERT(s2.length() == 0);
        ASSERT(s2.capacity() == 0);
    }

    {
        String s1(STR("a")), s2(s1, 1, 0);
        ASSERT(s2.chars() == nullptr);
        ASSERT(s2.length() == 0);
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
        const char_t* p = nullptr;
        ASSERT_EXCEPTION(Exception, String s(p));
    }

    {
        String s(STR(""));
        ASSERT(s.chars() == nullptr);
        ASSERT(s.length() == 0);
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
        ASSERT(s.length() == 0);
        ASSERT(s.capacity() == 0);
    }

    {
        String s(STR("a"), 1, 0);
        ASSERT(s.chars() == nullptr);
        ASSERT(s.length() == 0);
        ASSERT(s.capacity() == 0);
    }

    {
        String s(STR("a"), 0, 1);
        ASSERT(s == STR("a"));
        ASSERT(s.length() == 1);
        ASSERT(s.capacity() == 2);
    }

    // String(int capacity)

    ASSERT_EXCEPTION(Exception, String(-1));

    {
        String s(0);
        ASSERT(s.chars() == nullptr);
        ASSERT(s.length() == 0);
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

    // String(String&& other)

    {
        String s1 = STR("a");
        String s2(static_cast<String&&>(s1));

        ASSERT(s1.chars() == nullptr);
        ASSERT(s1.length() == 0);
        ASSERT(s1.capacity() == 0);

        ASSERT(s2 == STR("a"));
        ASSERT(s2.length() == 1);
        ASSERT(s2.capacity() == 2);
    }

    // String& operator=(const String& other);

    {
        String s1(STR("a")), s2;
        s2 = s1;
        ASSERT(s2 == STR("a"));
    }

    // String& operator=(const char_t* chars);

    {
        String s;
        s = STR("a");
        ASSERT(s == STR("a"));
    }

    // String& operator=(String&& other);

    {
        String s1(STR("a")), s2;
        s2 = static_cast<String&&>(s1);

        ASSERT(s1.chars() == nullptr);
        ASSERT(s1.length() == 0);
        ASSERT(s1.capacity() == 0);

        ASSERT(s2 == STR("a"));
        ASSERT(s2.length() == 1);
        ASSERT(s2.capacity() == 2);
    }

    // String& operator+=(const String& str);

    {
        String s(STR("a"));
        s += String(STR("b"));
        ASSERT(s == STR("ab"));
    }

    // String& operator+=(const char_t* chars)

    {
        String s(STR("a"));
        s += STR("b");
        ASSERT(s == STR("ab"));
    }

    // String& operator+=(const char_t ch)

    {
        String s(STR("a"));
        s += 'b';
        ASSERT(s == STR("ab"));
    }

    // length/capacity/str/chars/empty

    {
        String s;
        ASSERT(s == STR(""));
        ASSERT(s.chars() == nullptr);
        ASSERT(s.length() == 0);
        ASSERT(s.capacity() == 0);
        ASSERT(s.empty());
    }

    {
        String s(10);
        s = STR("a");
        ASSERT(s == STR("a"));
        ASSERT(s.chars() != nullptr);
        ASSERT(s.length() == 1);
        ASSERT(s.capacity() == 10);
        ASSERT(!s.empty());
    }

    // String substr(int pos, int len) const

    {
        String s(STR("abcd"));
        ASSERT(s.substr(1, 2) == STR("bc"));
    }

    // void ensureCapacity(int capacity)

    ASSERT_EXCEPTION(Exception, String().ensureCapacity(-1));

    {
        String s;
        s.ensureCapacity(0);
        ASSERT(s == STR(""));
        ASSERT(s.length() == 0);
        ASSERT(s.capacity() == 0);
    }

    {
        String s;
        s.ensureCapacity(10);
        ASSERT(s == STR(""));
        ASSERT(s.length() == 0);
        ASSERT(s.capacity() == 10);
    }

    {
        String s(STR("abc"));

        s.ensureCapacity(2);
        ASSERT(s == STR("abc"));
        ASSERT(s.length() == 3);
        ASSERT(s.capacity() == 4);

        s.ensureCapacity(10);
        ASSERT(s == STR("abc"));
        ASSERT(s.length() == 3);
        ASSERT(s.capacity() == 10);
    }

    // void shrinkToLength()

    {
        String s;
        s.shrinkToLength();
        ASSERT(s.chars() == nullptr);
        ASSERT(s.length() == 0);
        ASSERT(s.capacity() == 0);
    }

    {
        String s(10);
        s.shrinkToLength();
        ASSERT(s.chars() == nullptr);
        ASSERT(s.length() == 0);
        ASSERT(s.capacity() == 0);
    }

    {
        String s(10);
        s = STR("a");
        s.shrinkToLength();
        ASSERT(s == STR("a"));
        ASSERT(s.length() == 1);
        ASSERT(s.capacity() == 2);

        s.shrinkToLength();
        ASSERT(s == STR("a"));
        ASSERT(s.length() == 1);
        ASSERT(s.capacity() == 2);
    }

    // void assign(const String& str)

    {
        String s(STR("a"));
        s.assign(s);
        ASSERT(s == STR("a"));
    }

    {
        String s1(STR("a")), s2;
        s1.assign(s2);
        ASSERT(s1 == STR(""));
        ASSERT(s1.capacity() == 2);
    }

    {
        String s1, s2(STR("a"));
        s1.assign(s2);
        ASSERT(s1 == STR("a"));
        ASSERT(s1.capacity() == 2);
    }

    {
        String s1(10), s2(STR("a"));
        s1.assign(s2);
        ASSERT(s1 == STR("a"));
        ASSERT(s1.capacity() == 10);
    }

    // void assign(const char_t* chars)

    ASSERT_EXCEPTION(Exception, String().assign(nullptr));

    {
        String s(STR("a"));
        s.assign(s.chars());
        ASSERT(s == STR("a"));
    }

    {
        String s(STR("a"));
        s.assign(STR(""));
        ASSERT(s == STR(""));
        ASSERT(s.capacity() == 2);
    }

    {
        String s;
        s.assign(STR("a"));
        ASSERT(s == STR("a"));
        ASSERT(s.capacity() == 2);
    }

    {
        String s(10);
        s.assign(STR("a"));
        ASSERT(s == STR("a"));
        ASSERT(s.capacity() == 10);
    }

    // void append(const String& str)

    {
        String s(STR("a"));
        ASSERT_EXCEPTION(Exception, s.append(s));
    }

    {
        String s1(STR("a")), s2;
        s1.append(s2);
        ASSERT(s1 == STR("a"));
        ASSERT(s1.capacity() == 2);
    }

    {
        String s1, s2(STR("b"));
        s1.append(s2);
        ASSERT(s1 == STR("b"));
        ASSERT(s1.capacity() == 2);
    }

    {
        String s1(STR("a")), s2(STR("b"));
        s1.append(s2);
        ASSERT(s1 == STR("ab"));
        ASSERT(s1.capacity() == 3);
    }

    {
        String s1(10), s2(STR("b"));
        s1.append(s2);
        ASSERT(s1 == STR("b"));
        ASSERT(s1.capacity() == 10);
    }

    // void append(const char_t* chars)

    ASSERT_EXCEPTION(Exception, String(STR("a")).append(nullptr));

    {
        String s(STR("a"));
        ASSERT_EXCEPTION(Exception, s.append(s.chars()));
    }

    {
        String s(STR("a"));
        s.append(STR(""));
        ASSERT(s == STR("a"));
        ASSERT(s.capacity() == 2);
    }

    {
        String s;
        s.append(STR("b"));
        ASSERT(s == STR("b"));
        ASSERT(s.capacity() == 2);
    }

    {
        String s(STR("a"));
        s.append(STR("b"));
        ASSERT(s == STR("ab"));
        ASSERT(s.capacity() == 3);
    }

    {
        String s(10);
        s.append(STR("b"));
        ASSERT(s == STR("b"));
        ASSERT(s.capacity() == 10);
    }

    // void appendFormat(const char_t* format, ...)
    // void appendFormat(const char_t* format, va_list args)

    {
        String s(STR("val = "));
        s.appendFormat(STR("%d"), 123);
        ASSERT(s == STR("val = 123"));
    }

    // void insert(int pos, const String& str)

    ASSERT_EXCEPTION(Exception, String(STR("a")).insert(-1, String()));
    ASSERT_EXCEPTION(Exception, String(STR("a")).insert(2, String()));

    {
        String s(STR("a"));
        ASSERT_EXCEPTION(Exception, s.insert(0, s));
    }

    {
        String s1(STR("a")), s2;
        s1.insert(0, s2);
        ASSERT(s1 == STR("a"));
        ASSERT(s1.capacity() == 2);
    }

    {
        String s1, s2(STR("b"));
        s1.insert(0, s2);
        ASSERT(s1 == STR("b"));
        ASSERT(s1.capacity() == 2);
    }

    {
        String s1(STR("a")), s2(STR("b"));
        s1.insert(0, s2);
        ASSERT(s1 == STR("ba"));
        ASSERT(s1.capacity() == 3);
    }

    {
        String s1(STR("a")), s2(STR("b"));
        s1.insert(1, s2);
        ASSERT(s1 == STR("ab"));
        ASSERT(s1.capacity() == 3);
    }

    {
        String s1(STR("ab")), s2(STR("c"));
        s1.insert(1, s2);
        ASSERT(s1 == STR("acb"));
        ASSERT(s1.capacity() == 4);
    }

    {
        String s1(10), s2(STR("c"));
        s1 = STR("ab");
        s1.insert(1, s2);
        ASSERT(s1 == STR("acb"));
        ASSERT(s1.capacity() == 10);
    }

    // void insert(int pos, const char_t* chars)

    ASSERT_EXCEPTION(Exception, String(STR("a")).insert(-1, STR("")));
    ASSERT_EXCEPTION(Exception, String(STR("a")).insert(2, STR("")));
    ASSERT_EXCEPTION(Exception, String(STR("a")).insert(0, nullptr));

    {
        String s(STR("a"));
        ASSERT_EXCEPTION(Exception, s.insert(0, s.chars()));
    }

    {
        String s(STR("a"));
        s.insert(0, STR(""));
        ASSERT(s == STR("a"));
        ASSERT(s.capacity() == 2);
    }

    {
        String s;
        s.insert(0, STR("b"));
        ASSERT(s == STR("b"));
        ASSERT(s.capacity() == 2);
    }

    {
        String s(STR("a"));
        s.insert(0, STR("b"));
        ASSERT(s == STR("ba"));
        ASSERT(s.capacity() == 3);
    }

    {
        String s(STR("a"));
        s.insert(1, STR("b"));
        ASSERT(s == STR("ab"));
        ASSERT(s.capacity() == 3);
    }

    {
        String s(STR("ab"));
        s.insert(1, STR("c"));
        ASSERT(s == STR("acb"));
        ASSERT(s.capacity() == 4);
    }

    {
        String s(10);
        s = STR("ab");
        s.insert(1, STR("c"));
        ASSERT(s == STR("acb"));
        ASSERT(s.capacity() == 10);
    }

    // void erase(int pos, int len)

    ASSERT_EXCEPTION(Exception, String(STR("a")).erase(-1, 0));
    ASSERT_EXCEPTION(Exception, String(STR("a")).erase(2, 0));
    ASSERT_EXCEPTION(Exception, String(STR("a")).erase(0, -1));
    ASSERT_EXCEPTION(Exception, String(STR("a")).erase(0, 2));

    {
        String s(STR("a"));
        s.erase(0, 0);
        ASSERT(s == STR("a"));
        ASSERT(s.capacity() == 2);
    }

    {
        String s(STR("a"));
        s.erase(0, 1);
        ASSERT(s.length() == 0);
        ASSERT(s.capacity() == 2);
    }

    {
        String s(STR("abc"));
        s.erase(0, 1);
        ASSERT(s == STR("bc"));
        ASSERT(s.capacity() == 4);
    }

    {
        String s(STR("abc"));
        s.erase(2, 1);
        ASSERT(s == STR("ab"));
        ASSERT(s.capacity() == 4);
    }

    // void erase(const String& str)

    {
        String s;
        ASSERT_EXCEPTION(Exception, s.erase(s));
    }

    {
        String s(STR("abc"));
        s.erase(String());
        ASSERT(s == STR("abc"));
        ASSERT(s.capacity() == 4);
    }

    {
        String s;
        s.erase(String(STR("abc")));
        ASSERT(s.length() == 0);
        ASSERT(s.capacity() == 0);
    }

    {
        String s(STR("abc"));
        s.erase(String(STR("x")));
        ASSERT(s == STR("abc"));
        ASSERT(s.capacity() == 4);
    }

    {
        String s(STR("abc"));
        s.erase(String(STR("abc")));
        ASSERT(s.length() == 0);
        ASSERT(s.capacity() == 4);
    }

    {
        String s(STR("abc"));
        s.erase(String(STR("b")));
        ASSERT(s == STR("ac"));
        ASSERT(s.capacity() == 4);
    }

    {
        String s(STR("abc"));
        s.erase(String(STR("a")));
        ASSERT(s == STR("bc"));
        ASSERT(s.capacity() == 4);
    }

    {
        String s(STR("abc"));
        s.erase(String(STR("c")));
        ASSERT(s == STR("ab"));
        ASSERT(s.capacity() == 4);
    }

    {
        String s(STR("aab"));
        s.erase(String(STR("a")));
        ASSERT(s == STR("b"));
        ASSERT(s.capacity() == 4);
    }

    {
        String s(STR("abcbd"));
        s.erase(String(STR("b")));
        ASSERT(s == STR("acd"));
        ASSERT(s.capacity() == 6);
    }

    // void erase(const char_t* chars)

    ASSERT_EXCEPTION(Exception, String().erase(nullptr));

    {
        String s;
        ASSERT_EXCEPTION(Exception, s.erase(s.chars()));
    }

    {
        String s(STR("abc"));
        s.erase(STR(""));
        ASSERT(s == STR("abc"));
        ASSERT(s.capacity() == 4);
    }

    {
        String s;
        s.erase(STR("abc"));
        ASSERT(s.length() == 0);
        ASSERT(s.capacity() == 0);
    }

    {
        String s(STR("abc"));
        s.erase(STR("x"));
        ASSERT(s == STR("abc"));
        ASSERT(s.capacity() == 4);
    }

    {
        String s(STR("abc"));
        s.erase(STR("abc"));
        ASSERT(s.length() == 0);
        ASSERT(s.capacity() == 4);
    }

    {
        String s(STR("abc"));
        s.erase(STR("b"));
        ASSERT(s == STR("ac"));
        ASSERT(s.capacity() == 4);
    }

    {
        String s(STR("abc"));
        s.erase(STR("a"));
        ASSERT(s == STR("bc"));
        ASSERT(s.capacity() == 4);
    }

    {
        String s(STR("abc"));
        s.erase(STR("c"));
        ASSERT(s == STR("ab"));
        ASSERT(s.capacity() == 4);
    }

    {
        String s(STR("aab"));
        s.erase(STR("a"));
        ASSERT(s == STR("b"));
        ASSERT(s.capacity() == 4);
    }

    {
        String s(STR("abcbd"));
        s.erase(STR("b"));
        ASSERT(s == STR("acd"));
        ASSERT(s.capacity() == 6);
    }

    // void clear()

    {
        String s;
        s.clear();
        ASSERT(s.chars() == nullptr);
        ASSERT(s.length() == 0);
        ASSERT(s.capacity() == 0);
    }

    {
        String s(STR("a"));
        s.clear();
        ASSERT(*s.chars() == 0);
        ASSERT(s.length() == 0);
        ASSERT(s.capacity() == 2);
    }

    // void reset()

    {
        String s;
        s.reset();
        ASSERT(s.chars() == nullptr);
        ASSERT(s.length() == 0);
        ASSERT(s.capacity() == 0);
    }

    {
        String s(STR("a"));
        s.reset();
        ASSERT(s.chars() == nullptr);
        ASSERT(s.length() == 0);
        ASSERT(s.capacity() == 0);
    }

    // String acquire(char_t* chars)

    ASSERT_EXCEPTION(Exception, String::acquire(nullptr));

    {
        char_t* p = Memory::allocate<char_t>(1);
        STRCPY(p, STR(""));
        String s = String::acquire(p);
        ASSERT(s.chars() == p);
        ASSERT(s == STR(""));
        ASSERT(s.length() == 0);
        ASSERT(s.capacity() == 1);
    }

    {
        char_t* p = Memory::allocate<char_t>(2);
        STRCPY(p, STR("a"));
        String s = String::acquire(p);
        ASSERT(s.chars() == p);
        ASSERT(s == STR("a"));
        ASSERT(s.length() == 1);
        ASSERT(s.capacity() == 2);
    }

    // char_t* release()

    {
        String s(STR("a"));
        char_t* p = s.release();
        ASSERT(STRCMP(p, STR("a")) == 0);
        ASSERT(s.chars() == nullptr);
        ASSERT(s.length() == 0);
        ASSERT(s.capacity() == 0);
        Memory::deallocate(p);
    }

    // void replace(int pos, int len, const String& str)

    ASSERT_EXCEPTION(Exception, String(STR("a")).replace(-1, 0, String()));
    ASSERT_EXCEPTION(Exception, String(STR("a")).replace(2, 0, String()));
    ASSERT_EXCEPTION(Exception, String(STR("a")).replace(0, -1, String()));
    ASSERT_EXCEPTION(Exception, String(STR("a")).replace(0, 2, String()));

    {
        String s;
        ASSERT_EXCEPTION(Exception, s.replace(0, 0, String()));
    }

    {
        String s(STR("ab"));
        s.replace(0, 1, String());
        ASSERT(s == STR("b"));
        ASSERT(s.length() == 1);
        ASSERT(s.capacity() == 3);
    }

    {
        String s(STR("a"));
        s.replace(0, 1, String(STR("b")));
        ASSERT(s == STR("b"));
        ASSERT(s.length() == 1);
        ASSERT(s.capacity() == 2);
    }

    {
        String s(STR("abcd"));
        s.replace(1, 2, String(STR("x")));
        ASSERT(s == STR("axd"));
        ASSERT(s.length() == 3);
        ASSERT(s.capacity() == 5);
    }

    {
        String s(STR("abcd"));
        s.replace(1, 2, String(STR("xyz")));
        ASSERT(s == STR("axyzd"));
        ASSERT(s.length() == 5);
        ASSERT(s.capacity() == 6);
    }

    {
        String s(STR("abcd"));
        s.replace(0, 2, String(STR("x")));
        ASSERT(s == STR("xcd"));
        ASSERT(s.length() == 3);
        ASSERT(s.capacity() == 5);
    }

    {
        String s(STR("abcd"));
        s.replace(2, 2, String(STR("x")));
        ASSERT(s == STR("abx"));
        ASSERT(s.length() == 3);
        ASSERT(s.capacity() == 5);
    }

    // void replace(int pos, int len, const String& str)

    ASSERT_EXCEPTION(Exception, String(STR("a")).replace(-1, 0, STR("")));
    ASSERT_EXCEPTION(Exception, String(STR("a")).replace(2, 0, STR("")));
    ASSERT_EXCEPTION(Exception, String(STR("a")).replace(0, -1, STR("")));
    ASSERT_EXCEPTION(Exception, String(STR("a")).replace(0, 2, STR("")));
    
    {
        String s;
        ASSERT_EXCEPTION(Exception, s.replace(0, 0, s.chars()));
    }

    {
        String s(STR("ab"));
        s.replace(0, 1, STR(""));
        ASSERT(s == STR("b"));
        ASSERT(s.length() == 1);
        ASSERT(s.capacity() == 3);
    }

    {
        String s(STR("a"));
        s.replace(0, 1, STR("b"));
        ASSERT(s == STR("b"));
        ASSERT(s.length() == 1);
        ASSERT(s.capacity() == 2);
    }

    {
        String s(STR("abcd"));
        s.replace(1, 2, STR("x"));
        ASSERT(s == STR("axd"));
        ASSERT(s.length() == 3);
        ASSERT(s.capacity() == 5);
    }

    {
        String s(STR("abcd"));
        s.replace(1, 2, STR("xyz"));
        ASSERT(s == STR("axyzd"));
        ASSERT(s.length() == 5);
        ASSERT(s.capacity() == 6);
    }

    {
        String s(STR("abcd"));
        s.replace(0, 2, STR("x"));
        ASSERT(s == STR("xcd"));
        ASSERT(s.length() == 3);
        ASSERT(s.capacity() == 5);
    }

    {
        String s(STR("abcd"));
        s.replace(2, 2, STR("x"));
        ASSERT(s == STR("abx"));
        ASSERT(s.length() == 3);
        ASSERT(s.capacity() == 5);
    }

    // void replace(const String& searchStr, const String& replaceStr)

    {
        String s;
        ASSERT_EXCEPTION(Exception, s.replace(s, String()));
        ASSERT_EXCEPTION(Exception, s.replace(String(), s));
    }

    {
        String s(STR("abc"));
        s.replace(String(), String());
        ASSERT(s == STR("abc"));
    }
     
    {
        String s(STR("abc"));
        s.replace(String(STR("b")), String());
        ASSERT(s == STR("ac"));
    }  

    {
        String s;
        s.replace(String(STR("a")), String(STR("b")));
        ASSERT(s == STR(""));
    }

    {
        String s(STR("abc"));
        s.replace(String(STR("x")), String(STR("y")));
        ASSERT(s == STR("abc"));
    }

    {
        String s(STR("abcd"));
        s.replace(String(STR("bc")), String(STR("x")));
        ASSERT(s == STR("axd"));
        ASSERT(s.length() == 3);
        ASSERT(s.capacity() == 5);
    }

    {
        String s(STR("abcd"));
        s.replace(String(STR("bc")), String(STR("xyz")));
        ASSERT(s == STR("axyzd"));
        ASSERT(s.length() == 5);
        ASSERT(s.capacity() == 6);
    }

    {
        String s(STR("abcd"));
        s.replace(String(STR("ab")), String(STR("x")));
        ASSERT(s == STR("xcd"));
        ASSERT(s.length() == 3);
        ASSERT(s.capacity() == 5);
    }

    {
        String s(STR("abcd"));
        s.replace(String(STR("cd")), String(STR("x")));
        ASSERT(s == STR("abx"));
        ASSERT(s.length() == 3);
        ASSERT(s.capacity() == 5);
    }

    {
        String s(STR("aab"));
        s.replace(String(STR("a")), String(STR("xy")));
        ASSERT(s == STR("xyxyb"));
        ASSERT(s.length() == 5);
        ASSERT(s.capacity() == 6);
    }

    {
        String s(STR("abcbd"));
        s.replace(String(STR("b")), String(STR("xy")));
        ASSERT(s == STR("axycxyd"));
        ASSERT(s.length() == 7);
        ASSERT(s.capacity() == 8);
    }

    // void replace(const char_t* searchChars, const char_t* replaceChars)

    {
        String s;
        ASSERT_EXCEPTION(Exception, s.replace(nullptr, STR("")));
        ASSERT_EXCEPTION(Exception, s.replace(s.chars(), STR("")));
        ASSERT_EXCEPTION(Exception, s.replace(STR(""), nullptr));
        ASSERT_EXCEPTION(Exception, s.replace(STR(""), s.chars()));
    }

    {
        String s(STR("abc"));
        s.replace(STR(""), STR(""));
        ASSERT(s == STR("abc"));
    }
     
    {
        String s(STR("abc"));
        s.replace(STR("b"), STR(""));
        ASSERT(s == STR("ac"));
    }  

    {
        String s;
        s.replace(STR("a"), STR("b"));
        ASSERT(s == STR(""));
    }

    {
        String s(STR("abc"));
        s.replace(STR("x"), STR("y"));
        ASSERT(s == STR("abc"));
    }

    {
        String s(STR("abcd"));
        s.replace(STR("bc"), STR("x"));
        ASSERT(s == STR("axd"));
        ASSERT(s.length() == 3);
        ASSERT(s.capacity() == 5);
    }

    {
        String s(STR("abcd"));
        s.replace(STR("bc"), STR("xyz"));
        ASSERT(s == STR("axyzd"));
        ASSERT(s.length() == 5);
        ASSERT(s.capacity() == 6);
    }

    {
        String s(STR("abcd"));
        s.replace(STR("ab"), STR("x"));
        ASSERT(s == STR("xcd"));
        ASSERT(s.length() == 3);
        ASSERT(s.capacity() == 5);
    }

    {
        String s(STR("abcd"));
        s.replace(STR("cd"), STR("x"));
        ASSERT(s == STR("abx"));
        ASSERT(s.length() == 3);
        ASSERT(s.capacity() == 5);
    }

    {
        String s(STR("aab"));
        s.replace(STR("a"), STR("xy"));
        ASSERT(s == STR("xyxyb"));
        ASSERT(s.length() == 5);
        ASSERT(s.capacity() == 6);
    }

    {
        String s(STR("abcbd"));
        s.replace(STR("b"), STR("xy"));
        ASSERT(s == STR("axycxyd"));
        ASSERT(s.length() == 7);
        ASSERT(s.capacity() == 8);
    }

    // void trim()

    {
        String s;
        s.trim();
        ASSERT(s == STR(""));
    }

    {
        String s(STR(" "));
        s.trim();
        ASSERT(s == STR(""));
        ASSERT(s.capacity() == 2);
    }

    {
        String s(STR(" a "));
        s.trim();
        ASSERT(s == STR("a"));
        ASSERT(s.capacity() == 4);
    }

    {
        String s(STR("  a  "));
        s.trim();
        ASSERT(s == STR("a"));
        ASSERT(s.capacity() == 6);
    }

    // void trimRight()

    {
        String s;
        s.trimRight();
        ASSERT(s == STR(""));
    }

    {
        String s(STR(" "));
        s.trimRight();
        ASSERT(s == STR(""));
        ASSERT(s.capacity() == 2);
    }

    {
        String s(STR(" a "));
        s.trimRight();
        ASSERT(s == STR(" a"));
        ASSERT(s.capacity() == 4);
    }

    {
        String s(STR("  a  "));
        s.trimRight();
        ASSERT(s == STR("  a"));
        ASSERT(s.capacity() == 6);
    }

    // void trimLeft()

    {
        String s;
        s.trimLeft();
        ASSERT(s == STR(""));
    }

    {
        String s(STR(" "));
        s.trimLeft();
        ASSERT(s == STR(""));
        ASSERT(s.capacity() == 2);
    }

    {
        String s(STR(" a "));
        s.trimLeft();
        ASSERT(s == STR("a "));
        ASSERT(s.capacity() == 4);
    }

    {
        String s(STR("  a  "));
        s.trimLeft();
        ASSERT(s == STR("a  "));
        ASSERT(s.capacity() == 6);
    }

    // void reverse()

    {
        String s;
        s.reverse();
        ASSERT(s == STR(""));
    }

    {
        String s(STR("a"));
        s.reverse();
        ASSERT(s == STR("a"));
    }

    {
        String s(STR("ab"));
        s.reverse();
        ASSERT(s == STR("ba"));
    }

    {
        String s(STR("abc"));
        s.reverse();
        ASSERT(s == STR("cba"));
    }

    // void toUpper()

    {
        String s(STR("abC"));
        s.toUpper();
        ASSERT(s == STR("ABC"));
    }

    // void toLower()

    {
        String s(STR("ABc"));
        s.toLower();
        ASSERT(s == STR("abc"));
    }

    // int compare(const String& str) const

    ASSERT(String(STR("ab")).compare(String(STR("a"))) > 0);
    ASSERT(String(STR("a")).compare(String(STR("ab"))) < 0);
    ASSERT(String(STR("a")).compare(String(STR("a"))) == 0);

    // int compare(const char_t* chars) const

    ASSERT_EXCEPTION(Exception, String().compare(nullptr));
    ASSERT(String(STR("ab")).compare(STR("a")) > 0);
    ASSERT(String(STR("a")).compare(STR("ab")) < 0);
    ASSERT(String(STR("a")).compare(STR("a")) == 0);

    // int compareNoCase(const String& str) const

    ASSERT(String(STR("AB")).compareNoCase(String(STR("a"))) > 0);
    ASSERT(String(STR("A")).compareNoCase(String(STR("ab"))) < 0);
    ASSERT(String(STR("A")).compareNoCase(String(STR("a"))) == 0);

    // int compareNoCase(const char_t* chars) const

    ASSERT_EXCEPTION(Exception, String().compareNoCase(nullptr));
    ASSERT(String(STR("AB")).compareNoCase(STR("a")) > 0);
    ASSERT(String(STR("A")).compareNoCase(STR("ab")) < 0);
    ASSERT(String(STR("A")).compareNoCase(STR("a")) == 0);

    // int find(const String& str, int pos) const

    ASSERT_EXCEPTION(Exception, String(STR("a")).find(String(), -1));
    ASSERT_EXCEPTION(Exception, String(STR("a")).find(String(), 2));

    {
        String s(STR("a"));
        ASSERT(s.find(String()) == String::INVALID_POS);
    }

    {
        String s;
        ASSERT(s.find(String(STR("a"))) == String::INVALID_POS);
    }

    {
        String s(STR("abcabc"));
        ASSERT(s.find(String(STR("bc"))) == 1);
        ASSERT(s.find(String(STR("bc")), 2) == 4);
        ASSERT(s.find(String(STR("bc")), s.length()) == String::INVALID_POS);
        ASSERT(s.find(String(STR("xy"))) == String::INVALID_POS);
    }

    // int find(const char_t* chars, int pos) const

    ASSERT_EXCEPTION(Exception, String(STR("a")).find(STR(""), -1));
    ASSERT_EXCEPTION(Exception, String(STR("a")).find(STR(""), 2));
    ASSERT_EXCEPTION(Exception, String(STR("a")).find(nullptr, 0));

    {
        String s(STR("a"));
        ASSERT(s.find(STR("")) == String::INVALID_POS);
    }

    {
        String s;
        ASSERT(s.find(STR("a")) == String::INVALID_POS);
    }

    {
        String s(STR("abcabc"));
        ASSERT(s.find(STR("bc")) == 1);
        ASSERT(s.find(STR("bc"), 2) == 4);
        ASSERT(s.find(STR("bc"), s.length()) == String::INVALID_POS);
        ASSERT(s.find(STR("xy")) == String::INVALID_POS);
    }

    // int findNoCase(const String& str, int pos) const

    ASSERT_EXCEPTION(Exception, String(STR("a")).findNoCase(String(), -1));
    ASSERT_EXCEPTION(Exception, String(STR("a")).findNoCase(String(), 2));

    {
        String s(STR("a"));
        ASSERT(s.findNoCase(String()) == String::INVALID_POS);
    }

    {
        String s;
        ASSERT(s.findNoCase(String(STR("a"))) == String::INVALID_POS);
    }

    {
        String s(STR("ABCABC"));
        ASSERT(s.findNoCase(String(STR("bc"))) == 1);
        ASSERT(s.findNoCase(String(STR("bc")), 2) == 4);
        ASSERT(s.findNoCase(String(STR("bc")), s.length()) == String::INVALID_POS);
        ASSERT(s.findNoCase(String(STR("xy"))) == String::INVALID_POS);
    }

    // int findNoCase(const char_t* chars, int pos) const

    ASSERT_EXCEPTION(Exception, String(STR("a")).findNoCase(STR(""), -1));
    ASSERT_EXCEPTION(Exception, String(STR("a")).findNoCase(STR(""), 2));
    ASSERT_EXCEPTION(Exception, String(STR("a")).findNoCase(nullptr, 0));

    {
        String s(STR("a"));
        ASSERT(s.findNoCase(STR("")) == String::INVALID_POS);
    }

    {
        String s;
        ASSERT(s.findNoCase(STR("a")) == String::INVALID_POS);
    }

    {
        String s(STR("ABCABC"));
        ASSERT(s.findNoCase(STR("bc")) == 1);
        ASSERT(s.findNoCase(STR("bc"), 2) == 4);
        ASSERT(s.findNoCase(STR("bc"), s.length()) == String::INVALID_POS);
        ASSERT(s.findNoCase(STR("xy")) == String::INVALID_POS);
    }

    // bool startsWith(const String& str) const

    ASSERT(String().startsWith(String()) == false);
    ASSERT(String().startsWith(String(STR("a"))) == false);
    ASSERT(String(STR("a")).startsWith(String()) == false);
    ASSERT(String(STR("a")).startsWith(String(STR("a"))) == true);
    ASSERT(String(STR("ab")).startsWith(String(STR("a"))) == true);
    ASSERT(String(STR("a")).startsWith(String(STR("ab"))) == false);

    // bool startsWith(const char_t* chars) const

    ASSERT_EXCEPTION(Exception, String().startsWith(nullptr));
    ASSERT(String().startsWith(STR("")) == false);
    ASSERT(String().startsWith(STR("a")) == false);
    ASSERT(String(STR("a")).startsWith(STR("")) == false);
    ASSERT(String(STR("a")).startsWith(STR("a")) == true);
    ASSERT(String(STR("ab")).startsWith(STR("a")) == true);
    ASSERT(String(STR("a")).startsWith(STR("ab")) == false);

    // bool endsWith(const String& str) const

    ASSERT(String().endsWith(String()) == false);
    ASSERT(String().endsWith(String(STR("a"))) == false);
    ASSERT(String(STR("a")).endsWith(String()) == false);
    ASSERT(String(STR("a")).endsWith(String(STR("a"))) == true);
    ASSERT(String(STR("ab")).endsWith(String(STR("b"))) == true);
    ASSERT(String(STR("a")).endsWith(String(STR("ab"))) == false);

    // bool endsWith(const char_t* chars) const

    ASSERT_EXCEPTION(Exception, String().endsWith(nullptr));
    ASSERT(String().endsWith(STR("")) == false);
    ASSERT(String().endsWith(STR("a")) == false);
    ASSERT(String(STR("a")).endsWith(STR("")) == false);
    ASSERT(String(STR("a")).endsWith(STR("a")) == true);
    ASSERT(String(STR("ab")).endsWith(STR("b")) == true);
    ASSERT(String(STR("a")).endsWith(STR("ab")) == false);

    // bool contains(const String& str) const

    ASSERT(String(STR("abc")).contains(String(STR("bc"))) == true);
    ASSERT(String(STR("abc")).contains(String(STR("xy"))) == false);

    // bool contains(const char_t* chars) const

    ASSERT(String(STR("abc")).contains(STR("bc")) == true);
    ASSERT(String(STR("abc")).contains(STR("xy")) == false);

    // String concat(_Args&&... args)

    ASSERT(String::concat(STR("a")) == STR("a"));
    ASSERT(String::concat(String(STR("a"))) == STR("a"));
    ASSERT(String::concat(STR("a"), STR("b")) == STR("ab"));
    ASSERT(String::concat(STR("a"), String(STR("b"))) == STR("ab"));
    ASSERT(String::concat(String(STR("a")), STR("b")) == STR("ab"));
    ASSERT(String::concat(String(STR("a")), String(STR("b"))) == STR("ab"));

    // conversion from string

    ASSERT(String(STR("true")).toBool() == true);
    ASSERT(String(STR("false")).toBool() == false);
    ASSERT(String(STR("-123")).toInt() == -123);
    ASSERT(String(STR("-123")).toInt32() == -123);
    ASSERT(String(STR("123")).toUInt32() == 123);
    ASSERT(String(STR("-123")).toInt64() == -123);
    ASSERT(String(STR("123")).toUInt64() == 123);
    ASSERT(String(STR("123.45")).toFloat() == 123.45f);
    ASSERT(String(STR("123.45")).toDouble() == 123.45);

    // conversion to string

    ASSERT(String::from(true) == STR("true"));
    ASSERT(String::from(false) == STR("false"));
    ASSERT(String::from(-123) == STR("-123"));
    ASSERT(String::from(123u) == STR("123"));
    ASSERT(String::from(-123l) == STR("-123"));
    ASSERT(String::from(123ul) == STR("123"));
    ASSERT(String::from(-123ll) == STR("-123"));
    ASSERT(String::from(123ull) == STR("123"));
    ASSERT(String::from(123.45f, 2) == STR("123.45"));
    ASSERT(String::from(123.45, 2) == STR("123.45"));

    // String format(const char_t* format, ...)
    // String format(const char_t* format, va_list args)

    ASSERT(String::format(STR("str = %s, int = %d, float = %g"), 
            STR("abc"), 123, 123.45) == STR("str = abc, int = 123, float = 123.45"));
}

void testArray()
{
    int elem[] = { 1, 2, 3 };
    const int* np = nullptr;
    const int* ep = elem;

    // Array()

    {
        Array<int> a;
        ASSERT(a.size() == 0);
        ASSERT(a.capacity() == 0);
        ASSERT(a.elements() == nullptr);
    }

    // Array(int size)

    ASSERT_EXCEPTION(Exception, Array<int> a(-1));

    {
        Array<int> a(0);
        ASSERT(a.size() == 0);
        ASSERT(a.capacity() == 0);
        ASSERT(a.elements() == nullptr);
    }

    {
        Array<int> a(1);
        ASSERT(a.size() == 1);
        ASSERT(a.capacity() == 1);
        ASSERT(a.elements() != nullptr);
        ASSERT(a[0] == 0);
    }

    // Array(int size, int capacity)

    ASSERT_EXCEPTION(Exception, Array<int> a(-1, 0));
    ASSERT_EXCEPTION(Exception, Array<int> a(0, -1));
    ASSERT_EXCEPTION(Exception, Array<int> a(1, 0));

    {
        Array<int> a(0, 0);
        ASSERT(a.size() == 0);
        ASSERT(a.capacity() == 0);
        ASSERT(a.elements() == nullptr);
    }

    {
        Array<int> a(0, 1);
        ASSERT(a.size() == 0);
        ASSERT(a.capacity() == 1);
        ASSERT(a.elements() != nullptr);
    }

    {
        Array<int> a(1, 1);
        ASSERT(a.size() == 1);
        ASSERT(a.capacity() == 1);
        ASSERT(a.elements() != nullptr);
        ASSERT(a[0] == 0);
    }

    // Array(int size, const _Type* elements)

    {
        ASSERT_EXCEPTION(Exception, Array<int>(-1, np));
        ASSERT_EXCEPTION(Exception, Array<int>(0, ep));
        ASSERT_EXCEPTION(Exception, Array<int>(1, np));

        {
            Array<int> a(0, np);
            ASSERT(a.size() == 0);
            ASSERT(a.capacity() == 0);
            ASSERT(a.elements() == nullptr);
        }

        {
            Array<int> a(1, ep);
            ASSERT(a.size() == 1);
            ASSERT(a.capacity() == 1);
            ASSERT(a.elements() != ep);
            ASSERT(a[0] == *ep);
        }
    }

    // Array(int size, int capacity, const _Type* elements)

    {
        ASSERT_EXCEPTION(Exception, Array<int>(-1, 0, np));
        ASSERT_EXCEPTION(Exception, Array<int>(0, -1, np));
        ASSERT_EXCEPTION(Exception, Array<int>(1, 0, np));
        ASSERT_EXCEPTION(Exception, Array<int>(0, 0, ep));
        ASSERT_EXCEPTION(Exception, Array<int>(1, 1, np));
    
        {
            Array<int> a(0, 0, np);
            ASSERT(a.size() == 0);
            ASSERT(a.capacity() == 0);
            ASSERT(a.elements() == nullptr);
        }

        {
            Array<int> a(1, 1, ep);
            ASSERT(a.size() == 1);
            ASSERT(a.capacity() == 1);
            ASSERT(a.elements() != ep);
            ASSERT(a[0] == *ep);
        }

        {
            Array<int> a(1, 2, ep);
            ASSERT(a.size() == 1);
            ASSERT(a.capacity() == 2);
            ASSERT(a.elements() != ep);
            ASSERT(a[0] == *ep);
        }
    }

    // Array(const Array<_Type>& other)

    {
        Array<int> a1, a2(a1);

        ASSERT(a1.size() == 0);
        ASSERT(a1.capacity() == 0);
        ASSERT(a1.elements() == nullptr);

        ASSERT(a2.size() == 0);
        ASSERT(a2.capacity() == 0);
        ASSERT(a2.elements() == nullptr);
    }

    {
        Array<int> a1(1), a2(a1);

        ASSERT(a1.size() == 1);
        ASSERT(a1.capacity() == 1);
        ASSERT(a1.elements() != nullptr);
        ASSERT(a1[0] == 0);

        ASSERT(a2.size() == 1);
        ASSERT(a2.capacity() == 1);
        ASSERT(a2.elements() != nullptr);
        ASSERT(a2[0] == 0);

        ASSERT(a1.elements() != a2.elements());
    }

    // Array(Array<_Type>&& other)

    {
        Array<int> a1(1), a2(static_cast<Array<int>&&>(a1));

        ASSERT(a1.size() == 0);
        ASSERT(a1.capacity() == 0);
        ASSERT(a1.elements() == nullptr);

        ASSERT(a2.size() == 1);
        ASSERT(a2.capacity() == 1);
        ASSERT(a2.elements() != nullptr);
        ASSERT(a2[0] == 0);
    }

    // Array<_Type>& operator=(const Array<_Type>& other)

    {
        Array<int> a1, a2(1);
        a2 = a1;

        ASSERT(a1.size() == 0);
        ASSERT(a1.capacity() == 0);
        ASSERT(a1.elements() == nullptr);

        ASSERT(a2.size() == 0);
        ASSERT(a2.capacity() == 1);
        ASSERT(a2.elements() != nullptr);
    }

    {
        Array<int> a1(1), a2;
        a2 = a1;

        ASSERT(a1.size() == 1);
        ASSERT(a1.capacity() == 1);
        ASSERT(a1.elements() != nullptr);
        ASSERT(a1[0] == 0);

        ASSERT(a2.size() == 1);
        ASSERT(a2.capacity() == 1);
        ASSERT(a2.elements() != nullptr);
        ASSERT(a2[0] == 0);

        ASSERT(a1.elements() != a2.elements());
    }

    // Array<_Type>& operator=(Array<_Type>&& other)

    {
        Array<int> a1(1), a2;
        a2 = static_cast<Array<int>&&>(a1);

        ASSERT(a1.size() == 0);
        ASSERT(a1.capacity() == 0);
        ASSERT(a1.elements() == nullptr);

        ASSERT(a2.size() == 1);
        ASSERT(a2.capacity() == 1);
        ASSERT(a2.elements() != nullptr);
        ASSERT(a2[0] == 0);
    }

    // _Type& operator[](int index)

    {
        Array<int> a(2, ep);
        ASSERT(a[0] == 1);
        ASSERT(a[1] == 2);
    }

    // const _Type& operator[](int index) const

    {
        Array<int> a(2, ep);
        const Array<int>& ca = a;
        ASSERT(ca[0] == 1);
        ASSERT(ca[1] == 2);
    }

    // int size() const

    {
        Array<int> a;
        ASSERT(a.size() == 0);
        ASSERT(a.capacity() == 0);
        ASSERT(a.empty());
        ASSERT(a.elements() == nullptr);
    }

    {
        Array<int> a(3, 5, ep);

        ASSERT(a.size() == 3);
        ASSERT(a.capacity() == 5);
        ASSERT(!a.empty());
        ASSERT(a.elements() != nullptr);
        ASSERT(*a.elements() == 1);
        ASSERT(*(a.elements() + 1) == 2);
        ASSERT(*(a.elements() + 2) == 3);

        const Array<int>& ca = a;
        ASSERT(ca.elements() != nullptr);
        ASSERT(*ca.elements() == 1);
        ASSERT(*(ca.elements() + 1) == 2);
        ASSERT(*(ca.elements() + 2) == 3);
    }

    // void ensureCapacity(int capacity)

    {
        Array<int> a(3, ep);
        ASSERT(a.capacity() == 3);
        a.ensureCapacity(2);
        ASSERT(a.capacity() == 3);
        ASSERT(a[0] == 1);
        ASSERT(a[1] == 2);
        ASSERT(a[2] == 3);
    }

    {
        Array<int> a(3, ep);
        ASSERT(a.capacity() == 3);
        a.ensureCapacity(5);
        ASSERT(a.capacity() == 5);
        ASSERT(a[0] == 1);
        ASSERT(a[1] == 2);
        ASSERT(a[2] == 3);
    }

    // void shrinkToLength()

    {
        Array<int> a(3, ep);
        ASSERT(a.capacity() == 3);
        a.shrinkToLength();
        ASSERT(a.capacity() == 3);
        ASSERT(a[0] == 1);
        ASSERT(a[1] == 2);
        ASSERT(a[2] == 3);
    }

    {
        Array<int> a(3, 5, ep);
        ASSERT(a.capacity() == 5);
        a.shrinkToLength();
        ASSERT(a.capacity() == 3);
        ASSERT(a[0] == 1);
        ASSERT(a[1] == 2);
        ASSERT(a[2] == 3);
    }

    // void assign(const Array<_Type>& other)
}

void testFoundation()
{
    testString();
    testArray();
}

int MAIN()
{
    try
    {
        testFoundation();
    }
    catch (Exception& ex)
    {
        Console::writeLine(ex.message());
        return 1;
    }
    catch (...)
    {
        Console::writeLine(STR("unknown error"));
        return 1;
    }

    return 0;
}
