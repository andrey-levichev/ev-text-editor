#ifndef FOUNDATION_INCLUDED
#define FOUNDATION_INCLUDED

#include <new>
#include <stdarg.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include <ctype.h>

// 32/64 bit

#if defined(__x86_64) || defined(_M_X64) || defined(__sparcv9) || defined(_ARCH_PPC64)
#define ARCH_64BIT
#else
#define ARCH_32BIT
#endif

// processor architecture

#if defined(__sparc) || defined(__sparcv9)

#define ARCH_SPARC
#define ARCH_BIG_ENDIAN

#elif defined(_ARCH_PPC) || defined(_ARCH_PPC64)

#define ARCH_POWER
#define ARCH_BIG_ENDIAN

#elif defined(__i386) || defined(__x86_64) || defined(_M_IX86) ||defined(_M_X64)

#define ARCH_INTEL
#define ARCH_LITTLE_ENDIAN

#elif defined(__arm__) || defined(_M_ARM)

#define ARCH_ARM
#define ARCH_LITTLE_ENDIAN

#endif

// platform

#if defined(_WIN32)
#define PLATFORM_WINDOWS
#elif defined(__linux__)
#define PLATFORM_LINUX
#elif defined(__APPLE__)
#define PLATFORM_APPLE
#elif defined(__sun)
#define PLATFORM_SOLARIS
#elif defined(_AIX)
#define PLATFORM_AIX
#elif defined(__ANDROID__)
#define PLATFORM_ANDRIOD
#endif

#ifdef __unix
#define PLATFORM_UNIX
#endif

// compiler and version

#if defined(__ICL) || defined(__ICC)

// Intel C++

#define COMPILER_INTEL_CPP
#ifdef __ICL
#define COMPILER_VERSION __ICL
#else
#define COMPILER_VERSION __ICC
#endif

#elif defined(__clang__)

// clang

#define COMPILER_CLANG
#define COMPILER_VERSION (__clang_major__ * 100 + __clang_minor__)

#elif defined(_MSC_VER)

// Visual C++

#define COMPILER_VISUAL_CPP
#define COMPILER_VERSION _MSC_VER

#elif defined(__GNUC__)

// gcc

#define COMPILER_GCC
#define COMPILER_VERSION (__GNUC__ * 100 + __GNUC_MINOR__)

#elif defined(__SUNPRO_CC)

// Solaris Studio

#define COMPILER_SOLARIS_STUDIO
#define COMPILER_VERSION __SUNPRO_CC

#elif defined(__IBMCPP__)

// IBM XL C/C++

#define COMPILER_XL_CPP
#define COMPILER_VERSION __IBMCPP__

#endif

// platform specific includes and macros

#ifdef PLATFORM_WINDOWS

#include <windows.h>
#include <io.h>
#include <fcntl.h>

#define MAIN wmain
#define STR(arg) L##arg
typedef wchar_t char_t;

#define STRLEN wcslen
#define STRCPY wcscpy
#define STRSET wmemset
#define STRCMP wcscmp
#define STRNCMP wcsncmp
#define STRICMP _wcsicmp
#define STRNICMP _wcsnicmp
#define STRSTR wcsstr
#define STRISTR wcscasestr
#define STRTOL wcstol
#define STRTOUL wcstoul
#define STRTOLL wcstoll
#define STRTOULL wcstoull
#define STRTOF wcstof
#define STRTOD wcstod
#define ISSPACE iswspace
#define ISPRINT iswprint
#define ISALNUM iswalnum
#define TOLOWER towlower
#define TOUPPER towupper
#define FPUTS fputws
#define PUTS _putws
#define PRINTF wprintf
#define SPRINTF swprintf
#define VPRINTF vwprintf
#define PUTCHAR putwchar
#define GETCHAR getwchar
#define CHAR_EOF WEOF

#else

#include <alloca.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define MAIN main
#define STR(arg) arg
typedef char char_t;

#define STRLEN strlen
#define STRCPY strcpy
#define STRSET memset
#define STRCMP strcmp
#define STRNCMP strncmp
#define STRICMP strcasecmp
#define STRNICMP strncasecmp
#define STRSTR strstr
#define STRISTR strcasestr
#define STRTOL strtol
#define STRTOUL strtoul
#define STRTOLL strtoll
#define STRTOULL strtoull
#define STRTOF strtof
#define STRTOD strtod
#define ISSPACE isspace
#define ISPRINT isprint
#define ISALNUM isalnum
#define TOLOWER tolower
#define TOUPPER toupper
#define FPUTS fputs
#define PUTS puts
#define PRINTF printf
#define SPRINTF snprintf
#define VPRINTF vprintf
#define PUTCHAR putchar
#define GETCHAR getchar
#define CHAR_EOF EOF

#endif

// assert macros

#define TO_STR(arg) #arg
#define STR_MACRO(arg) STR(arg)
#define NUM_MACRO(arg) STR_MACRO(TO_STR(arg))

#ifdef ABORT_ON_ASSERT_FAILURE

#define ASSERT_MSG(condition, message) \
    do { \
        if (!(condition)) { \
            PUTS(STR("assertion failed in ") \
                STR_MACRO(__FILE__) STR(" at line ") NUM_MACRO(__LINE__) STR(": ") message); \
            abort(); \
        } \
    } while (false)
    
#else
    
#define ASSERT_MSG(condition, message) \
    do { \
        if (!(condition)) \
            throw Exception(STR("assertion failed in ") \
                STR_MACRO(__FILE__) STR(" at line ") NUM_MACRO(__LINE__) STR(": ") message); \
    } while (false)

#endif

#define ASSERT(...) ASSERT_MSG(__VA_ARGS__, STR(#__VA_ARGS__))
#define ASSERT_FAIL(message) ASSERT_MSG(false, message)

