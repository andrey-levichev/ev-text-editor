#include <test.h>

const char* CHARS8 = "\x24\xc2\xa2\xe2\x82\xac\xf0\x90\x8d\x88";
const char16_t* CHARS16 = u"\x0024\x00a2\x20ac\xd800\xdf48";
const char32_t* CHARS32 = U"\x00000024\x000000a2\x000020ac\x00010348";

#ifdef CHAR_ENCODING_UTF8
const char_t* CHARS = CHARS8;
const char_t* BIG_CHAR = "\xf0\x90\x8d\x88";
#else
const char_t* CHARS = CHARS16;
const char_t* BIG_CHAR = u"\xd800\xdf48";
#endif

int hash(const UniquePtr<int>& val)
{
    return val ? *val : 0;
}

bool equalsTo(const UniquePtr<int>& left, const UniquePtr<int>& right)
{
    if (left)
    {
        if (right)
            return *left == *right;
        else
            return false;
    }
    else
    {
        if (right)
            return false;
        else
            return true;
    }
}

template<typename _Char>
int compareUniString(const _Char* str1, const _Char* str2)
{
    for (; *str1 == *str2; str1++, str2++)
        if (!*str1)
            return 0;

    return static_cast<unsigned>(*str1) >
        static_cast<unsigned>(*str2) ? 1 : -1;
}

template<typename _SeqType, typename _ElemType>
bool compareSequence(const _SeqType& seq, const _ElemType* values)
{
    auto iter = seq.constIterator();
    while (!iter.moveNext())
        if (iter.value() != *values++)
            return false;
    return true;
}

template<typename _IterType>
bool compareSequenceIter(_IterType& iter)
{
    return true;
}

template<typename _IterType, typename _Arg, typename... _Args>
bool compareSequenceIter(_IterType& iter, _Arg&& arg, _Args&&... args)
{
    if (iter.moveNext())
    {
        if (iter.value() == arg)
            return compareSequenceIter(iter, static_cast<_Args&&>(args)...);
        else
            return false;
    }
    else
        return true;
}

template<typename _SeqType, typename... _Args>
bool compareSequence(const _SeqType& seq, _Args&&... args)
{
    auto iter = seq.constIterator();
    return compareSequenceIter(iter, static_cast<_Args&&>(args)...);
}

void testUniquePtr()
{
    // UniquePtr()

    {
        UniquePtr<Test> p;
        ASSERT_EXCEPTION(NullPointerException, *p);
        ASSERT_EXCEPTION(NullPointerException, p->val());
        ASSERT(!p);
        ASSERT(!p.ptr());
    }

    // UniquePtr(UniquePtr<_Type>&& other)

    {
        UniquePtr<Test> p1 = createUnique<Test>();
        Test* p = p1.ptr();
        ASSERT(p1);

        UniquePtr<Test> p2 = static_cast<UniquePtr<Test>&&>(p1);
        ASSERT(!p1);
        ASSERT(p2 && p2.ptr() == p);
    } 

    // UniquePtr& operator=(UniquePtr<_Type>&& other)

    {
        UniquePtr<Test> p1 = createUnique<Test>();
        Test* p = p1.ptr();
        ASSERT(p1);

        UniquePtr<Test> p2;
        p2 = static_cast<UniquePtr<Test>&&>(p1);
        ASSERT(!p1);
        ASSERT(p2 && p2.ptr() == p);
    }

    // _Type& operator*() const

    // _Type* operator->() const

    // operator bool() const

    // bool empty() const

    // _Type* ptr() const

    // void create(_Args&&... args)

    {
        UniquePtr<Test> p;
        ASSERT(!p);
        ASSERT(p.empty());

        p.create(1);
        ASSERT(p);
        ASSERT(p.ptr());
        ASSERT((*p).val() == 1);
        ASSERT(p->val() == 1);
        ASSERT(!p.empty());

        p.create(2);
        ASSERT(p);
        ASSERT(p.ptr());
        ASSERT((*p).val() == 2);
        ASSERT(p->val() == 2);
        ASSERT(!p.empty());
    }

    // void reset()

    {
        UniquePtr<Test> p;
        ASSERT(!p);
        p.reset();
        ASSERT(!p);
    }

    {
        UniquePtr<Test> p;
        ASSERT(!p);
        p.create();
        ASSERT(p);
        p.reset();
        ASSERT(!p);
    }

    // bool operator==(const UniquePtr<_Type>& left, const UniquePtr<_Type>& right)

    {
        UniquePtr<Test> p1 = createUnique<Test>();
        UniquePtr<Test> p2 = createUnique<Test>();
        ASSERT(!(p1 == p2));
    }

    // bool operator!=(const UniquePtr<_Type>& left, const UniquePtr<_Type>& right)

    {
        UniquePtr<Test> p1 = createUnique<Test>();
        UniquePtr<Test> p2 = createUnique<Test>();
        ASSERT(p1 != p2);
    }

    // UniquePtr<_T> createUnique(_Args&&... args)

    {
        UniquePtr<Test> p = createUnique<Test>(1);
        ASSERT(p);
        ASSERT(p.ptr());
        ASSERT((*p).val() == 1);
        ASSERT(p->val() == 1);
    }
}

void testSharedPtr()
{
    // SharedPtr()

    {
        SharedPtr<Test> p;
        ASSERT_EXCEPTION(NullPointerException, *p);
        ASSERT_EXCEPTION(NullPointerException, p->val());
        ASSERT(!p);
        ASSERT(!p.ptr());
        ASSERT(p.refCount() == 0);
    }

    // SharedPtr(const SharedPtr<_Type>& other)

    {
        SharedPtr<Test> p1 = createShared<Test>();
        Test* p = p1.ptr();
        ASSERT(p1);

        {
            SharedPtr<Test> p2 = p1;
            ASSERT(p1 && p1.ptr() == p);
            ASSERT(p2 && p2.ptr() == p);
            ASSERT(p1.refCount() == 2 && p2.refCount() == 2);
        }

        ASSERT(p1 && p1.ptr() == p);
        ASSERT(p1.refCount() == 1);
    } 

    // SharedPtr(SharedPtr<_Type>&& other)

    {
        SharedPtr<Test> p1 = createShared<Test>();
        Test* p = p1.ptr();
        ASSERT(p1);

        SharedPtr<Test> p2 = static_cast<SharedPtr<Test>&&>(p1);
        ASSERT(!p1);
        ASSERT(p2 && p2.ptr() == p);
        ASSERT(p1.refCount() == 0 && p2.refCount() == 1);
    } 

    // SharedPtr<_Type>& operator=(const SharedPtr<_Type>& other)

    {
        SharedPtr<Test> p1 = createShared<Test>();
        Test* p = p1.ptr();
        ASSERT(p1);

        {
            SharedPtr<Test> p2;
            p2 = p1;
            ASSERT(p1 && p1.ptr() == p);
            ASSERT(p2 && p2.ptr() == p);
            ASSERT(p1.refCount() == 2 && p2.refCount() == 2);
        }

        ASSERT(p1 && p1.ptr() == p);
        ASSERT(p1.refCount() == 1);
    } 

    // SharedPtr<_Type>& operator=(SharedPtr<_Type>&& other)

    {
        SharedPtr<Test> p1 = createShared<Test>();
        Test* p = p1.ptr();
        ASSERT(p1);

        SharedPtr<Test> p2;
        p2 = static_cast<SharedPtr<Test>&&>(p1);
        ASSERT(!p1);
        ASSERT(p2 && p2.ptr() == p);
        ASSERT(p1.refCount() == 0 && p2.refCount() == 1);
    }

    // _Type& operator*() const

    // _Type* operator->() const

    // operator bool() const

    // bool empty() const

    // _Type* ptr() const

    // int refCount() const

    // void create(_Args&&... args)

    {
        SharedPtr<Test> p;
        ASSERT(!p);
        ASSERT(p.empty());

        p.create(1);
        ASSERT(p);
        ASSERT(p.ptr());
        ASSERT((*p).val() == 1);
        ASSERT(p->val() == 1);
        ASSERT(!p.empty());

        p.create(2);
        ASSERT(p);
        ASSERT(p.ptr());
        ASSERT((*p).val() == 2);
        ASSERT(p->val() == 2);
        ASSERT(!p.empty());
    }

    // void reset()

    {
        SharedPtr<Test> p;
        ASSERT(!p);
        p.reset();
        ASSERT(!p);
    }

    {
        SharedPtr<Test> p;
        ASSERT(!p);
        p.create();
        ASSERT(p);
        p.reset();
        ASSERT(!p);
    }

    // bool operator==(const SharedPtr<_Type>& left, const SharedPtr<_Type>& right)

    {
        SharedPtr<Test> p1 = createShared<Test>();
        SharedPtr<Test> p2 = p1;
        ASSERT(p1 == p2);
    }

    // bool operator!=(const SharedPtr<_Type>& left, const SharedPtr<_Type>& right)

    {
        SharedPtr<Test> p1 = createShared<Test>();
        SharedPtr<Test> p2 = createShared<Test>();
        ASSERT(p1 != p2);
    }

    // SharedPtr<_T> createShared(_Args&&... args)

    {
        SharedPtr<Test> p = createShared<Test>(1);
        ASSERT(p);
        ASSERT(p.ptr());
        ASSERT((*p).val() == 1);
        ASSERT(p->val() == 1);
        ASSERT(p.refCount() == 1);
    }
}

