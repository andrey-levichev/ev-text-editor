#include <foundation.h>

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

    // String(int capacity)

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

    // String(String&& other)

    {
        String s1 = STR("a");
        String s2(static_cast<String&&>(s1));

        ASSERT(s1.chars() == nullptr);
        ASSERT(s1.empty());
        ASSERT(s1.capacity() == 0);

        ASSERT(s2 == "a");
        ASSERT(s2.length() == 1);
        ASSERT(s2.capacity() == 2);
    }

    // String& operator=(const String& other);

    {
        String s1(STR("a")), s2;
        s2 = s1;
        ASSERT(s2 == "a");
    }

    // String& operator=(const char_t* chars);

    {
        String s;
        s = STR("a");
        ASSERT(s == "a");
    }

    // String& operator=(String&& other);

    {
        String s1(STR("a")), s2;
        s2 = static_cast<String&&>(s1);

        ASSERT(s1.chars() == nullptr);
        ASSERT(s1.empty());
        ASSERT(s1.capacity() == 0);

        ASSERT(s2 == "a");
        ASSERT(s2.length() == 1);
        ASSERT(s2.capacity() == 2);
    }

    // String& operator+=(const String& str);

    {
        String s(STR("a"));
        s += String(STR("b"));
        ASSERT(s == "ab");
    }

    {
        String s(STR("a"));
        s += STR("b");
        ASSERT(s == "ab");
    }

    // length/capacity/str/chars/empty

    {
        String s;
        ASSERT(s.length() == 0);
        ASSERT(s.capacity() == 0);
        ASSERT(STRCMP(s.str(), STR("")) == 0);
        ASSERT(s.chars() == nullptr);
        ASSERT(s.empty());
    }

    {
        String s(10);
        s = STR("a");
        ASSERT(s.length() == 1);
        ASSERT(s.capacity() == 10);
        ASSERT(STRCMP(s.str(), STR("a")) == 0);
        ASSERT(s.chars() != nullptr);
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
        ASSERT(s.capacity() == 0);
    }

    {
        String s;
        s.ensureCapacity(10);
        ASSERT(s.capacity() == 10);
    }

    {
        String s(STR("abc"));

        s.ensureCapacity(2);
        ASSERT(s.capacity() == 4);
        ASSERT(s == STR("abc"));

        s.ensureCapacity(10);
        ASSERT(s.capacity() == 10);
        ASSERT(s == STR("abc"));
    }

    // void shrinkToLength()

    {
        String s(10);
        s = STR("abc");
        s.shrinkToLength();
        ASSERT(s.capacity() == 4);
        s.shrinkToLength();
        ASSERT(s.capacity() == 4);
    }

    // void assign(const String& str)

    {
        String s1(STR("a")), s2;
        s1.assign(s2);
        ASSERT(s1.chars() != nullptr);
        ASSERT(s1.length() == 0);
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
        s.assign(STR(""));
        ASSERT(s.chars() != nullptr);
        ASSERT(s.length() == 0);
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
        String s1(STR("a")), s2;
        s1.append(s2);
        ASSERT(s1 == STR("a"));
        ASSERT(s1.capacity() == 2);
    }

    {
        String s1, s2(STR("b"));
        s1.append(s2);
        ASSERT(s1 == STR("b"));
        ASSERT(s1.capacity() == 4);
    }

    {
        String s1(STR("a")), s2(STR("b"));
        s1.append(s2);
        ASSERT(s1 == STR("ab"));
        ASSERT(s1.capacity() == 6);
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
        s.append(STR(""));
        ASSERT(s == STR("a"));
        ASSERT(s.capacity() == 2);
    }

    {
        String s;
        s.append(STR("b"));
        ASSERT(s == STR("b"));
        ASSERT(s.capacity() == 4);
    }

    {
        String s(STR("a"));
        s.append(STR("b"));
        ASSERT(s == STR("ab"));
        ASSERT(s.capacity() == 6);
    }

    {
        String s(10);
        s.append(STR("b"));
        ASSERT(s == STR("b"));
        ASSERT(s.capacity() == 10);
    }

    // void insert(int pos, const String& str)

    ASSERT_EXCEPTION(Exception, String(STR("a")).insert(-1, String()));
    ASSERT_EXCEPTION(Exception, String(STR("a")).insert(2, String()));

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
        ASSERT(s1.capacity() == 4);
    }

    {
        String s1(STR("a")), s2(STR("b"));
        s1.insert(0, s2);
        ASSERT(s1 == STR("ba"));
        ASSERT(s1.capacity() == 6);
    }

    {
        String s1(STR("a")), s2(STR("b"));
        s1.insert(1, s2);
        ASSERT(s1 == STR("ab"));
        ASSERT(s1.capacity() == 6);
    }

    {
        String s1(10), s2(STR("b"));
        s1.insert(0, s2);
        ASSERT(s1 == STR("b"));
        ASSERT(s1.capacity() == 10);
    }

    // void insert(int pos, const char_t* chars)

    ASSERT_EXCEPTION(Exception, String(STR("a")).insert(-1, STR("b")));
    ASSERT_EXCEPTION(Exception, String(STR("a")).insert(2, STR("b")));
    ASSERT_EXCEPTION(Exception, String(STR("a")).insert(0, nullptr));

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
        ASSERT(s.capacity() == 4);
    }

    {
        String s(STR("a"));
        s.insert(0, STR("b"));
        ASSERT(s == STR("ba"));
        ASSERT(s.capacity() == 6);
    }

    {
        String s(STR("a"));
        s.insert(1, STR("b"));
        ASSERT(s == STR("ab"));
        ASSERT(s.capacity() == 6);
    }

    {
        String s(10);
        s.insert(0, STR("b"));
        ASSERT(s == STR("b"));
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
        ASSERT(STRCMP(s.chars(), STR("")) == 0);
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

    {
        char* p = nullptr;
        ASSERT_EXCEPTION(Exception, String(p));
    }

    {
        char* p = Memory::allocateArray<char_t>(1);
        STRCPY(p, "");
        String s = String::acquire(p);
        ASSERT(s.chars() == p);
        ASSERT(s == STR(""));
        ASSERT(s.length() == 0);
        ASSERT(s.capacity() == 1);
    }

    {
        char* p = Memory::allocateArray<char_t>(2);
        STRCPY(p, "a");
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
        String s(STR("ab"));
        s.replace(0, 1, String());
        ASSERT(s == STR("b"));
        ASSERT(s.length() == 1);
        ASSERT(s.capacity() == 3);
    }

    {
        String s(STR("a"));
        s.replace(0, 1, String("b"));
        ASSERT(s == STR("b"));
        ASSERT(s.length() == 1);
        ASSERT(s.capacity() == 2);
    }

    {
        String s(STR("abcd"));
        s.replace(1, 2, String("x"));
        ASSERT(s == STR("axd"));
        ASSERT(s.length() == 3);
        ASSERT(s.capacity() == 5);
    }

    {
        String s(STR("abcd"));
        s.replace(1, 2, String("xyz"));
        ASSERT(s == STR("axyzd"));
        ASSERT(s.length() == 5);
        ASSERT(s.capacity() == 12);
    }

    {
        String s(STR("abcd"));
        s.replace(0, 2, String("x"));
        ASSERT(s == STR("xcd"));
        ASSERT(s.length() == 3);
        ASSERT(s.capacity() == 5);
    }

    {
        String s(STR("abcd"));
        s.replace(2, 2, String("x"));
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
        ASSERT(s.capacity() == 12);
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
        ASSERT(s.empty());
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
        ASSERT(s.capacity() == 12);
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
        ASSERT(s.capacity() == 12);
    }

    {
        String s(STR("abcbd"));
        s.replace(String(STR("b")), String(STR("xy")));
        ASSERT(s == STR("axycxyd"));
        ASSERT(s.length() == 7);
        ASSERT(s.capacity() == 16);
    }

    // void replace(const char_t* searchChars, const char_t* replaceChars)

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
        ASSERT(s.empty());
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
        ASSERT(s.capacity() == 12);
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
        ASSERT(s.capacity() == 12);
    }

    {
        String s(STR("abcbd"));
        s.replace(STR("b"), STR("xy"));
        ASSERT(s == STR("axycxyd"));
        ASSERT(s.length() == 7);
        ASSERT(s.capacity() == 16);
    }

    // void trim()

    {
        String s;
        s.trim();
        ASSERT(s.empty());
    }

    {
        String s(STR(" "));
        s.trim();
        ASSERT(s.empty());
        ASSERT(s.capacity() == 2);
    }

    {
        String s(STR(" a"));
        s.trim();
        ASSERT(s == STR("a"));
        ASSERT(s.capacity() == 3);
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
        ASSERT(s.empty());
    }

    {
        String s(STR(" "));
        s.trimRight();
        ASSERT(s.empty());
        ASSERT(s.capacity() == 2);
    }

    {
        String s(STR(" a"));
        s.trimRight();
        ASSERT(s == STR(" a"));
        ASSERT(s.capacity() == 3);
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
        ASSERT(s.empty());
    }

    {
        String s(STR(" "));
        s.trimLeft();
        ASSERT(s.empty());
        ASSERT(s.capacity() == 2);
    }

    {
        String s(STR(" a"));
        s.trimLeft();
        ASSERT(s == STR("a"));
        ASSERT(s.capacity() == 3);
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
        ASSERT(s.empty());
    }

    {
        String s(STR("a"));
        s.reverse();
        ASSERT(s == STR("a"));
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
}

void testFoundation()
{
    testString();
}