#define ASSERT_EXCEPTION(exception, ...) \
    do { \
        try { \
            __VA_ARGS__; \
            ASSERT_FAIL(STR("expected ") STR(#exception)); \
        } \
        catch (exception&) {} \
        catch (...) { \
            ASSERT_FAIL(STR("expected ") STR(#exception)); \
        } \
    } while(false)

#define ASSERT_NO_EXCEPTION(...) \
    do { \
        try { __VA_ARGS__; } \
        catch (...) { \
            ASSERT_FAIL(STR("unexpected exception")); \
        } \
    } while (false)

#define ASSERT_EXCEPTION_MSG(exception, msg, ...) \
    do { \
        try { \
            __VA_ARGS__; \
            ASSERT_FAIL(STR("expected ") STR(#exception)); \
        } \
        catch (exception& ex) \
        { \
            ASSERT_MSG(STRSTR(ex.message(), msg) != nullptr, \
                STR(#exception) STR(" doesn't contain expected message")); \
        } \
        catch (...) { \
            ASSERT_FAIL(STR("expected ") STR(#exception)); \
        } \
    } while(false)

// string helper functions

inline char_t* STRNCPY(char_t* destStr, const char_t* srcStr, int len)
{
    memcpy(destStr, srcStr, len * sizeof(char_t));
    return destStr + len;
}

inline char_t* STRMOVE(char_t* destStr, const char_t* srcStr, int len)
{
    memmove(destStr, srcStr, len * sizeof(char_t));
    return destStr + len;
}

#ifdef PLATFORM_WINDOWS

inline const wchar_t* wcscasestr(const wchar_t* str, const wchar_t* searchStr)
{
    int index = FindNLSStringEx(LOCALE_NAME_USER_DEFAULT, FIND_FROMSTART | LINGUISTIC_IGNORECASE, 
        str, -1, searchStr, -1, nullptr, nullptr, nullptr, 0);

    return index >= 0 ? str + index : nullptr;
}

#endif

// equality function

template<typename _Type>
inline bool equalsTo(const _Type& left, const _Type& right)
{
    return left == right;
}

// hash function

template<typename _Type1, typename _Type2>
inline int hash(const _Type1& val1, const _Type2& val2);

inline int hash(bool val)
{
    return val;
}

inline int hash(int val)
{
    return val;
}

inline int hash(unsigned val)
{
    return *reinterpret_cast<int*>(&val);
}

inline int hash(long val)
{
    if (sizeof(long) == 8)
    {
        int* h = reinterpret_cast<int*>(&val);
        return hash(*h, *(h + 1));
    }
    else
        return *reinterpret_cast<int*>(&val);
}

inline int hash(unsigned long val)
{
    if (sizeof(unsigned long) == 8)
    {
        int* h = reinterpret_cast<int*>(&val);
        return hash(*h, *(h + 1));
    }
    else
        return *reinterpret_cast<int*>(&val);
}

inline int hash(long long val)
{
    int* h = reinterpret_cast<int*>(&val);
    return hash(*h, *(h + 1));
}

inline int hash(unsigned long long val)
{
    int* h = reinterpret_cast<int*>(&val);
    return hash(*h, *(h + 1));
}

inline int hash(float val)
{
    return *reinterpret_cast<int*>(&val);
}

inline int hash(double val)
{
    int* h = reinterpret_cast<int*>(&val);
    return hash(*h, *(h + 1));
}

inline int hash(const char_t* val)
{
    int h = 0;

    while (*val)
        h = 33 * h + *val++;

    return h;
}

inline int hash(const void* val)
{
    return hash(*reinterpret_cast<intptr_t*>(&val));
}

template<typename _Type1, typename _Type2>
inline int hash(const _Type1& val1, const _Type2& val2)
{
    return 33 * hash(val1) + hash(val2);
}

// Exception

class Exception
{
public:
    Exception() : _message(STR("operation failed"))
    {
    }

    Exception(const char_t* message) : _message(message)
    {
    }

    virtual ~Exception()
    {
    }

    const char_t* message() const
    {
        return _message;
    }

protected:
    const char_t* _message;
};

// OutOfMemoryException

class OutOfMemoryException : public Exception
{
public:
    OutOfMemoryException() : Exception(STR("out of memory"))
    {
    }
};

// NullPointerException

class NullPointerException : public Exception
{
public:
    NullPointerException() : Exception(STR("null pointer"))
    {
    }
};

// Memory

namespace Memory
{

template<typename _Type>
inline _Type* allocate()
{
    _Type* ptr = static_cast<_Type*>(malloc(sizeof(_Type)));

    if (ptr)
        return ptr;
    else
        throw OutOfMemoryException();
}

template<typename _Type>
inline _Type* allocate(int size)
{
    _Type* ptr = static_cast<_Type*>(malloc(sizeof(_Type) * size));

    if (ptr)
        return ptr;
    else
        throw OutOfMemoryException();
}

template<typename _Type>
inline _Type* reallocate(_Type* ptr, int size)
{
    ptr = static_cast<_Type*>(realloc(ptr, sizeof(_Type) * size));

    if (ptr)
        return ptr;
    else
        throw OutOfMemoryException();
}

inline void deallocate(void* ptr)
{
    free(ptr);
}

template<typename _Type, typename... _Args>
inline _Type* create(_Args&&... args)
{
    _Type* ptr = allocate<_Type>();

    try
    {
        ::new(ptr) _Type(static_cast<_Args&&>(args)...);
    }
    catch (...)
    {
        deallocate(ptr);
        throw;
    }

    return ptr;
}

template<typename _Type>
inline void destroy(_Type* ptr)
{
    if (ptr)
    {
        ptr->~_Type();
        deallocate(ptr);
    }
}

template<typename _Type>
inline _Type* createArrayCopy(int size, const _Type* elements)
{
    _Type* ptr = allocate<_Type>(size);

    int i = 0;

    try
    {
        for (; i < size; ++i)
            ::new(ptr + i) _Type(elements[i]);
    }
    catch (...)
    {
        while (i-- > 0)
            ptr[i].~_Type();

        deallocate(ptr);
        throw;
    }

    return ptr;
}

template<typename _Type>
inline _Type* createArrayResize(int size, _Type* elements, int newSize)
{
    _Type* ptr = allocate<_Type>(newSize);

    int i = 0;

    try
    {
        if (newSize > size)
        {
            for (; i < size; ++i)
                ::new(ptr + i) _Type(static_cast<_Type&&>(elements[i]));
            for (; i < newSize; ++i)
                ::new(ptr + i) _Type();
        }
        else
        {
            for (; i < newSize; ++i)
                ::new(ptr + i) _Type(static_cast<_Type&&>(elements[i]));
        }
    }
    catch (...)
    {
        while (i-- > 0)
            ptr[i].~_Type();

        deallocate(ptr);
        throw;
    }

    return ptr;
}

template<typename _Type, typename... _Args>
inline _Type* createArrayFill(int size, _Args&&... args)
{
    _Type* ptr = allocate<_Type>(size);

    int i = 0;

    try
    {
        for (; i < size; ++i)
            ::new(ptr + i) _Type(static_cast<_Args&&>(args)...);
    }
    catch (...)
    {
        while (i-- > 0)
            ptr[i].~_Type();

        deallocate(ptr);
        throw;
    }

    return ptr;
}

template<typename _Type>
inline void destroyArray(int size, _Type* elements)
{
    if (elements)
    {
        for (int i = 0; i < size; ++i)
            elements[i].~_Type();

        deallocate(elements);
    }
}

};

// swap

template<typename _Type>
void swap(_Type& left, _Type& right)
{
    _Type tmp(static_cast<_Type&&>(left));
    left = static_cast<_Type&&>(right);
    right = static_cast<_Type&&>(tmp);
}

// UniquePtr

template<typename _Type>
class UniquePtr
{
public:
    UniquePtr()
        : _ptr(nullptr)
    {
    }

    UniquePtr(const UniquePtr<_Type>&) = delete;

    UniquePtr(UniquePtr<_Type>&& other)
    {
        _ptr = other._ptr;
        other._ptr = nullptr;
    }

    ~UniquePtr()
    {
        Memory::destroy(_ptr);
    }

    UniquePtr& operator=(const UniquePtr<_Type>&) = delete;

    UniquePtr& operator=(UniquePtr<_Type>&& other)
    {
        Memory::destroy(_ptr);
        _ptr = other._ptr;
        other._ptr = nullptr;
        return *this;
    }

    _Type& operator*() const
    {
        if (_ptr)
            return *_ptr;
        else
            throw NullPointerException();
    }

    _Type* operator->() const
    {
        if (_ptr)
            return _ptr;
        else
            throw NullPointerException();
    }

    operator bool()
    {
        return _ptr != nullptr;
    }

    _Type* ptr() const
    {
        return _ptr;
    }

    template<typename... _Args>
    void create(_Args&&... args)
    {
        Memory::destroy(_ptr);
        _ptr = Memory::create<_Type>(static_cast<_Args&&>(args)...);
    }

    void reset()
    {
        Memory::destroy(_ptr);
        _ptr = nullptr;
    }

    friend void swap(UniquePtr<_Type>& left, UniquePtr<_Type>& right)
    {
        swap(left._ptr, right._ptr);
    }

    template<typename _T, typename... _Args>
    friend UniquePtr<_T> createUnique(_Args&&... args);

    friend bool operator==(const UniquePtr<_Type>& left, const UniquePtr<_Type>& right)
    {
        return left._ptr == right._ptr;
    }

    friend bool operator!=(const UniquePtr<_Type>& left, const UniquePtr<_Type>& right)
    {
        return left._ptr != right._ptr;
    }
    
    template<typename _T>
    friend int hash(const UniquePtr<_T>& val);

private:
    UniquePtr(_Type* ptr)
        : _ptr(ptr)
    {
    }

private:
    _Type* _ptr;
};

template<typename _Type, typename... _Args>
inline UniquePtr<_Type> createUnique(_Args&&... args)
{
    return UniquePtr<_Type>(Memory::create<_Type>(static_cast<_Args&&>(args)...));
}

template<typename _Type>
inline int hash(const UniquePtr<_Type>& val)
{
    return hash(*reinterpret_cast<const intptr_t*>(&val._ptr));
}

// SharedPtr

template<typename _Type>
class SharedPtr
{
public:
    SharedPtr() :
        _sharedPtr(nullptr)
    {
    }

    SharedPtr(const SharedPtr<_Type>& other)
    {
        _sharedPtr = other._sharedPtr;
        if (_sharedPtr)
            addRef();
    }

    ~SharedPtr()
    {
        if (_sharedPtr)
            releaseRef();
    }

    SharedPtr<_Type>& operator=(const SharedPtr<_Type>& other)
    {
        if (_sharedPtr)
            releaseRef();

        _sharedPtr = other._sharedPtr;

        if (_sharedPtr)
            addRef();

        return *this;
    }

    _Type& operator*() const
    {
        if (_sharedPtr)
            return *reinterpret_cast<_Type*>(_sharedPtr);
        else
            throw NullPointerException();
    }

    _Type* operator->() const
    {
        if (_sharedPtr)
            return reinterpret_cast<_Type*>(_sharedPtr);
        else
            throw NullPointerException();
    }

    operator bool()
    {
        return _sharedPtr != nullptr;
    }

    _Type* ptr() const
    {
        if (_sharedPtr)
            return reinterpret_cast<_Type*>(_sharedPtr);
        else
            return nullptr;
    }

    int refCount() const
    {
        if (_sharedPtr)
            return _sharedPtr->refCount;
        else
            return 0;
    }

    template<typename... _Args>
    void create(_Args&&... args)
    {
        if (_sharedPtr)
            releaseRef();

        _sharedPtr = Memory::create<RefCountedObject>(static_cast<_Args&&>(args)...);
    }

    void reset()
    {
        if (_sharedPtr)
            releaseRef();

        _sharedPtr = nullptr;
    }

    friend void swap(SharedPtr<_Type>& left, SharedPtr<_Type>& right)
    {
        swap(left._sharedPtr, right._sharedPtr);
    }

    template<typename _T, typename... _Args>
    friend SharedPtr<_T> createShared(_Args&&... args);

    friend bool operator==(const SharedPtr<_Type>& left, const SharedPtr<_Type>& right)
    {
        return left._sharedPtr == right._sharedPtr;
    }

    friend bool operator!=(const SharedPtr<_Type>& left, const SharedPtr<_Type>& right)
    {
        return left._sharedPtr != right._sharedPtr;
    }
    
    template<typename _T>
    friend int hash(const SharedPtr<_T>& val);

private:
    struct RefCountedObject
    {
        _Type object;
        int refCount;

        template<typename... _Args>
        RefCountedObject(_Args&&... args) :
            object(static_cast<_Args&&>(args)...)
        {
            refCount = 1;
        }
    };

private:
    SharedPtr(RefCountedObject* sharedPtr)
        : _sharedPtr(sharedPtr)
    {
    }

    void addRef()
    {
        ++_sharedPtr->refCount;
    }

    void releaseRef()
    {
        if (--_sharedPtr->refCount == 0)
            Memory::destroy(_sharedPtr);
    }

private:
    RefCountedObject* _sharedPtr;
};

template<typename _Type, typename... _Args>
inline SharedPtr<_Type> createShared(_Args&&... args)
{
    return SharedPtr<_Type>(Memory::create<typename SharedPtr<_Type>::RefCountedObject>(static_cast<_Args&&>(args)...));
}

template<typename _Type>
inline int hash(const SharedPtr<_Type>& val)
{
    return hash(*reinterpret_cast<const intptr_t*>(&val._sharedPtr));
}

// String

class String
{
public:
    static const int INVALID_POS = -1;

public:
    String() :
        _length(0),
        _capacity(0),
        _chars(nullptr)
    {
    }

    String(int count, char_t c);
    String(const String& other);
    String(const String& other, int pos, int len);
    String(const char_t* chars);
    String(const char_t* chars, int pos, int len);

    explicit String(int capacity);

    String(String&& other)
    {
        _length = other._length;
        _capacity = other._capacity;
        _chars = other._chars;

        other._length = 0;
        other._capacity = 0;
        other._chars = nullptr;
    }

    ~String();

    String& operator=(const String& other);
    String& operator=(const char_t* chars);
    String& operator=(String&& other);
    
    String& operator+=(const String& str);
    String& operator+=(const char_t* chars);

    int length() const
    {
        return _length;
    }

    int capacity() const
    {
        return _capacity;
    }

    const char_t* str() const
    {
        return _chars ? _chars : STR("");
    }
    
    char_t* chars()
    {
        return _chars;
    }

    bool empty() const
    {
        return _length == 0;
    }

    String substr(int pos, int len) const;

    void ensureCapacity(int capacity);
    void shrinkToLength();

    void assign(const String& str);
    void assign(const char_t* chars);
    
    void append(const String& str);
    void append(const char_t* chars);
    void appendFormat(const char_t* format, ...);
    void appendFormat(const char_t* format, va_list args);

    void insert(int pos, const String& str);
    void insert(int pos, const char_t* chars);
    
    void erase(int pos, int len);
    void erase(const String& str);
    void erase(const char_t* chars);
    
    void clear();
    void reset();
    
    static String acquire(char_t* chars)
    {
        return String(chars);
    }
    
    char_t* release();

    void replace(int pos, int len, const String& str);
    void replace(int pos, int len, const char_t* chars);
    void replace(const String& searchStr, const String& replaceStr);
    void replace(const char_t* searchChars, const char_t* replaceChars);
    
    void trim();
    void trimRight();
    void trimLeft();
    void reverse();
    void toUpper();
    void toLower();

    int compare(const String& str) const;
    int compare(const char_t* chars) const;
    int compareNoCase(const String& str) const;
    int compareNoCase(const char_t* chars) const;

    int find(const String& str, int pos = 0) const;
    int find(const char_t* chars, int pos = 0) const;
    int findNoCase(const String& str, int pos = 0) const;
    int findNoCase(const char_t* chars, int pos = 0) const;
    
    bool startsWith(const String& str) const;
    bool startsWith(const char_t* chars) const;
    
    bool endsWith(const String& str) const;
    bool endsWith(const char_t* chars) const;
    
    bool contains(const String& str) const;
    bool contains(const char_t* chars) const;
    
    template<typename... _Args>
    static String concat(_Args&&... args)
    {
        char_t* chars = concatInternal(0, args...);
        return String(chars);
    }
    
    // conversion from string

    bool toBool() const;
    int toInt() const;
    int32_t toInt32() const;
    uint32_t toUInt32() const;
    int64_t toInt64() const;
    uint64_t toUInt64() const;
    float toFloat() const;
    double toDouble() const;

    // conversion to string

    static String from(bool value);
    static String from(int value);
    static String from(unsigned value);
    static String from(long value);
    static String from(unsigned long value);
    static String from(long long value);
    static String from(unsigned long long value);
    static String from(float value, int precision);
    static String from(double value, int precision);
    
    // formatting

    static String format(const char_t* format, ...);
    static String format(const char_t* format, va_list args);
    
    friend void swap(String& left, String& right)
    {
        swap(left._length, right._length);
        swap(left._capacity, right._capacity);
        swap(left._chars, right._chars);
    }

    friend int hash(const String& val)
    {
        return hash(val.str());
    }

private:
    explicit String(char_t* chars);
    
    template<typename... _Args>
    static char_t* concatInternal(int totalLen, const char_t* chars, _Args&&... args)
    {
        int len = STRLEN(chars);
        char_t* destChars = concatInternal(totalLen + len, args...);
        STRNCPY(destChars + totalLen, chars, len);
        return destChars;
    }
    
    static char_t* concatInternal(int totalLen, const char_t* chars)
    {
        int len = STRLEN(chars);
        char_t* destChars = Memory::allocate<char_t>(totalLen + len + 1);
        destChars[totalLen + len] = 0;        
        STRNCPY(destChars + totalLen, chars, len);
        return destChars;
    }
    
    template<typename... _Args>
    static char_t* concatInternal(int totalLen, const String& str, _Args&&... args)
    {
        if (str._chars)
        {
            int len = STRLEN(str._chars);
            char_t* destChars = concatInternal(totalLen + len, args...);
            STRNCPY(destChars + totalLen, str._chars, len);
            return destChars;
        }
        else
            return concatInternal(totalLen, args...);
    }
    
    static char_t* concatInternal(int totalLen, const String& str)
    {
        if (str._chars)
        {
            int len = STRLEN(str._chars);
            char_t* destChars = Memory::allocate<char_t>(totalLen + len + 1);
            destChars[totalLen + len] = 0;        
            STRNCPY(destChars + totalLen, str._chars, len);
            return destChars;
        }
        else
        {
            char_t* destChars = Memory::allocate<char_t>(totalLen + 1);
            destChars[totalLen] = 0;        
            return destChars;
        }
    }

private:
    int _length;
    int _capacity;
    char_t* _chars;
};

// string concatenation

String operator+(const String& left, const String& right);
String operator+(const String& left, const char_t* right);
String operator+(const char_t* left, const String& right);

// string compare

bool operator==(const String& left, const String& right);
bool operator!=(const String& left, const String& right);
bool operator<(const String& left, const String& right);
bool operator<=(const String& left, const String& right);
bool operator>(const String& left, const String& right);
bool operator>=(const String& left, const String& right);
bool operator==(const char_t* left, const String& right);
bool operator==(const String& left, const char_t* right);
bool operator!=(const char_t* left, const String& right);
bool operator!=(const String& left, const char_t* right);
bool operator<(const char_t* left, const String& right);
bool operator<(const String& left, const char_t* right);
bool operator<=(const char_t* left, const String& right);
bool operator<=(const String& left, const char_t* right);
bool operator>(const char_t* left, const String& right);
bool operator>(const String& left, const char_t* right);
bool operator>=(const char_t* left, const String& right);
bool operator>=(const String& left, const char_t* right);

// Array

template<typename>
class ArrayIterator;

template<typename>
class ConstArrayIterator;

template<typename _Type>
class Array
{
public:
    template<typename>
    friend class ArrayIterator;

    template<typename>
    friend class ConstArrayIterator;

    typedef ArrayIterator<_Type> Iterator;
    typedef ConstArrayIterator<_Type> ConstIterator;

public:
    Array() : 
        _size(0), _elements(nullptr)
    {
    }

    template<typename... _Args>
    Array(int size, _Args&&... args)
    {
        if (size > 0)
        {
            _size = size;
            _elements = Memory::createArrayFill<_Type>(_size, static_cast<_Args&&>(args)...);
        }
        else if (size == 0)
        {
            _size = 0;
            _elements = nullptr;
        }
    }
    
    Array(int size, const _Type* elements)
    {
        _size = size;
        _elements = Memory::createArrayCopy(size, elements);
    }

    Array(const Array<_Type>& other)
    {
        _size = other._size;
        _elements = Memory::createArrayCopy(_size, other._elements);
    }

    Array(Array<_Type>&& other)
    {
        _size = other._size;
        _elements = other._elements;
        other._size = 0;
        other._elements = nullptr;
    }

    ~Array()
    {
        Memory::destroyArray(_size, _elements);
    }

    Array<_Type>& operator=(const Array<_Type>& other)
    {
        Array<_Type> tmp(other);
        swap(*this, tmp);
        return *this;
    }

    Array<_Type>& operator=(Array<_Type>&& other)
    {
        Memory::destroyArray(_size, _elements);

        _size = other._size;
        _elements = other._elements;
        other._size = 0;
        other._elements = nullptr;

        return *this;
    }

    _Type& operator[](int index)
    {
        return _elements[index];
    }

    const _Type& operator[](int index) const
    {
        return _elements[index];
    }

    _Type* elements()
    {
        return _elements;
    }

    const _Type* elements() const
    {
        return _elements;
    }

    int size() const
    {
        return _size;
    }

    bool empty() const
    {
        return _size == 0;
    }

    void clear()
    {
        Memory::destroyArray(_size, _elements);
        _size = 0;
        _elements = nullptr;
    }
    
    static Array<_Type> acquire(int size, _Type* elements)
    {
        return Array<_Type>(size, elements);
    }
    
    _Type* release()
    {
        _Type* elements = _elements;
        _size = 0;
        _elements = nullptr;
        
        return elements;
    }

    void resize(int size)
    {
        Array<_Type> tmp(*this, size);
        swap(*this, tmp);
    }

    friend void swap(Array<_Type>& left, Array<_Type>& right)
    {
        swap(left._size, right._size);
        swap(left._elements, right._elements);
    }

private:
    Array(int size, _Type* elements) :
        _size(size), _elements(elements)
    {
    }
    
    Array(Array<_Type>& other, int size)
    {
        _size = size;
        _elements = Memory::createArrayResize(other._size, other._elements, size);
    }

private:
    int _size;
    _Type* _elements;
};

// ArrayIterator

template<typename _Type>
class ArrayIterator
{
public:
    ArrayIterator(Array<_Type>& array) : _array(array), _index(-1)
    {
    }

    _Type& value() const
    {
        if (_index >= 0)
            return _array._elements[_index];
        else
            throw Exception(STR("no current value"));
    }

    bool moveNext()
    {
        if (_index < 0)
            _index = 0;
        else
            ++_index;

        return _index < _array._size;
    }

    bool movePrev()
    {
        if (_index < 0)
            _index = _array._size - 1;
        else
            --_index;

        return _index >= 0;
    }

private:
    Array<_Type>& _array;
    int _index;
};

// ConstArrayIterator

template<typename _Type>
class ConstArrayIterator
{
public:
    ConstArrayIterator(const Array<_Type>& array) : _array(array), _index(-1)
    {
    }

    const _Type& value() const
    {
        if (_index >= 0)
            return _array._elements[_index];
        else
            throw Exception(STR("no current value"));
    }

    bool moveNext()
    {
        if (_index < 0)
            _index = 0;
        else
            ++_index;

        return _index < _array._size;
    }

    bool movePrev()
    {
        if (_index < 0)
            _index = _array._size - 1;
        else
            --_index;

        return _index >= 0;
    }

private:
    const Array<_Type>& _array;
    int _index;
};

// ListNode

template<typename _Type>
struct ListNode
{
    _Type value;
    ListNode<_Type>* prev;
    ListNode<_Type>* next;

    ListNode(const _Type& value, ListNode<_Type>* prev, ListNode<_Type>* next) :
        value(value), prev(prev), next(next)
    {
    }

    ListNode(_Type&& value, ListNode<_Type>* prev, ListNode<_Type>* next) :
        value(static_cast<_Type&&>(value)), prev(prev), next(next)
    {
    }
};

// List

template<typename>
class ListIterator;

template<typename>
class ConstListIterator;

template<typename _Type>
class List
{
public:
    template<typename>
    friend class ListIterator;

    template<typename>
    friend class ConstListIterator;

    typedef ListIterator<_Type> Iterator;
    typedef ConstListIterator<_Type> ConstIterator;

public:
    List() :
        _front(nullptr), _back(nullptr)
    {
    }

    List(const List<_Type>& other) :
        _front(nullptr), _back(nullptr)
    {
        try
        {
            for (auto node = other.front(); node; node = node->next)
                pushBack(node->value);
        }
        catch (...)
        {
            destroyNodes();
            throw;
        }
    }

    List(List<_Type>&& other)
    {
        _front = other._front;
        _back = other._back;
        other._front = nullptr;
        other._back = nullptr;
    }

    ~List()
    {
        destroyNodes();
    }

    List<_Type>& operator=(const List<_Type>& other)
    {
        List<_Type> tmp(other);
        swap(*this, tmp);
        return *this;
    }

    List<_Type>& operator=(List<_Type>&& other)
    {
        destroyNodes();

        _front = other._front;
        _back = other._back;
        other._front = nullptr;
        other._back = nullptr;

        return *this;
    }

    int size() const
    {
        int cnt = 0;

        for (auto node = _front; node; node = node->next)
            ++cnt;

        return cnt;
    }

    bool empty() const
    {
        return _front == nullptr;
    }

    ListNode<_Type>* front()
    {
        return _front;
    }

    const ListNode<_Type>* front() const
    {
        return _front;
    }

    ListNode<_Type>* back()
    {
        return _back;
    }

    const ListNode<_Type>* back() const
    {
        return _back;
    }

    void popFront()
    {
        if (_front)
        {
            auto node = _front;
            _front = _front->next;

            if (_front)
                _front->prev = nullptr;
            else
                _back = nullptr;

            Memory::destroy(node);
        }
        else
            throw Exception(STR("list is empty"));
    }

    void popBack()
    {
        if (_back)
        {
            auto node = _back;
            _back = _back->prev;

            if (_back)
                _back->next = nullptr;
            else
                _front = nullptr;

            Memory::destroy(node);
        }
        else
            throw Exception(STR("list is empty"));
    }

    void pushFront(const _Type& value)
    {
        if (_front)
        {
            auto node = Memory::create<ListNode<_Type>>(value, nullptr, _front);
            _front->prev = node;
            _front = node;
        }
        else
        {
            auto node = Memory::create<ListNode<_Type>>(value, nullptr, nullptr);
            _front = _back = node;
        }
    }

    void pushFront(_Type&& value)
    {
        if (_front)
        {
            auto node = Memory::create<ListNode<_Type>>(static_cast<_Type&&>(value), nullptr, _front);
            _front->prev = node;
            _front = node;
        }
        else
        {
            auto node = Memory::create<ListNode<_Type>>(static_cast<_Type&&>(value), nullptr, nullptr);
            _front = _back = node;
        }
    }

    void pushBack(const _Type& value)
    {
        if (_front)
        {
            auto node = Memory::create<ListNode<_Type>>(value, _back, nullptr);
            _back->next = node;
            _back = node;
        }
        else
        {
            auto node = Memory::create<ListNode<_Type>>(value, nullptr, nullptr);
            _front = _back = node;
        }
    }

    void pushBack(_Type&& value)
    {
        if (_front)
        {
            auto node = Memory::create<ListNode<_Type>>(static_cast<_Type&&>(value), _back, nullptr);
            _back->next = node;
            _back = node;
        }
        else
        {
            auto node = Memory::create<ListNode<_Type>>(static_cast<_Type&&>(value), nullptr, nullptr);
            _front = _back = node;
        }
    }

    void insertBefore(ListNode<_Type>* pos, const _Type& value)
    {
        auto node = Memory::create<ListNode<_Type>>(value, pos->prev, pos);

        if (pos->prev)
            pos->prev->next = node;
        else
            _front = node;

        pos->prev = node;
    }

    void insertBefore(ListNode<_Type>* pos, _Type&& value)
    {
        auto node = Memory::create<ListNode<_Type>>(static_cast<_Type&&>(value), pos->prev, pos);

        if (pos->prev)
            pos->prev->next = node;
        else
            _front = node;

        pos->prev = node;
    }

    void insertAfter(ListNode<_Type>* pos, const _Type& value)
    {
        auto node = Memory::create<ListNode<_Type>>(value, pos, pos->next);

        if (pos->next)
            pos->next->prev = node;
        else
            _back = node;

        pos->next = node;
    }

    void insertAfter(ListNode<_Type>* pos, _Type&& value)
    {
        auto node = Memory::create<ListNode<_Type>>(static_cast<_Type&&>(value), pos, pos->next);

        if (pos->next)
            pos->next->prev = node;
        else
            _back = node;

        pos->next = node;
    }

    void remove(ListNode<_Type>* pos)
    {
        if (pos->prev)
            pos->prev->next = pos->next;
        else
            _front = pos->next;

        if (pos->next)
            pos->next->prev = pos->prev;
        else
            _back = pos->prev;

        Memory::destroy(pos);
    }

    void clear()
    {
        destroyNodes();
        _front = _back = nullptr;
    }

    friend void swap(List<_Type>& left, List<_Type>& right)
    {
        swap(left._front, right._front);
        swap(left._back, right._back);
    }

private:
    void destroyNodes()
    {
        for (auto node = _front; node;)
        {
            auto n = node;
            node = node->next;
            Memory::destroy(n);
        }
    }

private:
    ListNode<_Type>* _front;
    ListNode<_Type>* _back;
};

// ListIterator

template<typename _Type>
class ListIterator
{
public:
    ListIterator(List<_Type>& list) : _list(list), _node(nullptr)
    {
    }

    _Type& value() const
    {
        if (_node)
            return _node->value;
        else
            throw Exception(STR("no current value"));
    }

    bool moveNext()
    {
        if (_node)
            _node = _node->next;
        else
            _node = _list.front();

        return _node != nullptr;
    }

    bool movePrev()
    {
        if (_node)
            _node = _node->prev;
        else
            _node = _list.back();

        return _node != nullptr;
    }

private:
    List<_Type>& _list;
    ListNode<_Type>* _node;
};

// ConstListIterator

template<typename _Type>
class ConstListIterator
{
public:
    ConstListIterator(const List<_Type>& list) : _list(list), _node(nullptr)
    {
    }

    const _Type& value() const
    {
        if (_node)
            return _node->value;
        else
            throw Exception(STR("no current value"));
    }

    bool moveNext()
    {
        if (_node)
            _node = _node->next;
        else
            _node = _list.front();

        return _node != nullptr;
    }

    bool movePrev()
    {
        if (_node)
            _node = _node->prev;
        else
            _node = _list.back();

        return _node != nullptr;
    }

private:
    const List<_Type>& _list;
    const ListNode<_Type>* _node;
};

// Map

template<typename, typename>
class MapIterator;

template<typename, typename>
class ConstMapIterator;

template<typename _Key, typename _Value>
class Map
{
public:
    static const float MAX_LOAD_FACTOR;

    template<typename, typename>
    friend class MapIterator;

    template<typename, typename>
    friend class ConstMapIterator;

    typedef MapIterator<_Key, _Value> Iterator;
    typedef ConstMapIterator<_Key, _Value> ConstIterator;

public:
    Map(int numBuckets = 0) : _keyValues(numBuckets), _size(0)
    {
    }

    Map(const Map<_Key, _Value>& other) :
        _keyValues(other._keyValues),
        _size(other._size)
    {
    }

    Map(Map<_Key, _Value>&& other) :
        _keyValues(other._keyValues)
    {
        _size = other._size;
        other._size = 0;
    }

    _Value& operator[](const _Key& key)
    {
        if (_keyValues.empty())
            _keyValues.resize(1);
        else if (loadFactor() > MAX_LOAD_FACTOR)
            rehash(_keyValues.size() * 2 + 1);

        List<KeyValue>& kvList = getBucket(key);

        for (auto kvNode = kvList.front(); kvNode; kvNode = kvNode->next)
        {
            if (equalsTo(kvNode->value.key, key))
                return kvNode->value.value;
        }

        kvList.pushBack(KeyValue(key));
        ++_size;

        return kvList.back()->value.value;
    }

    _Value& operator[](_Key&& key)
    {
        if (_keyValues.empty())
            _keyValues.resize(1);
        else if (loadFactor() > MAX_LOAD_FACTOR)
            rehash(_keyValues.size() * 2 + 1);

        List<KeyValue>& kvList = getBucket(key);

        for (auto kvNode = kvList.front(); kvNode; kvNode = kvNode->next)
        {
            if (equalsTo(kvNode->value.key, key))
                return kvNode->value.value;
        }

        kvList.pushBack(KeyValue(static_cast<_Key&&>(key)));
        ++_size;

        return kvList.back()->value.value;
    }

    const _Value& operator[](const _Key& key) const
    {
        const _Value* value = find(key);
        if (value)
            return *value;
        else
            throw Exception(STR("not found"));
    }

    Map<_Key, _Value>& operator=(const Map<_Key, _Value>& other)
    {
        Map<_Key, _Value> tmp(other);
        swap(*this, tmp);
        return *this;
    }

    Map<_Key, _Value>& operator=(Map<_Key, _Value>&& other)
    {
        clear();

        _keyValues = other._keyValues;
        _size = other._size;
        other._size = 0;

        return *this;
    }

    int size() const
    {
        return _size;
    }

    int numBuckets() const
    {
        return _keyValues.size();
    }

    bool empty() const
    {
        return _size == 0;
    }

    float loadFactor() const
    {
        return static_cast<float>(_size) / _keyValues.size();
    }

    _Value* find(const _Key& key)
    {
        if (!_keyValues.empty())
        {
            List<KeyValue>& kvList = getBucket(key);

            for (auto kvNode = kvList.front(); kvNode; kvNode = kvNode->next)
            {
                if (equalsTo(kvNode->value.key, key))
                    return &kvNode->value.value;
            }
        }

        return nullptr;
    }

    const _Value* find(const _Key& key) const
    {
        if (!_keyValues.empty())
        {
            const List<KeyValue>& kvList = getBucket(key);

            for (auto kvNode = kvList.front(); kvNode; kvNode = kvNode->next)
            {
                if (equalsTo(kvNode->value.key, key))
                    return &kvNode->value.value;
            }
        }

        return nullptr;
    }

    void insert(const _Key& key, const _Value& value)
    {
        if (_keyValues.empty())
            _keyValues.resize(1);
        else if (loadFactor() > MAX_LOAD_FACTOR)
            rehash(_keyValues.size() * 2 + 1);

        List<KeyValue>& kvList = getBucket(key);

        for (auto kvNode = kvList.front(); kvNode; kvNode = kvNode->next)
        {
            if (equalsTo(kvNode->value.key, key))
            {
                kvNode->value.value = value;
                return;
            }
        }

        kvList.pushBack(KeyValue(key, value));
        ++_size;
    }

    void insert(_Key&& key, _Value&& value)
    {
        if (_keyValues.empty())
            _keyValues.resize(1);
        else if (loadFactor() > MAX_LOAD_FACTOR)
            rehash(_keyValues.size() * 2 + 1);

        List<KeyValue>& kvList = getBucket(key);

        for (auto kvNode = kvList.front(); kvNode; kvNode = kvNode->next)
        {
            if (equalsTo(kvNode->value.key, key))
            {
                kvNode->value.value = static_cast<_Value&&>(value);
                return;
            }
        }

        kvList.pushBack(KeyValue(
            static_cast<_Key&&>(key), static_cast<_Value&&>(value)));
            
        ++_size;
    }

    bool remove(const _Key& key)
    {
        if (!_keyValues.empty())
        {
            List<KeyValue>& kvList = getBucket(key);

            for (auto kvNode = kvList.front(); kvNode; kvNode = kvNode->next)
            {
                if (equalsTo(kvNode->value.key, key))
                {
                    kvList.remove(kvNode);
                    --_size;
                    return true;
                }
            }
        }

        return false;
    }

    void clear()
    {
        _keyValues.clear();
        _size = 0;
    }

    void rehash(int numBuckets)
    {
        Map<_Key, _Value> tmp(numBuckets);

        for (int i = 0; i < _keyValues.size(); ++i)
        {
            for (auto kvNode = _keyValues[i].front(); kvNode; kvNode = kvNode->next)
                tmp.insert(static_cast<_Key&&>(kvNode->value.key), static_cast<_Value&&>(kvNode->value.value));
        }

        swap(*this, tmp);
    }

    friend void swap(Map<_Key, _Value>& left, Map<_Key, _Value>& right)
    {
        swap(left._keyValues, right._keyValues);
        swap(left._size, right._size);
    }

private:
    struct KeyValue
    {
        _Key key;
        _Value value;

        KeyValue(const _Key& key) :
            key(key)
        {
        }

        KeyValue(_Key&& key) :
            key(static_cast<_Key&&>(key))
        {
        }

        KeyValue(const _Key& key, const _Value& value) :
            key(key), value(value)
        {
        }

        KeyValue(_Key&& key, _Value&& value) :
            key(static_cast<_Key&&>(key)), 
            value(static_cast<_Value&&>(value))
        {
        }
    };

private:
    List<KeyValue>& getBucket(const _Key& key)
    {
        return _keyValues[getBucketIndex(key)];
    }

    const List<KeyValue>& getBucket(const _Key& key) const
    {
        return _keyValues[getBucketIndex(key)];
    }

    int getBucketIndex(const _Key& key) const
    {
        int h = hash(key);
        return (h >> 1) % _keyValues.size();
    }

private:
    Array<List<KeyValue>> _keyValues;
    int _size;
};

template<typename _Key, typename _Value>
const float Map<_Key, _Value>::MAX_LOAD_FACTOR = 0.75f;

// MapIterator

template<typename _Key, typename _Value>
class MapIterator
{
public:
    MapIterator(Map<_Key, _Value>& map) : _map(map), _index(0), _node(nullptr)
    {
    }

    typename Map<_Key, _Value>::KeyValue& value() const
    {
        if (_node)
            return _node->value;
        else
            throw Exception(STR("no current value"));
    }

    bool moveNext()
    {
        if (_node)
        {
            if (_node->next)
            {
                _node = _node->next;
                return true;
            }
            else
            {
                for (++_index; _index < _map._keyValues.size(); ++_index)
                {
                    _node = _map._keyValues[_index].front();

                    if (_node)
                        return true;
                }
            }
        }
        else
        {
            for (_index = 0; _index < _map._keyValues.size(); ++_index)
            {
                _node = _map._keyValues[_index].front();

                if (_node)
                    return true;
            }
        }

        _index = 0;
        _node = nullptr;
        return false;
    }

private:
    Map<_Key, _Value>& _map;
    int _index;
    ListNode<typename Map<_Key, _Value>::KeyValue>* _node;
};

// ConstMapIterator

template<typename _Key, typename _Value>
class ConstMapIterator
{
public:
    ConstMapIterator(const Map<_Key, _Value>& map) : _map(map), _index(0), _node(nullptr)
    {
    }

    const typename Map<_Key, _Value>::KeyValue& value() const
    {
        if (_node)
            return _node->value;
        else
            throw Exception(STR("no current value"));
    }

    bool moveNext()
    {
        if (_node)
        {
            if (_node->next)
            {
                _node = _node->next;
                return true;
            }
            else
            {
                for (++_index; _index < _map._keyValues.size(); ++_index)
                {
                    _node = _map._keyValues[_index].front();

                    if (_node)
                        return true;
                }
            }
        }
        else
        {
            for (_index = 0; _index < _map._keyValues.size(); ++_index)
            {
                _node = _map._keyValues[_index].front();

                if (_node)
                    return true;
            }
        }

        _index = 0;
        _node = nullptr;
        return false;
    }

private:
    const Map<_Key, _Value>& _map;
    int _index;
    const ListNode<typename Map<_Key, _Value>::KeyValue>* _node;
};

// Set

template<typename>
class ConstSetIterator;

template<typename _Type>
class Set
{
public:
    static const float MAX_LOAD_FACTOR;

    template<typename>
    friend class ConstSetIterator;

    typedef ConstSetIterator<_Type> ConstIterator;

public:
    Set(int numBuckets = 0) : _values(numBuckets), _size(0)
    {
    }

    Set(const Set<_Type>& other) :
        _values(other._values),
        _size(other._size)
    {
    }

    Set(Set<_Type>&& other) :
        _values(other._values)
    {
        _size = other._size;
        other._size = 0;
    }

    Set<_Type>& operator=(const Set<_Type>& other)
    {
        Set<_Type> tmp(other);
        swap(*this, tmp);
        return *this;
    }

    Set<_Type>& operator=(Set<_Type>&& other)
    {
        clear();

        _values = other._values;
        _size = other._size;
        other._size = 0;

        return *this;
    }

    int size() const
    {
        return _size;
    }

    int numBuckets() const
    {
        return _values.size();
    }

    bool empty() const
    {
        return _size == 0;
    }

    float loadFactor() const
    {
        return static_cast<float>(_size) / _values.size();
    }

    _Type* find(const _Type& value)
    {
        List<_Type>& vList = getBucket(value);

        for (auto vNode = vList.front(); vNode; vNode = vNode->next)
        {
            if (equalsTo(vNode->value, value))
                return &vNode->value;
        }

        return nullptr;
    }

    const _Type* find(const _Type& value) const
    {
        const List<_Type>& vList = getBucket(value);

        for (auto vNode = vList.front(); vNode; vNode = vNode->next)
        {
            if (equalsTo(vNode->value, value))
                return &vNode->value;
        }

        return nullptr;
    }

    void insert(const _Type& value)
    {
        if (_values.empty())
            _values.resize(1);
        else if (loadFactor() > MAX_LOAD_FACTOR)
            rehash(_values.size() * 2 + 1);

        List<_Type>& vList = getBucket(value);

        for (auto vNode = vList.front(); vNode; vNode = vNode->next)
        {
            if (equalsTo(vNode->value, value))
                return;
        }

        vList.pushBack(value);
        ++_size;
    }

    void insert(_Type&& value)
    {
        if (_values.empty())
            _values.resize(1);
        else if (loadFactor() > MAX_LOAD_FACTOR)
            rehash(_values.size() * 2 + 1);

        List<_Type>& vList = getBucket(value);

        for (auto vNode = vList.front(); vNode; vNode = vNode->next)
        {
            if (equalsTo(vNode->value, value))
                return;
        }

        vList.pushBack(static_cast<_Type&&>(value));
        ++_size;
    }

    _Type&& remove()
    {
        for (int i = 0; i < _values.size(); ++i)
        {
            List<_Type>& vList = _values[i];

            if (!vList.empty())
            {
                _Type&& value = static_cast<_Type&&>(vList.front()->value);
                vList.popFront();
                --_size;
                return static_cast<_Type&&>(value);
            }
        }

        throw Exception(STR("set is empty"));
    }

    bool remove(const _Type& value)
    {
        List<_Type>& vList = getBucket(value);

        for (auto vNode = vList.front(); vNode; vNode = vNode->next)
        {
            if (equalsTo(vNode->value, value))
            {
                vList.remove(vNode);
                --_size;
                return true;
            }
        }

        return false;
    }

    void clear()
    {
        _values.clear();
        _size = 0;
    }

    void rehash(int numBuckets)
    {
        Set<_Type> tmp(numBuckets);

        for (int i = 0; i < _values.size(); ++i)
        {
            for (auto vNode = _values[i].front(); vNode; vNode = vNode->next)
                tmp.insert(static_cast<_Type&&>(vNode->value));
        }

        swap(*this, tmp);
    }

    friend void swap(Set<_Type>& left, Set<_Type>& right)
    {
        swap(left._values, right._values);
        swap(left._size, right._size);
    }

private:
    List<_Type>& getBucket(const _Type& value)
    {
        return _values[getBucketIndex(value)];
    }

    const List<_Type>& getBucket(const _Type& value) const
    {
        return _values[getBucketIndex(value)];
    }

    int getBucketIndex(const _Type& value) const
    {
        int h = hash(value);
        return (h >> 1) % _values.size();
    }

private:
    Array<List<_Type>> _values;
    int _size;
};

template<typename _Type>
const float Set<_Type>::MAX_LOAD_FACTOR = 0.75f;

// ConstSetIterator

template<typename _Type>
class ConstSetIterator
{
public:
    ConstSetIterator(const Set<_Type>& set) : _set(set), _index(0), _node(nullptr)
    {
    }

    const _Type& value() const
    {
        if (_node)
            return _node->value;
        else
            throw Exception(STR("no current value"));
    }

    bool moveNext()
    {
        if (_node)
        {
            if (_node->next)
            {
                _node = _node->next;
                return true;
            }
            else
            {
                for (++_index; _index < _set._values.size(); ++_index)
                {
                    _node = _set._values[_index].front();

                    if (_node)
                        return true;
                }
            }
        }
        else
        {
            for (_index = 0; _index < _set._values.size(); ++_index)
            {
                _node = _set._values[_index].front();

                if (_node)
                    return true;
            }
        }

        _index = 0;
        _node = nullptr;
        return false;
    }

private:
    const Set<_Type>& _set;
    int _index;
    const ListNode<_Type>* _node;
};

// Console

class Console
{
public:
#ifdef PLATFORM_WINDOWS
    static void enableUnicode();
    static void openConsole();
#else
    static void setCharInputMode();
    static void setLineInputMode();
#endif

    static void write(char_t ch);
    static void write(const String& str);
    static void write(const char_t* format, ...);

    static void writeLine();
    static void writeLine(const String& str);
    static void writeLine(const char_t* format, ...);

    static String readLine();

    static void getSize(int& width, int& height);
    static void clearScreen();

    static void showCursor();
    static void hideCursor();
    static void setCursorPosition(int line, int column);
};

// File

enum FileMode
{
    FILE_MODE_CREATE_ALWAYS, // create or truncate if exists
    FILE_MODE_CREATE_NEW, // create new or fail if exists
    FILE_MODE_OPEN_ALWAYS, // open if exists or create 
    FILE_MODE_OPEN_EXISTING, // open if exists or fail
    FILE_MODE_TRUNCATE_EXISTING // open existing and truncate or fail
};

class File
{
public:
    File();
    File(const String& fileName,
        FileMode openMode = FILE_MODE_OPEN_EXISTING);
        
    File(const File&) = delete;
    
    ~File();
    
    File& operator=(const File&) = delete;
    
    bool open(const String& fileName, 
        FileMode openMode = FILE_MODE_OPEN_EXISTING);
        
    void close();
    
    Array<uint8_t> readBytes();
    void writeBytes(const Array<uint8_t>& bytes);
    
    String readString();
    void writeString(const String& str);
    
    int64_t size() const;
    
private:
#ifdef PLATFORM_WINDOWS
    HANDLE _handle;
#else
    int _handle;
#endif
};

#endif

