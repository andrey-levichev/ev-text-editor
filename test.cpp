#include <test.h>

int hash(const Unique<int>& val)
{
    return val ? *val : 0;
}

template<typename _Char>
int compareUniString(const _Char* str1, const _Char* str2)
{
    ASSERT(str1);
    ASSERT(str2);

    for (; *str1 == *str2; str1++, str2++)
        if (!*str1)
            return 0;

    return static_cast<unsigned>(*str1) >
        static_cast<unsigned>(*str2) ? 1 : -1;
}

template<typename _SeqType, typename _ElemType>
bool compareArray(const _SeqType& seq, int size, const _ElemType* values)
{
    ASSERT(size >= 0);
    ASSERT(values);

    auto iter = seq.constIterator();
    bool hasNext = iter.moveNext();

    while (hasNext && size > 0)
    {
        if (iter.value() != *values++)
            return false;

        hasNext = iter.moveNext();
        --size;
    }

    return !hasNext && size == 0;
}

template<typename _IterType>
bool compareSequenceIter(_IterType& iter)
{
    return !iter.moveNext();
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
        return false;
}

template<typename _SeqType, typename... _Args>
bool compareSequence(const _SeqType& seq, _Args&&... args)
{
    auto iter = seq.constIterator();
    return compareSequenceIter(iter, static_cast<_Args&&>(args)...);
}

void testSupport()
{
    // compareUniString

    ASSERT(compareUniString("abc", "abc") == 0);
    ASSERT(compareUniString("abc", "abC") > 0);
    ASSERT(compareUniString("abC", "abc") < 0);
    ASSERT(compareUniString("ab", "abc") < 0);
    ASSERT(compareUniString("abc", "ab") > 0);

    // compareArray

    {
        int vals[] = { 1 };
        Array<int> a;
        ASSERT(compareArray(a, 0, vals));
        ASSERT(!compareArray(a, 1, vals));
    }

    {
        int vals[] = { 1, 2 };
        Array<int> a(1, vals);
        ASSERT(!compareArray(a, 0, vals));
        ASSERT(compareArray(a, 1, vals));
        ASSERT(!compareArray(a, 2, vals));
    }

    {
        int vals[] = { 1, 2, 3 };
        Array<int> a(2, vals);
        ASSERT(!compareArray(a, 1, vals));
        ASSERT(compareArray(a, 2, vals));
        ASSERT(!compareArray(a, 3, vals));

        vals[1] = 0;
        ASSERT(!compareArray(a, 2, vals));
    }

    // compareSequence

    {
        Array<int> a;
        ASSERT(compareSequence(a));
        ASSERT(!compareSequence(a, 1));
    }

    {
        Array<int> a;
        a.addLast(1);

        ASSERT(!compareSequence(a));
        ASSERT(compareSequence(a, 1));
        ASSERT(!compareSequence(a, 1, 2));
    }

    {
        Array<int> a;
        a.addLast(1);
        a.addLast(2);

        ASSERT(!compareSequence(a, 1));
        ASSERT(compareSequence(a, 1, 2));
        ASSERT(!compareSequence(a, 1, 0));
        ASSERT(!compareSequence(a, 1, 2, 3));
    }
}

void testSwapBytes()
{
    {
        uint16_t v = 0x1122;
        ASSERT(swapBytes(v) == 0x2211);
    }

    {
        uint32_t v = 0x11223344;
        ASSERT(swapBytes(v) == 0x44332211);
    }

    {
        uint64_t v = 0x1122334455667788ull;
        ASSERT(swapBytes(v) == 0x8877665544332211ull);
    }

    {
        uint16_t v[] = { 0x1122, 0x3344 };
        swapBytes(v, 2);
        ASSERT(v[0] == 0x2211);
        ASSERT(v[1] == 0x4433);
    }

    {
        uint32_t v[] = { 0x11223344, 0x44332211 };
        swapBytes(v, 2);
        ASSERT(v[0] == 0x44332211);
        ASSERT(v[1] == 0x11223344);
    }

    {
        uint64_t v[] = { 0x1122334455667788ull, 0x8877665544332211ull };
        swapBytes(v, 2);
        ASSERT(v[0] == 0x8877665544332211ull);
        ASSERT(v[1] == 0x1122334455667788ull);
    }
}

void testUnique()
{
    // Unique()

    {
        Unique<Test> p;
        ASSERT_EXCEPTION(NullPointerException, *p);
        ASSERT_EXCEPTION(NullPointerException, p->val());
        ASSERT(p.empty());
        ASSERT(!p.ptr());
    }

    // Unique(Unique<_Type>&& other)

    {
        Unique<Test> p1 = createUnique<Test>();
        Test* p = p1.ptr();
        ASSERT(!p1.empty());

        Unique<Test> p2 = static_cast<Unique<Test>&&>(p1);
        ASSERT(p1.empty());
        ASSERT(!p2.empty() && p2.ptr() == p);
    }

    // Unique& operator=(Unique<_Type>&& other)

    {
        Unique<Test> p1 = createUnique<Test>();
        Test* p = p1.ptr();
        ASSERT(!p1.empty());

        Unique<Test> p2;
        p2 = static_cast<Unique<Test>&&>(p1);
        ASSERT(p1.empty());
        ASSERT(!p2.empty() && p2.ptr() == p);
    }

    // _Type& operator*() const

    // _Type* operator->() const

    // operator _Type&() const

    // bool empty() const

    // _Type* ptr() const

    // void create(_Args&&... args)

    {
        Unique<Test> p;
        ASSERT(p.empty());

        p.create(1);
        ASSERT(p.ptr());
        ASSERT((*p).val() == 1);
        ASSERT(p->val() == 1);
        ASSERT(!p.empty());

        Test& t = p;
        ASSERT(t == *p);

        p.create(2);
        ASSERT(p.ptr());
        ASSERT((*p).val() == 2);
        ASSERT(p->val() == 2);
        ASSERT(!p.empty());
    }

    // void reset()

    {
        Unique<Test> p;
        ASSERT(p.empty());
        p.reset();
        ASSERT(p.empty());
    }

    {
        Unique<Test> p;
        ASSERT(p.empty());
        p.create();
        ASSERT(!p.empty());
        p.reset();
        ASSERT(p.empty());
    }

    // bool operator==(const Unique<_Type>& left, const Unique<_Type>& right)

    {
        Unique<Test> p1 = createUnique<Test>(1);
        Unique<Test> p2 = createUnique<Test>(1);
        ASSERT(p1 == p2);
    }

    // bool operator!=(const Unique<_Type>& left, const Unique<_Type>& right)

    {
        Unique<Test> p1 = createUnique<Test>(1);
        Unique<Test> p2 = createUnique<Test>(2);
        ASSERT(p1 != p2);
    }

    // Unique<_T> createUnique(_Args&&... args)

    {
        Unique<Test> p = createUnique<Test>(1);
        ASSERT(!p.empty());
        ASSERT(p.ptr());
        ASSERT((*p).val() == 1);
        ASSERT(p->val() == 1);
    }
}

void testShared()
{
    // Shared()

    {
        Shared<Test> p;
        ASSERT_EXCEPTION(NullPointerException, *p);
        ASSERT_EXCEPTION(NullPointerException, p->val());
        ASSERT(p.empty());
        ASSERT(!p.ptr());
        ASSERT(p.refCount() == 0);
    }

    // Shared(const Shared<_Type>& other)

    {
        Shared<Test> p1 = createShared<Test>();
        Test* p = p1.ptr();
        ASSERT(!p1.empty());

        {
            Shared<Test> p2 = p1;
            ASSERT(!p1.empty() && p1.ptr() == p);
            ASSERT(!p2.empty() && p2.ptr() == p);
            ASSERT(p1.refCount() == 2 && p2.refCount() == 2);
        }

        ASSERT(!p1.empty() && p1.ptr() == p);
        ASSERT(p1.refCount() == 1);
    }

    // Shared(Shared<_Type>&& other)

    {
        Shared<Test> p1 = createShared<Test>();
        Test* p = p1.ptr();
        ASSERT(!p1.empty());

        Shared<Test> p2 = static_cast<Shared<Test>&&>(p1);
        ASSERT(p1.empty());
        ASSERT(!p2.empty() && p2.ptr() == p);
        ASSERT(p1.refCount() == 0 && p2.refCount() == 1);
    }

    // Shared<_Type>& operator=(const Shared<_Type>& other)

    {
        Shared<Test> p1 = createShared<Test>();
        Test* p = p1.ptr();
        ASSERT(!p1.empty());

        {
            Shared<Test> p2;
            p2 = p1;
            ASSERT(!p1.empty() && p1.ptr() == p);
            ASSERT(!p2.empty() && p2.ptr() == p);
            ASSERT(p1.refCount() == 2 && p2.refCount() == 2);
        }

        ASSERT(!p1.empty() && p1.ptr() == p);
        ASSERT(p1.refCount() == 1);
    }

    // Shared<_Type>& operator=(Shared<_Type>&& other)

    {
        Shared<Test> p1 = createShared<Test>();
        Test* p = p1.ptr();
        ASSERT(!p1.empty());

        Shared<Test> p2;
        p2 = static_cast<Shared<Test>&&>(p1);
        ASSERT(p1.empty());
        ASSERT(!p2.empty() && p2.ptr() == p);
        ASSERT(p1.refCount() == 0 && p2.refCount() == 1);
    }

    // _Type& operator*() const

    // _Type* operator->() const

    // operator _Type&() const

    // bool empty() const

    // _Type* ptr() const

    // int refCount() const

    // void create(_Args&&... args)

    {
        Shared<Test> p;
        ASSERT(p.empty());

        p.create(1);
        ASSERT(p.ptr());
        ASSERT((*p).val() == 1);
        ASSERT(p->val() == 1);
        ASSERT(!p.empty());

        Test& t = p;
        ASSERT(t == *p);

        p.create(2);
        ASSERT(p.ptr());
        ASSERT((*p).val() == 2);
        ASSERT(p->val() == 2);
        ASSERT(!p.empty());
    }

    // void reset()

    {
        Shared<Test> p;
        ASSERT(p.empty());
        p.reset();
        ASSERT(p.empty());
    }

    {
        Shared<Test> p;
        ASSERT(p.empty());
        p.create();
        ASSERT(!p.empty());
        p.reset();
        ASSERT(p.empty());
    }

    // bool operator==(const Shared<_Type>& left, const Shared<_Type>& right)

    {
        Shared<Test> p1 = createShared<Test>(1);
        Shared<Test> p2 = p1;
        ASSERT(p1 == p2);
    }

    {
        Shared<Test> p1 = createShared<Test>(1);
        Shared<Test> p2 = createShared<Test>(1);
        ASSERT(p1 == p2);
    }

    // bool operator!=(const Shared<_Type>& left, const Shared<_Type>& right)

    {
        Shared<Test> p1 = createShared<Test>(1);
        Shared<Test> p2 = createShared<Test>(2);
        ASSERT(p1 != p2);
    }

    // Shared<_T> createShared(_Args&&... args)

    {
        Shared<Test> p = createShared<Test>(1);
        ASSERT(!p.empty());
        ASSERT(p.ptr());
        ASSERT((*p).val() == 1);
        ASSERT(p->val() == 1);
        ASSERT(p.refCount() == 1);
    }
}