void testString()
{
    unichar_t zc = 0;
    const char_t* np = NULL;
    char_t ep[] = { 0 };

    // Unicode support

    {
        char32_t ch;
        ASSERT(utf8CharToUnicode("\x24", ch) == 1 && ch == 0x24);
        ASSERT(utf8CharToUnicode("\xc2\xa2", ch) == 2 && ch == 0xa2);
        ASSERT(utf8CharToUnicode("\xe2\x82\xac", ch) == 3 && ch == 0x20ac);
        ASSERT(utf8CharToUnicode("\xf0\x90\x8d\x88", ch) == 4 && ch == 0x10348);
    }

    {
        char s[4];
        ASSERT(unicodeCharToUtf8(0x24, s) == 1 && memcmp(s, "\x24", 1) == 0);
        ASSERT(unicodeCharToUtf8(0xa2, s) == 2 && memcmp(s, "\xc2\xa2", 2) == 0);
        ASSERT(unicodeCharToUtf8(0x20ac, s) == 3 && memcmp(s, "\xe2\x82\xac", 3) == 0);
        ASSERT(unicodeCharToUtf8(0x10348, s) == 4 && memcmp(s, "\xf0\x90\x8d\x88", 4) == 0);
    }

    {
        char32_t ch;
        ASSERT(utf16CharToUnicode(u"\x0024", ch) == 1 && ch == 0x24);
        ASSERT(utf16CharToUnicode(u"\x00a2", ch) == 1 && ch == 0xa2);
        ASSERT(utf16CharToUnicode(u"\x20ac", ch) == 1 && ch == 0x20ac);
        ASSERT(utf16CharToUnicode(u"\xd800\xdf48", ch) == 2 && ch == 0x10348);
    }

    {
        char16_t s[2];
        ASSERT(unicodeCharToUtf16(0x24, s) == 1 && memcmp(s, u"\x0024", 2) == 0);
        ASSERT(unicodeCharToUtf16(0xa2, s) == 1 && memcmp(s, u"\x00a2", 2) == 0);
        ASSERT(unicodeCharToUtf16(0x20ac, s) == 1 && memcmp(s, u"\x20ac", 2) == 0);
        ASSERT(unicodeCharToUtf16(0x10348, s) == 2 && memcmp(s, u"\xd800\xdf48", 4) == 0);
    }

    {
        char chars8[100];
        char16_t chars16[100];
        char32_t chars32[100];

        utf8StringToUnicode(CHARS8, chars32);
        ASSERT(compareUniString(chars32, CHARS32) == 0);

        unicodeStringToUtf8(CHARS32, chars8);
        ASSERT(compareUniString(chars8, CHARS8) == 0);

        utf16StringToUnicode(CHARS16, chars32);
        ASSERT(compareUniString(chars32, CHARS32) == 0);

        unicodeStringToUtf16(CHARS32, chars16);
        ASSERT(compareUniString(chars16, CHARS16) == 0);

        utf8StringToUtf16(CHARS8, chars16);
        ASSERT(compareUniString(chars16, CHARS16) == 0);

        utf16StringToUtf8(CHARS16, chars8);
        ASSERT(compareUniString(chars8, CHARS8) == 0);
    }

    {
        const char* pos = CHARS8;
        ASSERT(utf8CharAt(pos) == 0x24);
        pos = utf8CharForward(pos);
        ASSERT(utf8CharAt(pos) == 0xa2);
        pos = utf8CharForward(pos);
        ASSERT(utf8CharAt(pos) == 0x20ac);
        pos = utf8CharForward(pos);
        ASSERT(utf8CharAt(pos) == 0x10348);
        pos = utf8CharForward(pos);
        ASSERT(utf8CharAt(pos) == 0);
        pos = utf8CharForward(pos);
        ASSERT(utf8CharAt(pos) == 0);
        pos = utf8CharBack(pos, CHARS8);
        ASSERT(utf8CharAt(pos) == 0x10348);
        pos = utf8CharBack(pos, CHARS8);
        ASSERT(utf8CharAt(pos) == 0x20ac);
        pos = utf8CharBack(pos, CHARS8);
        ASSERT(utf8CharAt(pos) == 0xa2);
        pos = utf8CharBack(pos, CHARS8);
        ASSERT(utf8CharAt(pos) == 0x24);
        pos = utf8CharBack(pos, CHARS8);
        ASSERT(utf8CharAt(pos) == 0x24);
    }

    ASSERT(utf8CharLength(0x24) == 1);
    ASSERT(utf8CharLength(0xa2) == 2);
    ASSERT(utf8CharLength(0x20ac) == 3);
    ASSERT(utf8CharLength(0x10348) == 4);
    ASSERT(utf8StringLength(CHARS8) == 4);

    {
        const char16_t* pos = CHARS16;
        ASSERT(utf16CharAt(pos) == 0x24);
        pos = utf16CharForward(pos);
        ASSERT(utf16CharAt(pos) == 0xa2);
        pos = utf16CharForward(pos);
        ASSERT(utf16CharAt(pos) == 0x20ac);
        pos = utf16CharForward(pos);
        ASSERT(utf16CharAt(pos) == 0x10348);
        pos = utf16CharForward(pos);
        ASSERT(utf16CharAt(pos) == 0);
        pos = utf16CharForward(pos);
        ASSERT(utf16CharAt(pos) == 0);
        pos = utf16CharBack(pos, CHARS16);
        ASSERT(utf16CharAt(pos) == 0x10348);
        pos = utf16CharBack(pos, CHARS16);
        ASSERT(utf16CharAt(pos) == 0x20ac);
        pos = utf16CharBack(pos, CHARS16);
        ASSERT(utf16CharAt(pos) == 0xa2);
        pos = utf16CharBack(pos, CHARS16);
        ASSERT(utf16CharAt(pos) == 0x24);
        pos = utf16CharBack(pos, CHARS16);
        ASSERT(utf16CharAt(pos) == 0x24);
    }

    ASSERT(utf16CharLength(0x24) == 1);
    ASSERT(utf16CharLength(0xa2) == 1);
    ASSERT(utf16CharLength(0x20ac) == 1);
    ASSERT(utf16CharLength(0x10348) == 2);
    ASSERT(utf16StringLength(CHARS16) == 4);

    // String()

    {
        String s;
        ASSERT(!s.chars());
        ASSERT(s.length() == 0);
        ASSERT(s.capacity() == 0);
    }

    // String(const String& other)

    {
        String s1, s2(s1);
        ASSERT(!s2.chars());
        ASSERT(s2.length() == 0);
        ASSERT(s2.capacity() == 0);
    }

    {
        String s1(STR("a")), s2(s1);
        ASSERT(s2 == STR("a"));
        ASSERT(s2.length() == 1);
        ASSERT(s2.capacity() == 2);
    }

    // String(const char_t* pos, int len = -1)

    ASSERT_EXCEPTION(Exception, String(np, 1));

    {
        String s(np);
        ASSERT(!s.chars());
        ASSERT(s.length() == 0);
        ASSERT(s.capacity() == 0);
    }

    {
        String s(np, 0);
        ASSERT(!s.chars());
        ASSERT(s.length() == 0);
        ASSERT(s.capacity() == 0);
    }

    {
        String s(STR(""));
        ASSERT(!s.chars());
        ASSERT(s.length() == 0);
        ASSERT(s.capacity() == 0);
    }

    {
        String s(STR(""), 0);
        ASSERT(!s.chars());
        ASSERT(s.length() == 0);
        ASSERT(s.capacity() == 0);
    }

    {
        String s(STR(""), 1);
        ASSERT(!s.chars());
        ASSERT(s.length() == 0);
        ASSERT(s.capacity() == 0);
    }

    {
        String s(STR("a"));
        ASSERT(s == STR("a"));
        ASSERT(s.length() == 1);
        ASSERT(s.capacity() == 2);
    }

    {
        String s(STR("a"), 1);
        ASSERT(s == STR("a"));
        ASSERT(s.length() == 1);
        ASSERT(s.capacity() == 2);
    }

    {
        String s(STR("a"), 2);
        ASSERT(s == STR("a"));
        ASSERT(s.length() == 1);
        ASSERT(s.capacity() == 2);
    }

    // String(unichar_t ch, int len = 1)

    ASSERT_EXCEPTION(Exception, String(zc, 1));
    ASSERT_EXCEPTION(Exception, String('a', -1));

    {
        String s('a', 0);
        ASSERT(!s.chars());
        ASSERT(s.length() == 0);
        ASSERT(s.capacity() == 0);
    }

    {
        String s('a', 1);
        ASSERT(s == STR("a"));
        ASSERT(s.length() == 1);
        ASSERT(s.capacity() == 2);
    }

    {
        String s('a', 2);
        ASSERT(s == STR("aa"));
        ASSERT(s.length() == 2);
        ASSERT(s.capacity() == 3);
    }

    // String(String&& other)

    {
        String s1 = STR("a");
        String s2(static_cast<String&&>(s1));

        ASSERT(!s1.chars());
        ASSERT(s1.length() == 0);
        ASSERT(s1.capacity() == 0);

        ASSERT(s2 == STR("a"));
        ASSERT(s2.length() == 1);
        ASSERT(s2.capacity() == 2);
    }

    // String& operator=(const String& other);
    
    {
        String s;
        s = String(STR("abc"));
        ASSERT(s == STR("abc"));
    }

    // String& operator=(const char_t* chars);
 
    {
        String s;
        s = STR("abc");
        ASSERT(s == STR("abc"));
    }

    // String& operator=(String&& other);

    {
        String s1(STR("a")), s2;
        s2 = static_cast<String&&>(s1);

        ASSERT(!s1.chars());
        ASSERT(s1.length() == 0);
        ASSERT(s1.capacity() == 0);

        ASSERT(s2 == STR("a"));
        ASSERT(s2.length() == 1);
        ASSERT(s2.capacity() == 2);
    }

    // String& operator+=(const String& str);

    {
        String s1(STR("a")), s2(STR("b"));
        s1 += s2;
        ASSERT(s1 == STR("ab"));
    }

    // String& operator+=(const char_t* chars)

    {
        String s(STR("a"));
        s += STR("b");
        ASSERT(s == STR("ab"));
    }

    // String& operator+=(unichar_t ch)

    {
        String s(STR("a"));
        s += 'b';
        ASSERT(s == STR("ab"));
    }

    // length/charLength/capacity/str/chars/empty

    {
        String s;
        ASSERT(s == STR(""));
        ASSERT(s.length() == 0);
        ASSERT(s.charLength() == 0);
        ASSERT(s.capacity() == 0);
        ASSERT(strCompare(s.str(), STR("")) == 0);
        ASSERT(!s.chars());
        ASSERT(s.empty());
    }

    {
        String s;
        s.ensureCapacity(10);
        s = BIG_CHAR;
        ASSERT(s == BIG_CHAR);
#ifdef CHAR_ENCODING_UTF16
        ASSERT(s.length() == 2);
#else
        ASSERT(s.length() == 4);
#endif
        ASSERT(s.charLength() == 1);
        ASSERT(s.capacity() == 10);
        ASSERT(strCompare(s.str(), BIG_CHAR) == 0);
        ASSERT(strCompare(s.chars(), BIG_CHAR) == 0);
        ASSERT(!s.empty());
    }

    // char_t* charPosition(int n)

    ASSERT(!String().charPosition(0));

    {
        String s(CHARS);
        ASSERT_EXCEPTION(Exception, s.charPosition(-1));
        ASSERT_EXCEPTION(Exception, s.charPosition(s.length() + 1));
        ASSERT(s.charPosition(0) == s.chars());
#ifdef CHAR_ENCODING_UTF16
        ASSERT(s.charPosition(3) == s.chars() + 3);
#else
        ASSERT(s.charPosition(3) == s.chars() + 6);
#endif        
    }

    // unichar_t charAt(const char_t* pos) const
    // char_t* charForward(char_t* pos, int n = 1)
    // char_t* charBack(char_t* pos, int n = 1)

    {
        char_t pos[1] = { 0 };
        ASSERT(String().charAt(NULL) == 0);
        ASSERT_EXCEPTION(Exception, String().charAt(pos));
        ASSERT(!String().charForward(NULL));
        ASSERT_EXCEPTION(Exception, String().charForward(pos));
        ASSERT(!String().charBack(NULL));
        ASSERT_EXCEPTION(Exception, String().charBack(pos));
    }

    {
        String s(CHARS);

        ASSERT_EXCEPTION(Exception, s.charAt(s.chars() - 1));
        ASSERT_EXCEPTION(Exception, s.charAt(s.chars() + s.length() + 1));
        ASSERT_EXCEPTION(Exception, s.charForward(s.chars() - 1));
        ASSERT_EXCEPTION(Exception, s.charForward(s.chars() + s.length() + 1));
        ASSERT_EXCEPTION(Exception, s.charForward(s.chars(), -1));
        ASSERT_EXCEPTION(Exception, s.charBack(s.chars() - 1));
        ASSERT_EXCEPTION(Exception, s.charBack(s.chars() + s.length() + 1));
        ASSERT_EXCEPTION(Exception, s.charBack(s.chars() + s.length(), -1));

        char_t* pos = s.chars();
        ASSERT(s.charAt(pos) == 0x24);
        pos = s.charForward(pos);
        ASSERT(s.charAt(pos) == 0xa2);
        pos = s.charForward(pos);
        ASSERT(s.charAt(pos) == 0x20ac);
        pos = s.charForward(pos);
        ASSERT(s.charAt(pos) == 0x10348);
        pos = s.charForward(pos);
        ASSERT(s.charAt(pos) == 0);
        pos = s.charForward(pos);
        ASSERT(s.charAt(pos) == 0);
        pos = s.charBack(pos);
        ASSERT(s.charAt(pos) == 0x10348);
        pos = s.charBack(pos);
        ASSERT(s.charAt(pos) == 0x20ac);
        pos = s.charBack(pos);
        ASSERT(s.charAt(pos) == 0xa2);
        pos = s.charBack(pos);
        ASSERT(s.charAt(pos) == 0x24);
        pos = s.charBack(pos);
        ASSERT(s.charAt(pos) == 0x24);

        ASSERT(s.charForward(s.chars(), 0) == s.chars());
        ASSERT(s.charBack(s.chars(), 0) == s.chars());
    }

    // const char_t* charPosition(int n) const

    {
        String s;
        const String& cs = s;
        ASSERT(!cs.charPosition(0));
    }

    {
        String s(CHARS);
        const String& cs = s;

        ASSERT_EXCEPTION(Exception, cs.charPosition(-1));
        ASSERT_EXCEPTION(Exception, cs.charPosition(cs.length() + 1));
        ASSERT(cs.charPosition(0) == cs.chars());
#ifdef CHAR_ENCODING_UTF16
        ASSERT(cs.charPosition(3) == cs.chars() + 3);
#else
        ASSERT(cs.charPosition(3) == cs.chars() + 6);
#endif        
    }

    // unichar_t charAt(const char_t* pos) const
    // const char_t* charForward(const char_t* pos, int n = 1) const
    // const char_t* charBack(const char_t* pos, int n = 1) const

    {
        const char_t* pos = STR("");
        ASSERT(String().charAt(NULL) == 0);
        ASSERT_EXCEPTION(Exception, String().charAt(pos));
        ASSERT(!String().charForward(np));
        ASSERT_EXCEPTION(Exception, String().charForward(pos));
        ASSERT(!String().charBack(np));
        ASSERT_EXCEPTION(Exception, String().charBack(pos));
    }

    {
        String s(CHARS);
        const String& cs = s;

        ASSERT_EXCEPTION(Exception, cs.charAt(cs.chars() - 1));
        ASSERT_EXCEPTION(Exception, cs.charAt(cs.chars() + cs.length() + 1));
        ASSERT_EXCEPTION(Exception, cs.charForward(cs.chars() - 1));
        ASSERT_EXCEPTION(Exception, cs.charForward(cs.chars() + cs.length() + 1));
        ASSERT_EXCEPTION(Exception, cs.charForward(cs.chars(), -1));
        ASSERT_EXCEPTION(Exception, cs.charBack(cs.chars() - 1));
        ASSERT_EXCEPTION(Exception, cs.charBack(cs.chars() + cs.length() + 1));
        ASSERT_EXCEPTION(Exception, cs.charBack(cs.chars() + cs.length(), -1));

        const char_t* pos = cs.chars();
        ASSERT(cs.charAt(pos) == 0x24);
        pos = cs.charForward(pos);
        ASSERT(cs.charAt(pos) == 0xa2);
        pos = cs.charForward(pos);
        ASSERT(cs.charAt(pos) == 0x20ac);
        pos = cs.charForward(pos);
        ASSERT(cs.charAt(pos) == 0x10348);
        pos = cs.charForward(pos);
        ASSERT(cs.charAt(pos) == 0);
        pos = cs.charForward(pos);
        ASSERT(cs.charAt(pos) == 0);
        pos = cs.charBack(pos);
        ASSERT(cs.charAt(pos) == 0x10348);
        pos = cs.charBack(pos);
        ASSERT(cs.charAt(pos) == 0x20ac);
        pos = cs.charBack(pos);
        ASSERT(cs.charAt(pos) == 0xa2);
        pos = cs.charBack(pos);
        ASSERT(cs.charAt(pos) == 0x24);
        pos = cs.charBack(pos);
        ASSERT(cs.charAt(pos) == 0x24);

        ASSERT(cs.charForward(cs.chars(), 0) == cs.chars());
        ASSERT(cs.charBack(cs.chars(), 0) == cs.chars());
    }

    // String substr(const char_t* pos, int len = -1) const

    {
        String s(STR("abcd"));
        ASSERT(s.substr(s.charPosition(1), 2) == STR("bc"));
    }

    // int compare(const String& str) const

    ASSERT(String(STR("a")).compare(String()) > 0);
    ASSERT(String().compare(String(STR("a"))) < 0);
    ASSERT(String().compare(String()) == 0);
    ASSERT(String(STR("ab")).compare(String(STR("a"))) > 0);
    ASSERT(String(STR("a")).compare(String(STR("ab"))) < 0);
    ASSERT(String(STR("a")).compare(String(STR("a"))) == 0);

    // int compare(const char_t* chars) const

    ASSERT(String(STR("a")).compare(NULL) > 0);
    ASSERT(String().compare(NULL) == 0);
    ASSERT(String(STR("ab")).compare(STR("a")) > 0);
    ASSERT(String(STR("a")).compare(STR("ab")) < 0);
    ASSERT(String(STR("a")).compare(STR("a")) == 0);

    // int compareNoCase(const String& str) const

    ASSERT(String(STR("a")).compareNoCase(String()) > 0);
    ASSERT(String().compareNoCase(String(STR("a"))) < 0);
    ASSERT(String().compareNoCase(String()) == 0);
    ASSERT(String(STR("AB")).compareNoCase(String(STR("a"))) > 0);
    ASSERT(String(STR("A")).compareNoCase(String(STR("ab"))) < 0);
    ASSERT(String(STR("A")).compareNoCase(String(STR("a"))) == 0);

    // int compareNoCase(const char_t* chars) const

    ASSERT(String(STR("a")).compareNoCase(NULL) > 0);
    ASSERT(String().compareNoCase(NULL) == 0);
    ASSERT(String(STR("AB")).compareNoCase(STR("a")) > 0);
    ASSERT(String(STR("A")).compareNoCase(STR("ab")) < 0);
    ASSERT(String(STR("A")).compareNoCase(STR("a")) == 0);

    // char_t* find(const String& str, char_t* pos = NULL)

    {
        String s;
        ASSERT(!s.find(String()));
        ASSERT(!s.find(String(STR("a"))));
        ASSERT_EXCEPTION(Exception, s.find(String(), ep));
    }

    {
        String s(STR("abcdabcd"));
        ASSERT(s.find(String(STR("bc"))) == s.charPosition(1));
        ASSERT(s.find(String(STR("bc")), s.charPosition(2)) == s.charPosition(5));
        ASSERT(!s.find(String(STR("bc")), s.charPosition(6)));
        ASSERT(!s.find(String(STR("xy"))));

        ASSERT_EXCEPTION(Exception, s.find(String(), s.chars() - 1));
        ASSERT_EXCEPTION(Exception, s.find(String(), s.chars() + s.length() + 1));
        ASSERT(!s.find(String()));
    }

    // char_t* find(const char_t* chars, char_t* pos = NULL)

    {
        String s;
        ASSERT(!s.find(np));
        ASSERT(!s.find(STR("a")));
        ASSERT_EXCEPTION(Exception, s.find(np, ep));
    }

    {
        String s(STR("abcdabcd"));
        ASSERT(s.find(STR("bc")) == s.charPosition(1));
        ASSERT(s.find(STR("bc"), s.charPosition(2)) == s.charPosition(5));
        ASSERT(!s.find(STR("bc"), s.charPosition(6)));
        ASSERT(!s.find(STR("xy")));

        ASSERT_EXCEPTION(Exception, s.find(np, s.chars() - 1));
        ASSERT_EXCEPTION(Exception, s.find(np, s.chars() + s.length() + 1));
        ASSERT(!s.find(np));
    }

    // char_t* find(unichar_t ch, char_t* pos = NULL)

    {
        String s;
        ASSERT_EXCEPTION(Exception, s.find(zc));
        ASSERT(!s.find('a'));
        ASSERT_EXCEPTION(Exception, s.find('a', ep));
    }

    {
        String s(STR("abcabc"));
        ASSERT(s.find('b') == s.charPosition(1));
        ASSERT(s.find('b', s.charPosition(2)) == s.charPosition(4));
        ASSERT(!s.find('b', s.charPosition(5)));
        ASSERT(!s.find('x'));

        ASSERT_EXCEPTION(Exception, s.find('a', s.chars() - 1));
        ASSERT_EXCEPTION(Exception, s.find('a', s.chars() + s.length() + 1));
    }

    // char_t* findNoCase(const String& str, char_t* pos = NULL)

    {
        String s;
        ASSERT(!s.findNoCase(String()));
        ASSERT(!s.findNoCase(String(STR("a"))));
        ASSERT_EXCEPTION(Exception, s.findNoCase(String(), ep));
    }

    {
        String s(STR("ABCDABCD"));
        ASSERT(s.findNoCase(String(STR("bc"))) == s.charPosition(1));
        ASSERT(s.findNoCase(String(STR("bc")), s.charPosition(2)) == s.charPosition(5));
        ASSERT(!s.findNoCase(String(STR("bc")), s.charPosition(6)));
        ASSERT(!s.findNoCase(String(STR("xy"))));

        ASSERT_EXCEPTION(Exception, s.findNoCase(String(), s.chars() - 1));
        ASSERT_EXCEPTION(Exception, s.findNoCase(String(), s.chars() + s.length() + 1));
        ASSERT(!s.findNoCase(String()));
    }

    // char_t* findNoCase(const char_t* chars, char_t* pos = NULL)

    {
        String s;
        ASSERT(!s.findNoCase(np));
        ASSERT(!s.findNoCase(STR("a")));
        ASSERT_EXCEPTION(Exception, s.findNoCase(np, ep));
    }

    {
        String s(STR("ABCDABCD"));
        ASSERT(s.findNoCase(STR("bc")) == s.charPosition(1));
        ASSERT(s.findNoCase(STR("bc"), s.charPosition(2)) == s.charPosition(5));
        ASSERT(!s.findNoCase(STR("bc"), s.charPosition(6)));
        ASSERT(!s.findNoCase(STR("xy")));

        ASSERT_EXCEPTION(Exception, s.findNoCase(np, s.chars() - 1));
        ASSERT_EXCEPTION(Exception, s.findNoCase(np, s.chars() + s.length() + 1));
        ASSERT(!s.findNoCase(np));
    }

    // char_t* findNoCase(unichar_t ch, char_t* pos = NULL)

    {
        String s;
        ASSERT_EXCEPTION(Exception, s.findNoCase(zc));
        ASSERT(!s.findNoCase('a'));
        ASSERT_EXCEPTION(Exception, s.findNoCase('a', ep));
    }

    {
        String s(STR("ABCABC"));
        ASSERT(s.findNoCase('b') == s.charPosition(1));
        ASSERT(s.findNoCase('b', s.charPosition(2)) == s.charPosition(4));
        ASSERT(!s.findNoCase('b', s.charPosition(5)));
        ASSERT(!s.findNoCase('x'));

        ASSERT_EXCEPTION(Exception, s.findNoCase('a', s.chars() - 1));
        ASSERT_EXCEPTION(Exception, s.findNoCase('a', s.chars() + s.length() + 1));
    }

    // char_t* find(const String& str, char_t* pos = NULL)

    {
        String s;
        const String& cs = s;
        ASSERT(!cs.find(String()));
        ASSERT(!cs.find(String(STR("a"))));
        ASSERT_EXCEPTION(Exception, cs.find(String(), ep));
    }

    {
        String s(STR("abcdabcd"));
        const String& cs = s;
        ASSERT(cs.find(String(STR("bc"))) == cs.charPosition(1));
        ASSERT(cs.find(String(STR("bc")), cs.charPosition(2)) == cs.charPosition(5));
        ASSERT(!cs.find(String(STR("bc")), cs.charPosition(6)));
        ASSERT(!cs.find(String(STR("xy"))));

        ASSERT_EXCEPTION(Exception, cs.find(String(), cs.chars() - 1));
        ASSERT_EXCEPTION(Exception, cs.find(String(), cs.chars() + cs.length() + 1));
        ASSERT(!cs.find(String()));
    }

    // char_t* find(const char_t* chars, char_t* pos = NULL)

    {
        String s;
        const String& cs = s;
        ASSERT(!cs.find(np));
        ASSERT(!cs.find(STR("a")));
        ASSERT_EXCEPTION(Exception, cs.find(np, ep));
    }

    {
        String s(STR("abcdabcd"));
        const String& cs = s;
        ASSERT(cs.find(STR("bc")) == cs.charPosition(1));
        ASSERT(cs.find(STR("bc"), cs.charPosition(2)) == cs.charPosition(5));
        ASSERT(!cs.find(STR("bc"), cs.charPosition(6)));
        ASSERT(!cs.find(STR("xy")));

        ASSERT_EXCEPTION(Exception, cs.find(np, cs.chars() - 1));
        ASSERT_EXCEPTION(Exception, cs.find(np, cs.chars() + cs.length() + 1));
        ASSERT(!cs.find(np));
    }

    // char_t* find(unichar_t ch, char_t* pos = NULL)

    {
        String s;
        const String& cs = s;
        ASSERT_EXCEPTION(Exception, cs.find(zc));
        ASSERT(!cs.find('a'));
        ASSERT_EXCEPTION(Exception, cs.find('a', ep));
    }

    {
        String s(STR("abcabc"));
        const String& cs = s;
        ASSERT(cs.find('b') == cs.charPosition(1));
        ASSERT(cs.find('b', cs.charPosition(2)) == cs.charPosition(4));
        ASSERT(!cs.find('b', cs.charPosition(5)));
        ASSERT(!cs.find('x'));

        ASSERT_EXCEPTION(Exception, cs.find('a', cs.chars() - 1));
        ASSERT_EXCEPTION(Exception, cs.find('a', cs.chars() + cs.length() + 1));
    }

    // char_t* findNoCase(const String& str, char_t* pos = NULL)

    {
        String s;
        const String& cs = s;
        ASSERT(!cs.findNoCase(String()));
        ASSERT(!cs.findNoCase(String(STR("a"))));
        ASSERT_EXCEPTION(Exception, cs.findNoCase(String(), ep));
    }

    {
        String s(STR("ABCDABCD"));
        const String& cs = s;
        ASSERT(cs.findNoCase(String(STR("bc"))) == cs.charPosition(1));
        ASSERT(cs.findNoCase(String(STR("bc")), cs.charPosition(2)) == cs.charPosition(5));
        ASSERT(!cs.findNoCase(String(STR("bc")), cs.charPosition(6)));
        ASSERT(!cs.findNoCase(String(STR("xy"))));

        ASSERT_EXCEPTION(Exception, cs.findNoCase(String(), cs.chars() - 1));
        ASSERT_EXCEPTION(Exception, cs.findNoCase(String(), cs.chars() + cs.length() + 1));
        ASSERT(!cs.findNoCase(String()));
    }

    // char_t* findNoCase(const char_t* chars, char_t* pos = NULL)

    {
        String s;
        const String& cs = s;
        ASSERT(!cs.findNoCase(np));
        ASSERT(!cs.findNoCase(STR("a")));
        ASSERT_EXCEPTION(Exception, cs.findNoCase(np, ep));
    }

    {
        String s(STR("ABCDABCD"));
        const String& cs = s;
        ASSERT(cs.findNoCase(STR("bc")) == cs.charPosition(1));
        ASSERT(cs.findNoCase(STR("bc"), cs.charPosition(2)) == cs.charPosition(5));
        ASSERT(!cs.findNoCase(STR("bc"), cs.charPosition(6)));
        ASSERT(!cs.findNoCase(STR("xy")));

        ASSERT_EXCEPTION(Exception, cs.findNoCase(np, cs.chars() - 1));
        ASSERT_EXCEPTION(Exception, cs.findNoCase(np, cs.chars() + cs.length() + 1));
        ASSERT(!cs.findNoCase(np));
    }

    // char_t* findNoCase(unichar_t ch, char_t* pos = NULL)

    {
        String s;
        const String& cs = s;
        ASSERT_EXCEPTION(Exception, cs.findNoCase(zc));
        ASSERT(!cs.findNoCase('a'));
        ASSERT_EXCEPTION(Exception, cs.findNoCase('a', ep));
    }

    {
        String s(STR("ABCABC"));
        const String& cs = s;
        ASSERT(cs.findNoCase('b') == cs.charPosition(1));
        ASSERT(cs.findNoCase('b', cs.charPosition(2)) == cs.charPosition(4));
        ASSERT(!cs.findNoCase('b', cs.charPosition(5)));
        ASSERT(!cs.findNoCase('x'));

        ASSERT_EXCEPTION(Exception, cs.findNoCase('a', cs.chars() - 1));
        ASSERT_EXCEPTION(Exception, cs.findNoCase('a', cs.chars() + cs.length() + 1));
    }

    // bool startsWith(const String& str) const

    ASSERT(String().startsWith(String()) == false);
    ASSERT(String().startsWith(String(STR("a"))) == false);
    ASSERT(String(STR("a")).startsWith(String()) == false);
    ASSERT(String(STR("a")).startsWith(String(STR("a"))) == true);
    ASSERT(String(STR("ab")).startsWith(String(STR("a"))) == true);
    ASSERT(String(STR("a")).startsWith(String(STR("ab"))) == false);

    // bool startsWith(const char_t* chars) const

    ASSERT_EXCEPTION(Exception, String().startsWith(NULL));
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

    ASSERT_EXCEPTION(Exception, String().endsWith(NULL));
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
        ASSERT(!s.chars());
        ASSERT(s.length() == 0);
        ASSERT(s.capacity() == 0);
    }

    {
        String s;
        s.ensureCapacity(10);
        s.shrinkToLength();
        ASSERT(!s.chars());
        ASSERT(s.length() == 0);
        ASSERT(s.capacity() == 0);
    }

    {
        String s;
        s.ensureCapacity(10);
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

    // void assign(const String& other)

    {
        String s(STR("a"));
        ASSERT_EXCEPTION(Exception, s.assign(s));
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
        String s1, s2(STR("a"));
        s1.ensureCapacity(10);
        s1.assign(s2);
        ASSERT(s1 == STR("a"));
        ASSERT(s1.capacity() == 10);
    }

    // void assign(const char_t* chars)

    ASSERT_EXCEPTION(Exception, String().assign(np));

    {
        String s(STR("a"));
        ASSERT_EXCEPTION(Exception, s.assign(s.chars()));
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
        String s;
        s.ensureCapacity(10);
        s.assign(STR("a"));
        ASSERT(s == STR("a"));
        ASSERT(s.capacity() == 10);
    }

    // void assign(unichar_t ch, int len = 1)

    ASSERT_EXCEPTION(Exception, String().assign('a', -1));
    ASSERT_EXCEPTION(Exception, String().assign(0, 1));

    {
        String s;
        s.assign('a', 1);
        ASSERT(s == STR("a"));
        ASSERT(s.capacity() == 2);
    }

    {
        String s(STR("a"));
        s.assign('b', 2);
        ASSERT(s == STR("bb"));
        ASSERT(s.capacity() == 3);
    }

    {
        String s(STR("a"));
        s.assign('b', 0);
        ASSERT(s == STR(""));
        ASSERT(s.capacity() == 2);
    }

    {
        String s;
        s.ensureCapacity(10);
        s.assign('a', 1);
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
        ASSERT(s1.capacity() == 4);
    }

    {
        String s1(STR("a")), s2(STR("b"));
        s1.append(s2);
        ASSERT(s1 == STR("ab"));
        ASSERT(s1.capacity() == 6);
    }

    {
        String s1, s2(STR("b"));
        s1.ensureCapacity(10);
        s1.append(s2);
        ASSERT(s1 == STR("b"));
        ASSERT(s1.capacity() == 10);
    }

    // void append(const char_t* chars)

    {
        String s(STR("a"));
        ASSERT_EXCEPTION(Exception, s.append(s.chars()));
    }

    {
        String s(STR("a"));
        s.append(np);
        ASSERT(s == STR("a"));
        ASSERT(s.capacity() == 2);
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
        ASSERT(s.capacity() == 4);
    }

    {
        String s(STR("a"));
        s.append(STR("b"));
        ASSERT(s == STR("ab"));
        ASSERT(s.capacity() == 6);
    }

    {
        String s;
        s.ensureCapacity(10);
        s.append(STR("b"));
        ASSERT(s == STR("b"));
        ASSERT(s.capacity() == 10);
    }

    // void append(char_t ch, int len)

    ASSERT_EXCEPTION(Exception, String().append('a', -1));
    ASSERT_EXCEPTION(Exception, String().append(0, 1));

    {
        String s;
        s.append('a', 1);
        ASSERT(s == STR("a"));
        ASSERT(s.capacity() == 4);
        s.append('b', 2);
        ASSERT(s == STR("abb"));
        ASSERT(s.capacity() == 4);
    }

    // void appendFormat(const char_t* format, ...)
    // void appendFormat(const char_t* format, va_list args)

    {
        String s(STR("result: "));
        s.appendFormat(STR("val = %d"), 123);
        ASSERT(s == STR("result: val = 123"));
    }

    // void insert(char_t* pos, const String& str)

    {
        String s(STR("a"));
        ASSERT_EXCEPTION(Exception, s.insert(s.chars() - 1, String()));
        ASSERT_EXCEPTION(Exception, s.insert(s.chars() + s.length() + 1, String()));
    }

    {
        String s(STR("a"));
        ASSERT_EXCEPTION(Exception, s.insert(s.chars(), s));
    }

    {
        String s1(STR("a")), s2;
        s1.insert(s1.chars(), s2);
        ASSERT(s1 == STR("a"));
        ASSERT(s1.capacity() == 2);
    }

    {
        String s1, s2(STR("b"));
        s1.insert(s1.chars(), s2);
        ASSERT(s1 == STR("b"));
        ASSERT(s1.capacity() == 4);
    }

    {
        String s1(STR("a")), s2(STR("b"));
        s1.insert(s1.chars(), s2);
        ASSERT(s1 == STR("ba"));
        ASSERT(s1.capacity() == 6);
    }

    {
        String s1(STR("a")), s2(STR("b"));
        s1.insert(s1.charPosition(1), s2);
        ASSERT(s1 == STR("ab"));
        ASSERT(s1.capacity() == 6);
    }

    {
        String s1(STR("ab")), s2(STR("c"));
        s1.insert(s1.charPosition(1), s2);
        ASSERT(s1 == STR("acb"));
        ASSERT(s1.capacity() == 8);
    }

    {
        String s1, s2(STR("c"));
        s1.ensureCapacity(10);
        s1 = STR("ab");
        s1.insert(s1.charPosition(1), s2);
        ASSERT(s1 == STR("acb"));
        ASSERT(s1.capacity() == 10);
    }

    // void insert(char_t* pos, const char_t* chars)

    {
        String s(STR("a"));
        ASSERT_EXCEPTION(Exception, s.insert(s.chars() - 1, STR("")));
        ASSERT_EXCEPTION(Exception, s.insert(s.chars() + s.length() + 1, STR("")));
    }

    {
        String s(STR("a"));
        ASSERT_EXCEPTION(Exception, s.insert(s.chars(), s.chars()));
    }

    {
        String s(STR("a"));
        s.insert(s.chars(), STR(""));
        ASSERT(s == STR("a"));
        ASSERT(s.capacity() == 2);
    }

    {
        String s;
        s.insert(s.chars(), STR("b"));
        ASSERT(s == STR("b"));
        ASSERT(s.capacity() == 4);
    }

    {
        String s(STR("a"));
        s.insert(s.chars(), STR("b"));
        ASSERT(s == STR("ba"));
        ASSERT(s.capacity() == 6);
    }

    {
        String s(STR("a"));
        s.insert(s.charPosition(1), STR("b"));
        ASSERT(s == STR("ab"));
        ASSERT(s.capacity() == 6);
    }

    {
        String s(STR("ab"));
        s.insert(s.charPosition(1), STR("c"));
        ASSERT(s == STR("acb"));
        ASSERT(s.capacity() == 8);
    }

    {
        String s;
        s.ensureCapacity(10);
        s = STR("ab");
        s.insert(s.charPosition(1), STR("c"));
        ASSERT(s == STR("acb"));
        ASSERT(s.capacity() == 10);
    }

    // void insert(char_t* pos, unichar_t ch, int len = 1)

    {
        String s(STR("a"));
        ASSERT_EXCEPTION(Exception, s.insert(s.chars() - 1, 'a'));
        ASSERT_EXCEPTION(Exception, s.insert(s.chars() + s.length() + 1, 'a'));
        ASSERT_EXCEPTION(Exception, s.insert(s.chars(), 0, 1));
        ASSERT_EXCEPTION(Exception, s.insert(s.chars(), 'a', -1));
    }

    {
        String s(STR("a"));
        s.insert(s.chars(), 'b', 0);
        ASSERT(s == STR("a"));
        ASSERT(s.capacity() == 2);
    }

    {
        String s;
        s.insert(s.chars(), 'a');
        ASSERT(s == STR("a"));
        ASSERT(s.capacity() == 4);
    }

    {
        String s(STR("a"));
        s.insert(s.chars(), 'b');
        ASSERT(s == STR("ba"));
        ASSERT(s.capacity() == 6);
    }

    {
        String s(STR("a"));
        s.insert(s.charPosition(1), 'b');
        ASSERT(s == STR("ab"));
        ASSERT(s.capacity() == 6);
    }

    {
        String s(STR("ab"));
        s.insert(s.charPosition(1), 'c');
        ASSERT(s == STR("acb"));
        ASSERT(s.capacity() == 8);
    }

    {
        String s(STR("ab"));
        s.insert(s.charPosition(1), 'c', 2);
        ASSERT(s == STR("accb"));
        ASSERT(s.capacity() == 10);
    }

    {
        String s;
        s.ensureCapacity(10);
        s = STR("ab");
        s.insert(s.charPosition(1), 'c');
        ASSERT(s == STR("acb"));
        ASSERT(s.capacity() == 10);
    }

    // erase(char_t* pos, int len = -1)

    {
        String s(STR("a"));
        ASSERT_EXCEPTION(Exception, s.erase(s.chars() - 1));
        ASSERT_EXCEPTION(Exception, s.erase(s.chars() + s.length() + 1));
    }

    {
        String s(STR("a"));
        s.erase(s.chars(), 0);
        ASSERT(s == STR("a"));
        ASSERT(s.capacity() == 2);
    }

    {
        String s(STR("a"));
        s.erase(s.chars(), 1);
        ASSERT(s.length() == 0);
        ASSERT(s.capacity() == 2);
    }

    {
        String s(STR("abc"));
        s.erase(s.chars(), 1);
        ASSERT(s == STR("bc"));
        ASSERT(s.capacity() == 4);
    }

    {
        String s(STR("abc"));
        s.erase(s.charPosition(2), 1);
        ASSERT(s == STR("ab"));
        ASSERT(s.capacity() == 4);
    }

    {
        String s(STR("abc"));
        s.erase(s.charPosition(1), 1);
        ASSERT(s == STR("ac"));
        ASSERT(s.capacity() == 4);
    }

    {
        String s(STR("abc"));
        s.erase(s.charPosition(1));
        ASSERT(s == STR("a"));
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
        ASSERT_EXCEPTION(Exception, s.erase(s));
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

    {
        String s(STR("abc"));
        s.erase(np);
        ASSERT(s == STR("abc"));
        ASSERT(s.capacity() == 4);
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
        ASSERT_EXCEPTION(Exception, s.erase(s.chars()));
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

    // void replace(char_t* pos, const String& str, int len = -1)

    {
        String s(STR("a"));
        ASSERT_EXCEPTION(Exception, s.replace(s.chars() - 1, String()));
        ASSERT_EXCEPTION(Exception, s.replace(s.chars() + s.length() + 1, String()));
    }

    {
        String s(STR("abc"));
        ASSERT_EXCEPTION(Exception, s.replace(s.chars(), s));
    }

    {
        String s;
        s.replace(s.chars(), String(STR("a")));
        ASSERT(s == STR("a"));
        ASSERT(s.length() == 1);
        ASSERT(s.capacity() == 2);
    }

    {
        String s(STR("a"));
        s.replace(s.chars(), String());
        ASSERT(s == STR(""));
        ASSERT(s.length() == 0);
        ASSERT(s.capacity() == 2);
    }

    {
        String s(STR("a"));
        s.replace(s.chars(), String(STR("b")));
        ASSERT(s == STR("b"));
        ASSERT(s.length() == 1);
        ASSERT(s.capacity() == 2);
    }

    {
        String s(STR("ab"));
        s.replace(s.chars(), String(), 1);
        ASSERT(s == STR("b"));
        ASSERT(s.length() == 1);
        ASSERT(s.capacity() == 3);
    }

    {
        String s(STR("ab"));
        s.replace(s.charPosition(1), String(), 1);
        ASSERT(s == STR("a"));
        ASSERT(s.length() == 1);
        ASSERT(s.capacity() == 3);
    }

    {
        String s(STR("abcd"));
        s.replace(s.charPosition(1), String(), 2);
        ASSERT(s == STR("ad"));
        ASSERT(s.length() == 2);
        ASSERT(s.capacity() == 5);
    }

    {
        String s(STR("abcd"));
        s.replace(s.charPosition(1), String(STR("x")), 2);
        ASSERT(s == STR("axd"));
        ASSERT(s.length() == 3);
        ASSERT(s.capacity() == 5);
    }

    {
        String s(STR("abcd"));
        s.replace(s.charPosition(1), String(STR("xyz")), 2);
        ASSERT(s == STR("axyzd"));
        ASSERT(s.length() == 5);
        ASSERT(s.capacity() == 6);
    }

    {
        String s(STR("abcd"));
        s.replace(s.charPosition(2), String(STR("x")));
        ASSERT(s == STR("abx"));
        ASSERT(s.length() == 3);
        ASSERT(s.capacity() == 5);
    }

    // void replace(char_t* pos, const char_t* chars, int len = -1) 

    {
        String s(STR("a"));
        ASSERT_EXCEPTION(Exception, s.replace(s.chars() - 1, STR("")));
        ASSERT_EXCEPTION(Exception, s.replace(s.chars() + s.length() + 1, STR("")));
    }

    {
        String s(STR("abc"));
        ASSERT_EXCEPTION(Exception, s.replace(s.chars(), s.chars()));
    }

    {
        String s;
        s.replace(s.chars(), STR("a"));
        ASSERT(s == STR("a"));
        ASSERT(s.length() == 1);
        ASSERT(s.capacity() == 2);
    }

    {
        String s(STR("a"));
        s.replace(s.chars(), STR(""));
        ASSERT(s == STR(""));
        ASSERT(s.length() == 0);
        ASSERT(s.capacity() == 2);
    }

    {
        String s(STR("a"));
        s.replace(s.chars(), STR("b"));
        ASSERT(s == STR("b"));
        ASSERT(s.length() == 1);
        ASSERT(s.capacity() == 2);
    }

    {
        String s(STR("ab"));
        s.replace(s.chars(), STR(""), 1);
        ASSERT(s == STR("b"));
        ASSERT(s.length() == 1);
        ASSERT(s.capacity() == 3);
    }

    {
        String s(STR("ab"));
        s.replace(s.charPosition(1), STR(""), 1);
        ASSERT(s == STR("a"));
        ASSERT(s.length() == 1);
        ASSERT(s.capacity() == 3);
    }

    {
        String s(STR("abcd"));
        s.replace(s.charPosition(1), STR(""), 2);
        ASSERT(s == STR("ad"));
        ASSERT(s.length() == 2);
        ASSERT(s.capacity() == 5);
    }

    {
        String s(STR("abcd"));
        s.replace(s.charPosition(1), STR("x"), 2);
        ASSERT(s == STR("axd"));
        ASSERT(s.length() == 3);
        ASSERT(s.capacity() == 5);
    }

    {
        String s(STR("abcd"));
        s.replace(s.charPosition(1), STR("xyz"), 2);
        ASSERT(s == STR("axyzd"));
        ASSERT(s.length() == 5);
        ASSERT(s.capacity() == 6);
    }

    {
        String s(STR("abcd"));
        s.replace(s.charPosition(2), STR("x"));
        ASSERT(s == STR("abx"));
        ASSERT(s.length() == 3);
        ASSERT(s.capacity() == 5);
    }

    // void replace(const String& searchStr, const String& replaceStr)

    {
        String s;
        s.replace(String(STR("a")), String(STR("b")));
        ASSERT(s == STR(""));
    }

    {
        String s(STR("abc"));
        s.replace(String(), String(STR("a")));
        ASSERT(s == STR("abc"));
    }
     
    {
        String s(STR("abc"));
        s.replace(String(STR("b")), String());
        ASSERT(s == STR("ac"));
    }  

    {
        String s(STR("a"));
        ASSERT_EXCEPTION(Exception, s.replace(s, String((STR("a")))));
        ASSERT_EXCEPTION(Exception, s.replace(String((STR("a"))), s));
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
        s.replace(STR("a"), STR("b"));
        ASSERT(s == STR(""));
    }

    {
        String s(STR("abc"));
        s.replace(STR(""), STR("a"));
        ASSERT(s == STR("abc"));
        s.replace(np,  STR("a"));
        ASSERT(s == STR("abc"));
    }
     
    {
        String s(STR("abc"));
        s.replace(STR("b"), STR(""));
        ASSERT(s == STR("ac"));
    }

    {
        String s(STR("abc"));
        s.replace(STR("b"), np);
        ASSERT(s == STR("ac"));
    }  

    {
        String s(STR("a"));
        ASSERT_EXCEPTION(Exception, s.replace(s.chars(), STR("a")));
        ASSERT_EXCEPTION(Exception, s.replace(STR("a"), s.chars()));
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

    // void clear()

    {
        String s;
        s.clear();
        ASSERT(!s.chars());
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
        ASSERT(!s.chars());
        ASSERT(s.length() == 0);
        ASSERT(s.capacity() == 0);
    }

    {
        String s(STR("a"));
        s.reset();
        ASSERT(!s.chars());
        ASSERT(s.length() == 0);
        ASSERT(s.capacity() == 0);
    }

    // String acquire(char_t* chars)

    {
        char_t* p = Memory::allocate<char_t>(1);
        strCopy(p, STR(""));
        String s = String::acquire(p);
        ASSERT(s.chars() == p);
        ASSERT(s == STR(""));
        ASSERT(s.length() == 0);
        ASSERT(s.capacity() == 1);
    }

    {
        char_t* p = Memory::allocate<char_t>(2);
        strCopy(p, STR("a"));
        String s = String::acquire(p);
        ASSERT(s.chars() == p);
        ASSERT(s == STR("a"));
        ASSERT(s.length() == 1);
        ASSERT(s.capacity() == 2);
    }

    // char_t* release()

    {
        String s;
        char_t* p = s.release();
        ASSERT(!p);
        ASSERT(!s.chars());
        ASSERT(s.length() == 0);
        ASSERT(s.capacity() == 0);
    }
    {
        String s(STR("a"));
        char_t* p = s.release();
        ASSERT(strCompare(p, STR("a")) == 0);
        ASSERT(!s.chars());
        ASSERT(s.length() == 0);
        ASSERT(s.capacity() == 0);
        Memory::deallocate(p);
    }

    // String concat(_Args&&... args)

    ASSERT(String::concat(STR("")) == STR(""));
    ASSERT(String::concat(STR("a")) == STR("a"));
    ASSERT(String::concat(String(STR("a"))) == STR("a"));
    ASSERT(String::concat(STR("a"), STR("b")) == STR("ab"));
    ASSERT(String::concat(STR("a"), STR("")) == STR("a"));
    ASSERT(String::concat(STR("a"), STR(""), STR("b")) == STR("ab"));
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
    
    ASSERT_EXCEPTION(Exception, String(STR("abc")).toBool());
    ASSERT_EXCEPTION(Exception, String(STR("abc")).toInt());
    ASSERT_EXCEPTION(Exception, String(STR("abc")).toInt32());
    ASSERT_EXCEPTION(Exception, String(STR("abc")).toUInt32());
    ASSERT_EXCEPTION(Exception, String(STR("abc")).toInt64());
    ASSERT_EXCEPTION(Exception, String(STR("abc")).toUInt64());
    ASSERT_EXCEPTION(Exception, String(STR("abc")).toFloat());
    ASSERT_EXCEPTION(Exception, String(STR("abc")).toDouble());

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

    // String operator+(const String& left, const String& right)
    // String operator+(const String& left, const char_t* right)
    // String operator+(const char_t* left, const String& right)

    ASSERT(String(STR("ab")) + String(STR("cd")) == STR("abcd"));
    ASSERT(String(STR("ab")) + STR("cd") == STR("abcd"));
    ASSERT(STR("ab") + String(STR("cd")) == STR("abcd"));

    // string compare operators

    ASSERT(String(STR("ab")) == String(STR("ab")));
    ASSERT(String(STR("ab")) != String(STR("xy")));
    ASSERT(String(STR("ab")) < String(STR("abc")));
    ASSERT(String(STR("ab")) <= String(STR("abc")));
    ASSERT(String(STR("abc")) > String(STR("ab")));
    ASSERT(String(STR("abc")) >= String(STR("ab")));

    ASSERT(STR("ab") == String(STR("ab")));
    ASSERT(STR("ab") != String(STR("xy")));
    ASSERT(STR("ab") < String(STR("abc")));
    ASSERT(STR("ab") <= String(STR("abc")));
    ASSERT(STR("abc") > String(STR("ab")));
    ASSERT(STR("abc") >= String(STR("ab")));

    ASSERT(String(STR("ab")) == STR("ab"));
    ASSERT(String(STR("ab")) != STR("xy"));
    ASSERT(String(STR("ab")) < STR("abc"));
    ASSERT(String(STR("ab")) <= STR("abc"));
    ASSERT(String(STR("abc")) > STR("ab"));
    ASSERT(String(STR("abc")) >= STR("ab"));
}

void testStringIterator()
{
    {
        String s;
        auto iter = s.constIterator();

        ASSERT_EXCEPTION(Exception, iter.value());
        ASSERT(!iter.moveNext());
        ASSERT(!iter.movePrev());
    }

    {
        String s(CHARS);
        auto iter = s.constIterator();

        ASSERT_EXCEPTION(Exception, iter.value());
        ASSERT(iter.moveNext());
        ASSERT(iter.value() == 0x24);
        ASSERT(iter.moveNext());
        ASSERT(iter.value() == 0xa2);
        ASSERT(iter.moveNext());
        ASSERT(iter.value() == 0x20ac);
        ASSERT(iter.moveNext());
        ASSERT(iter.value() == 0x10348);
        ASSERT(!iter.moveNext());
        ASSERT_EXCEPTION(Exception, iter.value());
    }

    {
        String s(CHARS);
        auto iter = s.constIterator();

        ASSERT_EXCEPTION(Exception, iter.value());
        ASSERT(iter.movePrev());
        ASSERT(iter.value() == 0x10348);
        ASSERT(iter.movePrev());
        ASSERT(iter.value() == 0x20ac);
        ASSERT(iter.movePrev());
        ASSERT(iter.value() == 0xa2);
        ASSERT(iter.movePrev());
        ASSERT(iter.value() == 0x24);
        ASSERT(!iter.movePrev());
        ASSERT_EXCEPTION(Exception, iter.value());
    }

    {
        String s(CHARS);
        auto iter = s.constIterator();

        ASSERT_EXCEPTION(Exception, iter.value());
        ASSERT(iter.moveNext());
        ASSERT(iter.value() == 0x24);
        ASSERT(iter.moveNext());
        ASSERT(iter.value() == 0xa2);
        ASSERT(iter.movePrev());
        ASSERT(iter.value() == 0x24);
        ASSERT(!iter.movePrev());
        ASSERT_EXCEPTION(Exception, iter.value());
    }

    {
        String s(CHARS);
        auto iter = s.constIterator();

        ASSERT_EXCEPTION(Exception, iter.value());
        ASSERT(iter.moveNext());
        ASSERT(iter.value() == 0x24);
        ASSERT(iter.moveNext());
        ASSERT(iter.value() == 0xa2);
        iter.reset();
        ASSERT_EXCEPTION(Exception, iter.value());
        ASSERT(iter.moveNext());
        ASSERT(iter.value() == 0x24);
    }
}

void testArray()
{
    int elem[] = { 1, 2, 3 };
    const int* np = NULL;
    const int* ep = elem;

    // Array()

    {
        Array<int> a;
        ASSERT(a.size() == 0);
        ASSERT(a.capacity() == 0);
        ASSERT(!a.values());
    }

    // Array(int size)

    ASSERT_EXCEPTION(Exception, Array<int> a(-1));

    {
        Array<int> a(0);
        ASSERT(a.size() == 0);
        ASSERT(a.capacity() == 0);
        ASSERT(!a.values());
    }

    {
        Array<int> a(1);
        ASSERT(a.size() == 1);
        ASSERT(a.capacity() == 1);
        ASSERT(a.values());
        ASSERT(a[0] == 0);
    }

    // Array(int size, const _Type& value)

    ASSERT_EXCEPTION(Exception, Array<int> a(-1, 123));

    {
        Array<int> a(0, 123);
        ASSERT(a.size() == 0);
        ASSERT(a.capacity() == 0);
        ASSERT(!a.values());
    }

    {
        Array<int> a(1, 123);
        ASSERT(a.size() == 1);
        ASSERT(a.capacity() == 1);
        ASSERT(a.values());
        ASSERT(a[0] == 123);
    }

    // Array(int size, const _Type* values)

    ASSERT_EXCEPTION(Exception, Array<int>(-1, np));
    ASSERT_EXCEPTION(Exception, Array<int>(0, ep));
    ASSERT_EXCEPTION(Exception, Array<int>(1, np));

    {
        Array<int> a(0, np);
        ASSERT(a.size() == 0);
        ASSERT(a.capacity() == 0);
        ASSERT(!a.values());
    }

    {
        Array<int> a(3, ep);
        ASSERT(a.size() == 3);
        ASSERT(a.capacity() == 3);
        ASSERT(a.values() != ep);
        ASSERT(compareSequence(a, ep));
    }

    // Array(const Array<_Type>& other)

    {
        Array<int> a1, a2(a1);
        ASSERT(a2.size() == 0);
        ASSERT(a2.capacity() == 0);
        ASSERT(!a2.values());
    }

    {
        Array<int> a1(3, ep), a2(a1);
        ASSERT(a2.size() == 3);
        ASSERT(a2.capacity() == 3);
        ASSERT(a2.values());
        ASSERT(compareSequence(a2, ep));
        ASSERT(a1.values() != a2.values());
    }

    // Array(Array<_Type>&& other)

    {
        Array<int> a1(3, ep), a2(static_cast<Array<int>&&>(a1));

        ASSERT(a1.size() == 0);
        ASSERT(a1.capacity() == 0);
        ASSERT(!a1.values());

        ASSERT(a2.size() == 3);
        ASSERT(a2.capacity() == 3);
        ASSERT(a2.values());
        ASSERT(compareSequence(a2, ep));
    }

    // Array<_Type>& operator=(const Array<_Type>& other)

    {
        Array<int> a1(3, ep), a2;
        a2 = a1;
        ASSERT(a2.size() == 3);
        ASSERT(a2.capacity() == 3);
        ASSERT(a2.values());
        ASSERT(compareSequence(a2, ep));
        ASSERT(a1.values() != a2.values());
    }

    // Array<_Type>& operator=(Array<_Type>&& other)

    {
        Array<int> a1(3, ep), a2;
        a2 = static_cast<Array<int>&&>(a1);

        ASSERT(a1.size() == 0);
        ASSERT(a1.capacity() == 0);
        ASSERT(!a1.values());

        ASSERT(a2.size() == 3);
        ASSERT(a2.capacity() == 3);
        ASSERT(a2.values());
        ASSERT(compareSequence(a2, ep));
    }

    // _Type& operator[](int index)

    {
        Array<int> a(3, ep);
        ASSERT(a[0] == 1);
        ASSERT(a[1] == 2);
        ASSERT(a[2] == 3);
    }

    // const _Type& operator[](int index) const

    {
        Array<int> a(3, ep);
        const Array<int>& ca = a;
        ASSERT(ca[0] == 1);
        ASSERT(ca[1] == 2);
        ASSERT(ca[2] == 3);
    }

    // _Type& value(int index)

    {
        Array<int> a(3, ep);
        ASSERT_EXCEPTION(Exception, a.value(-1));
        ASSERT_EXCEPTION(Exception, a.value(3));
        ASSERT(a.value(0) == 1);
        ASSERT(a.value(1) == 2);
        ASSERT(a.value(2) == 3);
    }

    // const _Type& value(int index) const

    {
        Array<int> a(3, ep);
        const Array<int>& ca = a;
        ASSERT_EXCEPTION(Exception, ca.value(-1));
        ASSERT_EXCEPTION(Exception, ca.value(3));
        ASSERT(ca.value(0) == 1);
        ASSERT(ca.value(1) == 2);
        ASSERT(ca.value(2) == 3);
    }

    // int size() const
    // int capacity() const
    // bool empty() const
    // _Type* values()
    // const _Type* values() const

    {
        Array<int> a;
        ASSERT(a.size() == 0);
        ASSERT(a.capacity() == 0);
        ASSERT(a.empty());
        ASSERT(!a.values());
    }

    {
        Array<int> a(3, ep);

        ASSERT(a.size() == 3);
        ASSERT(a.capacity() == 3);
        ASSERT(!a.empty());
        ASSERT(a.values());
        ASSERT(*a.values() == 1);
        ASSERT(*(a.values() + 1) == 2);
        ASSERT(*(a.values() + 2) == 3);

        const Array<int>& ca = a;
        ASSERT(ca.values());
        ASSERT(*ca.values() == 1);
        ASSERT(*(ca.values() + 1) == 2);
        ASSERT(*(ca.values() + 2) == 3);
    }

    // _Type& front()
    // const _Type& front() const
    // _Type& back()
    // const _Type& back() const

    {
        Array<int> a;
        ASSERT_EXCEPTION(Exception, a.front());
        ASSERT_EXCEPTION(Exception, a.back());

        const Array<int>& ca = a;
        ASSERT_EXCEPTION(Exception, ca.front());
        ASSERT_EXCEPTION(Exception, ca.back());
    }

    {
        Array<int> a(3, ep);
        ASSERT(a.front() == 1);
        ASSERT(a.back() == 3);

        const Array<int>& ca = a;
        ASSERT(ca.front() == 1);
        ASSERT(ca.back() == 3);
    }

    // int find(const _Type& value) const

    {
        Array<int> a;
        ASSERT(a.find(0) < 0);
    }

    {
        Array<int> a(3, ep);
        ASSERT(a.find(2) == 1);
        ASSERT(a.find(0) < 0);
    }

    // void ensureCapacity(int capacity)

    ASSERT_EXCEPTION(Exception, Array<int>().ensureCapacity(-1));

    {
        Array<int> a(3, ep);
        ASSERT(a.capacity() == 3);
        a.ensureCapacity(2);
        ASSERT(a.capacity() == 3);
        ASSERT(compareSequence(a, ep));
    }

    {
        Array<int> a(3, ep);
        ASSERT(a.capacity() == 3);
        a.ensureCapacity(5);
        ASSERT(a.capacity() == 5);
        ASSERT(compareSequence(a, ep));
    }

    {
        Array<int> a(3, ep);
        ASSERT(a.capacity() == 3);
        a.ensureCapacity(0);
        ASSERT(a.capacity() == 3);
        ASSERT(compareSequence(a, ep));
    }

    // void shrinkToLength()

    {
        Array<int> a(3, ep);
        ASSERT(a.capacity() == 3);
        a.shrinkToLength();
        ASSERT(a.capacity() == 3);
        ASSERT(compareSequence(a, ep));
    }

    {
        Array<int> a(3, ep);
        a.ensureCapacity(10);
        ASSERT(a.capacity() == 10);
        a.shrinkToLength();
        ASSERT(a.capacity() == 3);
        ASSERT(compareSequence(a, ep));
    }

    // void resize(int size)

    ASSERT_EXCEPTION(Exception, Array<int>().resize(-1));

    {
        Array<int> a;
        a.resize(1);
        ASSERT(a[0] == 0);
        ASSERT(a.size() == 1);
        ASSERT(a.capacity() == 1);
    }

    {
        Array<int> a(3, ep);
        a.resize(4);
        ASSERT(a[0] == 1);
        ASSERT(a[1] == 2);
        ASSERT(a[2] == 3);
        ASSERT(a[3] == 0);
        ASSERT(a.size() == 4);
        ASSERT(a.capacity() == 4);
    }

    {
        Array<int> a(3, ep);
        a.resize(2);
        ASSERT(a[0] == 1);
        ASSERT(a[1] == 2);
        ASSERT(a.size() == 2);
        ASSERT(a.capacity() == 3);
    }

    {
        Array<int> a(3, ep);
        a.resize(0);
        ASSERT(a.size() == 0);
        ASSERT(a.capacity() == 3);
    }

    // void resize(int size, const _Type& value)

    ASSERT_EXCEPTION(Exception, Array<int>().resize(-1, 123));

    {
        Array<int> a;
        a.resize(1, 123);
        ASSERT(a[0] == 123);
        ASSERT(a.size() == 1);
        ASSERT(a.capacity() == 1);
    }

    {
        Array<int> a(3, ep);
        a.resize(4, 123);
        ASSERT(a[0] == 1);
        ASSERT(a[1] == 2);
        ASSERT(a[2] == 3);
        ASSERT(a[3] == 123);
        ASSERT(a.size() == 4);
        ASSERT(a.capacity() == 4);
    }

    {
        Array<int> a(3, ep);
        a.resize(2, 123);
        ASSERT(a[0] == 1);
        ASSERT(a[1] == 2);
        ASSERT(a.size() == 2);
        ASSERT(a.capacity() == 3);
    }

    {
        Array<int> a(3, ep);
        a.resize(0, 123);
        ASSERT(a.size() == 0);
        ASSERT(a.capacity() == 3);
    }

    // void assign(int size, const _Type& value)

    ASSERT_EXCEPTION(Exception, Array<int>().assign(-1, 123));

    {
        Array<int> a;
        a.assign(0, 123);
        ASSERT(a.size() == 0);
        ASSERT(a.capacity() == 0);
    }

    {
        Array<int> a;
        a.assign(1, 123);
        ASSERT(a[0] == 123);
        ASSERT(a.size() == 1);
        ASSERT(a.capacity() == 1);
    }

    {
        Array<int> a(3, ep);
        a.assign(0, 123);
        ASSERT(a.size() == 0);
        ASSERT(a.capacity() == 3);
    }

    {
        Array<int> a(3, ep);
        a.assign(1, 123);
        ASSERT(a[0] == 123);
        ASSERT(a.size() == 1);
        ASSERT(a.capacity() == 3);
    }

    // void assign(int size, const _Type* values)

    ASSERT_EXCEPTION(Exception, Array<int>().assign(-1, ep));
    ASSERT_EXCEPTION(Exception, Array<int>().assign(0, ep));
    ASSERT_EXCEPTION(Exception, Array<int>().assign(1, np));

    {
        Array<int> a(3, ep);
        ASSERT_EXCEPTION(Exception, a.assign(a.size(), a.values()));
    }

    {
        Array<int> a;
        a.assign(0, np);
        ASSERT(a.size() == 0);
        ASSERT(a.capacity() == 0);
    }

    {
        Array<int> a;
        a.assign(3, ep);
        ASSERT(a.size() == 3);
        ASSERT(a.capacity() == 3);
        ASSERT(compareSequence(a, ep));
    }

    {
        Array<int> a(3, ep);
        a.assign(0, np);
        ASSERT(a.size() == 0);
        ASSERT(a.capacity() == 3);
    }

    {
        Array<int> a(3, ep);
        a.assign(1, ep);
        ASSERT(a[0] == 1);
        ASSERT(a.size() == 1);
        ASSERT(a.capacity() == 3);
    }

    // void assign(const Array<_Type>& other)

    {
        Array<int> a(3, ep);
        ASSERT_EXCEPTION(Exception, a.assign(a));
    }

    {
        Array<int> a1, a2(3, ep);
        a2.assign(a1);
        ASSERT(a2.size() == 0);
        ASSERT(a2.capacity() == 3);
        ASSERT(a2.values());
    }

    {
        Array<int> a1(3, ep), a2;
        a2.assign(a1);
        ASSERT(a2.size() == 3);
        ASSERT(a2.capacity() == 3);
        ASSERT(compareSequence(a2, ep));
    }

    {
        Array<int> a1(1), a2(3, ep);
        a2.assign(a1);
        ASSERT(a2.size() == 1);
        ASSERT(a2.capacity() == 3);
        ASSERT(a2.values());
        ASSERT(a2[0] == 0);
    }

    // void popBack()

    ASSERT_EXCEPTION(Exception, Array<int>().popBack());

    {
        Array<int> a(3, ep);
        a.popBack();
        ASSERT(a.size() == 2);
    }

    // void pushBack(const _Type& value)

    {
        Array<int> a;
        int v1 = 1, v2 = 2, v3 = 3;

        a.pushBack(v1);
        ASSERT(a.back() == v1);
        ASSERT(a.size() == 1);
        ASSERT(a.capacity() == 1);

        a.pushBack(v2);
        ASSERT(a.back() == v2);
        ASSERT(a.size() == 2);
        ASSERT(a.capacity() == 3);

        a.pushBack(v3);
        ASSERT(a.back() == v3);
        ASSERT(a.size() == 3);
        ASSERT(a.capacity() == 3);
    }

    // void pushBack(_Type&& value)

    {
        Array<int> a;

        a.pushBack(1);
        ASSERT(a.back() == 1);
        ASSERT(a.size() == 1);
        ASSERT(a.capacity() == 1);

        a.pushBack(2);
        ASSERT(a.back() == 2);
        ASSERT(a.size() == 2);
        ASSERT(a.capacity() == 3);

        a.pushBack(3);
        ASSERT(a.back() == 3);
        ASSERT(a.size() == 3);
        ASSERT(a.capacity() == 3);
    }

    // void insert(int index, const _Type& value)

    {
        Array<int> a(1);
        int v = 1;
        ASSERT_EXCEPTION(Exception, a.insert(-1, v));
        ASSERT_EXCEPTION(Exception, a.insert(2, v));
    }

    {
        Array<int> a;
        int v1 = 1, v2 = 2, v3 = 3, v4 = 4;

        a.insert(0, v1);
        ASSERT(a.back() == v1);
        ASSERT(a.size() == 1);
        ASSERT(a.capacity() == 1);

        a.insert(1, v2);
        ASSERT(a.back() == v2);
        ASSERT(a.size() == 2);
        ASSERT(a.capacity() == 3);

        a.insert(0, v3);
        ASSERT(a.front() == v3);
        ASSERT(a.size() == 3);
        ASSERT(a.capacity() == 3);

        a.insert(1, v4);
        ASSERT(a[1] == v4);
        ASSERT(a.size() == 4);
        ASSERT(a.capacity() == 7);
    }

    // void insert(int index, _Type&& value)

    {
        Array<int> a(1);
        ASSERT_EXCEPTION(Exception, a.insert(-1, 1));
        ASSERT_EXCEPTION(Exception, a.insert(2, 1));
    }

    {
        Array<int> a;

        a.insert(0, 1);
        ASSERT(a.back() == 1);
        ASSERT(a.size() == 1);
        ASSERT(a.capacity() == 1);

        a.insert(1, 2);
        ASSERT(a.back() == 2);
        ASSERT(a.size() == 2);
        ASSERT(a.capacity() == 3);

        a.insert(0, 3);
        ASSERT(a.front() == 3);
        ASSERT(a.size() == 3);
        ASSERT(a.capacity() == 3);

        a.insert(1, 4);
        ASSERT(a[1] == 4);
        ASSERT(a.size() == 4);
        ASSERT(a.capacity() == 7);
    }

    // void remove(int index)

    {
        Array<int> a(1);
        ASSERT_EXCEPTION(Exception, a.remove(-1));
        ASSERT_EXCEPTION(Exception, a.remove(1));
    }

    {
        Array<int> a(3, ep);
        a.remove(0);
        ASSERT(a[0] == 2);
        ASSERT(a[1] == 3);
        ASSERT(a.size() == 2);
        ASSERT(a.capacity() == 3);
    }

    {
        Array<int> a(3, ep);
        a.remove(2);
        ASSERT(a[0] == 1);
        ASSERT(a[1] == 2);
        ASSERT(a.size() == 2);
        ASSERT(a.capacity() == 3);
    }

    {
        Array<int> a(3, ep);
        a.remove(1);
        ASSERT(a[0] == 1);
        ASSERT(a[1] == 3);
        ASSERT(a.size() == 2);
        ASSERT(a.capacity() == 3);
    }

    // void clear()

    {
        Array<int> a;
        a.clear();
        ASSERT(a.size() == 0);
        ASSERT(a.capacity() == 0);
        ASSERT(!a.values());
    }

    {
        Array<int> a(3, ep);
        a.clear();
        ASSERT(a.size() == 0);
        ASSERT(a.capacity() == 3);
        ASSERT(a.values());
    }
    
    // void reset()

    {
        Array<int> a;
        a.reset();
        ASSERT(a.size() == 0);
        ASSERT(a.capacity() == 0);
        ASSERT(!a.values());
    }

    {
        Array<int> a(3, ep);
        a.reset();
        ASSERT(a.size() == 0);
        ASSERT(a.capacity() == 0);
        ASSERT(!a.values());
    }
   
    // Array<_Type> acquire(int size, _Type* values)

    {
        int* e = Memory::createArrayCopy(3, 3, ep);
        Array<int> a = Array<int>::acquire(3, e);
        ASSERT(a.values() == e);
        ASSERT(compareSequence(a, ep));
        ASSERT(a.size() == 3);
        ASSERT(a.capacity() == 3);
    }

    // _Type* release()

    {
        Array<int> a(3, ep);
        int* e = a.release();
        ASSERT(e[0] == 1 && e[1] == 2 && e[2] == 3);
        ASSERT(a.size() == 0);
        ASSERT(a.capacity() == 0);
        ASSERT(!a.values());
        Memory::deallocate(e);
    }

    // UniquePtr

    {
        Array<UniquePtr<int>> a1;
        Array<UniquePtr<int>> a2(2);
    }

    {
        Array<UniquePtr<int>> a;
        a.pushBack(createUnique<int>(1));
        a.pushBack(createUnique<int>(2));
        ASSERT(*a[0] == 1);
        ASSERT(*a[1] == 2);
        ASSERT(a.find(createUnique<int>(0)) < 0);
        ASSERT(a.find(createUnique<int>(2)) == 1);
    }

    {
        Array<UniquePtr<int>> a;
        a.ensureCapacity(2);
        ASSERT(a.capacity() == 2);
        a.pushBack(createUnique<int>(1));
        a.shrinkToLength();
        ASSERT(a.capacity() == 1);
        a.resize(3);
        ASSERT(a.capacity() == 3);
    }

    {
        Array<UniquePtr<int>> a;
        a.insert(0, createUnique<int>(1));
        a.insert(0, createUnique<int>(2));
        a.popBack();
        ASSERT(*a.back() == 2);
    }

    {
        Array<UniquePtr<int>> a;
        a.pushBack(createUnique<int>(1));
        a.pushBack(createUnique<int>(2));
        a.remove(0);
        ASSERT(*a.front() == 2);
    }

    {
        Array<UniquePtr<int>> a;
        a.pushBack(createUnique<int>(1));
        a.pushBack(createUnique<int>(2));
        a.clear();
        ASSERT(a.empty());
    }
}

void testArrayIterator()
{
    int elem[] = { 1, 2, 3 };
    const int* ep = elem;

    {
        Array<int> a;
        auto iter = a.iterator();
        ASSERT_EXCEPTION(Exception, iter.value());
        ASSERT(!iter.moveNext());
        ASSERT(!iter.movePrev());
    }

    {
        Array<int> a(3, ep);
        auto iter = a.iterator();
        ASSERT_EXCEPTION(Exception, iter.value());
        ASSERT(iter.moveNext());
        ASSERT(iter.value() == 1);
        ASSERT(iter.moveNext());
        ASSERT(iter.value() == 2);
        ASSERT(iter.moveNext());
        ASSERT(iter.value() == 3);
        ASSERT(!iter.moveNext());
        ASSERT_EXCEPTION(Exception, iter.value());
    }

    {
        Array<int> a(3, ep);
        auto iter = a.iterator();
        ASSERT_EXCEPTION(Exception, iter.value());
        ASSERT(iter.movePrev());
        ASSERT(iter.value() == 3);
        ASSERT(iter.movePrev());
        ASSERT(iter.value() == 2);
        ASSERT(iter.movePrev());
        ASSERT(iter.value() == 1);
        ASSERT(!iter.movePrev());
        ASSERT_EXCEPTION(Exception, iter.value());
    }

    {
        Array<int> a(3, ep);
        auto iter = a.iterator();
        ASSERT_EXCEPTION(Exception, iter.value());
        ASSERT(iter.moveNext());
        ASSERT(iter.value() == 1);
        ASSERT(iter.moveNext());
        ASSERT(iter.value() == 2);
        ASSERT(iter.movePrev());
        ASSERT(iter.value() == 1);
        ASSERT(!iter.movePrev());
        ASSERT_EXCEPTION(Exception, iter.value());
    }

    {
        Array<int> a(3, ep);
        auto iter = a.iterator();
        ASSERT_EXCEPTION(Exception, iter.value());
        ASSERT(iter.moveNext());
        ASSERT(iter.value() == 1);
        ASSERT(iter.moveNext());
        ASSERT(iter.value() == 2);
        iter.reset();
        ASSERT_EXCEPTION(Exception, iter.value());
        ASSERT(iter.moveNext());
        ASSERT(iter.value() == 1);
    }

    {
        Array<int> a;
        auto iter = a.constIterator();
        ASSERT_EXCEPTION(Exception, iter.value());
        ASSERT(!iter.moveNext());
        ASSERT(!iter.movePrev());
    }

    {
        Array<int> a(3, ep);
        auto iter = a.constIterator();
        ASSERT_EXCEPTION(Exception, iter.value());
        ASSERT(iter.moveNext());
        ASSERT(iter.value() == 1);
        ASSERT(iter.moveNext());
        ASSERT(iter.value() == 2);
        ASSERT(iter.moveNext());
        ASSERT(iter.value() == 3);
        ASSERT(!iter.moveNext());
        ASSERT_EXCEPTION(Exception, iter.value());
    }

    {
        Array<int> a(3, ep);
        auto iter = a.constIterator();
        ASSERT_EXCEPTION(Exception, iter.value());
        ASSERT(iter.movePrev());
        ASSERT(iter.value() == 3);
        ASSERT(iter.movePrev());
        ASSERT(iter.value() == 2);
        ASSERT(iter.movePrev());
        ASSERT(iter.value() == 1);
        ASSERT(!iter.movePrev());
        ASSERT_EXCEPTION(Exception, iter.value());
    }

    {
        Array<int> a(3, ep);
        auto iter = a.constIterator();
        ASSERT_EXCEPTION(Exception, iter.value());
        ASSERT(iter.moveNext());
        ASSERT(iter.value() == 1);
        ASSERT(iter.moveNext());
        ASSERT(iter.value() == 2);
        ASSERT(iter.movePrev());
        ASSERT(iter.value() == 1);
        ASSERT(!iter.movePrev());
        ASSERT_EXCEPTION(Exception, iter.value());
    }

    {
        Array<int> a(3, ep);
        auto iter = a.constIterator();
        ASSERT_EXCEPTION(Exception, iter.value());
        ASSERT(iter.moveNext());
        ASSERT(iter.value() == 1);
        ASSERT(iter.moveNext());
        ASSERT(iter.value() == 2);
        iter.reset();
        ASSERT_EXCEPTION(Exception, iter.value());
        ASSERT(iter.moveNext());
        ASSERT(iter.value() == 1);
    }
}

void testList()
{
    int elem[] = { 1, 2, 3 };
    const int* np = NULL;
    const int* ep = elem;

    // List()

    {
        List<int> l;
        ASSERT(!l.front());
        ASSERT(!l.back());
        ASSERT(l.size() == 0);
        ASSERT(l.empty());
    }

    // List(int size)

    ASSERT_EXCEPTION(Exception, List<int>(-1));

    {
        List<int> l(0);
        ASSERT(!l.front());
        ASSERT(!l.back());
        ASSERT(l.size() == 0);
        ASSERT(l.empty());
    }

    {
        List<int> l(1);
        ASSERT(l.front());
        ASSERT(l.back());
        ASSERT(l.size() == 1);
        ASSERT(!l.empty());
        ASSERT(l.front()->value == 0);
    }

    // List(int size, const _Type& value)

    ASSERT_EXCEPTION(Exception, List<int>(-1, 123));

    {
        List<int> l(0, 123);
        ASSERT(!l.front());
        ASSERT(!l.back());
        ASSERT(l.size() == 0);
        ASSERT(l.empty());
    }

    {
        List<int> l(1, 123);
        ASSERT(l.front());
        ASSERT(l.back());
        ASSERT(l.size() == 1);
        ASSERT(!l.empty());
        ASSERT(l.front()->value == 123);
    }

    // List(int size, const _Type* values)

    ASSERT_EXCEPTION(Exception, List<int>(-1, ep));
    ASSERT_EXCEPTION(Exception, List<int>(0, ep));
    ASSERT_EXCEPTION(Exception, List<int>(1, np));

    {
        List<int> l(0, np);
        ASSERT(!l.front());
        ASSERT(!l.back());
        ASSERT(l.size() == 0);
        ASSERT(l.empty());
    }

    {
        List<int> l(3, ep);
        ASSERT(l.front());
        ASSERT(l.back());
        ASSERT(l.size() == 3);
        ASSERT(!l.empty());
        ASSERT(compareSequence(l, ep));
    }

    // List(const List<_Type>& other)

    {
        List<int> l1, l2(l1);
        ASSERT(!l2.front());
        ASSERT(!l2.back());
        ASSERT(l2.size() == 0);
        ASSERT(l2.empty());
    }

    {
        List<int> l1(3, ep), l2(l1);
        ASSERT(l2.front());
        ASSERT(l2.back());
        ASSERT(l2.size() == 3);
        ASSERT(!l2.empty());
        ASSERT(compareSequence(l2, ep));
    }

    // List(List<_Type>&& other)

    {
        List<int> l1, l2(static_cast<List<int>&&>(l1));
        ASSERT(!l2.front());
        ASSERT(!l2.back());
        ASSERT(l2.size() == 0);
        ASSERT(l2.empty());
    }

    {
        List<int> l1(3, ep), l2(static_cast<List<int>&&>(l1));

        ASSERT(!l1.front());
        ASSERT(!l1.back());
        ASSERT(l1.size() == 0);
        ASSERT(l1.empty());

        ASSERT(l2.front());
        ASSERT(l2.back());
        ASSERT(l2.size() == 3);
        ASSERT(!l2.empty());
        ASSERT(compareSequence(l2, ep));
    }

    //  List<_Type>& operator=(const List<_Type>& other)

    {
        List<int> l1(3, ep), l2;
        l2 = l1;
        ASSERT(l2.front());
        ASSERT(l2.back());
        ASSERT(l2.size() == 3);
        ASSERT(!l2.empty());
        ASSERT(compareSequence(l2, ep));
    }

    // List<_Type>& operator=(List<_Type>&& other)

    {
        List<int> l1, l2;
        l2 = static_cast<List<int>&&>(l1);
        ASSERT(!l2.front());
        ASSERT(!l2.back());
        ASSERT(l2.size() == 0);
        ASSERT(l2.empty());
    }

    {
        List<int> l1(3, ep), l2;
        l2 = static_cast<List<int>&&>(l1);

        ASSERT(!l1.front());
        ASSERT(!l1.back());
        ASSERT(l1.size() == 0);
        ASSERT(l1.empty());

        ASSERT(l2.front());
        ASSERT(l2.back());
        ASSERT(l2.size() == 3);
        ASSERT(!l2.empty());
        ASSERT(compareSequence(l2, ep));
    }

    // int size() const
    // bool empty() const
    // ListNode<_Type>* front()
    // ListNode<_Type>* back()

    {
        List<int> l;

        ASSERT(!l.front());
        ASSERT(!l.back());
        ASSERT(l.size() == 0);
        ASSERT(l.empty());

        l.pushBack(1);
        ASSERT(l.front());
        ASSERT(l.back());
        ASSERT(l.size() == 1);
        ASSERT(!l.empty());
        ASSERT(l.front()->value == 1);
        ASSERT(l.back()->value == 1);

        l.pushBack(2);
        ASSERT(l.front());
        ASSERT(l.back());
        ASSERT(l.size() == 2);
        ASSERT(!l.empty());
        ASSERT(l.front()->value == 1);
        ASSERT(l.back()->value == 2);
    }

    // const ListNode<_Type>* front() const
    // const ListNode<_Type>* back() const

    {
        List<int> l;
        const List<int>& cl = l;

        ASSERT(!cl.front());
        ASSERT(!cl.back());

        l.pushBack(1);
        ASSERT(cl.front());
        ASSERT(cl.back());
        ASSERT(cl.front()->value == 1);
        ASSERT(cl.back()->value == 1);

        l.pushBack(2);
        ASSERT(cl.front());
        ASSERT(cl.back());
        ASSERT(cl.front()->value == 1);
        ASSERT(cl.back()->value == 2);
    }


    // ListNode<_Type>* find(const _Type& value)

    {
        List<int> l;
        ASSERT(!l.find(0));
    }

    {
        List<int> l(3, ep);
        ASSERT(l.find(2) == l.front()->next);
        ASSERT(!l.find(0));
    }

    // const ListNode<_Type>* find(const _Type& value) const

    {
        List<int> l;
        const List<int>& cl = l;
        ASSERT(!cl.find(0));
    }

    {
        List<int> l(3, ep);
        const List<int>& cl = l;
        ASSERT(cl.find(2) == cl.front()->next);
        ASSERT(!cl.find(0));
    }

    // void assign(int size, const _Type& value)

    {
        List<int> l;
        l.assign(0, 123);
        ASSERT(l.empty());
    }

    {
        List<int> l;
        l.assign(1, 123);
        ASSERT(!l.empty());
        ASSERT(l.front()->value == 123);
    }
    
    // void assign(int size, const _Type* values)

    {
        List<int> l;
        l.assign(0, np);
        ASSERT(l.empty());
    }

    {
        List<int> l;
        l.assign(3, ep);
        ASSERT(!l.empty());
        ASSERT(compareSequence(l, ep));
    }

    // void assign(const List<_Type>& other)

    {
        List<int> l1, l2;
        l2.assign(l1);
        ASSERT(l2.empty());
    }

    {
        List<int> l1(3, ep), l2;
        l2.assign(l1);
        ASSERT(!l2.empty());
        ASSERT(compareSequence(l2, ep));
    }

    // void popFront()

    ASSERT_EXCEPTION(Exception, List<int>().popFront());

    {
        List<int> l(3, ep);

        l.popFront();
        ASSERT(l.front()->value == 2);
        ASSERT(l.size() == 2);

        l.popFront();
        ASSERT(l.front()->value == 3);
        ASSERT(l.size() == 1);

        l.popFront();
        ASSERT(l.empty());
    }

    // void popBack()

    ASSERT_EXCEPTION(Exception, List<int>().popBack());

    {
        List<int> l(3, ep);

        l.popBack();
        ASSERT(l.back()->value == 2);
        ASSERT(l.size() == 2);

        l.popBack();
        ASSERT(l.back()->value == 1);
        ASSERT(l.size() == 1);

        l.popBack();
        ASSERT(l.empty());
    }

    // void pushFront(const _Type& value)

    {
        List<int> l;
        int v1 = 1, v2 = 2;
        l.pushFront(v1);
        ASSERT(l.front()->value == 1);
        l.pushFront(v2);
        ASSERT(l.front()->value == 2);
    }

    // void pushFront(_Type&& value)

    {
        List<int> l;
        l.pushFront(1);
        ASSERT(l.front()->value == 1);
        l.pushFront(2);
        ASSERT(l.front()->value == 2);
    }

    // void pushBack(const _Type& value)

    {
        List<int> l;
        int v1 = 1, v2 = 2;
        l.pushBack(v1);
        ASSERT(l.back()->value == 1);
        l.pushBack(v2);
        ASSERT(l.back()->value == 2);
    }

    // void pushBack(_Type&& value)

    {
        List<int> l;
        l.pushBack(1);
        ASSERT(l.back()->value == 1);
        l.pushBack(2);
        ASSERT(l.back()->value == 2);
    }

    // ListNode<_Type>* insertBefore(ListNode<_Type>* pos, const _Type& value)

    {
        int v;
        ASSERT_EXCEPTION(Exception, List<int>().insertBefore(NULL, v));
    }

    {
        List<int> l;
        int v1 = 1, v2 = 2, v3 = 3;
        l.pushBack(v1);
        ASSERT(l.insertBefore(l.front(), v2)->value == v2);
        ASSERT(l.insertBefore(l.back(), v3)->value == v3);
        ASSERT(compareSequence(l, 2, 3, 1));
    }

    // ListNode<_Type>* insertBefore(ListNode<_Type>* pos, _Type&& value)

    ASSERT_EXCEPTION(Exception, List<int>().insertBefore(NULL, 0));

    {
        List<int> l;
        l.pushBack(1);
        ASSERT(l.insertBefore(l.front(), 2)->value == 2);
        ASSERT(l.insertBefore(l.back(), 3)->value == 3);
        ASSERT(compareSequence(l, 2, 3, 1));
    }

    // ListNode<_Type>* insertAfter(ListNode<_Type>* pos, const _Type& value)

    {
        int v;
        ASSERT_EXCEPTION(Exception, List<int>().insertAfter(NULL, v));
    }

    {
        List<int> l;
        int v1 = 1, v2 = 2, v3 = 3;
        l.pushBack(v1);
        ASSERT(l.insertAfter(l.back(), v2)->value == v2);
        ASSERT(l.insertAfter(l.front(), v3)->value == v3);
        ASSERT(compareSequence(l, 1, 3, 2));
    }

    // ListNode<_Type>* insertAfter(ListNode<_Type>* pos, _Type&& value)

    ASSERT_EXCEPTION(Exception, List<int>().insertAfter(NULL, 0));

    {
        List<int> l;
        l.pushBack(1);
        ASSERT(l.insertAfter(l.back(), 2)->value == 2);
        ASSERT(l.insertAfter(l.front(), 3)->value == 3);
        ASSERT(compareSequence(l, 1, 3, 2));
    }

    // void remove(ListNode<_Type>* pos)

    ASSERT_EXCEPTION(Exception, List<int>().remove(NULL));

    {
        List<int> l(3, ep);
        l.remove(l.front());
        ASSERT(compareSequence(l, 2, 3));
    }

    {
        List<int> l(3, ep);
        l.remove(l.back());
        ASSERT(compareSequence(l, 1, 2));
    }

    {
        List<int> l(3, ep);
        l.remove(l.front()->next);
        ASSERT(compareSequence(l, 1, 3));
    }

    // void clear()

    {
        List<int> l;
        l.clear();
        ASSERT(!l.front());
        ASSERT(!l.back());
        ASSERT(l.size() == 0);
        ASSERT(l.empty());
    }

    {
        List<int> l(3, ep);
        l.clear();
        ASSERT(!l.front());
        ASSERT(!l.back());
        ASSERT(l.size() == 0);
        ASSERT(l.empty());
    }

    // UniquePtr

    {
        List<UniquePtr<int>> l1;
        List<UniquePtr<int>> l2(2);
    }

    {
         List<UniquePtr<int>> l;
         l.pushBack(createUnique<int>(1));
         l.pushBack(createUnique<int>(2));
         ASSERT(!l.find(createUnique<int>(0)));
         ASSERT(l.find(createUnique<int>(2)));
    }

    {
         List<UniquePtr<int>> l;
         l.pushBack(createUnique<int>(1));
         l.insertBefore(l.front(), createUnique<int>(2));
         l.popFront();
         ASSERT(*l.back()->value == 1);
    }

    {
         List<UniquePtr<int>> l;
         l.pushFront(createUnique<int>(1));
         l.insertAfter(l.back(), createUnique<int>(2));
         l.popBack();
         ASSERT(*l.back()->value == 1);
    }

    {
         List<UniquePtr<int>> l;
         l.pushBack(createUnique<int>(1));
         l.pushBack(createUnique<int>(2));
         l.remove(l.front());
         ASSERT(*l.back()->value == 2);
    }

    {
         List<UniquePtr<int>> l;
         l.pushBack(createUnique<int>(1));
         l.pushBack(createUnique<int>(2));
         l.clear();
         ASSERT(l.empty());
    }
}

void testListIterator()
{
    int elem[] = { 1, 2, 3 };
    const int* ep = elem;

    {
        List<int> l;
        auto iter = l.iterator();
        ASSERT_EXCEPTION(Exception, iter.value());
        ASSERT(!iter.moveNext());
        ASSERT(!iter.movePrev());
    }

    {
        List<int> l(3, ep);
        auto iter = l.iterator();
        ASSERT_EXCEPTION(Exception, iter.value());
        ASSERT(iter.moveNext());
        ASSERT(iter.value() == 1);
        ASSERT(iter.moveNext());
        ASSERT(iter.value() == 2);
        ASSERT(iter.moveNext());
        ASSERT(iter.value() == 3);
        ASSERT(!iter.moveNext());
        ASSERT_EXCEPTION(Exception, iter.value());
    }

    {
        List<int> l(3, ep);
        auto iter = l.iterator();
        ASSERT_EXCEPTION(Exception, iter.value());
        ASSERT(iter.movePrev());
        ASSERT(iter.value() == 3);
        ASSERT(iter.movePrev());
        ASSERT(iter.value() == 2);
        ASSERT(iter.movePrev());
        ASSERT(iter.value() == 1);
        ASSERT(!iter.movePrev());
        ASSERT_EXCEPTION(Exception, iter.value());
    }

    {
        List<int> l(3, ep);
        auto iter = l.iterator();
        ASSERT_EXCEPTION(Exception, iter.value());
        ASSERT(iter.moveNext());
        ASSERT(iter.value() == 1);
        ASSERT(iter.moveNext());
        ASSERT(iter.value() == 2);
        ASSERT(iter.movePrev());
        ASSERT(iter.value() == 1);
        ASSERT(!iter.movePrev());
        ASSERT_EXCEPTION(Exception, iter.value());
    }

    {
        List<int> l(3, ep);
        auto iter = l.iterator();
        ASSERT_EXCEPTION(Exception, iter.value());
        ASSERT(iter.moveNext());
        ASSERT(iter.value() == 1);
        ASSERT(iter.moveNext());
        ASSERT(iter.value() == 2);
        iter.reset();
        ASSERT_EXCEPTION(Exception, iter.value());
        ASSERT(iter.moveNext());
        ASSERT(iter.value() == 1);
    }

    {
        List<int> l;
        auto iter = l.constIterator();
        ASSERT_EXCEPTION(Exception, iter.value());
        ASSERT(!iter.moveNext());
        ASSERT(!iter.movePrev());
    }

    {
        List<int> l(3, ep);
        auto iter = l.constIterator();
        ASSERT_EXCEPTION(Exception, iter.value());
        ASSERT(iter.moveNext());
        ASSERT(iter.value() == 1);
        ASSERT(iter.moveNext());
        ASSERT(iter.value() == 2);
        ASSERT(iter.moveNext());
        ASSERT(iter.value() == 3);
        ASSERT(!iter.moveNext());
        ASSERT_EXCEPTION(Exception, iter.value());
    }

    {
        List<int> l(3, ep);
        auto iter = l.constIterator();
        ASSERT_EXCEPTION(Exception, iter.value());
        ASSERT(iter.movePrev());
        ASSERT(iter.value() == 3);
        ASSERT(iter.movePrev());
        ASSERT(iter.value() == 2);
        ASSERT(iter.movePrev());
        ASSERT(iter.value() == 1);
        ASSERT(!iter.movePrev());
        ASSERT_EXCEPTION(Exception, iter.value());
    }

    {
        List<int> l(3, ep);
        auto iter = l.constIterator();
        ASSERT_EXCEPTION(Exception, iter.value());
        ASSERT(iter.moveNext());
        ASSERT(iter.value() == 1);
        ASSERT(iter.moveNext());
        ASSERT(iter.value() == 2);
        ASSERT(iter.movePrev());
        ASSERT(iter.value() == 1);
        ASSERT(!iter.movePrev());
        ASSERT_EXCEPTION(Exception, iter.value());
    }

    {
        List<int> l(3, ep);
        auto iter = l.constIterator();
        ASSERT_EXCEPTION(Exception, iter.value());
        ASSERT(iter.moveNext());
        ASSERT(iter.value() == 1);
        ASSERT(iter.moveNext());
        ASSERT(iter.value() == 2);
        iter.reset();
        ASSERT_EXCEPTION(Exception, iter.value());
        ASSERT(iter.moveNext());
        ASSERT(iter.value() == 1);
    }
}

void testMap()
{
    // Map(int numBuckets = 0)

    ASSERT_EXCEPTION(Exception, Map<int, int>(-1));

    {
        Map<int, int> m;
        ASSERT(m.size() == 0);
        ASSERT(m.numBuckets() == 0);
        ASSERT(m.maxLoadFactor() == 0.75f);
        ASSERT(m.empty());
    }

    {
        Map<int, int> m(10);
        ASSERT(m.size() == 0);
        ASSERT(m.numBuckets() == 10);
        ASSERT(m.maxLoadFactor() == 0.75f);
        ASSERT(m.empty());
    }

    // Map(const Map<_Key, _Value>& other)

    {
        Map<int, int> m1, m2(m1);
        ASSERT(m2.size() == 0);
        ASSERT(m2.numBuckets() == 0);
        ASSERT(m2.maxLoadFactor() == 0.75f);
        ASSERT(m2.empty());
    }

    {
        Map<int, int> m1;
        m1.insert(1, 10);
        Map<int, int> m2(m1);
        ASSERT(m1.size() == 1);
        ASSERT(m1.numBuckets() == 1);
        ASSERT(m2.maxLoadFactor() == 0.75f);
        ASSERT(!m2.empty());
        ASSERT(m2[1] == 10);
    }

    // Map(Map<_Key, _Value>&& other)
   
    {
        Map<int, int> m1, m2(static_cast<Map<int, int>&&>(m1));

        ASSERT(m1.size() == 0);
        ASSERT(m1.numBuckets() == 0);
        ASSERT(m2.maxLoadFactor() == 0.75f);
        ASSERT(m1.empty());

        ASSERT(m2.size() == 0);
        ASSERT(m2.numBuckets() == 0);
        ASSERT(m2.maxLoadFactor() == 0.75f);
        ASSERT(m2.empty());
    }

    {
        Map<int, int> m1;
        m1.insert(1, 10);

        Map<int, int> m2(static_cast<Map<int, int>&&>(m1));

        ASSERT(m1.size() == 0);
        ASSERT(m1.numBuckets() == 0);
        ASSERT(m1.maxLoadFactor() == 0.75f);
        ASSERT(m1.empty());

        ASSERT(m2.size() == 1);
        ASSERT(m2.numBuckets() == 1);
        ASSERT(m2.maxLoadFactor() == 0.75f);
        ASSERT(!m2.empty());
        ASSERT(m2[1] == 10);
    }

    // _Value& operator[](const _Key& key)
    // _Value& value(const _Key& key)

    {
        Map<int, int> m;
        int k1 = 1, k2 = 2;
        int v1 = 10, v2 = 20, v3 = 30;

        m[k1] = v1;
        ASSERT(m.size() == 1);
        ASSERT(m.numBuckets() == 1);
        ASSERT(!m.empty());
        ASSERT(m[k1] == v1);

        m[k1] = v2;
        ASSERT(m.size() == 1);
        ASSERT(m.numBuckets() == 3);
        ASSERT(!m.empty());
        ASSERT(m[k1] == v2);

        m[k2] = v3;
        ASSERT(m.size() == 2);
        ASSERT(m.numBuckets() == 3);
        ASSERT(!m.empty());
        ASSERT(m[k1] == v2);
        ASSERT(m[k2] == v3);
    }

    // _Value& operator[](_Key&& key)
    // _Value& value(_Key&& key)

    {
        Map<int, int> m;

        m[1] = 10;
        ASSERT(m.size() == 1);
        ASSERT(m.numBuckets() == 1);
        ASSERT(!m.empty());
        ASSERT(m[1] == 10);

        m[1] = 20;
        ASSERT(m.size() == 1);
        ASSERT(m.numBuckets() == 3);
        ASSERT(!m.empty());
        ASSERT(m[1] == 20);

        m[2] = 30;
        ASSERT(m.size() == 2);
        ASSERT(m.numBuckets() == 3);
        ASSERT(!m.empty());
        ASSERT(m[1] == 20);
        ASSERT(m[2] == 30);
    }

    // const _Value& operator[](const _Key& key) const
    // const _Value& value(const _Key& key) const

    {
        Map<int, int> m;
        m.insert(1, 10);

        const Map<int, int>& cm = m;
        ASSERT(cm[1] == 10);
        ASSERT_EXCEPTION(Exception, cm[2]);
    }

    // Map<_Key, _Value>& operator=(const Map<_Key, _Value>& other)

    {
        Map<int, int> m1;
        m1.insert(1, 10);
        Map<int, int> m2;
        m2 = m1;
        ASSERT(m1.size() == 1);
        ASSERT(m1.numBuckets() == 1);
        ASSERT(!m2.empty());
        ASSERT(m2[1] == 10);
    }

    // Map<_Key, _Value>& operator=(Map<_Key, _Value>&& other)

    {
        Map<int, int> m1, m2;
        m2 = static_cast<Map<int, int>&&>(m1);

        ASSERT(m1.size() == 0);
        ASSERT(m1.numBuckets() == 0);
        ASSERT(m1.empty());

        ASSERT(m2.size() == 0);
        ASSERT(m2.numBuckets() == 0);
        ASSERT(m2.empty());
    }

    {
        Map<int, int> m1;
        m1.insert(1, 10);

        Map<int, int> m2;
        m2 = static_cast<Map<int, int>&&>(m1);

        ASSERT(m1.size() == 0);
        ASSERT(m1.numBuckets() == 0);
        ASSERT(m1.empty());

        ASSERT(m2.size() == 1);
        ASSERT(m2.numBuckets() == 1);
        ASSERT(!m2.empty());
        ASSERT(m2[1] == 10);
    }

    // int size() const
    // int numBuckets() const
    // bool empty() const
    // float loadFactor() const

    {
        Map<int, int> m;
        ASSERT(m.size() == 0);
        ASSERT(m.numBuckets() == 0);
        ASSERT(m.empty());
        ASSERT(isnan(m.loadFactor()));
    }

    {
        Map<int, int> m;
        m.insert(1, 10);

        ASSERT(m.size() == 1);
        ASSERT(m.numBuckets() == 1);
        ASSERT(!m.empty());
        ASSERT(m.loadFactor() == 1);
    }
            
    {
        Map<int, int> m;
        m.insert(1, 10);
        m.insert(2, 20);

        ASSERT(m.size() == 2);
        ASSERT(m.numBuckets() == 3);
        ASSERT(!m.empty());
        ASSERT(m.loadFactor() == 2.0f / 3.0f);
    }

    // float maxLoadFactor() const
    // void maxLoadFactor(float loadFactor)

    {
        Map<int, int> m;
        ASSERT_EXCEPTION(Exception, m.maxLoadFactor(0));
        m.maxLoadFactor(0.5f);
        ASSERT(m.maxLoadFactor() == 0.5f);
    }

    // _Value* find(const _Key& key)

    {
        Map<int, int> m;
        m.insert(1, 10);
        ASSERT(*m.find(1) == 10);
        ASSERT(!m.find(2));
    }

    // const _Value* find(const _Key& key) const

    {
        Map<int, int> m;
        m.insert(1, 10);

        const Map<int, int>& cm = m;
        ASSERT(*cm.find(1) == 10);
        ASSERT(!cm.find(2));
    }

    // void assign(const Map<_Key, _Value>& other)

    {
        Map<int, int> m1, m2;
        m2.assign(m1);
        ASSERT(m2.size() == 0);
        ASSERT(m2.numBuckets() == 0);
        ASSERT(m2.empty());
    }

    {
        Map<int, int> m1;
        m1.insert(1, 10);
        Map<int, int> m2;
        m2.assign(m1);
        ASSERT(m1.size() == 1);
        ASSERT(m1.numBuckets() == 1);
        ASSERT(!m2.empty());
        ASSERT(m2[1] == 10);
    }

    // void insert(const _Key& key, const _Value& value)

    {
        Map<int, int> m;
        int k = 1, v = 10;
        m.insert(k, v);
        ASSERT(m[k] == v);
    }

    // void insert(_Key&& key, _Value&& value)

    {
        Map<int, int> m;
        m.insert(1, 10);
        ASSERT(m[1] == 10);
    }

    // bool remove(const _Key& key)

    {
        Map<int, int> m;
        m.insert(1, 10);
        m.remove(1);
        ASSERT(m.empty());
    }

    // void clear()

    {
        Map<int, int> m;
        m.insert(1, 10);
        m.insert(2, 20);
        m.clear();
        ASSERT(m.empty());
    }

    // void rehash(int numBuckets)

    {
        Map<int, int> m;
        ASSERT_EXCEPTION(Exception, m.rehash(-1));
    }

    {
        Map<int, int> m;
        m.rehash(0);
        ASSERT(m.size() == 0);
        ASSERT(m.numBuckets() == 0);
    }

    {
        Map<int, int> m;
        m.insert(1, 10);
        m.insert(2, 20);
        ASSERT(m.size() == 2);
        ASSERT(m.numBuckets() == 3);

        m.rehash(0);
        ASSERT(m.size() == 2);
        ASSERT(m.numBuckets() == 3);
    }

    {
        Map<int, int> m;
        m.insert(1, 10);
        m.insert(2, 20);
        ASSERT(m.size() == 2);
        ASSERT(m.numBuckets() == 3);

        m.rehash(10);
        ASSERT(m.size() == 2);
        ASSERT(m.numBuckets() == 10);
    }

    // UniquePtr

    {
        Map<UniquePtr<int>, UniquePtr<int>> m;
        m[createUnique<int>(1)] = createUnique<int>(10);
        ASSERT(**m.find(createUnique<int>(1)) == 10);
        const Map<UniquePtr<int>, UniquePtr<int>>& cm = m;
        ASSERT(*cm[createUnique<int>(1)] == 10);
        ASSERT(**cm.find(createUnique<int>(1)) == 10);
    }

    {
        Map<UniquePtr<int>, UniquePtr<int>> m;
        m.insert(createUnique<int>(1), createUnique<int>(10));
        m.insert(createUnique<int>(1), createUnique<int>(20));
        m.insert(createUnique<int>(2), createUnique<int>(20));
        ASSERT(m.size() == 2);
        ASSERT(m.remove(createUnique<int>(1)));
        ASSERT(!m.remove(createUnique<int>(3)));
        ASSERT(m.size() == 1);
    }

    {
        Map<UniquePtr<int>, UniquePtr<int>> m;
        m.insert(createUnique<int>(1), createUnique<int>(10));
        m.insert(createUnique<int>(2), createUnique<int>(20));
        m.rehash(10);
        m.clear();
    }
}

void testMapIterator()
{
    {
        Map<int, int> m;
        auto iter = m.iterator();
        ASSERT_EXCEPTION(Exception, iter.value());
        ASSERT(!iter.moveNext());
    }

    {
        Map<int, int> m;
        m.insert(1, 10);
        m.insert(2, 20);
        m.insert(3, 30);

        auto iter = m.iterator();
        int sum1 = 0, sum2 = 0;

        ASSERT_EXCEPTION(Exception, iter.value());
        ASSERT(iter.moveNext());
        sum1 += iter.value().key;
        sum2 += iter.value().value;
        ASSERT(iter.moveNext());
        sum1 += iter.value().key;
        sum2 += iter.value().value;
        ASSERT(iter.moveNext());
        sum1 += iter.value().key;
        sum2 += iter.value().value;
        ASSERT(!iter.moveNext());
        ASSERT_EXCEPTION(Exception, iter.value());

        ASSERT(sum1 == 6 && sum2 == 60);
    }

    {
        Map<int, int> m;
        m.insert(1, 10);
        m.insert(2, 20);
        m.insert(3, 30);

        auto iter = m.iterator();
        ASSERT_EXCEPTION(Exception, iter.value());
        ASSERT(iter.moveNext());
        ASSERT_NO_EXCEPTION(iter.value());
        iter.reset();
        ASSERT_EXCEPTION(Exception, iter.value());
    }

    {
        Map<int, int> m;
        auto iter = m.constIterator();
        ASSERT_EXCEPTION(Exception, iter.value());
        ASSERT(!iter.moveNext());
    }

    {
        Map<int, int> m;
        m.insert(1, 10);
        m.insert(2, 20);
        m.insert(3, 30);

        auto iter = m.constIterator();
        int sum1 = 0, sum2 = 0;

        ASSERT_EXCEPTION(Exception, iter.value());
        ASSERT(iter.moveNext());
        sum1 += iter.value().key;
        sum2 += iter.value().value;
        ASSERT(iter.moveNext());
        sum1 += iter.value().key;
        sum2 += iter.value().value;
        ASSERT(iter.moveNext());
        sum1 += iter.value().key;
        sum2 += iter.value().value;
        ASSERT(!iter.moveNext());
        ASSERT_EXCEPTION(Exception, iter.value());

        ASSERT(sum1 == 6 && sum2 == 60);
    }

    {
        Map<int, int> m;
        m.insert(1, 10);
        m.insert(2, 20);
        m.insert(3, 30);

        auto iter = m.constIterator();
        ASSERT_EXCEPTION(Exception, iter.value());
        ASSERT(iter.moveNext());
        ASSERT_NO_EXCEPTION(iter.value());
        iter.reset();
        ASSERT_EXCEPTION(Exception, iter.value());
    }
}

void testSet()
{
    // Set(int numBuckets = 0)

    ASSERT_EXCEPTION(Exception, Set<int>(-1));

    {
        Set<int> s;
        ASSERT(s.size() == 0);
        ASSERT(s.numBuckets() == 0);
        ASSERT(s.maxLoadFactor() == 0.75f);
        ASSERT(s.empty());
    }

    {
        Set<int> s(10);
        ASSERT(s.size() == 0);
        ASSERT(s.numBuckets() == 10);
        ASSERT(s.maxLoadFactor() == 0.75f);
        ASSERT(s.empty());
    }

    // Set(const Set<_Type>& other)

    {
        Set<int> s1, s2(s1);
        ASSERT(s2.size() == 0);
        ASSERT(s2.numBuckets() == 0);
        ASSERT(s2.maxLoadFactor() == 0.75f);
        ASSERT(s2.empty());
    }

    {
        Set<int> s1;
        s1.insert(1);
        Set<int> s2(s1);
        ASSERT(s2.size() == 1);
        ASSERT(s2.numBuckets() == 1);
        ASSERT(s2.maxLoadFactor() == 0.75f);
        ASSERT(!s2.empty());
        ASSERT(s2.find(1));
    }

    // Set(Set<_Type>&& other)
   
    {
        Set<int> s1, s2(static_cast<Set<int>&&>(s1));

        ASSERT(s1.size() == 0);
        ASSERT(s1.numBuckets() == 0);
        ASSERT(s1.maxLoadFactor() == 0.75f);
        ASSERT(s1.empty());

        ASSERT(s2.size() == 0);
        ASSERT(s2.numBuckets() == 0);
        ASSERT(s2.maxLoadFactor() == 0.75f);
        ASSERT(s2.empty());
    }

    {
        Set<int> s1;
        s1.insert(1);

        Set<int> s2(static_cast<Set<int>&&>(s1));

        ASSERT(s1.size() == 0);
        ASSERT(s1.numBuckets() == 0);
        ASSERT(s1.maxLoadFactor() == 0.75f);
        ASSERT(s1.empty());

        ASSERT(s2.size() == 1);
        ASSERT(s2.numBuckets() == 1);
        ASSERT(s2.maxLoadFactor() == 0.75f);
        ASSERT(!s2.empty());
        ASSERT(s2.find(1));
    }

    // Set<_Type>& operator=(const Set<_Type>& other)

    {
        Set<int> s1;
        s1.insert(1);
        Set<int> s2;
        s2 = s1;
        ASSERT(s1.size() == 1);
        ASSERT(s1.numBuckets() == 1);
        ASSERT(!s2.empty());
        ASSERT(s2.find(1));
    }

    // Set<_Type>& operator=(Set<_Type>&& other)

    {
        Set<int> s1, s2;
        s2 = static_cast<Set<int>&&>(s1);

        ASSERT(s1.size() == 0);
        ASSERT(s1.numBuckets() == 0);
        ASSERT(s1.empty());

        ASSERT(s2.size() == 0);
        ASSERT(s2.numBuckets() == 0);
        ASSERT(s2.empty());
    }

    {
        Set<int> s1;
        s1.insert(1);

        Set<int> s2;
        s2 = static_cast<Set<int>&&>(s1);

        ASSERT(s1.size() == 0);
        ASSERT(s1.numBuckets() == 0);
        ASSERT(s1.empty());

        ASSERT(s2.size() == 1);
        ASSERT(s2.numBuckets() == 1);
        ASSERT(!s2.empty());
        ASSERT(s2.find(1));
    }

    // int size() const
    // int numBuckets() const
    // bool empty() const
    // float loadFactor() const

    {
        Set<int> s;
        ASSERT(s.size() == 0);
        ASSERT(s.numBuckets() == 0);
        ASSERT(s.empty());
        ASSERT(isnan(s.loadFactor()));
    }

    {
        Set<int> s;
        s.insert(1);

        ASSERT(s.size() == 1);
        ASSERT(s.numBuckets() == 1);
        ASSERT(!s.empty());
        ASSERT(s.loadFactor() == 1);
    }
            
    {
        Set<int> s;
        s.insert(1);
        s.insert(2);

        ASSERT(s.size() == 2);
        ASSERT(s.numBuckets() == 3);
        ASSERT(!s.empty());
        ASSERT(s.loadFactor() == 2.0f / 3.0f);
    }

    // float maxLoadFactor() const
    // void maxLoadFactor(float loadFactor)

    {
        Set<int> s;
        ASSERT_EXCEPTION(Exception, s.maxLoadFactor(0));
        s.maxLoadFactor(0.5f);
        ASSERT(s.maxLoadFactor() == 0.5f);
    }

    // const _Type* find(const _Type& value) const

    {
        Set<int> s;
        s.insert(1);

        const Set<int>& cs = s;
        ASSERT(*cs.find(1) == 1);
        ASSERT(!cs.find(2));
    }

    // void assign(const Set<_Type>& other)

    {
        Set<int> s1, s2;
        s2.assign(s1);
        ASSERT(s2.size() == 0);
        ASSERT(s2.numBuckets() == 0);
        ASSERT(s2.empty());
    }

    {
        Set<int> s1;
        s1.insert(1);
        Set<int> s2;
        s2.assign(s1);
        ASSERT(s1.size() == 1);
        ASSERT(s1.numBuckets() == 1);
        ASSERT(!s2.empty());
        ASSERT(s2.find(1));
    }

    // void insert(const _Type& value)

    {
        Set<int> s;
        int v = 1;
        s.insert(v);
        ASSERT(s.find(v));
    }

    // void insert(_Type&& value)

    {
        Set<int> s;
        s.insert(1);
        ASSERT(s.find(1));
    }

    // _Type remove()

    ASSERT_EXCEPTION(Exception, Set<int>().remove());

    {
        Set<int> s;
        s.insert(1);
        ASSERT(s.remove() == 1);
    }

    // bool remove(const _Type& value)

    {
        Set<int> s;
        s.insert(1);
        ASSERT(!s.remove(2));
        ASSERT(s.remove(1));
        ASSERT(s.empty());
    }

    // void clear()

    {
        Set<int> s;
        s.insert(1);
        s.insert(2);
        s.clear();
        ASSERT(s.empty());
    }

    // void rehash(int numBuckets)

    {
        Set<int> s;
        ASSERT_EXCEPTION(Exception, s.rehash(-1));
    }

    {
        Set<int> s;
        s.rehash(0);
        ASSERT(s.size() == 0);
        ASSERT(s.numBuckets() == 0);
    }

    {
        Set<int> s;
        s.insert(1);
        s.insert(2);
        ASSERT(s.size() == 2);
        ASSERT(s.numBuckets() == 3);

        s.rehash(0);
        ASSERT(s.size() == 2);
        ASSERT(s.numBuckets() == 3);
    }

    {
        Set<int> s;
        s.insert(1);
        s.insert(2);
        ASSERT(s.size() == 2);
        ASSERT(s.numBuckets() == 3);

        s.rehash(10);
        ASSERT(s.size() == 2);
        ASSERT(s.numBuckets() == 10);
    }

    // UniquePtr

    {
        Set<UniquePtr<int>> s;
        s.insert(createUnique<int>(1));
        ASSERT(s.find(createUnique<int>(1)));
    }

    {
        Set<UniquePtr<int>> s;
        s.insert(createUnique<int>(1));
        UniquePtr<int> p = s.remove();
        ASSERT(*p == 1);
    }

    {
        Set<UniquePtr<int>> s;
        s.insert(createUnique<int>(1));
        ASSERT(!s.remove(createUnique<int>(2)));
        ASSERT(s.remove(createUnique<int>(1)));
    }

    {
        Set<UniquePtr<int>> s;
        s.insert(createUnique<int>(1));
        s.insert(createUnique<int>(2));
        s.rehash(10);
        s.clear();
    }
}

void testSetIterator()
{
    {
        Set<int> s;
        auto iter = s.constIterator();
        ASSERT_EXCEPTION(Exception, iter.value());
        ASSERT(!iter.moveNext());
    }

    {
        Set<int> s;
        s.insert(1);
        s.insert(2);
        s.insert(3);

        auto iter = s.constIterator();
        int sum = 0;

        ASSERT_EXCEPTION(Exception, iter.value());
        ASSERT(iter.moveNext());
        sum += iter.value();
        ASSERT(iter.moveNext());
        sum += iter.value();
        ASSERT(iter.moveNext());
        sum += iter.value();
        ASSERT(!iter.moveNext());
        ASSERT_EXCEPTION(Exception, iter.value());

        ASSERT(sum == 6);
    }

    {
        Set<int> s;
        s.insert(1);
        s.insert(2);
        s.insert(3);

        auto iter = s.constIterator();
        ASSERT_EXCEPTION(Exception, iter.value());
        ASSERT(iter.moveNext());
        ASSERT_NO_EXCEPTION(iter.value());
        iter.reset();
        ASSERT_EXCEPTION(Exception, iter.value());
    }
}

void testFoundation()
{
    testUniquePtr();
    testSharedPtr();
    testString();
    testStringIterator();
    testArray();
    testArrayIterator();
    testList();
    testListIterator();
    testMap();
    testMapIterator();
    testSet();
    testSetIterator();
}

void testFile()
{
#ifdef CHAR_ENCODING_UTF8
    const char_t* str = "\x24\xc2\xa2\n\xe2\x82\xac\xf0\x90\x8d\x88\n";
#else
    const char_t* str = u"\x0024\x00a2\n\x20ac\xd800\xdf48\n";
#endif
    
    byte_t BYTES_UTF8_UNIX[] = { 0x24, 0xc2, 0xa2, 0x0a, 0xe2, 0x82, 0xac, 0xf0, 0x90, 0x8d, 0x88, 0x0a };
    byte_t BYTES_UTF8_BOM_UNIX[] = { 0xef, 0xbb, 0xbf, 0x24, 0xc2, 0xa2, 0x0a, 0xe2, 0x82, 0xac, 0xf0, 0x90, 0x8d, 0x88, 0x0a };
    byte_t BYTES_UTF16_LE_WIN[] = { 0xff, 0xfe, 0x24, 0x00, 0xa2, 0x00, 0x0d, 0x00, 0x0a, 0x00, 0xac, 0x20, 0x00, 0xd8, 0x48, 0xdf, 0x0d, 0x00, 0x0a, 0x00 };
    byte_t BYTES_UTF16_LE_UNIX[] = { 0xff, 0xfe, 0x24, 0x00, 0xa2, 0x00, 0x0a, 0x00, 0xac, 0x20, 0x00, 0xd8, 0x48, 0xdf, 0x0a, 0x00 };
    byte_t BYTES_UTF16_BE_UNIX[] = { 0xfe, 0xff, 0x00, 0x24, 0x00, 0xa2, 0x00, 0x0a, 0x20, 0xac, 0xd8, 0x00, 0xdf, 0x48, 0x00, 0x0a };

    // File()
    // File(const String& fileName, FileMode openMode = FILE_MODE_OPEN_EXISTING)
    // bool isOpen() const
    // bool open(const String& fileName, FileMode openMode = FILE_MODE_OPEN_EXISTING)
    // void close()
        
    {
        File f;
        ASSERT(!f.isOpen());
        ASSERT(f.open(STR("test.txt")));
        ASSERT(f.isOpen());
        ASSERT_EXCEPTION(Exception, f.open(STR("test.txt")));
        f.close();
        ASSERT(!f.isOpen());
    }
    
    {
        File f(STR("test.txt"));
        ASSERT(f.isOpen());
        ASSERT_EXCEPTION(Exception, f.open(STR("test.txt")));
        f.close();
        ASSERT(!f.isOpen());
    }
    
    ASSERT_EXCEPTION(Exception, File(STR("no_such_file"), FILE_MODE_OPEN_EXISTING));
    
    {
        File f;
        ASSERT(!f.open(STR("no_such_file"), FILE_MODE_OPEN_EXISTING));
    }
    
    // template<typename _Type>
    // void write(const Array<_Type>& data)
    // int64_t size() const
    
    {
        File f;
        ASSERT_EXCEPTION(Exception, f.write(Array<byte_t>()));
        ASSERT_EXCEPTION(Exception, f.size());
    }
    
    {
        File f(STR("test.txt"), FILE_MODE_CREATE);
        Array<byte_t> bytes(sizeof(BYTES_UTF8_UNIX), BYTES_UTF8_UNIX);
        f.write(bytes);
        ASSERT(f.size() == bytes.size());
    }
    
    // template<typename _Type>
    // Array<_Type> read()

    {
        File f;
        ASSERT_EXCEPTION(Exception, f.read<byte_t>());
    }
    
    {
        File f(STR("test.txt"));
        ByteArray bytes = f.read<byte_t>();
        ASSERT(memcmp(bytes.values(), BYTES_UTF8_UNIX, sizeof(BYTES_UTF8_UNIX)) == 0);
    }        
        
    // String readString(TextEncoding& encoding, bool& bom, bool& crLf)
    
    {
        File f(STR("test.txt"), FILE_MODE_CREATE);
        f.write(Array<byte_t>(sizeof(BYTES_UTF8_UNIX), BYTES_UTF8_UNIX));
        
        TextEncoding encoding;
        bool bom, crLf;
        String s = f.readString(encoding, bom, crLf);
        ASSERT(s == str);
        ASSERT(encoding == TEXT_ENCODING_UTF8);
        ASSERT(!bom);
        ASSERT(!crLf);
        
    }
    
    {
        File f(STR("test.txt"), FILE_MODE_CREATE);
        f.write(Array<byte_t>(sizeof(BYTES_UTF8_BOM_UNIX), BYTES_UTF8_BOM_UNIX));
        
        TextEncoding encoding;
        bool bom, crLf;
        String s = f.readString(encoding, bom, crLf);
        ASSERT(s == str);
        ASSERT(encoding == TEXT_ENCODING_UTF8);
        ASSERT(bom);
        ASSERT(!crLf);
        
    }
    
    {
        File f(STR("test.txt"), FILE_MODE_CREATE);
        f.write(Array<byte_t>(sizeof(BYTES_UTF16_LE_WIN), BYTES_UTF16_LE_WIN));
        
        TextEncoding encoding;
        bool bom, crLf;
        String s = f.readString(encoding, bom, crLf);
        ASSERT(s == str);
        ASSERT(encoding == TEXT_ENCODING_UTF16_LE);
        ASSERT(bom);
        ASSERT(crLf);        
    }
    
    {
        File f(STR("test.txt"), FILE_MODE_CREATE);
        f.write(Array<byte_t>(sizeof(BYTES_UTF16_LE_UNIX), BYTES_UTF16_LE_UNIX));
        
        TextEncoding encoding;
        bool bom, crLf;
        String s = f.readString(encoding, bom, crLf);
        ASSERT(s == str);
        ASSERT(encoding == TEXT_ENCODING_UTF16_LE);
        ASSERT(bom);
        ASSERT(!crLf);        
    }
    
    {
        File f(STR("test.txt"), FILE_MODE_CREATE);
        f.write(Array<byte_t>(sizeof(BYTES_UTF16_BE_UNIX), BYTES_UTF16_BE_UNIX));
        
        TextEncoding encoding;
        bool bom, crLf;
        String s = f.readString(encoding, bom, crLf);
        ASSERT(s == str);
        ASSERT(encoding == TEXT_ENCODING_UTF16_BE);
        ASSERT(bom);
        ASSERT(!crLf);        
    }
    
    // void writeString(const String& str, TextEncoding encoding, bool bom, bool crLf)
    
    {
        File f(STR("test.txt"), FILE_MODE_CREATE);
        f.writeString(str, TEXT_ENCODING_UTF8, false, false);
        ByteArray bytes = f.read<byte_t>();
        ASSERT(memcmp(bytes.values(), BYTES_UTF8_UNIX, sizeof(BYTES_UTF8_UNIX)) == 0);
    }

    {
        File f(STR("test.txt"), FILE_MODE_CREATE);
        f.writeString(str, TEXT_ENCODING_UTF8, true, false);
        ByteArray bytes = f.read<byte_t>();
        ASSERT(memcmp(bytes.values(), BYTES_UTF8_BOM_UNIX, sizeof(BYTES_UTF8_BOM_UNIX)) == 0);
    }

    {
        File f(STR("test.txt"), FILE_MODE_CREATE);
        f.writeString(str, TEXT_ENCODING_UTF16_LE, true, true);
        ByteArray bytes = f.read<byte_t>();
        ASSERT(memcmp(bytes.values(), BYTES_UTF16_LE_WIN, sizeof(BYTES_UTF16_LE_WIN)) == 0);
    }

    {
        File f(STR("test.txt"), FILE_MODE_CREATE);
        f.writeString(str, TEXT_ENCODING_UTF16_LE, true, false);
        ByteArray bytes = f.read<byte_t>();
        ASSERT(memcmp(bytes.values(), BYTES_UTF16_LE_UNIX, sizeof(BYTES_UTF16_LE_UNIX)) == 0);
    }
    
    {
        File f(STR("test.txt"), FILE_MODE_CREATE);
        f.writeString(str, TEXT_ENCODING_UTF16_BE, true, false);
        ByteArray bytes = f.read<byte_t>();
        ASSERT(memcmp(bytes.values(), BYTES_UTF16_BE_UNIX, sizeof(BYTES_UTF16_BE_UNIX)) == 0);
    }
}

void testConsole()
{
    Console::clear();

    int width, height;
    Console::getSize(width, height);
    Console::writeFormatted(STR("console size: %dx%d"), width, height);
    Console::setCursorPosition(height - 3, 2);
    Console::write(STR("press ENTER to hide cursor"));
    Console::readChar();
    Console::showCursor(false);
    Console::setCursorPosition(height - 2, 2);
    Console::write(STR("press ENTER to show cursor"));
    Console::readChar();
    Console::showCursor(true);
    Console::setCursorPosition(height - 1, 2);
    Console::write(STR("press ENTER to exit"));
    Console::readChar();
}

void writeFormatted(const char_t* format, ...)
{
    va_list args;

    va_start(args, format);
    Console::writeFormatted(format, args);
    va_end(args);
}

void writeLineFormatted(const char_t* format, ...)
{
    va_list args;

    va_start(args, format);
    Console::writeLineFormatted(format, args);
    va_end(args);
}

void testConsoleWrite()
{
    Console::clear();

    Console::write(String(STR("aaa")));
    Console::write(STR("bbb"));
    Console::write('c', 3);

    Console::writeLine();
    Console::writeLine(String(STR("ddd")));
    Console::writeLine(STR("eee"));
    Console::writeLine('f', 3);

    Console::write(10, 10, String(STR("ggg")));
    Console::write(11, 10, STR("iii"));
    Console::write(12, 10, 'j', 3);

    Console::writeFormatted(STR("%d"), 111);
    writeFormatted(STR("%d"), 222);

    Console::writeLineFormatted(STR("%d"), 333);
    writeLineFormatted(STR("%d"), 444);
}

void testConsoleReadChar()
{
    Console::writeLine(STR("Type characters followed by ENTER (q to exit)"));

    while (true)
    {
        unichar_t ch = Console::readChar();
        Console::write(ch);
        if (ch == 'q')
            break;
    }
}

void testConsoleReadLine()
{
    Console::writeLine(STR("Type characters followed by ENTER (quit to exit)"));

    while (true)
    {
        String line = Console::readLine();
        Console::writeLine(line);
        if (line == STR("quit"))
            break;
    }
}

void testConsoleReadKeys()
{
    Console::writeLine(STR("Press any key or key combination (ESC to exit)"));
    Console::setLineMode(false);

    while (true)
    {
        const Array<Key>& keys = Console::readKeys();

        for (int i = 0; i < keys.size(); ++i)
        {
            Key key = keys[i];

            if (key.ctrl)
                Console::write(STR("ctrl "));
            if (key.alt)
                Console::write(STR("alt "));
            if (key.shift)
                Console::write(STR("shift "));

            switch (key.code)
            {
            case KEY_ESC:
                Console::writeLine(STR("KEY_ESC"));
                Console::setLineMode(true);
                return;
            case KEY_TAB:
                Console::writeLine(STR("KEY_TAB"));
                break;
            case KEY_BACKSPACE:
                Console::writeLine(STR("KEY_BACKSPACE"));
                break;
            case KEY_ENTER:
                Console::writeLine(STR("KEY_ENTER"));
                break;
            case KEY_UP:
                Console::writeLine(STR("KEY_UP"));
                break;
            case KEY_DOWN:
                Console::writeLine(STR("KEY_DOWN"));
                break;
            case KEY_LEFT:
                Console::writeLine(STR("KEY_LEFT"));
                break;
            case KEY_RIGHT:
                Console::writeLine(STR("KEY_RIGHT"));
                break;
            case KEY_INSERT:
                Console::writeLine(STR("KEY_INSERT"));
                break;
            case KEY_DELETE:
                Console::writeLine(STR("KEY_DELETE"));
                break;
            case KEY_HOME:
                Console::writeLine(STR("KEY_HOME"));
                break;
            case KEY_END:
                Console::writeLine(STR("KEY_END"));
                break;
            case KEY_PGUP:
                Console::writeLine(STR("KEY_PGUP"));
                break;
            case KEY_PGDN:
                Console::writeLine(STR("KEY_PGDN"));
                break;
            case KEY_F1:
                Console::writeLine(STR("KEY_F1"));
                break;
            case KEY_F2:
                Console::writeLine(STR("KEY_F2"));
                break;
            case KEY_F3:
                Console::writeLine(STR("KEY_F3"));
                break;
            case KEY_F4:
                Console::writeLine(STR("KEY_F4"));
                break;
            case KEY_F5:
                Console::writeLine(STR("KEY_F5"));
                break;
            case KEY_F6:
                Console::writeLine(STR("KEY_F6"));
                break;
            case KEY_F7:
                Console::writeLine(STR("KEY_F7"));
                break;
            case KEY_F8:
                Console::writeLine(STR("KEY_F8"));
                break;
            case KEY_F9:
                Console::writeLine(STR("KEY_F9"));
                break;
            case KEY_F10:
                Console::writeLine(STR("KEY_F10"));
                break;
            case KEY_F11:
                Console::writeLine(STR("KEY_F11"));
                break;
            case KEY_F12:
                Console::writeLine(STR("KEY_F12"));
                break;
            default:
                if (charIsPrint(key.ch))
                    Console::writeLine(key.ch);
                else
                    Console::writeLineFormatted(STR("\\x%02x"), 
                        static_cast<unsigned>(key.ch));
                break;
            }
        }
    }
}

#ifndef PLATFORM_WINDOWS

void testKeys()
{
    char chars[10];

    pollfd pfd;
    pfd.fd = STDIN_FILENO;
    pfd.events = POLLIN;
    pfd.revents = 0;

    Console::setLineMode(false);

    while (true)
    {
        if (poll(&pfd, 1, -1) > 0)
        {
            int len;
            ioctl(STDIN_FILENO, FIONREAD, &len);

            read(STDIN_FILENO, chars, len);

            for (int i = 0; i < len; ++i)
                printf("%x ", static_cast<uint8_t>(chars[i]));

            for (int i = 0; i < len; ++i)
                if (isprint(chars[i]))
                    putchar(chars[i]);
                else
                    printf("\\x%x", static_cast<uint8_t>(chars[i]));

            printf("\n");
        }
    }

    Console::setLineMode(true);
}

#endif

int MAIN(int argc, const char_t** argv)
{
    try
    {
        Console::initialize();
        Console::setLineMode(true);
        
        // testFoundation();
        testFile();
        // testConsole();
        // testConsoleWrite();
        // testConsoleReadChar();
        // testConsoleReadLine();
        // testConsoleReadKeys();
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