void testBuffer()
{
    int z = 0;
    int elem[] = { 1, 2, 3 };
    const int* np = NULL;
    const int* ep = elem;

    // Buffer()

    {
        Buffer<int> b;
        ASSERT(b.size() == 0);
        ASSERT(!b.values());
    }

    // Buffer(int size)

    ASSERT_EXCEPTION(Exception, Buffer<int>(-1));

    {
        Buffer<int> b(0);
        ASSERT(b.size() == 0);
        ASSERT(!b.values());
    }

    {
        Buffer<int> b(3);
        ASSERT(b.size() == 3);
        ASSERT(b.values());
    }

    // Buffer(int size, const _Type& value)

    ASSERT_EXCEPTION(Exception, Buffer<int>(-1, z));

    {
        Buffer<int> b(0, z);
        ASSERT(b.size() == 0);
        ASSERT(!b.values());
    }

    {
        Buffer<int> b(3, 123);
        ASSERT(b.size() == 3);
        ASSERT(b.values());
        ASSERT(b[0] == 123 && b[1] == 123 && b[2] == 123);
    }

    // Buffer(int size, const _Type* values)

    ASSERT_EXCEPTION(Exception, Buffer<int>(-1, np));
    ASSERT_EXCEPTION(Exception, Buffer<int>(1, np));

    {
        Buffer<int> b(0, np);
        ASSERT(b.size() == 0);
        ASSERT(!b.values());
    }

    {
        Buffer<int> b(3, ep);
        ASSERT(b.size() == 3);
        ASSERT(b.values());
        ASSERT(b[0] == 1 && b[1] == 2 && b[2] == 3);
    }

    // Buffer(const Buffer<_Type>& other)

    {
        Buffer<int> b1, b2(b1);
        ASSERT(b2.size() == 0);
        ASSERT(!b2.values());
    }

    {
        Buffer<int> b1(3, ep), b2(b1);
        ASSERT(b2.size() == 3);
        ASSERT(b2.values());
        ASSERT(b2[0] == 1 && b2[1] == 2 && b2[2] == 3);
    }

    // Buffer(Buffer<_Type>&& other)

    {
        Buffer<int> b1(3, ep), b2(static_cast<Buffer<int>&&>(b1));

        ASSERT(b1.size() == 0);
        ASSERT(!b1.values());

        ASSERT(b2.size() == 3);
        ASSERT(b2.values());
        ASSERT(b2[0] == 1 && b2[1] == 2 && b2[2] == 3);
    }

    // Buffer<_Type>& operator=(const Buffer<_Type>& other)

    {
        Buffer<int> b1(3, ep), b2;
        b2 = b1;
        ASSERT(b2.size() == 3);
        ASSERT(b2.values());
        ASSERT(b2[0] == 1 && b2[1] == 2 && b2[2] == 3);
    }

    // Buffer<_Type>& operator=(Buffer<_Type>&& other)

    {
        Buffer<int> b1(3, ep), b2;
        b2 = static_cast<Buffer<int>&&>(b1);

        ASSERT(b1.size() == 0);
        ASSERT(!b1.values());

        ASSERT(b2.size() == 3);
        ASSERT(b2.values());
        ASSERT(b2[0] == 1 && b2[1] == 2 && b2[2] == 3);
    }

    // _Type& operator[](int index)

    {
        Buffer<int> b(3, ep);
        ASSERT_EXCEPTION(Exception, b[-1]);
        ASSERT_EXCEPTION(Exception, b[3]);
        ASSERT(b[0] == 1 && b[1] == 2 && b[2] == 3);
    }

    // const _Type& operator[](int index) const

    {
        Buffer<int> b(3, ep);
        const Buffer<int>& cb = b;
        ASSERT_EXCEPTION(Exception, cb[-1]);
        ASSERT_EXCEPTION(Exception, cb[3]);
        ASSERT(cb[0] == 1 && cb[1] == 2 && cb[2] == 3);
    }

    // _Type& value(int index)

    {
        Buffer<int> b(3, ep);
        ASSERT_EXCEPTION(Exception, b.value(-1));
        ASSERT_EXCEPTION(Exception, b.value(3));
        ASSERT(b.value(0) == 1 && b.value(1) == 2 && b.value(2) == 3);
    }

    // const _Type& operator[](int index) const

    {
        Buffer<int> b(3, ep);
        const Buffer<int>& cb = b;
        ASSERT_EXCEPTION(Exception, cb.value(-1));
        ASSERT_EXCEPTION(Exception, cb.value(3));
        ASSERT(cb.value(0) == 1 && cb.value(1) == 2 && cb.value(2) == 3);
    }

    // int size() const
    // bool empty() const
    // _Type* values()
    // const _Type* values() const

    {
        Buffer<int> b(0, np);
        const Buffer<int>& cb = b;
        ASSERT(b.size() == 0);
        ASSERT(b.empty());
        ASSERT(!b.values());
        ASSERT(!cb.values());
    }

    {
        Buffer<int> b(3, ep);
        const Buffer<int>& cb = b;
        ASSERT(b.size() == 3);
        ASSERT(!b.empty());
        ASSERT(b.values());
        ASSERT(cb.values());
    }

    // void resize(int size)

    ASSERT_EXCEPTION(Exception, Buffer<int>().resize(-1));

    {
        Buffer<int> b;
        b.resize(3);
        ASSERT(b.size() == 3);
        ASSERT(b.values());
    }

    {
        Buffer<int> b(3, 0);
        b.resize(0);
        ASSERT(b.size() == 0);
        ASSERT(!b.values());
    }

    // void assign(const Buffer<_Type>& other)

    {
        Buffer<int> b1(3, ep), b2;
        b2.assign(b1);
        ASSERT(b2.size() == 3);
        ASSERT(b2.values());
        ASSERT(b2[0] == 1 && b2[1] == 2 && b2[2] == 3);
    }

    // void assign(int size, const _Type& value)

    ASSERT_EXCEPTION(Exception, Buffer<int>().assign(-1, z));

    {
        Buffer<int> b;
        b.assign(0, z);
        ASSERT(b.size() == 0);
        ASSERT(!b.values());
    }

    {
        Buffer<int> b;
        b.assign(3, 123);
        ASSERT(b.size() == 3);
        ASSERT(b.values());
        ASSERT(b[0] == 123 && b[1] == 123 && b[2] == 123);
    }

    // void assign(int size, const _Type* values)

    ASSERT_EXCEPTION(Exception, Buffer<int>().assign(-1, np));
    ASSERT_EXCEPTION(Exception, Buffer<int>().assign(1, np));

    {
        Buffer<int> b;
        b.assign(0, np);
        ASSERT(b.size() == 0);
        ASSERT(!b.values());
    }

    {
        Buffer<int> b;
        b.assign(3, ep);
        ASSERT(b.size() == 3);
        ASSERT(b.values());
        ASSERT(b[0] == 1 && b[1] == 2 && b[2] == 3);
    }

    // void append(const Buffer<_Type>& other)

    {
        Buffer<int> b1(3, ep), b2(1, 123);
        b2.append(b1);
        ASSERT(b2.size() == 4);
        ASSERT(b2[0] == 123 && b2[1] == 1 && b2[2] == 2 && b2[3] == 3);
    }

    // void append(int size, const _Type& value)

    ASSERT_EXCEPTION(Exception, Buffer<int>().append(-1, z));

    {
        Buffer<int> b(1, 123);
        b.append(0, z);
        ASSERT(b.size() == 1);
        ASSERT(b[0] == 123);
    }

    {
        Buffer<int> b(1, 123);
        b.append(3, 456);
        ASSERT(b.size() == 4);
        ASSERT(b[0] == 123 && b[1] == 456 && b[2] == 456 && b[3] == 456);
    }

    // void append(int size, const _Type* values)

    ASSERT_EXCEPTION(Exception, Buffer<int>().append(-1, np));
    ASSERT_EXCEPTION(Exception, Buffer<int>().append(1, np));

    {
        Buffer<int> b(1, 123);
        b.append(0, np);
        ASSERT(b.size() == 1);
        ASSERT(b[0] == 123);
    }

    {
        Buffer<int> b(1, 123);
        b.append(3, ep);
        ASSERT(b.size() == 4);
        ASSERT(b[0] == 123 && b[1] == 1 && b[2] == 2 && b[3] == 3);
    }

    // void clear()

    {
        Buffer<int> b;
        b.clear();
        ASSERT(b.size() == 0);
        ASSERT(!b.values());
    }

    {
        Buffer<int> b(3, ep);
        b.clear();
        ASSERT(b.size() == 3);
        ASSERT(b.values());
        ASSERT(b[0] == 0 && b[1] == 0 && b[2] == 0);
    }

    // void reset()

    {
        Buffer<int> b;
        b.reset();
        ASSERT(b.size() == 0);
        ASSERT(!b.values());
    }

    {
        Buffer<int> b(3, ep);
        b.reset();
        ASSERT(b.size() == 0);
        ASSERT(!b.values());
    }

    // Buffer<_Type> acquire(int size, _Type* values)
    // _Type* release()

    {
        int v;
        ASSERT_EXCEPTION(Exception, Buffer<int>::acquire(-1, &v));
        ASSERT_EXCEPTION(Exception, Buffer<int>::acquire(1, NULL));
    }

    {
        Buffer<int> b1;
        int size = b1.size();
        int* values = b1.release();
        ASSERT(b1.size() == 0);
        ASSERT(!b1.values());

        Buffer<int> b2;
        b2.acquire(size, values);
        ASSERT(b2.size() == 0);
        ASSERT(!b2.values());
    }

    {
        Buffer<int> b1(3, ep);
        int size = b1.size();
        int* values = b1.release();
        ASSERT(b1.size() == 0);
        ASSERT(!b1.values());

        Buffer<int> b2 = b2.acquire(size, values);
        ASSERT(b2.size() == 3);
        ASSERT(b2.values());
        ASSERT(b2[0] == 1 && b2[1] == 2 && b2[2] == 3);
    }
}

void testString()
{
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

    unichar_t zc = 0;
    const char_t* np = NULL;

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
        char32_t ch;
        ASSERT(utf16CharToUnicodeSwapBytes(u"\x2400", ch) == 1 && ch == 0x24);
        ASSERT(utf16CharToUnicodeSwapBytes(u"\xa200", ch) == 1 && ch == 0xa2);
        ASSERT(utf16CharToUnicodeSwapBytes(u"\xac20", ch) == 1 && ch == 0x20ac);
        ASSERT(utf16CharToUnicodeSwapBytes(u"\x00d8\x48df", ch) == 2 && ch == 0x10348);
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
        pos = utf8CharBack(pos);
        ASSERT(utf8CharAt(pos) == 0x10348);
        pos = utf8CharBack(pos);
        ASSERT(utf8CharAt(pos) == 0x20ac);
        pos = utf8CharBack(pos);
        ASSERT(utf8CharAt(pos) == 0xa2);
        pos = utf8CharBack(pos);
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
        pos = utf16CharBack(pos);
        ASSERT(utf16CharAt(pos) == 0x10348);
        pos = utf16CharBack(pos);
        ASSERT(utf16CharAt(pos) == 0x20ac);
        pos = utf16CharBack(pos);
        ASSERT(utf16CharAt(pos) == 0xa2);
        pos = utf16CharBack(pos);
        ASSERT(utf16CharAt(pos) == 0x24);
    }

    ASSERT(utf16CharLength(0x24) == 1);
    ASSERT(utf16CharLength(0xa2) == 1);
    ASSERT(utf16CharLength(0x20ac) == 1);
    ASSERT(utf16CharLength(0x10348) == 2);
    ASSERT(utf16StringLength(CHARS16) == 4);

    {
        unichar_t ch = CHAR('q');
        ASSERT(ch == 0x71);
    }

#if !defined(PLATFORM_SOLARIS) && !defined(PLATFORM_AIX)
    {
        unichar_t ch = CHAR('й');
        ASSERT(ch == 0x439);
    }
#endif

    ASSERT(charIsAlphaNum(CHAR('q')));
    ASSERT(charToUpper(CHAR('q')) == CHAR('Q'));
    ASSERT(strCompareNoCase(STR("qwerty"), STR("QWERTY")) == 0);
    ASSERT(strFindNoCase(STR("qwerty"), STR("WER")) != NULL);

#if defined(PLATFORM_WINDOWS) && defined(PLATFORM_APPLE)
    ASSERT(charIsAlphaNum(CHAR('й')));
    ASSERT(charToUpper(CHAR('й')) == CHAR('Й'));
    ASSERT(strCompareNoCase(STR("йцукенг"), STR("ЙЦУКЕНГ")) == 0);
    ASSERT(strFindNoCase(STR("йцукенг"), STR("ЦУК")) != NULL);
#endif

    // String()

    {
        String s;
        ASSERT(s == STR(""));
        ASSERT(s.length() == 0);
        ASSERT(s.capacity() == 0);
    }

    // String(const String& other)

    {
        String s1, s2(s1);
        ASSERT(s2 == STR(""));
        ASSERT(s2.length() == 0);
        ASSERT(s2.capacity() == 0);
    }

    {
        String s1(STR("a")), s2(s1);
        ASSERT(s2 == STR("a"));
        ASSERT(s2.length() == 1);
        ASSERT(s2.capacity() == 2);
    }

    // String(const char_t* chars, int len = -1)

    ASSERT_EXCEPTION(Exception, String(np, 1));

    {
        String s(np);
        ASSERT(s == STR(""));
        ASSERT(s.length() == 0);
        ASSERT(s.capacity() == 0);
    }

    {
        String s(np, 0);
        ASSERT(s == STR(""));
        ASSERT(s.length() == 0);
        ASSERT(s.capacity() == 0);
    }

    {
        String s(STR(""));
        ASSERT(s == STR(""));
        ASSERT(s.length() == 0);
        ASSERT(s.capacity() == 0);
    }

    {
        String s(STR(""), 0);
        ASSERT(s == STR(""));
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

    // String(unichar_t ch, int n = 1)

    ASSERT_EXCEPTION(Exception, String(zc, 1));
    ASSERT_EXCEPTION(Exception, String('a', -1));

    {
        String s('a', 0);
        ASSERT(s == STR(""));
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

        ASSERT(s1 == STR(""));
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

        ASSERT(s1 == STR(""));
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
        ASSERT(strCompare(s.chars(), STR("")) == 0);
        ASSERT(s == STR(""));
        ASSERT(s.empty());
    }

    // unichar_t charAt(int pos) const
    // int charForward(int pos, int n = 1) const
    // int charBack(int pos, int n = 1) const

    {
        String s;
        ASSERT(s.charAt(0) == 0);
        ASSERT(s.charForward(0, 0) == 0);
        ASSERT(s.charForward(0, 1) == 0);
        ASSERT(s.charBack(0, 0) == 0);
        ASSERT(s.charBack(0, 1) == 0);
    }

    {
        String s;
        s.ensureCapacity(10);
        ASSERT(s == STR(""));

        ASSERT(s.charAt(0) == 0);
        ASSERT(s.charForward(0, 0) == 0);
        ASSERT(s.charForward(0, 1) == 0);
        ASSERT(s.charBack(0, 0) == 0);
        ASSERT(s.charBack(0, 1) == 0);
    }

    {
        String s(STR("a"));

        ASSERT(s.charAt(0) == 'a');
        ASSERT(s.charAt(1) == 0);

        ASSERT(s.charForward(0, 0) == 0);
        ASSERT(s.charForward(0, 1) == 1);
        ASSERT(s.charForward(0, 2) == 1);
        ASSERT(s.charForward(1, 0) == 1);
        ASSERT(s.charForward(1, 1) == 1);

        ASSERT(s.charBack(0, 0) == 0);
        ASSERT(s.charBack(0, 1) == 0);
        ASSERT(s.charBack(1, 0) == 1);
        ASSERT(s.charBack(1, 1) == 0);
        ASSERT(s.charBack(1, 2) == 0);
    }

    {
        String s(CHARS);

        ASSERT(s.charForward(1, 0) == 1);
        ASSERT(s.charBack(1, 0) == 1);

#ifdef CHAR_ENCODING_UTF16
        ASSERT(s.charForward(0, 3) == 3);
        ASSERT(s.charForward(0, 4) == 5);
        ASSERT(s.charForward(0, 5) == 5);
        ASSERT(s.charForward(0, 10) == 5);
#else
        ASSERT(s.charForward(0, 3) == 6);
        ASSERT(s.charForward(0, 4) == 10);
        ASSERT(s.charForward(0, 5) == 10);
        ASSERT(s.charForward(0, 10) == 10);
#endif
        ASSERT(s.charBack(s.length(), 3) == 1);
        ASSERT(s.charBack(s.length(), 4) == 0);
        ASSERT(s.charBack(s.length(), 5) == 0);
        ASSERT(s.charBack(s.length(), 10) == 0);
    }

    {
        String s(CHARS);

        ASSERT_EXCEPTION(Exception, s.charAt(-1));
        ASSERT_EXCEPTION(Exception, s.charAt(s.length() + 1));
        ASSERT_EXCEPTION(Exception, s.charForward(-1));
        ASSERT_EXCEPTION(Exception, s.charForward(s.length() + 1));
        ASSERT_EXCEPTION(Exception, s.charBack(-1));
        ASSERT_EXCEPTION(Exception, s.charBack(s.length() + 1));

        int pos = 0;
        ASSERT(s.charAt(pos) == 0x24);
        pos = s.charForward(pos);
        ASSERT(pos == 1);
        ASSERT(s.charAt(pos) == 0xa2);
        pos = s.charForward(pos);
#ifdef CHAR_ENCODING_UTF16
        ASSERT(pos == 2);
#else
        ASSERT(pos == 3);
#endif
        ASSERT(s.charAt(pos) == 0x20ac);
        pos = s.charForward(pos);
#ifdef CHAR_ENCODING_UTF16
        ASSERT(pos == 3);
#else
        ASSERT(pos == 6);
#endif
        ASSERT(s.charAt(pos) == 0x10348);
        pos = s.charForward(pos);
#ifdef CHAR_ENCODING_UTF16
        ASSERT(pos == 5);
#else
        ASSERT(pos == 10);
#endif
        pos = s.charForward(pos);
#ifdef CHAR_ENCODING_UTF16
        ASSERT(pos == 5);
#else
        ASSERT(pos == 10);
#endif

        pos = s.length();
        pos = s.charBack(pos);
#ifdef CHAR_ENCODING_UTF16
        ASSERT(pos == 3);
#else
        ASSERT(pos == 6);
#endif
        ASSERT(s.charAt(pos) == 0x10348);
        pos = s.charBack(pos);
#ifdef CHAR_ENCODING_UTF16
        ASSERT(pos == 2);
#else
        ASSERT(pos == 3);
#endif
        ASSERT(s.charAt(pos) == 0x20ac);
        pos = s.charBack(pos);
        ASSERT(pos == 1);
        ASSERT(s.charAt(pos) == 0xa2);
        pos = s.charBack(pos);
        ASSERT(pos == 0);
        ASSERT(s.charAt(pos) == 0x24);
        pos = s.charBack(pos);
        ASSERT(pos == 0);
    }

    // String substr(int pos, int len = -1) const

    {
        String s;
        ASSERT(s.substr(0) == STR(""));
        ASSERT(s.substr(0, 0) == STR(""));
        ASSERT_EXCEPTION(Exception, s.substr(0, 1));
    }

    {
        String s(STR("abcd"));
        ASSERT_EXCEPTION(Exception, s.substr(-1));
        ASSERT_EXCEPTION(Exception, s.substr(s.length() + 1));
        ASSERT_EXCEPTION(Exception, s.substr(1, 4));
        ASSERT(s.substr(1, 2) == STR("bc"));
        ASSERT(s.substr(1) == STR("bcd"));
    }

    // int compare(const String& str, bool caseSensitive = true) const

    ASSERT(String(STR("a")).compare(String()) > 0);
    ASSERT(String().compare(String(STR("a"))) < 0);
    ASSERT(String().compare(String()) == 0);
    ASSERT(String(STR("ab")).compare(String(STR("a"))) > 0);
    ASSERT(String(STR("a")).compare(String(STR("ab"))) < 0);
    ASSERT(String(STR("a")).compare(String(STR("a"))) == 0);

    ASSERT(String(STR("a")).compare(String(), false) > 0);
    ASSERT(String().compare(String(STR("a")), false) < 0);
    ASSERT(String().compare(String(), false) == 0);
    ASSERT(String(STR("AB")).compare(String(STR("a")), false) > 0);
    ASSERT(String(STR("A")).compare(String(STR("ab")), false) < 0);
    ASSERT(String(STR("A")).compare(String(STR("a")), false) == 0);

    // int compare(const char_t* chars, bool caseSensitive = true) const

    ASSERT(String(STR("a")).compare(NULL) > 0);
    ASSERT(String().compare(NULL) == 0);
    ASSERT(String(STR("ab")).compare(STR("a")) > 0);
    ASSERT(String(STR("a")).compare(STR("ab")) < 0);
    ASSERT(String(STR("a")).compare(STR("a")) == 0);

    ASSERT(String(STR("a")).compare(NULL, false) > 0);
    ASSERT(String().compare(NULL, false) == 0);
    ASSERT(String(STR("AB")).compare(STR("a"), false) > 0);
    ASSERT(String(STR("A")).compare(STR("ab"), false) < 0);
    ASSERT(String(STR("A")).compare(STR("a"), false) == 0);

    // int find(const String& str, bool caseSensitive = true, int pos = 0) const

    {
        String s;
        ASSERT(s.find(String()) == INVALID_POSITION);
        ASSERT(s.find(String(STR("a"))) == INVALID_POSITION);
    }

    {
        String s(STR("abcdabcd"));
        ASSERT(s.find(String(STR("bc"))) == 1);
        ASSERT(s.find(String(STR("bc")), true, 2) == 5);
        ASSERT(s.find(String(STR("bc")), true, 6) == INVALID_POSITION);
        ASSERT(s.find(String(STR("xy"))) == INVALID_POSITION);

        ASSERT_EXCEPTION(Exception, s.find(String(), true, -1));
        ASSERT_EXCEPTION(Exception, s.find(String(), true, s.length() + 1));
        ASSERT(s.find(String()) == INVALID_POSITION);
    }

    {
        String s;
        ASSERT(s.find(String(), false) == INVALID_POSITION);
        ASSERT(s.find(String(STR("a"), false)) == INVALID_POSITION);
    }

    {
        String s(STR("ABCDABCD"));
        ASSERT(s.find(String(STR("bc")), false) == 1);
        ASSERT(s.find(String(STR("bc")), false, 2) == 5);
        ASSERT(s.find(String(STR("bc")), false, 6) == INVALID_POSITION);
        ASSERT(s.find(String(STR("xy")), false) == INVALID_POSITION);

        ASSERT_EXCEPTION(Exception, s.find(String(), false, -1));
        ASSERT_EXCEPTION(Exception, s.find(String(), false, s.length() + 1));
        ASSERT(s.find(String(), false) == INVALID_POSITION);
    }

    // int find(const char_t* chars, bool caseSensitive = true, int pos = 0) const

    {
        String s;
        ASSERT(s.find(np) == INVALID_POSITION);
        ASSERT(s.find(STR("")) == INVALID_POSITION);
        ASSERT(s.find(STR("a")) == INVALID_POSITION);
    }

    {
        String s(STR("abcdabcd"));
        ASSERT(s.find(STR("bc")) == 1);
        ASSERT(s.find(STR("bc"), true, 2) == 5);
        ASSERT(s.find(STR("bc"), true, 6) == INVALID_POSITION);
        ASSERT(s.find(STR("xy")) == INVALID_POSITION);

        ASSERT_EXCEPTION(Exception, s.find(STR(""), true, -1));
        ASSERT_EXCEPTION(Exception, s.find(STR(""), true, s.length() + 1));
        ASSERT(s.find(np) == INVALID_POSITION);
        ASSERT(s.find(STR("")) == INVALID_POSITION);
    }

    {
        String s;
        ASSERT(s.find(np, false) == INVALID_POSITION);
        ASSERT(s.find(STR(""), false) == INVALID_POSITION);
        ASSERT(s.find(STR("a"), false) == INVALID_POSITION);
    }

    {
        String s(STR("ABCDABCD"));
        ASSERT(s.find(STR("bc"), false) == 1);
        ASSERT(s.find(STR("bc"), false, 2) == 5);
        ASSERT(s.find(STR("bc"), false, 6) == INVALID_POSITION);
        ASSERT(s.find(STR("xy"), false) == INVALID_POSITION);

        ASSERT_EXCEPTION(Exception, s.find(STR(""), false, -1));
        ASSERT_EXCEPTION(Exception, s.find(STR(""), false, s.length() + 1));
        ASSERT(s.find(np, false) == INVALID_POSITION);
        ASSERT(s.find(STR(""), false) == INVALID_POSITION);
    }

    // int find(unichar_t ch, bool caseSensitive = true, int pos = 0) const

    {
        String s;
        ASSERT(s.find('a') == INVALID_POSITION);
    }

    {
        String s(STR("abcabc"));
        ASSERT(s.find('b') == 1);
        ASSERT(s.find('b', true, 2) == 4);
        ASSERT(s.find('b', true, 5) == INVALID_POSITION);
        ASSERT(s.find('x') == INVALID_POSITION);

        ASSERT_EXCEPTION(Exception, s.find(zc));
        ASSERT_EXCEPTION(Exception, s.find('a', true, -1));
        ASSERT_EXCEPTION(Exception, s.find('a', true, s.length() + 1));
    }

    {
        String s;
        ASSERT(s.find('a', false) == INVALID_POSITION);
    }

    {
        String s(STR("ABCABC"));
        ASSERT(s.find('b', false) == 1);
        ASSERT(s.find('b', false, 2) == 4);
        ASSERT(s.find('b', false, 5) == INVALID_POSITION);
        ASSERT(s.find('x', false) == INVALID_POSITION);

        ASSERT_EXCEPTION(Exception, s.find(zc, false));
        ASSERT_EXCEPTION(Exception, s.find('a', false, -1));
        ASSERT_EXCEPTION(Exception, s.find('a', false, s.length() + 1));
    }

    // bool startsWith(const String& str, bool caseSensitive = true) const

    ASSERT(String().startsWith(String()) == false);
    ASSERT(String().startsWith(String(STR("a"))) == false);
    ASSERT(String(STR("a")).startsWith(String()) == false);
    ASSERT(String(STR("a")).startsWith(String(STR("a"))) == true);
    ASSERT(String(STR("ab")).startsWith(String(STR("a"))) == true);
    ASSERT(String(STR("a")).startsWith(String(STR("ab"))) == false);
    ASSERT(String(STR("A")).startsWith(String(STR("a")), false) == true);
    ASSERT(String(STR("AB")).startsWith(String(STR("a")), false) == true);
    ASSERT(String(STR("A")).startsWith(String(STR("ab")), false) == false);

    // bool startsWith(const char_t* chars, bool caseSensitive = true) const

    ASSERT(String().startsWith(STR("")) == false);
    ASSERT(String().startsWith(STR("a")) == false);
    ASSERT(String(STR("a")).startsWith(STR("")) == false);
    ASSERT(String(STR("a")).startsWith(STR("a")) == true);
    ASSERT(String(STR("ab")).startsWith(STR("a")) == true);
    ASSERT(String(STR("a")).startsWith(STR("ab")) == false);
    ASSERT(String(STR("A")).startsWith(STR("a"), false) == true);
    ASSERT(String(STR("AB")).startsWith(STR("a"), false) == true);
    ASSERT(String(STR("A")).startsWith(STR("ab"), false) == false);

    // bool endsWith(const String& str, bool caseSensitive = true) const

    ASSERT(String().endsWith(String()) == false);
    ASSERT(String().endsWith(String(STR("a"))) == false);
    ASSERT(String(STR("a")).endsWith(String()) == false);
    ASSERT(String(STR("a")).endsWith(String(STR("a"))) == true);
    ASSERT(String(STR("ab")).endsWith(String(STR("b"))) == true);
    ASSERT(String(STR("a")).endsWith(String(STR("ab"))) == false);
    ASSERT(String(STR("A")).endsWith(String(STR("a")), false) == true);
    ASSERT(String(STR("AB")).endsWith(String(STR("b")), false) == true);
    ASSERT(String(STR("A")).endsWith(String(STR("ab")), false) == false);

    // bool endsWith(const char_t* chars, bool caseSensitive = true) const

    ASSERT(String().endsWith(STR("")) == false);
    ASSERT(String().endsWith(STR("a")) == false);
    ASSERT(String(STR("a")).endsWith(STR("")) == false);
    ASSERT(String(STR("a")).endsWith(STR("a")) == true);
    ASSERT(String(STR("ab")).endsWith(STR("b")) == true);
    ASSERT(String(STR("a")).endsWith(STR("ab")) == false);
    ASSERT(String(STR("A")).endsWith(STR("a"), false) == true);
    ASSERT(String(STR("AB")).endsWith(STR("b"), false) == true);
    ASSERT(String(STR("A")).endsWith(STR("ab"), false) == false);

    // bool contains(const String& str) const

    ASSERT(String(STR("abc")).contains(String(STR("bc"))) == true);
    ASSERT(String(STR("abc")).contains(String(STR("xy"))) == false);
    ASSERT(String(STR("ABC")).contains(String(STR("bc")), false) == true);
    ASSERT(String(STR("ABC")).contains(String(STR("xy")), false) == false);

    // bool contains(const char_t* chars, bool caseSensitive = true) const

    ASSERT(String(STR("abc")).contains(STR("bc")) == true);
    ASSERT(String(STR("abc")).contains(STR("xy")) == false);
    ASSERT(String(STR("ABC")).contains(STR("bc"), false) == true);
    ASSERT(String(STR("ABC")).contains(STR("xy"), false) == false);

    // void ensureCapacity(int capacity, bool caseSensitive = true)

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
        ASSERT(s == STR(""));
        ASSERT(s.length() == 0);
        ASSERT(s.capacity() == 0);
    }

    {
        String s;
        s.ensureCapacity(10);
        s.shrinkToLength();
        ASSERT(s == STR(""));
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

    // void assign(const char_t* chars, int len = -1)

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

    ASSERT_EXCEPTION(Exception, String().assign(STR(""), 1));

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

    {
        String s;
        s.assign(STR("abcde"), 3);
        ASSERT(s == STR("abc"));
        ASSERT(s.capacity() == 4);
    }

    // void assign(unichar_t ch, int n = 1)

    ASSERT_EXCEPTION(Exception, String().assign('a', -1));
    ASSERT_EXCEPTION(Exception, String().assign(zc, 1));

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

    // void append(const char_t* chars, int len = -1)

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

    ASSERT_EXCEPTION(Exception, String().append(STR(""), 1));

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

    {
        String s(STR("a"));
        s.append(STR("bcdef"), 3);
        ASSERT(s == STR("abcd"));
        ASSERT(s.capacity() == 10);
    }

    // void append(unichar_t ch, int n = 1)

    ASSERT_EXCEPTION(Exception, String().append('a', -1));
    ASSERT_EXCEPTION(Exception, String().append(zc, 1));

    {
        String s;
        s.append('a', 1);
        ASSERT(s == STR("a"));
        ASSERT(s.capacity() == 4);
        s.append('b', 2);
        ASSERT(s == STR("abb"));
        ASSERT(s.capacity() == 4);
        s.append('c', 0);
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

    // void insert(int pos, const String& str)

    {
        String s(STR("a"));
        ASSERT_EXCEPTION(Exception, s.insert(-1, String()));
        ASSERT_EXCEPTION(Exception, s.insert(s.length() + 1, String()));
    }

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
        ASSERT(s1.capacity() == 6);
    }

    {
        String s1(STR("a")), s2(STR("b"));
        s1.insert(1, s2);
        ASSERT(s1 == STR("ab"));
        ASSERT(s1.capacity() == 6);
    }

    {
        String s1(STR("ab")), s2(STR("c"));
        s1.insert(1, s2);
        ASSERT(s1 == STR("acb"));
        ASSERT(s1.capacity() == 8);
    }

    {
        String s1, s2(STR("c"));
        s1.ensureCapacity(10);
        s1 = STR("ab");
        s1.insert(1, s2);
        ASSERT(s1 == STR("acb"));
        ASSERT(s1.capacity() == 10);
    }

    // void insert(int pos, const char_t* chars, int len = -1)

    {
        String s(STR("a"));
        ASSERT_EXCEPTION(Exception, s.insert(-1, STR("")));
        ASSERT_EXCEPTION(Exception, s.insert(s.length() + 1, STR("")));
    }

    {
        String s(STR("a"));
        ASSERT_EXCEPTION(Exception, s.insert(0, s.chars()));
    }

    {
        String s(STR("a"));
        s.insert(0, np);
        ASSERT(s == STR("a"));
        ASSERT(s.capacity() == 2);
    }

    {
        String s(STR("a"));
        s.insert(0, STR(""));
        ASSERT(s == STR("a"));
        ASSERT(s.capacity() == 2);
    }

    {
        String s(STR("a"));
        ASSERT_EXCEPTION(Exception, s.insert(0, STR(""), 1));
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
        ASSERT(s.capacity() == 6);
    }

    {
        String s(STR("a"));
        s.insert(1, STR("b"));
        ASSERT(s == STR("ab"));
        ASSERT(s.capacity() == 6);
    }

    {
        String s(STR("ab"));
        s.insert(1, STR("c"));
        ASSERT(s == STR("acb"));
        ASSERT(s.capacity() == 8);
    }

    {
        String s;
        s.ensureCapacity(10);
        s = STR("ab");
        s.insert(1, STR("c"));
        ASSERT(s == STR("acb"));
        ASSERT(s.capacity() == 10);
    }

    {
        String s(STR("ab"));
        s.insert(1, STR("cdefg"), 3);
        ASSERT(s == STR("acdeb"));
        ASSERT(s.capacity() == 12);
    }

    // void insert(int pos, unichar_t ch, int n = 1)

    {
        String s(STR("a"));
        ASSERT_EXCEPTION(Exception, s.insert(-1, 'a'));
        ASSERT_EXCEPTION(Exception, s.insert(s.length() + 1, 'a'));
        ASSERT_EXCEPTION(Exception, s.insert(0, zc, 1));
        ASSERT_EXCEPTION(Exception, s.insert(0, 'a', -1));
    }

    {
        String s(STR("a"));
        s.insert(0, 'b', 0);
        ASSERT(s == STR("a"));
        ASSERT(s.capacity() == 2);
    }

    {
        String s;
        s.insert(0, 'a');
        ASSERT(s == STR("a"));
        ASSERT(s.capacity() == 2);
    }

    {
        String s(STR("a"));
        s.insert(0, 'b');
        ASSERT(s == STR("ba"));
        ASSERT(s.capacity() == 6);
    }

    {
        String s(STR("a"));
        s.insert(1, 'b');
        ASSERT(s == STR("ab"));
        ASSERT(s.capacity() == 6);
    }

    {
        String s(STR("ab"));
        s.insert(1, 'c');
        ASSERT(s == STR("acb"));
        ASSERT(s.capacity() == 8);
    }

    {
        String s(STR("ab"));
        s.insert(1, 'c', 2);
        ASSERT(s == STR("accb"));
        ASSERT(s.capacity() == 10);
    }

    {
        String s;
        s.ensureCapacity(10);
        s = STR("ab");
        s.insert(1, 'c');
        ASSERT(s == STR("acb"));
        ASSERT(s.capacity() == 10);
    }

    // void erase(int pos, int len = -1)

    {
        String s(STR("a"));
        ASSERT_EXCEPTION(Exception, s.erase(-1));
        ASSERT_EXCEPTION(Exception, s.erase(s.length() + 1));
    }

    {
        String s;
        s.erase(0);
        ASSERT(s.length() == 0);
        s.erase(0, 0);
        ASSERT(s.length() == 0);
        ASSERT_EXCEPTION(Exception, s.erase(0, 1));
    }

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

    {
        String s(STR("abc"));
        s.erase(1, 1);
        ASSERT(s == STR("ac"));
        ASSERT(s.capacity() == 4);
    }

    {
        String s(STR("abc"));
        s.erase(1);
        ASSERT(s == STR("a"));
        ASSERT(s.capacity() == 4);
    }

    {
        String s(STR("abc"));
        ASSERT_EXCEPTION(Exception, s.erase(1, 3));
    }

    // void eraseString(const String& str, bool caseSensitive = true)

    {
        String s(STR("abc"));
        s.eraseString(String());
        ASSERT(s == STR("abc"));
        ASSERT(s.capacity() == 4);
    }

    {
        String s;
        s.eraseString(String(STR("abc")));
        ASSERT(s.length() == 0);
        ASSERT(s.capacity() == 0);
    }

    {
        String s(STR("abc"));
        ASSERT_EXCEPTION(Exception, s.eraseString(s));
    }

    {
        String s(STR("abc"));
        s.eraseString(String(STR("x")));
        ASSERT(s == STR("abc"));
        ASSERT(s.capacity() == 4);
    }

    {
        String s(STR("abc"));
        s.eraseString(String(STR("abc")));
        ASSERT(s.length() == 0);
        ASSERT(s.capacity() == 4);
    }

    {
        String s(STR("abc"));
        s.eraseString(String(STR("b")));
        ASSERT(s == STR("ac"));
        ASSERT(s.capacity() == 4);
    }

    {
        String s(STR("abc"));
        s.eraseString(String(STR("a")));
        ASSERT(s == STR("bc"));
        ASSERT(s.capacity() == 4);
    }

    {
        String s(STR("abc"));
        s.eraseString(String(STR("c")));
        ASSERT(s == STR("ab"));
        ASSERT(s.capacity() == 4);
    }

    {
        String s(STR("aab"));
        s.eraseString(String(STR("a")));
        ASSERT(s == STR("b"));
        ASSERT(s.capacity() == 4);
    }

    {
        String s(STR("abcbd"));
        s.eraseString(String(STR("b")));
        ASSERT(s == STR("acd"));
        ASSERT(s.capacity() == 6);
    }

    {
        String s(STR("ABCBD"));
        s.eraseString(String(STR("b")), false);
        ASSERT(s == STR("ACD"));
        ASSERT(s.capacity() == 6);
    }

    // void eraseString(const char_t* chars, bool caseSensitive = true)

    {
        String s(STR("abc"));
        s.eraseString(np);
        ASSERT(s == STR("abc"));
        ASSERT(s.capacity() == 4);
    }

    {
        String s(STR("abc"));
        s.eraseString(STR(""));
        ASSERT(s == STR("abc"));
        ASSERT(s.capacity() == 4);
    }

    {
        String s;
        s.eraseString(STR("abc"));
        ASSERT(s.length() == 0);
        ASSERT(s.capacity() == 0);
    }

    {
        String s(STR("abc"));
        ASSERT_EXCEPTION(Exception, s.eraseString(s.chars()));
    }

    {
        String s(STR("abc"));
        s.eraseString(STR("x"));
        ASSERT(s == STR("abc"));
        ASSERT(s.capacity() == 4);
    }

    {
        String s(STR("abc"));
        s.eraseString(STR("abc"));
        ASSERT(s.length() == 0);
        ASSERT(s.capacity() == 4);
    }

    {
        String s(STR("abc"));
        s.eraseString(STR("b"));
        ASSERT(s == STR("ac"));
        ASSERT(s.capacity() == 4);
    }

    {
        String s(STR("abc"));
        s.eraseString(STR("a"));
        ASSERT(s == STR("bc"));
        ASSERT(s.capacity() == 4);
    }

    {
        String s(STR("abc"));
        s.eraseString(STR("c"));
        ASSERT(s == STR("ab"));
        ASSERT(s.capacity() == 4);
    }

    {
        String s(STR("aab"));
        s.eraseString(STR("a"));
        ASSERT(s == STR("b"));
        ASSERT(s.capacity() == 4);
    }

    {
        String s(STR("ABCBD"));
        s.eraseString(STR("b"), false);
        ASSERT(s == STR("ACD"));
        ASSERT(s.capacity() == 6);
    }

    // void replace(int pos, const String& str, int len = -1)

    {
        String s(STR("a"));
        ASSERT_EXCEPTION(Exception, s.replace(-1, String()));
        ASSERT_EXCEPTION(Exception, s.replace(s.length() + 1, String()));
    }

    {
        String s(STR("abc"));
        ASSERT_EXCEPTION(Exception, s.replace(0, s));
    }

    {
        String s;
        s.replace(0, String(STR("a")));
        ASSERT(s == STR("a"));
        ASSERT(s.length() == 1);
        ASSERT(s.capacity() == 2);
    }

    {
        String s;
        ASSERT_EXCEPTION(Exception, s.replace(0, String(STR("a")), 1));
    }

    {
        String s(STR("a"));
        s.replace(0, String());
        ASSERT(s == STR(""));
        ASSERT(s.length() == 0);
        ASSERT(s.capacity() == 2);
    }

    {
        String s(STR("a"));
        s.replace(0, String(STR("b")));
        ASSERT(s == STR("b"));
        ASSERT(s.length() == 1);
        ASSERT(s.capacity() == 2);
    }

    {
        String s(STR("ab"));
        s.replace(0, String(), 1);
        ASSERT(s == STR("b"));
        ASSERT(s.length() == 1);
        ASSERT(s.capacity() == 3);
    }

    {
        String s(STR("ab"));
        s.replace(1, String(), 1);
        ASSERT(s == STR("a"));
        ASSERT(s.length() == 1);
        ASSERT(s.capacity() == 3);
    }

    {
        String s(STR("abcd"));
        s.replace(1, String(), 2);
        ASSERT(s == STR("ad"));
        ASSERT(s.length() == 2);
        ASSERT(s.capacity() == 5);
    }

    {
        String s(STR("abcd"));
        s.replace(1, String(STR("x")), 2);
        ASSERT(s == STR("axd"));
        ASSERT(s.length() == 3);
        ASSERT(s.capacity() == 5);
    }

    {
        String s(STR("abcd"));
        s.replace(1, String(STR("xyz")), 2);
        ASSERT(s == STR("axyzd"));
        ASSERT(s.length() == 5);
        ASSERT(s.capacity() == 6);
    }

    {
        String s(STR("abcd"));
        s.replace(2, String(STR("x")));
        ASSERT(s == STR("abx"));
        ASSERT(s.length() == 3);
        ASSERT(s.capacity() == 5);
    }

    {
        String s(STR("abcd"));
        ASSERT_EXCEPTION(Exception, s.replace(1, String(STR("xyz")), 4));
    }

    // void replace(int pos, const char_t* chars, int len = -1)

    {
        String s(STR("a"));
        ASSERT_EXCEPTION(Exception, s.replace(-1, STR("")));
        ASSERT_EXCEPTION(Exception, s.replace(s.length() + 1, STR("")));
    }

    {
        String s(STR("abc"));
        ASSERT_EXCEPTION(Exception, s.replace(0, s.chars()));
    }

    {
        String s;
        s.replace(0, STR("a"));
        ASSERT(s == STR("a"));
        ASSERT(s.length() == 1);
        ASSERT(s.capacity() == 2);
    }

    {
        String s;
        ASSERT_EXCEPTION(Exception, s.replace(0, STR("a"), 1));
    }

    {
        String s(STR("a"));
        s.replace(0, STR(""));
        ASSERT(s == STR(""));
        ASSERT(s.length() == 0);
        ASSERT(s.capacity() == 2);
    }

    {
        String s(STR("a"));
        s.replace(0, STR("b"));
        ASSERT(s == STR("b"));
        ASSERT(s.length() == 1);
        ASSERT(s.capacity() == 2);
    }

    {
        String s(STR("ab"));
        s.replace(0, STR(""), 1);
        ASSERT(s == STR("b"));
        ASSERT(s.length() == 1);
        ASSERT(s.capacity() == 3);
    }

    {
        String s(STR("ab"));
        s.replace(1, STR(""), 1);
        ASSERT(s == STR("a"));
        ASSERT(s.length() == 1);
        ASSERT(s.capacity() == 3);
    }

    {
        String s(STR("abcd"));
        s.replace(1, STR(""), 2);
        ASSERT(s == STR("ad"));
        ASSERT(s.length() == 2);
        ASSERT(s.capacity() == 5);
    }

    {
        String s(STR("abcd"));
        s.replace(1, STR("x"), 2);
        ASSERT(s == STR("axd"));
        ASSERT(s.length() == 3);
        ASSERT(s.capacity() == 5);
    }

    {
        String s(STR("abcd"));
        s.replace(1, STR("xyz"), 2);
        ASSERT(s == STR("axyzd"));
        ASSERT(s.length() == 5);
        ASSERT(s.capacity() == 6);
    }

    {
        String s(STR("abcd"));
        s.replace(2, STR("x"));
        ASSERT(s == STR("abx"));
        ASSERT(s.length() == 3);
        ASSERT(s.capacity() == 5);
    }

    {
        String s(STR("abcd"));
        ASSERT_EXCEPTION(Exception, s.replace(1, STR("xyz"), 4));
    }

    // void replaceString(const String& searchStr, const String& replaceStr, bool caseSensitive = true)

    {
        String s;
        s.replaceString(String(STR("a")), String(STR("b")));
        ASSERT(s == STR(""));
    }

    {
        String s(STR("abc"));
        s.replaceString(String(), String(STR("a")));
        ASSERT(s == STR("abc"));
    }

    {
        String s(STR("abc"));
        s.replaceString(String(STR("b")), String());
        ASSERT(s == STR("ac"));
    }

    {
        String s(STR("a"));
        ASSERT_EXCEPTION(Exception, s.replaceString(s, String((STR("a")))));
        ASSERT_EXCEPTION(Exception, s.replaceString(String((STR("a"))), s));
    }

    {
        String s(STR("abc"));
        s.replaceString(String(STR("x")), String(STR("y")));
        ASSERT(s == STR("abc"));
    }

    {
        String s(STR("abcd"));
        s.replaceString(String(STR("bc")), String(STR("x")));
        ASSERT(s == STR("axd"));
        ASSERT(s.length() == 3);
        ASSERT(s.capacity() == 5);
    }

    {
        String s(STR("abcd"));
        s.replaceString(String(STR("bc")), String(STR("xyz")));
        ASSERT(s == STR("axyzd"));
        ASSERT(s.length() == 5);
        ASSERT(s.capacity() == 6);
    }

    {
        String s(STR("abcd"));
        s.replaceString(String(STR("ab")), String(STR("x")));
        ASSERT(s == STR("xcd"));
        ASSERT(s.length() == 3);
        ASSERT(s.capacity() == 5);
    }

    {
        String s(STR("abcd"));
        s.replaceString(String(STR("cd")), String(STR("x")));
        ASSERT(s == STR("abx"));
        ASSERT(s.length() == 3);
        ASSERT(s.capacity() == 5);
    }

    {
        String s(STR("aab"));
        s.replaceString(String(STR("a")), String(STR("xy")));
        ASSERT(s == STR("xyxyb"));
        ASSERT(s.length() == 5);
        ASSERT(s.capacity() == 6);
    }

    {
        String s(STR("abcbd"));
        s.replaceString(String(STR("b")), String(STR("xy")));
        ASSERT(s == STR("axycxyd"));
        ASSERT(s.length() == 7);
        ASSERT(s.capacity() == 8);
    }

    {
        String s(STR("ABCBD"));
        s.replaceString(String(STR("b")), String(STR("xy")), false);
        ASSERT(s == STR("AxyCxyD"));
        ASSERT(s.length() == 7);
        ASSERT(s.capacity() == 8);
    }

    // void replaceString(const char_t* searchChars, const char_t* replaceChars, bool caseSensitive = true)

    {
        String s;
        s.replaceString(STR("a"), STR("b"));
        ASSERT(s == STR(""));
    }

    {
        String s(STR("abc"));
        s.replaceString(STR(""), STR("a"));
        ASSERT(s == STR("abc"));
        s.replaceString(np,  STR("a"));
        ASSERT(s == STR("abc"));
    }

    {
        String s(STR("abc"));
        s.replaceString(STR("b"), STR(""));
        ASSERT(s == STR("ac"));
    }

    {
        String s(STR("abc"));
        s.replaceString(STR("b"), np);
        ASSERT(s == STR("ac"));
    }

    {
        String s(STR("a"));
        ASSERT_EXCEPTION(Exception, s.replaceString(s.chars(), STR("a")));
        ASSERT_EXCEPTION(Exception, s.replaceString(STR("a"), s.chars()));
    }

    {
        String s(STR("abc"));
        s.replaceString(STR("x"), STR("y"));
        ASSERT(s == STR("abc"));
    }

    {
        String s(STR("abcd"));
        s.replaceString(STR("bc"), STR("x"));
        ASSERT(s == STR("axd"));
        ASSERT(s.length() == 3);
        ASSERT(s.capacity() == 5);
    }

    {
        String s(STR("abcd"));
        s.replaceString(STR("bc"), STR("xyz"));
        ASSERT(s == STR("axyzd"));
        ASSERT(s.length() == 5);
        ASSERT(s.capacity() == 6);
    }

    {
        String s(STR("abcd"));
        s.replaceString(STR("ab"), STR("x"));
        ASSERT(s == STR("xcd"));
        ASSERT(s.length() == 3);
        ASSERT(s.capacity() == 5);
    }

    {
        String s(STR("abcd"));
        s.replaceString(STR("cd"), STR("x"));
        ASSERT(s == STR("abx"));
        ASSERT(s.length() == 3);
        ASSERT(s.capacity() == 5);
    }

    {
        String s(STR("aab"));
        s.replaceString(STR("a"), STR("xy"));
        ASSERT(s == STR("xyxyb"));
        ASSERT(s.length() == 5);
        ASSERT(s.capacity() == 6);
    }

    {
        String s(STR("abcbd"));
        s.replaceString(STR("b"), STR("xy"));
        ASSERT(s == STR("axycxyd"));
        ASSERT(s.length() == 7);
        ASSERT(s.capacity() == 8);
    }

    {
        String s(STR("ABCBD"));
        s.replaceString(STR("b"), STR("xy"), false);
        ASSERT(s == STR("AxyCxyD"));
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
        ASSERT(s == STR(""));
        ASSERT(s.length() == 0);
        ASSERT(s.capacity() == 0);
    }

    {
        String s(STR("a"));
        s.clear();
        ASSERT(s == STR(""));
        ASSERT(s.length() == 0);
        ASSERT(s.capacity() == 2);
    }

    // void reset()

    {
        String s;
        s.reset();
        ASSERT(s == STR(""));
        ASSERT(s.length() == 0);
        ASSERT(s.capacity() == 0);
    }

    {
        String s(STR("a"));
        s.reset();
        ASSERT(s == STR(""));
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
        ASSERT(s == STR(""));
        ASSERT(s.length() == 0);
        ASSERT(s.capacity() == 0);
    }
    {
        String s(STR("a"));
        char_t* p = s.release();
        ASSERT(strCompare(p, STR("a")) == 0);
        ASSERT(s == STR(""));
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

void testUnicode()
{
#ifdef CHAR_ENCODING_UTF8
    const char_t* str = "\x24\xc2\xa2\n\xe2\x82\xac\xf0\x90\x8d\x88\n";
#else
    const char_t* str = u"\x0024\x00a2\n\x20ac\xd800\xdf48\n";
#endif

    const byte_t BYTES_UTF8_UNIX[] = { 0x24, 0xc2, 0xa2, 0x0a, 0xe2, 0x82, 0xac, 0xf0, 0x90, 0x8d, 0x88, 0x0a };
    const byte_t BYTES_UTF8_BOM_UNIX[] = { 0xef, 0xbb, 0xbf, 0x24, 0xc2, 0xa2, 0x0a, 0xe2, 0x82, 0xac, 0xf0, 0x90, 0x8d, 0x88, 0x0a };
    ALIGN_AS(2) const byte_t BYTES_UTF16_LE_WIN[] = { 0xff, 0xfe, 0x24, 0x00, 0xa2, 0x00, 0x0d, 0x00, 0x0a, 0x00, 0xac, 0x20, 0x00, 0xd8, 0x48, 0xdf, 0x0d, 0x00, 0x0a, 0x00 };
    ALIGN_AS(2) const byte_t BYTES_UTF16_LE_UNIX[] = { 0xff, 0xfe, 0x24, 0x00, 0xa2, 0x00, 0x0a, 0x00, 0xac, 0x20, 0x00, 0xd8, 0x48, 0xdf, 0x0a, 0x00 };
    ALIGN_AS(2) const byte_t BYTES_UTF16_BE_UNIX[] = { 0xfe, 0xff, 0x00, 0x24, 0x00, 0xa2, 0x00, 0x0a, 0x20, 0xac, 0xd8, 0x00, 0xdf, 0x48, 0x00, 0x0a };

    // static String bytesToString(ByteBuffer& bytes, TextEncoding& encoding, bool& bom, bool& crLf)
    // static String bytesToString(int size, byte_t* bytes, TextEncoding& encoding, bool& bom, bool& crLf)

    {
        TextEncoding encoding;
        bool bom, crLf;
        ASSERT(Unicode::bytesToString(ByteBuffer(), encoding, bom, crLf).empty());
        ASSERT_EXCEPTION(Exception, Unicode::bytesToString(-1, BYTES_UTF8_UNIX, encoding, bom, crLf));
        ASSERT_EXCEPTION(Exception, Unicode::bytesToString(1, NULL, encoding, bom, crLf));
        ASSERT(Unicode::bytesToString(0, NULL, encoding, bom, crLf).empty());
    }

    {
        TextEncoding encoding;
        bool bom, crLf;
        String s = Unicode::bytesToString(sizeof(BYTES_UTF8_UNIX), BYTES_UTF8_UNIX, encoding, bom, crLf);
        ASSERT(s == str);
        ASSERT(encoding == TEXT_ENCODING_UTF8);
        ASSERT(!bom);
        ASSERT(!crLf);
    }

    {
        TextEncoding encoding;
        bool bom, crLf;
        String s = Unicode::bytesToString(sizeof(BYTES_UTF8_BOM_UNIX), BYTES_UTF8_BOM_UNIX, encoding, bom, crLf);
        ASSERT(s == str);
        ASSERT(encoding == TEXT_ENCODING_UTF8);
        ASSERT(bom);
        ASSERT(!crLf);
    }

    {
        TextEncoding encoding;
        bool bom, crLf;
        String s = Unicode::bytesToString(sizeof(BYTES_UTF16_LE_WIN), BYTES_UTF16_LE_WIN, encoding, bom, crLf);
        ASSERT(s == str);
        ASSERT(encoding == TEXT_ENCODING_UTF16_LE);
        ASSERT(bom);
        ASSERT(crLf);
    }

    {
        TextEncoding encoding;
        bool bom, crLf;
        String s = Unicode::bytesToString(sizeof(BYTES_UTF16_LE_UNIX), BYTES_UTF16_LE_UNIX, encoding, bom, crLf);
        ASSERT(s == str);
        ASSERT(encoding == TEXT_ENCODING_UTF16_LE);
        ASSERT(bom);
        ASSERT(!crLf);
    }

    {
        TextEncoding encoding;
        bool bom, crLf;
        String s = Unicode::bytesToString(sizeof(BYTES_UTF16_BE_UNIX), BYTES_UTF16_BE_UNIX, encoding, bom, crLf);
        ASSERT(s == str);
        ASSERT(encoding == TEXT_ENCODING_UTF16_BE);
        ASSERT(bom);
        ASSERT(!crLf);
    }

    // static ByteBuffer stringToBytes(const String& str, TextEncoding encoding, bool bom, bool crLf)

    {
        ByteBuffer bytes = Unicode::stringToBytes(str, TEXT_ENCODING_UTF8, false, false);
        ASSERT(memcmp(bytes.values(), BYTES_UTF8_UNIX, sizeof(BYTES_UTF8_UNIX)) == 0);
    }

    {
        ByteBuffer bytes = Unicode::stringToBytes(str, TEXT_ENCODING_UTF8, true, false);
        ASSERT(memcmp(bytes.values(), BYTES_UTF8_BOM_UNIX, sizeof(BYTES_UTF8_BOM_UNIX)) == 0);
    }

    {
        ByteBuffer bytes = Unicode::stringToBytes(str, TEXT_ENCODING_UTF16_LE, true, true);
        ASSERT(memcmp(bytes.values(), BYTES_UTF16_LE_WIN, sizeof(BYTES_UTF16_LE_WIN)) == 0);
    }

    {
        ByteBuffer bytes = Unicode::stringToBytes(str, TEXT_ENCODING_UTF16_LE, true, false);
        ASSERT(memcmp(bytes.values(), BYTES_UTF16_LE_UNIX, sizeof(BYTES_UTF16_LE_UNIX)) == 0);
    }

    {
        ByteBuffer bytes = Unicode::stringToBytes(str, TEXT_ENCODING_UTF16_BE, true, false);
        ASSERT(memcmp(bytes.values(), BYTES_UTF16_BE_UNIX, sizeof(BYTES_UTF16_BE_UNIX)) == 0);
    }
}

void testStringIterator()
{
    #ifdef CHAR_ENCODING_UTF8
    const char_t* CHARS = "\x24\xc2\xa2\xe2\x82\xac\xf0\x90\x8d\x88";
    #else
    const char_t* CHARS = u"\x0024\x00a2\x20ac\xd800\xdf48";
    #endif

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
        ASSERT(compareArray(a, 3, ep));
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
        ASSERT(compareArray(a2, 3, ep));
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
        ASSERT(compareArray(a2, 3, ep));
    }

    // Array<_Type>& operator=(const Array<_Type>& other)

    {
        Array<int> a1(3, ep), a2;
        a2 = a1;
        ASSERT(a2.size() == 3);
        ASSERT(a2.capacity() == 3);
        ASSERT(a2.values());
        ASSERT(compareArray(a2, 3, ep));
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
        ASSERT(compareArray(a2, 3, ep));
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

    // _Type& first()
    // const _Type& first() const
    // _Type& last()
    // const _Type& last() const

    {
        Array<int> a;
        ASSERT_EXCEPTION(Exception, a.first());
        ASSERT_EXCEPTION(Exception, a.last());

        const Array<int>& ca = a;
        ASSERT_EXCEPTION(Exception, ca.first());
        ASSERT_EXCEPTION(Exception, ca.last());
    }

    {
        Array<int> a(3, ep);
        ASSERT(a.first() == 1);
        ASSERT(a.last() == 3);

        const Array<int>& ca = a;
        ASSERT(ca.first() == 1);
        ASSERT(ca.last() == 3);
    }

    // int find(const _Type& value) const

    {
        Array<int> a;
        ASSERT(a.find(0) == INVALID_POSITION);
    }

    {
        Array<int> a(3, ep);
        ASSERT(a.find(2) == 1);
        ASSERT(a.find(0) == INVALID_POSITION);
    }

    // void ensureCapacity(int capacity)

    ASSERT_EXCEPTION(Exception, Array<int>().ensureCapacity(-1));

    {
        Array<int> a(3, ep);
        ASSERT(a.capacity() == 3);
        a.ensureCapacity(2);
        ASSERT(a.capacity() == 3);
        ASSERT(compareArray(a, 3, ep));
    }

    {
        Array<int> a(3, ep);
        ASSERT(a.capacity() == 3);
        a.ensureCapacity(5);
        ASSERT(a.capacity() == 5);
        ASSERT(compareArray(a, 3, ep));
    }

    {
        Array<int> a(3, ep);
        ASSERT(a.capacity() == 3);
        a.ensureCapacity(0);
        ASSERT(a.capacity() == 3);
        ASSERT(compareArray(a, 3, ep));
    }

    // void shrinkToLength()

    {
        Array<int> a(3, ep);
        ASSERT(a.capacity() == 3);
        a.shrinkToLength();
        ASSERT(a.capacity() == 3);
        ASSERT(compareArray(a, 3, ep));
    }

    {
        Array<int> a(3, ep);
        a.ensureCapacity(10);
        ASSERT(a.capacity() == 10);
        a.shrinkToLength();
        ASSERT(a.capacity() == 3);
        ASSERT(compareArray(a, 3, ep));
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
        ASSERT(compareArray(a, 3, ep));
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
        ASSERT(compareArray(a2, 3, ep));
    }

    {
        Array<int> a1(1), a2(3, ep);
        a2.assign(a1);
        ASSERT(a2.size() == 1);
        ASSERT(a2.capacity() == 3);
        ASSERT(a2.values());
        ASSERT(a2[0] == 0);
    }

    // void removeLast()

    ASSERT_EXCEPTION(Exception, Array<int>().removeLast());

    {
        Array<int> a(3, ep);
        a.removeLast();
        ASSERT(a.size() == 2);
    }

    // void addLast(const _Type& value)

    {
        Array<int> a;
        int v1 = 1, v2 = 2, v3 = 3;

        a.addLast(v1);
        ASSERT(a.last() == v1);
        ASSERT(a.size() == 1);
        ASSERT(a.capacity() == 1);

        a.addLast(v2);
        ASSERT(a.last() == v2);
        ASSERT(a.size() == 2);
        ASSERT(a.capacity() == 3);

        a.addLast(v3);
        ASSERT(a.last() == v3);
        ASSERT(a.size() == 3);
        ASSERT(a.capacity() == 3);
    }

    // void addLast(_Type&& value)

    {
        Array<int> a;

        a.addLast(1);
        ASSERT(a.last() == 1);
        ASSERT(a.size() == 1);
        ASSERT(a.capacity() == 1);

        a.addLast(2);
        ASSERT(a.last() == 2);
        ASSERT(a.size() == 2);
        ASSERT(a.capacity() == 3);

        a.addLast(3);
        ASSERT(a.last() == 3);
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
        ASSERT(a.last() == v1);
        ASSERT(a.size() == 1);
        ASSERT(a.capacity() == 1);

        a.insert(1, v2);
        ASSERT(a.last() == v2);
        ASSERT(a.size() == 2);
        ASSERT(a.capacity() == 3);

        a.insert(0, v3);
        ASSERT(a.first() == v3);
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
        ASSERT(a.last() == 1);
        ASSERT(a.size() == 1);
        ASSERT(a.capacity() == 1);

        a.insert(1, 2);
        ASSERT(a.last() == 2);
        ASSERT(a.size() == 2);
        ASSERT(a.capacity() == 3);

        a.insert(0, 3);
        ASSERT(a.first() == 3);
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

    // void sort()

    {
        Array<int> a;
        a.sort();
    }

    {
        int elems[] = { 12 };
        int sorted[] = { 12 };

        Array<int> a(1, elems);
        a.sort();
        ASSERT(compareArray(a, 1, sorted));
    }

    {
        int elems[] = { 45, 12 };
        int sorted[] = { 12, 45 };

        Array<int> a(2, elems);
        a.sort();
        ASSERT(compareArray(a, 2, sorted));
    }

    {
        int elems[] = { 97, 80, 51, 53, 38, 44, 28, 58, 91, 78 };
        int sorted[] = { 28, 38, 44, 51, 53, 58, 78, 80, 91, 97 };

        Array<int> a(10, elems);
        a.sort();
        ASSERT(compareArray(a, 10, sorted));
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
        ASSERT(compareArray(a, 3, ep));
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

    // Unique

    {
        Array<Unique<int>> a1;
        Array<Unique<int>> a2(2);
    }

    {
        Array<Unique<int>> a;
        a.addLast(createUnique<int>(1));
        a.addLast(createUnique<int>(2));
        ASSERT(*a[0] == 1);
        ASSERT(*a[1] == 2);
        ASSERT(a.find(createUnique<int>(0)) == INVALID_POSITION);
        ASSERT(a.find(createUnique<int>(2)) == 1);
    }

    {
        Array<Unique<int>> a;
        a.ensureCapacity(2);
        ASSERT(a.capacity() == 2);
        a.addLast(createUnique<int>(1));
        a.shrinkToLength();
        ASSERT(a.capacity() == 1);
        a.resize(3);
        ASSERT(a.capacity() == 3);
    }

    {
        Array<Unique<int>> a;
        a.insert(0, createUnique<int>(1));
        a.insert(0, createUnique<int>(2));
        a.removeLast();
        ASSERT(*a.last() == 2);
    }

    {
        Array<Unique<int>> a;
        a.addLast(createUnique<int>(1));
        a.addLast(createUnique<int>(2));
        a.remove(0);
        ASSERT(*a.first() == 2);
    }

    {
        Array<Unique<int>> a;
        a.addLast(createUnique<int>(1));
        a.addLast(createUnique<int>(2));
        a.clear();
        ASSERT(a.empty());
    }

    {
        Array<Unique<int>> a;
        a.addLast(createUnique<int>(3));
        a.addLast(createUnique<int>(2));
        a.addLast(createUnique<int>(1));
        a.sort();
        ASSERT(a[0] == 1 && a[1] == 2 && a[2] == 3);
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
        ASSERT(!l.first());
        ASSERT(!l.last());
        ASSERT(l.size() == 0);
        ASSERT(l.empty());
    }

    // List(int size)

    ASSERT_EXCEPTION(Exception, List<int>(-1));

    {
        List<int> l(0);
        ASSERT(!l.first());
        ASSERT(!l.last());
        ASSERT(l.size() == 0);
        ASSERT(l.empty());
    }

    {
        List<int> l(1);
        ASSERT(l.first());
        ASSERT(l.last());
        ASSERT(l.size() == 1);
        ASSERT(!l.empty());
        ASSERT(l.first()->value == 0);
    }

    // List(int size, const _Type& value)

    ASSERT_EXCEPTION(Exception, List<int>(-1, 123));

    {
        List<int> l(0, 123);
        ASSERT(!l.first());
        ASSERT(!l.last());
        ASSERT(l.size() == 0);
        ASSERT(l.empty());
    }

    {
        List<int> l(1, 123);
        ASSERT(l.first());
        ASSERT(l.last());
        ASSERT(l.size() == 1);
        ASSERT(!l.empty());
        ASSERT(l.first()->value == 123);
    }

    // List(int size, const _Type* values)

    ASSERT_EXCEPTION(Exception, List<int>(-1, ep));
    ASSERT_EXCEPTION(Exception, List<int>(1, np));

    {
        List<int> l(0, np);
        ASSERT(!l.first());
        ASSERT(!l.last());
        ASSERT(l.size() == 0);
        ASSERT(l.empty());
    }

    {
        List<int> l(3, ep);
        ASSERT(l.first());
        ASSERT(l.last());
        ASSERT(l.size() == 3);
        ASSERT(!l.empty());
        ASSERT(compareArray(l, 3, ep));
    }

    // List(const List<_Type>& other)

    {
        List<int> l1, l2(l1);
        ASSERT(!l2.first());
        ASSERT(!l2.last());
        ASSERT(l2.size() == 0);
        ASSERT(l2.empty());
    }

    {
        List<int> l1(3, ep), l2(l1);
        ASSERT(l2.first());
        ASSERT(l2.last());
        ASSERT(l2.size() == 3);
        ASSERT(!l2.empty());
        ASSERT(compareArray(l2, 3, ep));
    }

    // List(List<_Type>&& other)

    {
        List<int> l1, l2(static_cast<List<int>&&>(l1));
        ASSERT(!l2.first());
        ASSERT(!l2.last());
        ASSERT(l2.size() == 0);
        ASSERT(l2.empty());
    }

    {
        List<int> l1(3, ep), l2(static_cast<List<int>&&>(l1));

        ASSERT(!l1.first());
        ASSERT(!l1.last());
        ASSERT(l1.size() == 0);
        ASSERT(l1.empty());

        ASSERT(l2.first());
        ASSERT(l2.last());
        ASSERT(l2.size() == 3);
        ASSERT(!l2.empty());
        ASSERT(compareArray(l2, 3, ep));
    }

    //  List<_Type>& operator=(const List<_Type>& other)

    {
        List<int> l1(3, ep), l2;
        l2 = l1;
        ASSERT(l2.first());
        ASSERT(l2.last());
        ASSERT(l2.size() == 3);
        ASSERT(!l2.empty());
        ASSERT(compareArray(l2, 3, ep));
    }

    // List<_Type>& operator=(List<_Type>&& other)

    {
        List<int> l1, l2;
        l2 = static_cast<List<int>&&>(l1);
        ASSERT(!l2.first());
        ASSERT(!l2.last());
        ASSERT(l2.size() == 0);
        ASSERT(l2.empty());
    }

    {
        List<int> l1(3, ep), l2;
        l2 = static_cast<List<int>&&>(l1);

        ASSERT(!l1.first());
        ASSERT(!l1.last());
        ASSERT(l1.size() == 0);
        ASSERT(l1.empty());

        ASSERT(l2.first());
        ASSERT(l2.last());
        ASSERT(l2.size() == 3);
        ASSERT(!l2.empty());
        ASSERT(compareArray(l2, 3, ep));
    }

    // int size() const
    // bool empty() const
    // ListNode<_Type>* first()
    // ListNode<_Type>* last()

    {
        List<int> l;

        ASSERT(!l.first());
        ASSERT(!l.last());
        ASSERT(l.size() == 0);
        ASSERT(l.empty());

        l.addLast(1);
        ASSERT(l.first());
        ASSERT(l.last());
        ASSERT(l.size() == 1);
        ASSERT(!l.empty());
        ASSERT(l.first()->value == 1);
        ASSERT(l.last()->value == 1);

        l.addLast(2);
        ASSERT(l.first());
        ASSERT(l.last());
        ASSERT(l.size() == 2);
        ASSERT(!l.empty());
        ASSERT(l.first()->value == 1);
        ASSERT(l.last()->value == 2);
    }

    // const ListNode<_Type>* first() const
    // const ListNode<_Type>* last() const

    {
        List<int> l;
        const List<int>& cl = l;

        ASSERT(!cl.first());
        ASSERT(!cl.last());

        l.addLast(1);
        ASSERT(cl.first());
        ASSERT(cl.last());
        ASSERT(cl.first()->value == 1);
        ASSERT(cl.last()->value == 1);

        l.addLast(2);
        ASSERT(cl.first());
        ASSERT(cl.last());
        ASSERT(cl.first()->value == 1);
        ASSERT(cl.last()->value == 2);
    }


    // ListNode<_Type>* find(const _Type& value)

    {
        List<int> l;
        ASSERT(!l.find(0));
    }

    {
        List<int> l(3, ep);
        ASSERT(l.find(2) == l.first()->next);
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
        ASSERT(cl.find(2) == cl.first()->next);
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
        ASSERT(l.first()->value == 123);
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
        ASSERT(compareArray(l, 3, ep));
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
        ASSERT(compareArray(l2, 3, ep));
    }

    // void removeFirst()

    ASSERT_EXCEPTION(Exception, List<int>().removeFirst());

    {
        List<int> l(3, ep);

        l.removeFirst();
        ASSERT(l.first()->value == 2);
        ASSERT(l.size() == 2);

        l.removeFirst();
        ASSERT(l.first()->value == 3);
        ASSERT(l.size() == 1);

        l.removeFirst();
        ASSERT(l.empty());
    }

    // void removeLast()

    ASSERT_EXCEPTION(Exception, List<int>().removeLast());

    {
        List<int> l(3, ep);

        l.removeLast();
        ASSERT(l.last()->value == 2);
        ASSERT(l.size() == 2);

        l.removeLast();
        ASSERT(l.last()->value == 1);
        ASSERT(l.size() == 1);

        l.removeLast();
        ASSERT(l.empty());
    }

    // void addFirst(const _Type& value)

    {
        List<int> l;
        int v1 = 1, v2 = 2;
        l.addFirst(v1);
        ASSERT(l.first()->value == 1);
        l.addFirst(v2);
        ASSERT(l.first()->value == 2);
    }

    // void addFirst(_Type&& value)

    {
        List<int> l;
        l.addFirst(1);
        ASSERT(l.first()->value == 1);
        l.addFirst(2);
        ASSERT(l.first()->value == 2);
    }

    // void addLast(const _Type& value)

    {
        List<int> l;
        int v1 = 1, v2 = 2;
        l.addLast(v1);
        ASSERT(l.last()->value == 1);
        l.addLast(v2);
        ASSERT(l.last()->value == 2);
    }

    // void addLast(_Type&& value)

    {
        List<int> l;
        l.addLast(1);
        ASSERT(l.last()->value == 1);
        l.addLast(2);
        ASSERT(l.last()->value == 2);
    }

    // ListNode<_Type>* insertBefore(ListNode<_Type>* pos, const _Type& value)

    {
        int v;
        ASSERT_EXCEPTION(Exception, List<int>().insertBefore(NULL, v));
    }

    {
        List<int> l;
        int v1 = 1, v2 = 2, v3 = 3;
        l.addLast(v1);
        ASSERT(l.insertBefore(l.first(), v2)->value == v2);
        ASSERT(l.insertBefore(l.last(), v3)->value == v3);
        ASSERT(compareSequence(l, 2, 3, 1));
    }

    // ListNode<_Type>* insertBefore(ListNode<_Type>* pos, _Type&& value)

    ASSERT_EXCEPTION(Exception, List<int>().insertBefore(NULL, 0));

    {
        List<int> l;
        l.addLast(1);
        ASSERT(l.insertBefore(l.first(), 2)->value == 2);
        ASSERT(l.insertBefore(l.last(), 3)->value == 3);
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
        l.addLast(v1);
        ASSERT(l.insertAfter(l.last(), v2)->value == v2);
        ASSERT(l.insertAfter(l.first(), v3)->value == v3);
        ASSERT(compareSequence(l, 1, 3, 2));
    }

    // ListNode<_Type>* insertAfter(ListNode<_Type>* pos, _Type&& value)

    ASSERT_EXCEPTION(Exception, List<int>().insertAfter(NULL, 0));

    {
        List<int> l;
        l.addLast(1);
        ASSERT(l.insertAfter(l.last(), 2)->value == 2);
        ASSERT(l.insertAfter(l.first(), 3)->value == 3);
        ASSERT(compareSequence(l, 1, 3, 2));
    }

    // void remove(ListNode<_Type>* pos)

    ASSERT_EXCEPTION(Exception, List<int>().remove(NULL));

    {
        List<int> l(3, ep);
        l.remove(l.first());
        ASSERT(compareSequence(l, 2, 3));
    }

    {
        List<int> l(3, ep);
        l.remove(l.last());
        ASSERT(compareSequence(l, 1, 2));
    }

    {
        List<int> l(3, ep);
        l.remove(l.first()->next);
        ASSERT(compareSequence(l, 1, 3));
    }

    // void clear()

    {
        List<int> l;
        l.clear();
        ASSERT(!l.first());
        ASSERT(!l.last());
        ASSERT(l.size() == 0);
        ASSERT(l.empty());
    }

    {
        List<int> l(3, ep);
        l.clear();
        ASSERT(!l.first());
        ASSERT(!l.last());
        ASSERT(l.size() == 0);
        ASSERT(l.empty());
    }

    // Unique

    {
        List<Unique<int>> l1;
        List<Unique<int>> l2(2);
    }

    {
         List<Unique<int>> l;
         l.addLast(createUnique<int>(1));
         l.addLast(createUnique<int>(2));
         ASSERT(!l.find(createUnique<int>(0)));
         ASSERT(l.find(createUnique<int>(2)));
    }

    {
         List<Unique<int>> l;
         l.addLast(createUnique<int>(1));
         l.insertBefore(l.first(), createUnique<int>(2));
         l.removeFirst();
         ASSERT(*l.last()->value == 1);
    }

    {
         List<Unique<int>> l;
         l.addFirst(createUnique<int>(1));
         l.insertAfter(l.last(), createUnique<int>(2));
         l.removeLast();
         ASSERT(*l.last()->value == 1);
    }

    {
         List<Unique<int>> l;
         l.addLast(createUnique<int>(1));
         l.addLast(createUnique<int>(2));
         l.remove(l.first());
         ASSERT(*l.last()->value == 2);
    }

    {
         List<Unique<int>> l;
         l.addLast(createUnique<int>(1));
         l.addLast(createUnique<int>(2));
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
        m1.add(1, 10);
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
        m1.add(1, 10);

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
        m.add(1, 10);

        const Map<int, int>& cm = m;
        ASSERT(cm[1] == 10);
        ASSERT_EXCEPTION(Exception, cm[2]);
    }

    // Map<_Key, _Value>& operator=(const Map<_Key, _Value>& other)

    {
        Map<int, int> m1;
        m1.add(1, 10);
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
        m1.add(1, 10);

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
        m.add(1, 10);

        ASSERT(m.size() == 1);
        ASSERT(m.numBuckets() == 1);
        ASSERT(!m.empty());
        ASSERT(m.loadFactor() == 1);
    }

    {
        Map<int, int> m;
        m.add(1, 10);
        m.add(2, 20);

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
        m.add(1, 10);
        ASSERT(*m.find(1) == 10);
        ASSERT(!m.find(2));
    }

    // const _Value* find(const _Key& key) const

    {
        Map<int, int> m;
        m.add(1, 10);

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
        m1.add(1, 10);
        Map<int, int> m2;
        m2.assign(m1);
        ASSERT(m1.size() == 1);
        ASSERT(m1.numBuckets() == 1);
        ASSERT(!m2.empty());
        ASSERT(m2[1] == 10);
    }

    // void add(const _Key& key, const _Value& value)

    {
        Map<int, int> m;
        int k = 1, v = 10;
        m.add(k, v);
        ASSERT(m[k] == v);
    }

    // void add(_Key&& key, _Value&& value)

    {
        Map<int, int> m;
        m.add(1, 10);
        ASSERT(m[1] == 10);
    }

    // bool remove(const _Key& key)

    {
        Map<int, int> m;
        m.add(1, 10);
        m.remove(1);
        ASSERT(m.empty());
    }

    // void clear()

    {
        Map<int, int> m;
        m.add(1, 10);
        m.add(2, 20);
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
        m.add(1, 10);
        m.add(2, 20);
        ASSERT(m.size() == 2);
        ASSERT(m.numBuckets() == 3);

        m.rehash(0);
        ASSERT(m.size() == 2);
        ASSERT(m.numBuckets() == 3);
    }

    {
        Map<int, int> m;
        m.add(1, 10);
        m.add(2, 20);
        ASSERT(m.size() == 2);
        ASSERT(m.numBuckets() == 3);

        m.rehash(10);
        ASSERT(m.size() == 2);
        ASSERT(m.numBuckets() == 10);
    }

    // Unique

    {
        Map<Unique<int>, Unique<int>> m;
        m[createUnique<int>(1)] = createUnique<int>(10);
        ASSERT(**m.find(createUnique<int>(1)) == 10);
        const Map<Unique<int>, Unique<int>>& cm = m;
        ASSERT(*cm[createUnique<int>(1)] == 10);
        ASSERT(**cm.find(createUnique<int>(1)) == 10);
    }

    {
        Map<Unique<int>, Unique<int>> m;
        m.add(createUnique<int>(1), createUnique<int>(10));
        m.add(createUnique<int>(1), createUnique<int>(20));
        m.add(createUnique<int>(2), createUnique<int>(20));
        ASSERT(m.size() == 2);
        ASSERT(m.remove(createUnique<int>(1)));
        ASSERT(!m.remove(createUnique<int>(3)));
        ASSERT(m.size() == 1);
    }

    {
        Map<Unique<int>, Unique<int>> m;
        m.add(createUnique<int>(1), createUnique<int>(10));
        m.add(createUnique<int>(2), createUnique<int>(20));
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
        m.add(1, 10);
        m.add(2, 20);
        m.add(3, 30);

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
        m.add(1, 10);
        m.add(2, 20);
        m.add(3, 30);

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
        m.add(1, 10);
        m.add(2, 20);
        m.add(3, 30);

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
        m.add(1, 10);
        m.add(2, 20);
        m.add(3, 30);

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
        s1.add(1);
        Set<int> s2(s1);
        ASSERT(s2.size() == 1);
        ASSERT(s2.numBuckets() == 1);
        ASSERT(s2.maxLoadFactor() == 0.75f);
        ASSERT(!s2.empty());
        ASSERT(s2.contains(1));
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
        s1.add(1);

        Set<int> s2(static_cast<Set<int>&&>(s1));

        ASSERT(s1.size() == 0);
        ASSERT(s1.numBuckets() == 0);
        ASSERT(s1.maxLoadFactor() == 0.75f);
        ASSERT(s1.empty());

        ASSERT(s2.size() == 1);
        ASSERT(s2.numBuckets() == 1);
        ASSERT(s2.maxLoadFactor() == 0.75f);
        ASSERT(!s2.empty());
        ASSERT(s2.contains(1));
    }

    // Set<_Type>& operator=(const Set<_Type>& other)

    {
        Set<int> s1;
        s1.add(1);
        Set<int> s2;
        s2 = s1;
        ASSERT(s1.size() == 1);
        ASSERT(s1.numBuckets() == 1);
        ASSERT(!s2.empty());
        ASSERT(s2.contains(1));
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
        s1.add(1);

        Set<int> s2;
        s2 = static_cast<Set<int>&&>(s1);

        ASSERT(s1.size() == 0);
        ASSERT(s1.numBuckets() == 0);
        ASSERT(s1.empty());

        ASSERT(s2.size() == 1);
        ASSERT(s2.numBuckets() == 1);
        ASSERT(!s2.empty());
        ASSERT(s2.contains(1));
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
        s.add(1);

        ASSERT(s.size() == 1);
        ASSERT(s.numBuckets() == 1);
        ASSERT(!s.empty());
        ASSERT(s.loadFactor() == 1);
    }

    {
        Set<int> s;
        s.add(1);
        s.add(2);

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

    // bool contains(const _Type& value) const

    {
        Set<int> s;
        s.add(1);

        const Set<int>& cs = s;
        ASSERT(cs.contains(1));
        ASSERT(!cs.contains(2));
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
        s1.add(1);
        Set<int> s2;
        s2.assign(s1);
        ASSERT(s1.size() == 1);
        ASSERT(s1.numBuckets() == 1);
        ASSERT(!s2.empty());
        ASSERT(s2.contains(1));
    }

    // void add(const _Type& value)

    {
        Set<int> s;
        int v = 1;
        s.add(v);
        ASSERT(s.contains(v));
    }

    // void add(_Type&& value)

    {
        Set<int> s;
        s.add(1);
        ASSERT(s.contains(1));
    }

    // _Type remove()

    ASSERT_EXCEPTION(Exception, Set<int>().remove());

    {
        Set<int> s;
        s.add(1);
        ASSERT(s.remove() == 1);
    }

    // bool remove(const _Type& value)

    {
        Set<int> s;
        s.add(1);
        ASSERT(!s.remove(2));
        ASSERT(s.remove(1));
        ASSERT(s.empty());
    }

    // void clear()

    {
        Set<int> s;
        s.add(1);
        s.add(2);
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
        s.add(1);
        s.add(2);
        ASSERT(s.size() == 2);
        ASSERT(s.numBuckets() == 3);

        s.rehash(0);
        ASSERT(s.size() == 2);
        ASSERT(s.numBuckets() == 3);
    }

    {
        Set<int> s;
        s.add(1);
        s.add(2);
        ASSERT(s.size() == 2);
        ASSERT(s.numBuckets() == 3);

        s.rehash(10);
        ASSERT(s.size() == 2);
        ASSERT(s.numBuckets() == 10);
    }

    // Unique

    {
        Set<Unique<int>> s;
        s.add(createUnique<int>(1));
        ASSERT(s.contains(createUnique<int>(1)));
    }

    {
        Set<Unique<int>> s;
        s.add(createUnique<int>(1));
        Unique<int> p = s.remove();
        ASSERT(*p == 1);
    }

    {
        Set<Unique<int>> s;
        s.add(createUnique<int>(1));
        ASSERT(!s.remove(createUnique<int>(2)));
        ASSERT(s.remove(createUnique<int>(1)));
    }

    {
        Set<Unique<int>> s;
        s.add(createUnique<int>(1));
        s.add(createUnique<int>(2));
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
        s.add(1);
        s.add(2);
        s.add(3);

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
        s.add(1);
        s.add(2);
        s.add(3);

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
    testSwapBytes();
    testUnique();
    testShared();
    testBuffer();
    testString();
    testUnicode();
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

void testFileOpenSuccess(bool exists, int openMode)
{
    const char_t* filename = STR("test.txt");

    if (exists)
    {
        if (!File::exists(filename))
            File(filename, FILE_MODE_WRITE | FILE_MODE_CREATE);
    }
    else
    {
        if (File::exists(filename))
            File::remove(filename);
    }

    ASSERT_NO_EXCEPTION(File(filename, openMode));
}

void testFileOpenFailure(bool exists, int openMode)
{
    const char_t* filename = STR("test.txt");

    if (exists)
    {
        if (!File::exists(filename))
            File(filename, FILE_MODE_WRITE | FILE_MODE_CREATE);
    }
    else
    {
        if (File::exists(filename))
            File::remove(filename);
    }

    ASSERT_EXCEPTION(Exception, File(filename, openMode));
}

void testFile()
{
    const byte_t BYTES[] = { 1, 2, 3 };

    // File()
    // File(const String& filename, FileMode openMode = FILE_MODE_READ)
    // ~File()
    // bool isOpen() const
    // bool open(const String& filename, FileMode openMode = FILE_MODE_READ)
    // void close()

    {
        File f;
        ASSERT(!f.isOpen());
        ASSERT(f.open(STR("test.txt"), FILE_MODE_WRITE | FILE_MODE_CREATE));
        ASSERT(f.isOpen());
        ASSERT_EXCEPTION(Exception, f.open(STR("test.txt")));
        f.close();
        ASSERT(!f.isOpen());
        f.close();
        ASSERT(!f.isOpen());
    }

    {
        File f(STR("test.txt"));
        ASSERT(f.isOpen());
    }

    // static bool exists(const String& filename)
    // static void remove(const String& filename)

    ASSERT(File::exists(STR("test.txt")));
    ASSERT_NO_EXCEPTION(File::remove(STR("test.txt")));
    ASSERT(!File::exists(STR("test.txt")));
    ASSERT_EXCEPTION(Exception, File::remove(STR("test.txt")));

    // void write(const ByteBuffer& data)
    // void write(int size, const void* data)
    // int64_t size() const

    {
        File f;
        ASSERT_EXCEPTION(Exception, f.write(ByteBuffer()));
        ASSERT_EXCEPTION(Exception, f.size());
    }

    {
        File f(STR("test.txt"), FILE_MODE_WRITE | FILE_MODE_CREATE | FILE_MODE_TRUNCATE);
        f.write(ByteBuffer());
        ASSERT(f.size() == 0);
    }

    {
        File f(STR("test.txt"), FILE_MODE_WRITE | FILE_MODE_CREATE | FILE_MODE_TRUNCATE);
        ByteBuffer bytes(sizeof(BYTES), BYTES);
        f.write(bytes);
        ASSERT(f.size() == bytes.size());
    }

    {
        File f;
        ASSERT_EXCEPTION(Exception, f.write(sizeof(BYTES), BYTES));
        ASSERT_EXCEPTION(Exception, f.size());
    }

    {
        File f(STR("test.txt"), FILE_MODE_WRITE | FILE_MODE_CREATE | FILE_MODE_TRUNCATE);
        ASSERT_EXCEPTION(Exception, f.write(-1, BYTES));
        ASSERT_EXCEPTION(Exception, f.write(1, NULL));
        f.write(0, NULL);
        ASSERT(f.size() == 0);
    }

    {
        File f(STR("test.txt"), FILE_MODE_WRITE | FILE_MODE_CREATE | FILE_MODE_TRUNCATE);
        f.write(sizeof(BYTES), BYTES);
        ASSERT(f.size() == sizeof(BYTES));
    }

    // ByteBuffer read(int size = -1)
    // void read(int size, void* data)

    {
        File f;
        ASSERT_EXCEPTION(Exception, f.read());
    }

    {
        File f(STR("test.txt"));
        ByteBuffer bytes = f.read();
        ASSERT(bytes.size() == sizeof(BYTES));
        ASSERT(memcmp(bytes.values(), BYTES, sizeof(BYTES)) == 0);
    }

    {
        File f;
        byte_t bytes[sizeof(BYTES)];
        ASSERT_EXCEPTION(Exception, f.read(sizeof(bytes), bytes));
    }

    {
        File f(STR("test.txt"));
        byte_t bytes[sizeof(BYTES)];
        ASSERT_EXCEPTION(Exception, f.read(-1, bytes));
        ASSERT_EXCEPTION(Exception, f.read(1, NULL));
        ASSERT_NO_EXCEPTION(f.read(0, bytes));
    }

    {
        File f(STR("test.txt"));
        byte_t bytes[sizeof(BYTES)];
        f.read(sizeof(bytes), bytes);
        ASSERT(memcmp(bytes, BYTES, sizeof(bytes)) == 0);
    }

    // int64_t setPosition(int64_t offset, FilePosition position = FILE_POSITION_START)

    {
        File f(STR("test.txt"));
        f.setPosition(1);
        ByteBuffer bytes = f.read();
        ASSERT(bytes.size() == 2 && bytes[0] == 2 && bytes[1] == 3);

        f.setPosition(-1, FILE_POSITION_CURRENT);
        bytes = f.read();
        ASSERT(bytes.size() == 1 && bytes[0] == 3);

        f.setPosition(-2, FILE_POSITION_END);
        bytes = f.read();
        ASSERT(bytes.size() == 2 && bytes[0] == 2 && bytes[1] == 3);
    }

    // file open modes

    testFileOpenFailure(false, 0);

    testFileOpenFailure(false, FILE_MODE_READ);
    testFileOpenFailure(false, FILE_MODE_WRITE);
    testFileOpenSuccess(true, FILE_MODE_READ);
    testFileOpenSuccess(true, FILE_MODE_WRITE);

    testFileOpenSuccess(false, FILE_MODE_WRITE | FILE_MODE_CREATE);
    testFileOpenSuccess(true, FILE_MODE_WRITE | FILE_MODE_CREATE);

    testFileOpenSuccess(false, FILE_MODE_WRITE | FILE_MODE_CREATE | FILE_MODE_NEW_ONLY);
    testFileOpenFailure(true, FILE_MODE_WRITE | FILE_MODE_CREATE | FILE_MODE_NEW_ONLY);

    {
        File f(STR("test.txt"), FILE_MODE_WRITE | FILE_MODE_TRUNCATE);
        f.write(sizeof(BYTES), BYTES);
        ASSERT(f.size() == sizeof(BYTES));
    }

    {
        File f(STR("test.txt"), FILE_MODE_WRITE | FILE_MODE_TRUNCATE);
        ASSERT(f.size() == 0);
        f.write(sizeof(BYTES), BYTES);
        ASSERT(f.size() == sizeof(BYTES));
    }

    {
        File f(STR("test.txt"), FILE_MODE_WRITE | FILE_MODE_APPEND);
        ASSERT(f.size() == sizeof(BYTES));
        f.write(sizeof(BYTES), BYTES);
        ASSERT(f.size() == 2 * sizeof(BYTES));
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
    Console::readLine();
    Console::showCursor(false);
    Console::setCursorPosition(height - 2, 2);
    Console::write(STR("press ENTER to show cursor"));
    Console::readLine();
    Console::showCursor(true);
    Console::setCursorPosition(height - 1, 2);
    Console::write(STR("press ENTER to exit"));
    Console::readLine();
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
    Console::write(STR("bbb"), 3);
    Console::write('c', 3);

    Console::writeLine();
    Console::writeLine(String(STR("ddd")));
    Console::writeLine(STR("eee"));
    Console::writeLine(STR("eee"), 3);
    Console::writeLine('f', 3);

    Console::write(10, 10, String(STR("ggg")));
    Console::write(11, 10, STR("iii"));
    Console::write(12, 10, STR("iii"), 3);
    Console::write(13, 10, 'j', 3);

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

void testConsoleReadInput()
{
    Console::writeLine(STR("Press any key or key combination (ESC to exit)"));
    Console::setLineMode(false);

    while (true)
    {
        auto& inputEvents = Console::readInput();

        for (int i = 0; i < inputEvents.size(); ++i)
        {
            InputEvent event = inputEvents[i];

            if (event.eventType == INPUT_EVENT_TYPE_KEY)
            {
                KeyEvent keyEvent = event.event.keyEvent;

                Console::writeFormatted(STR("key %s: "), keyEvent.keyDown ? STR("down") : STR("up"));

                if (keyEvent.ctrl)
                    Console::write(STR("ctrl "));
                if (keyEvent.alt)
                    Console::write(STR("alt "));
                if (keyEvent.shift)
                    Console::write(STR("shift "));

                switch (keyEvent.key)
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
                    if (charIsPrint(keyEvent.ch))
                        Console::writeLine(keyEvent.ch);
                    else
                        Console::writeLineFormatted(STR("\\x%02x"),
                            static_cast<unsigned>(keyEvent.ch));
                    break;
                }
            }
            else if (event.eventType == INPUT_EVENT_TYPE_MOUSE)
            {
                MouseEvent mouseEvent = event.event.mouseEvent;

                switch (mouseEvent.button)
                {
                    case MOUSE_BUTTON_PRIMARY:
                        Console::writeFormatted(STR("primary mouse button: %s "),
                            mouseEvent.buttonDown ? STR("down") : STR("up"));
                        break;
                    case MOUSE_BUTTON_SECONDARY:
                        Console::writeFormatted(STR("secondary mouse button: %s "),
                            mouseEvent.buttonDown ? STR("down") : STR("up"));
                        break;
                    case MOUSE_BUTTON_WHEEL:
                        Console::writeFormatted(STR("mouse wheel: %s "),
                            mouseEvent.buttonDown ? STR("down") : STR("up"));
                        break;
                    case MOUSE_BUTTON_WHEEL_UP:
                        Console::writeFormatted(STR("mouse scrolled up "));
                        break;
                    case MOUSE_BUTTON_WHEEL_DOWN:
                        Console::writeFormatted(STR("mouse scrolled down "));
                        break;
                    default:
                        Console::writeFormatted(STR("mouse moved "));
                        break;
                }

                Console::writeFormatted(STR("position: %dx%d"), mouseEvent.x, mouseEvent.y);

                if (mouseEvent.ctrl)
                    Console::write(STR(" ctrl"));
                if (mouseEvent.alt)
                    Console::write(STR(" alt"));
                if (mouseEvent.shift)
                    Console::write(STR(" shift"));

                Console::writeLine();
            }
            else if (event.eventType == INPUT_EVENT_TYPE_WINDOW)
            {
                WindowEvent windowEvent = event.event.windowEvent;
                Console::writeLineFormatted(STR("window size: %dx%d"),
                    windowEvent.width, windowEvent.height);
            }
        }
    }
}

void testInput()
{
#ifdef PLATFORM_UNIX
    char chars[16];
    bool gotChars = false;

    termios ta;

    tcgetattr(STDIN_FILENO, &ta);
    ta.c_lflag &= ~(ECHO | ICANON);
    ta.c_cc[VTIME] = 0;
    ta.c_cc[VMIN] = 0;

    tcsetattr(STDIN_FILENO, TCSANOW, &ta);

    printf("\x1b[?1000h");
    printf("\x1b[?1006h");

    while (true)
    {
        int len = read(STDIN_FILENO, chars, sizeof(chars));

        if (len > 0)
        {
            gotChars = true;

            for (int i = 0; i < len; ++i)
            {
                if (isprint(chars[i]))
                {
                    putchar(chars[i]);
                    if (chars[i] == 'q')
                    {
                        printf("\n\x1b[?1000l");
                        ta.c_lflag |= ECHO | ICANON;
                        tcsetattr(STDIN_FILENO, TCSANOW, &ta);
                        return;
                    }
                }
                else
                    printf("\\x%02x", static_cast<uint8_t>(chars[i]));
            }
        }
        else
        {
            if (gotChars)
            {
                printf("\n");
                gotChars = false;
            }

            usleep(10000);
        }
    }
#else
    wint_t ch;

    do
    {
        ch = _getwch();
        _cwprintf(L"%c \\x%02x\n", ch, static_cast<uint8_t>(ch));
    }
    while (ch != 0x1b);
#endif
}

void printPlatformInfo()
{
    Console::write(STR("architecture:"));

#ifdef ARCH_32BIT
    Console::write(STR(" 32 bit"));
#endif
#ifdef ARCH_64BIT
    Console::write(STR(" 64 bit"));
#endif

#ifdef ARCH_INTEL
    Console::writeLine(STR(" Intel"));
#endif
#ifdef ARCH_ARM
    Console::writeLine(STR(" ARM"));
#endif
#ifdef ARCH_SPARC
    Console::writeLine(STR(" SPARC"));
#endif
#ifdef ARCH_POWER
    Console::writeLine(STR(" Power"));
#endif

    Console::write(STR("platform:"));
#ifdef PLATFORM_WINDOWS
    Console::write(STR(" Windows"));
#endif
#ifdef PLATFORM_APPLE
    Console::write(STR(" Apple"));
#endif
#ifdef PLATFORM_LINUX
    Console::write(STR(" Linux"));
#endif
#ifdef PLATFORM_SOLARIS
    Console::write(STR(" Solaris"));
#endif
#ifdef PLATFORM_AIX
    Console::write(STR(" AIX"));
#endif
#ifdef PLATFORM_ANROID
    Console::write(STR(" Android"));
#endif
#ifdef PLATFORM_UNIX
    Console::writeLine(STR(" UNIX"));
#else
    Console::writeLine();
#endif

    Console::write(STR("compiler:"));
#ifdef COMPILER_INTEL_CPP
    Console::write(STR(" Intel C++"));
#endif
#ifdef COMPILER_CLANG
    Console::write(STR(" clang"));
#endif
#ifdef COMPILER_VISUAL_CPP
    Console::write(STR(" Visual C++"));
#endif
#ifdef COMPILER_GCC
    Console::write(STR(" gcc"));
#endif
#ifdef COMPILER_SOLARIS_STUDIO
    Console::write(STR(" Solaris Studio"));
#endif
#ifdef COMPILER_XL_CPP
    Console::write(STR(" IBM XL C/C++"));
#endif
    Console::writeLineFormatted(STR(" version %d"), COMPILER_VERSION);
}

void runTests()
{
    printPlatformInfo();

    testSupport();
    testFoundation();
    testFile();
//    testConsole();
//    testConsoleWrite();
//    testConsoleReadChar();
//    testConsoleReadLine();
//    testConsoleReadInput();
}

enum TokenType
{
    TOKEN_TYPE_NONE,
    TOKEN_TYPE_STRING,
    TOKEN_TYPE_NUMBER,
    TOKEN_TYPE_IDENT,
    TOKEN_TYPE_KEYWORD,
    TOKEN_TYPE_TYPE,
    TOKEN_TYPE_COMMENT,
    TOKEN_TYPE_PREPROCESSOR
};

void setColor(TokenType tokenType)
{
    int colors[] = { 30, 33, 91, 30, 34, 36, 90, 35 };
    Console::writeFormatted(STR("\x1b[%dm"), colors[tokenType]);
}

void testHighlighting(const String& filename)
{
    File file(filename);
    ByteBuffer bytes = file.read();

    TextEncoding encoding;
    bool bom, crLf;
    String text(Unicode::bytesToString(bytes, encoding, bom, crLf));

    Set<String> keywords;
    keywords.add(STR("alignas"));
    keywords.add(STR("alignof"));
    keywords.add(STR("and"));
    keywords.add(STR("and_eq"));
    keywords.add(STR("asm"));
    keywords.add(STR("atomic_cancel"));
    keywords.add(STR("atomic_commit"));
    keywords.add(STR("atomic_noexcept"));
    keywords.add(STR("bitand"));
    keywords.add(STR("bitor"));
    keywords.add(STR("break"));
    keywords.add(STR("case"));
    keywords.add(STR("catch"));
    keywords.add(STR("class"));
    keywords.add(STR("compl"));
    keywords.add(STR("concept"));
    keywords.add(STR("const_cast"));
    keywords.add(STR("continue"));
    keywords.add(STR("co_await"));
    keywords.add(STR("co_return"));
    keywords.add(STR("co_yield"));
    keywords.add(STR("decltype"));
    keywords.add(STR("default"));
    keywords.add(STR("delete"));
    keywords.add(STR("do"));
    keywords.add(STR("dynamic_cast"));
    keywords.add(STR("else"));
    keywords.add(STR("enum"));
    keywords.add(STR("explicit"));
    keywords.add(STR("export"));
    keywords.add(STR("extern"));
    keywords.add(STR("false"));
    keywords.add(STR("for"));
    keywords.add(STR("friend"));
    keywords.add(STR("goto"));
    keywords.add(STR("if"));
    keywords.add(STR("import"));
    keywords.add(STR("inline"));
    keywords.add(STR("module"));
    keywords.add(STR("mutable"));
    keywords.add(STR("namespace"));
    keywords.add(STR("new"));
    keywords.add(STR("noexcept"));
    keywords.add(STR("not"));
    keywords.add(STR("not_eq"));
    keywords.add(STR("nullptr"));
    keywords.add(STR("operator"));
    keywords.add(STR("or"));
    keywords.add(STR("or_eq"));
    keywords.add(STR("private"));
    keywords.add(STR("protected"));
    keywords.add(STR("public"));
    keywords.add(STR("register"));
    keywords.add(STR("reflexpr"));
    keywords.add(STR("reinterpret_cast"));
    keywords.add(STR("requires"));
    keywords.add(STR("return"));
    keywords.add(STR("sizeof"));
    keywords.add(STR("static"));
    keywords.add(STR("static_assert"));
    keywords.add(STR("static_cast"));
    keywords.add(STR("struct"));
    keywords.add(STR("switch"));
    keywords.add(STR("synchronized"));
    keywords.add(STR("template"));
    keywords.add(STR("this"));
    keywords.add(STR("thread_local"));
    keywords.add(STR("throw"));
    keywords.add(STR("true"));
    keywords.add(STR("try"));
    keywords.add(STR("typedef"));
    keywords.add(STR("typeid"));
    keywords.add(STR("typename"));
    keywords.add(STR("union"));
    keywords.add(STR("using"));
    keywords.add(STR("virtual"));
    keywords.add(STR("while"));
    keywords.add(STR("xor"));
    keywords.add(STR("xor_eq"));
    keywords.add(STR("override"));
    keywords.add(STR("final"));
    keywords.add(STR("transaction_safe"));
    keywords.add(STR("transaction_safe_dynamic"));
    keywords.add(STR("_Pragma"));

    Set<String> types;
    types.add(STR("auto"));
    types.add(STR("bool"));
    types.add(STR("byte"));
    types.add(STR("char"));
    types.add(STR("char16_t"));
    types.add(STR("char32_t"));
    types.add(STR("const"));
    types.add(STR("constexpr"));
    types.add(STR("double"));
    types.add(STR("float"));
    types.add(STR("int"));
    types.add(STR("long"));
    types.add(STR("short"));
    types.add(STR("signed"));
    types.add(STR("unsigned"));
    types.add(STR("void"));
    types.add(STR("volatile"));
    types.add(STR("wchar_t"));
    types.add(STR("int8_t"));
    types.add(STR("int16_t"));
    types.add(STR("int32_t"));
    types.add(STR("int64_t"));
    types.add(STR("uint8_t"));
    types.add(STR("uint16_t"));
    types.add(STR("uint32_t"));
    types.add(STR("uint64_t"));
    types.add(STR("intptr_t"));
    types.add(STR("uintptr_t"));
    types.add(STR("intmax_t"));
    types.add(STR("uintmax_t"));
    types.add(STR("size_t"));
    types.add(STR("ptrdiff_t"));
    types.add(STR("nullptr_t"));
    types.add(STR("max_align_t"));
    types.add(STR("unichar_t"));
    types.add(STR("char_t"));
    types.add(STR("byte_t"));

    Set<String> preprocessor;
    preprocessor.add(STR("if"));
    preprocessor.add(STR("elif"));
    preprocessor.add(STR("else"));
    preprocessor.add(STR("endif"));
    preprocessor.add(STR("defined"));
    preprocessor.add(STR("ifdef"));
    preprocessor.add(STR("ifndef"));
    preprocessor.add(STR("define"));
    preprocessor.add(STR("undef"));
    preprocessor.add(STR("include"));
    preprocessor.add(STR("line"));
    preprocessor.add(STR("error"));
    preprocessor.add(STR("pragma"));

    int p = 0;
    unichar_t ch = 0;
    String ident;

    while (p < text.length())
    {
        ch = text.charAt(p);

        if (ch == '"' || ch == '\'')
        {
            unichar_t quote = ch, prevCh = 0;
            bool escape;

            setColor(TOKEN_TYPE_STRING);

            do
            {
                escape = prevCh != '\\' && ch == '\\';
                Console::write(ch);

                p = text.charForward(p);
                if (p < text.length())
                {
                    prevCh = ch;
                    ch = text.charAt(p);
                }
                else
                    break;
            }
            while (ch != quote || escape);

            Console::write(ch);
            setColor(TOKEN_TYPE_NONE);
            p = text.charForward(p);
        }
        else if (charIsDigit(ch))
        {
            setColor(TOKEN_TYPE_NUMBER);

            do
            {
                Console::write(ch);
                p = text.charForward(p);
                if (p < text.length())
                    ch = text.charAt(p);
                else
                    break;
            }
            while (charIsDigit(ch) || ch == 'x' || ch == 'X' ||
                ch == 'a' || ch == 'A' || ch == 'b' || ch == 'B' ||
                ch == 'c' || ch == 'C' || ch == 'd' || ch == 'D' ||
                ch == 'e' || ch == 'E' || ch == 'f' || ch == 'F' ||
                ch == '.' || ch == '+' || ch == '-');

            setColor(TOKEN_TYPE_NONE);
        }
        else if (charIsAlphaNum(ch) || ch == '_')
        {
            int s = p;

            do
            {
                p = text.charForward(p);
                if (p < text.length())
                    ch = text.charAt(p);
                else
                    break;
            }
            while (charIsAlphaNum(ch) || charIsDigit(ch) || ch == '_');

            ident = text.substr(s, p - s);

            if (keywords.contains(ident))
                setColor(TOKEN_TYPE_KEYWORD);
            else if (types.contains(ident))
                setColor(TOKEN_TYPE_TYPE);
            else
                setColor(TOKEN_TYPE_IDENT);

            Console::write(ident);
            setColor(TOKEN_TYPE_NONE);
        }
        else if (ch == '#')
        {
            int s = p;

            if (p < text.length())
            {
                p = text.charForward(p);
                ch = text.charAt(p);
                int q = p;

                while (charIsAlpha(ch))
                {
                    p = text.charForward(p);
                    if (p < text.length())
                        ch = text.charAt(p);
                    else
                        break;
                }

                ident = text.substr(q, p - q);

                if (preprocessor.contains(ident))
                {
                    char_t prevCh = 0;

                    while (p < text.length() && (ch != '\n' || prevCh == '\\'))
                    {
                        prevCh = ch;
                        p = text.charForward(p);
                        ch = text.charAt(p);
                    }

                    setColor(TOKEN_TYPE_PREPROCESSOR);
                    p = text.charForward(p);
                    Console::write(text.chars() + s, p - s);
                    setColor(TOKEN_TYPE_NONE);
                }
                else
                {
                    Console::write('#');
                    p = q;
                }
            }
        }
        else if (ch == '/')
        {
            if (p < text.length())
            {
                p = text.charForward(p);
                ch = text.charAt(p);

                if (ch == '*')
                {
                    setColor(TOKEN_TYPE_COMMENT);
                    Console::write(STR("/*"));

                    if (p < text.length())
                    {
                        char_t prevCh = 0;
                        p = text.charForward(p);
                        ch = text.charAt(p);

                        do
                        {
                            Console::write(ch);

                            p = text.charForward(p);
                            if (p < text.length())
                            {
                                prevCh = ch;
                                ch = text.charAt(p);
                            }
                            else
                                break;
                        }
                        while (!(prevCh == '*' && ch == '/'));

                        Console::write('/');
                        p = text.charForward(p);
                    }

                    setColor(TOKEN_TYPE_NONE);
                }
                else if (ch == '/')
                {
                    setColor(TOKEN_TYPE_COMMENT);
                    Console::write(STR("//"));

                    if (p < text.length())
                    {
                        p = text.charForward(p);
                        ch = text.charAt(p);

                        do
                        {
                            Console::write(ch);

                            p = text.charForward(p);
                            if (p < text.length())
                                ch = text.charAt(p);
                            else
                                break;
                        }
                        while (ch != '\n');

                        Console::write('\n');
                        p = text.charForward(p);
                    }

                    setColor(TOKEN_TYPE_NONE);
                }
                else
                    Console::write('/');
            }
        }
        else
        {
            Console::write(ch);
            p = text.charForward(p);
        }
    }

    Console::write(STR("\x1b[m\n"));
}

int MAIN(int argc, const char_t** argv)
{
    try
    {
        if (argc > 1)
            testHighlighting(argv[1]);
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
