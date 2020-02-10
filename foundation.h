#ifndef FOUNDATION_INCLUDED
#define FOUNDATION_INCLUDED

#include <new>
#include <stdarg.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include <wctype.h>
#include <ctype.h>
#include <math.h>
#include <locale.h>
#include <time.h>
#include <limits.h>

// 32/64 bit

#if defined(__x86_64) || defined(_M_X64) || defined(__aarch64__) || defined(_M_ARM64) || defined(__sparcv9) || \
    defined(_ARCH_PPC64)
#define ARCH_64BIT
#else
#define ARCH_32BIT
#endif

// processor architecture

#if defined(__i386) || defined(__x86_64) || defined(_M_IX86) || defined(_M_X64)

#define ARCH_INTEL
#define ARCH_LITTLE_ENDIAN

#elif defined(__arm__) || defined(__aarch64__) || defined(_M_ARM) || defined(_M_ARM64)

#define ARCH_ARM
#define ARCH_LITTLE_ENDIAN

#elif defined(__sparc) || defined(__sparcv9)

#define ARCH_SPARC
#define ARCH_BIG_ENDIAN

#elif defined(_ARCH_PPC) || defined(_ARCH_PPC64)

#define ARCH_POWER
#define ARCH_BIG_ENDIAN

#endif

// platform

#if defined(_WIN32)

#define PLATFORM_WINDOWS

#elif defined(__linux__)

#define PLATFORM_LINUX
#define PLATFORM_UNIX

#elif defined(__APPLE__)

#define PLATFORM_APPLE
#define PLATFORM_UNIX

#elif defined(__sun)

#define PLATFORM_SOLARIS
#define PLATFORM_UNIX

#elif defined(_AIX)

#define PLATFORM_AIX
#define PLATFORM_UNIX

#elif defined(__ANDROID__)

#define PLATFORM_ANDRIOD
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
#include <windowsx.h>
#include <io.h>
#include <fcntl.h>
#include <conio.h>

#ifdef GUI_MODE
#include <d2d1.h>
#include <dwrite.h>
#include <wincodec.h>
#endif

#undef min
#undef max

#else

#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>

#endif

#ifdef PLATFORM_LINUX
#ifdef GUI_MODE
#include <gtk/gtk.h>
#endif
#endif

#ifdef PLATFORM_SOLARIS
#include <sys/filio.h>
#include <alloca.h>
#endif

#ifdef PLATFORM_AIX
#include <alloca.h>
#endif

// typdefs and macros

#define CHAR(arg) U##arg
typedef char32_t unichar_t;

#if defined(PLATFORM_WINDOWS)

#define CHAR_ENCODING_UTF16
#define MAIN wmain
#define STR(arg) u##arg
#define CRLF true
#define NEWLINE STR("\r\n")

typedef char16_t char_t;

#else

#define CHAR_ENCODING_UTF8
#define MAIN main
#define STR(arg) u8##arg
#define CRLF false
#define NEWLINE STR("\n")

typedef char char_t;

#endif

typedef unsigned char byte_t;
#define INVALID_POSITION -1

#define TO_STR(arg) #arg
#define STR_MACRO(arg) STR(arg)
#define NUM_MACRO(arg) STR_MACRO(TO_STR(arg))

// diagnostic messages

class String;

void showMessage(const char_t* message);
void showMessage(const String& message);
void reportError(const char_t* message);
void reportError(const String& message);

// assert macros

class String;

void terminate(const char_t* message);
void terminate(const String& message);

#ifdef DISABLE_ASSERT

#define ASSERT_MSG(condition, msg)
#define ASSERT(...)
#define ASSERT_FAIL(msg)
#define ASSERT_EXCEPTION(exception, ...)
#define ASSERT_NO_EXCEPTION(...)
#define ASSERT_EXCEPTION_MSG(exception, msg, ...)

#else

#define ASSERT_MSG(condition, msg) \
    do \
    { \
        if (!(condition)) \
            throw Exception(STR("assertion failed in ") STR_MACRO(__FILE__) STR(" at line ") NUM_MACRO(__LINE__) \
                                STR(": ") msg); \
    } while (false)

#define ASSERT(...) ASSERT_MSG(__VA_ARGS__, STR(#__VA_ARGS__))
#define ASSERT_FAIL(msg) ASSERT_MSG(false, msg)

#define ASSERT_EXCEPTION(exception, ...) \
    do \
    { \
        try \
        { \
            __VA_ARGS__; \
        } \
        catch (exception&) \
        { \
            break; \
        } \
        catch (...) \
        { \
            ASSERT_FAIL(STR("expected ") STR(#exception)); \
        } \
        ASSERT_FAIL(STR("expected ") STR(#exception)); \
    } while (false)

#define ASSERT_NO_EXCEPTION(...) \
    do \
    { \
        try \
        { \
            __VA_ARGS__; \
        } \
        catch (...) \
        { \
            ASSERT_FAIL(STR("unexpected exception")); \
        } \
    } while (false)

#define ASSERT_EXCEPTION_MSG(exception, msg, ...) \
    do \
    { \
        try \
        { \
            __VA_ARGS__; \
        } \
        catch (exception & ex) \
        { \
            ASSERT_MSG(strFind(ex.message(), STR(msg)), STR(#exception) STR(" doesn't contain expected message")); \
            break; \
        } \
        catch (...) \
        { \
            ASSERT_FAIL(STR("expected ") STR(#exception)); \
        } \
        ASSERT_FAIL(STR("expected ") STR(#exception)); \
    } while (false)

#endif

#ifdef PLATFORM_WINDOWS

#define ASSERT_COM_SUCCEEDED(...) \
    { \
        HRESULT hr = __VA_ARGS__; \
        ASSERT_MSG(SUCCEEDED(hr), STR(#__VA_ARGS__)); \
    }

#endif

// debug logging

class String;

void logDebugMessage(const char_t* message);
void logDebugMessage(const String& message);

#define LOG_LOC \
    logDebugMessage(STR_MACRO(__FILE__) STR(":") NUM_MACRO(__LINE__) STR(": ") STR_MACRO(__FUNCTION__) NEWLINE)
#define LOG_MSG(msg) logDebugMessage(STR_MACRO(__FILE__) STR(":") NUM_MACRO(__LINE__) STR(": ") msg NEWLINE)
#define LOG_STMT(...) \
    __VA_ARGS__; \
    logDebugMessage(STR_MACRO(__FILE__) STR(":") NUM_MACRO(__LINE__) STR(": ") STR(#__VA_ARGS__) NEWLINE)

#define LOG_FMT(fmt, ...) \
    do \
    { \
        logDebugMessage(STR_MACRO(__FILE__) STR(":") NUM_MACRO(__LINE__) STR(": ")); \
        logDebugMessage(String::format(STR(fmt), __VA_ARGS__)); \
        logDebugMessage(NEWLINE); \
    } while (false)

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

// min/max

template<typename _Type>
const _Type& min(const _Type& left, const _Type& right)
{
    return left < right ? left : right;
}

template<typename _Type>
const _Type& max(const _Type& left, const _Type& right)
{
    return left > right ? left : right;
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
    int* h = reinterpret_cast<int*>(&val);
    if (sizeof(long) == 8)
        return hash(*h, *(h + 1));
    else
        return *h;
}

inline int hash(unsigned long val)
{
    int* h = reinterpret_cast<int*>(&val);
    if (sizeof(unsigned long) == 8)
        return hash(*h, *(h + 1));
    else
        return *h;
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
    int* h = reinterpret_cast<int*>(&val);
    return *h;
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
    intptr_t* h = reinterpret_cast<intptr_t*>(&val);
    return hash(*h);
}

template<typename _Type1, typename _Type2>
inline int hash(const _Type1& val1, const _Type2& val2)
{
    return 33 * hash(val1) + hash(val2);
}

// Timer

struct Timer
{
    static void sleep(int64_t usec);
    static int64_t ticks();
};

// byte swap

inline uint16_t swapBytes(uint16_t value)
{
    return (value << 8) | (value >> 8);
}

inline char16_t swapBytes(char16_t value)
{
    return swapBytes(static_cast<uint16_t>(value));
}

inline uint32_t swapBytes(uint32_t value)
{
    value = ((value << 8) & 0xff00ff00) | ((value >> 8) & 0xff00ff);
    return (value << 16) | (value >> 16);
}

inline char32_t swapBytes(char32_t value)
{
    return swapBytes(static_cast<uint32_t>(value));
}

inline uint64_t swapBytes(uint64_t value)
{
    value = ((value << 8) & 0xff00ff00ff00ff00ull) | ((value >> 8) & 0x00ff00ff00ff00ffull);
    value = ((value << 16) & 0xffff0000ffff0000ull) | ((value >> 16) & 0x0000ffff0000ffffull);
    return (value << 32) | (value >> 32);
}

inline void swapBytes(uint16_t* values, int len)
{
    ASSERT(values ? len >= 0 : len == 0);

    for (int i = 0; i < len; ++i)
        values[i] = swapBytes(values[i]);
}

inline void swapBytes(uint32_t* values, int len)
{
    ASSERT(values ? len >= 0 : len == 0);

    for (int i = 0; i < len; ++i)
        values[i] = swapBytes(values[i]);
}

inline void swapBytes(uint64_t* values, int len)
{
    ASSERT(values ? len >= 0 : len == 0);

    for (int i = 0; i < len; ++i)
        values[i] = swapBytes(values[i]);
}

// Memory

#define ALLOCATE_STACK(type, size) reinterpret_cast<type*>(alloca(sizeof(type) * (size)))

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
    ASSERT(size >= 0);

    if (size > 0)
    {
        _Type* ptr = static_cast<_Type*>(malloc(sizeof(_Type) * size));

        if (ptr)
            return ptr;
        else
            throw OutOfMemoryException();
    }
    else
        return nullptr;
}

template<typename _Type>
inline _Type* reallocate(_Type* ptr, int size)
{
    ASSERT(size >= 0);

    if (size > 0)
    {
        ptr = static_cast<_Type*>(realloc(ptr, sizeof(_Type) * size));

        if (!ptr)
            throw OutOfMemoryException();
        else
            return ptr;
    }
    else
    {
        free(ptr);
        return nullptr;
    }
}

inline void deallocate(void* ptr)
{
    free(ptr);
}

template<typename _Type, typename... _Args>
inline void construct(_Type* ptr, _Args&&... args)
{
    ASSERT(ptr);
    ::new (ptr) _Type(static_cast<_Args&&>(args)...);
}

template<typename _Type, typename... _Args>
inline _Type* create(_Args&&... args)
{
    _Type* ptr = allocate<_Type>();

    try
    {
        ::new (ptr) _Type(static_cast<_Args&&>(args)...);
    }
    catch (...)
    {
        deallocate(ptr);
        throw;
    }

    return ptr;
}

template<typename _Type>
inline void destruct(_Type* ptr)
{
    if (ptr)
        ptr->~_Type();
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
inline void destructArray(int size, _Type* values)
{
    ASSERT(values ? size >= 0 : size == 0);

    if (values)
    {
        while (size-- > 0)
            values[size].~_Type();
    }
}

template<typename _Type>
inline void destroyArray(int size, _Type* values)
{
    ASSERT(values ? size >= 0 : size == 0);

    if (values)
    {
        while (size-- > 0)
            values[size].~_Type();

        deallocate(values);
    }
}

template<typename _Type, typename... _Args>
inline _Type* createArrayFill(int size, int capacity, _Args&&... args)
{
    ASSERT(size >= 0);
    ASSERT(capacity >= 0 && size <= capacity);

    _Type* ptr = allocate<_Type>(capacity);
    int i = 0;

    try
    {
        for (; i < size; ++i)
            ::new (ptr + i) _Type(static_cast<_Args&&>(args)...);
    }
    catch (...)
    {
        destroyArray(i, ptr);
        throw;
    }

    return ptr;
}

template<typename _Type>
inline _Type* createArrayCopy(int size, int capacity, const _Type* values)
{
    ASSERT(values ? size >= 0 : size == 0);
    ASSERT(capacity >= 0 && size <= capacity);

    _Type* ptr = allocate<_Type>(capacity);
    int i = 0;

    try
    {
        for (; i < size; ++i)
            ::new (ptr + i) _Type(values[i]);
    }
    catch (...)
    {
        destroyArray(i, ptr);
        throw;
    }

    return ptr;
}

template<typename _Type>
inline _Type* createArrayMove(int size, int capacity, _Type* values)
{
    ASSERT(values ? size >= 0 : size == 0);
    ASSERT(capacity >= 0 && size <= capacity);

    _Type* ptr = allocate<_Type>(capacity);
    int i = 0;

    try
    {
        for (; i < size; ++i)
            ::new (ptr + i) _Type(static_cast<_Type&&>(values[i]));
    }
    catch (...)
    {
        destroyArray(i, ptr);
        throw;
    }

    return ptr;
}

} // namespace Memory

// swap

template<typename _Type>
void swap(_Type& left, _Type& right)
{
    _Type tmp(static_cast<_Type&&>(left));
    left = static_cast<_Type&&>(right);
    right = static_cast<_Type&&>(tmp);
}

// Unique

template<typename _Type>
class Unique
{
public:
    Unique() : _ptr(nullptr)
    {
    }

    Unique(const Unique<_Type>&) = delete;

    Unique(Unique<_Type>&& other) : _ptr(other._ptr)
    {
        other._ptr = nullptr;
    }

    template<typename _T>
    Unique(Unique<_T>&& other) : _ptr(other.release())
    {
    }

    ~Unique()
    {
        Memory::destroy(_ptr);
    }

    Unique<_Type>& operator=(const Unique<_Type>&) = delete;

    Unique<_Type>& operator=(Unique<_Type>&& other)
    {
        Unique<_Type> tmp(static_cast<Unique<_Type>&&>(other));
        swap(*this, tmp);
        return *this;
    }

    template<typename _T>
    Unique<_Type>& operator=(Unique<_T>&& other)
    {
        Unique<_Type> tmp(static_cast<Unique<_T>&&>(other));
        swap(*this, tmp);
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

    operator _Type&() const
    {
        if (_ptr)
            return *_ptr;
        else
            throw NullPointerException();
    }

    bool empty() const
    {
        return _ptr == nullptr;
    }

    _Type* ptr() const
    {
        return _ptr;
    }

    template<typename... _Args>
    void create(_Args&&... args)
    {
        reset();
        _ptr = Memory::create<_Type>(static_cast<_Args&&>(args)...);
    }

    void reset()
    {
        Memory::destroy(_ptr);
        _ptr = nullptr;
    }

    template<typename _T>
    static Unique<_Type> acquire(_T* ptr)
    {
        return Unique<_Type>(ptr);
    }

    _Type* release()
    {
        _Type* ptr = _ptr;
        _ptr = nullptr;
        return ptr;
    }

    friend void swap(Unique<_Type>& left, Unique<_Type>& right)
    {
        swap(left._ptr, right._ptr);
    }

    template<typename _T, typename... _Args>
    friend Unique<_T> createUnique(_Args&&... args);

    template<typename _T>
    friend int hash(const Unique<_T>& val);

protected:
    Unique(_Type* ptr) : _ptr(ptr)
    {
    }

protected:
    _Type* _ptr;
};

template<typename _Type, typename... _Args>
inline Unique<_Type> createUnique(_Args&&... args)
{
    return Unique<_Type>(Memory::create<_Type>(static_cast<_Args&&>(args)...));
}

template<typename _Type>
inline int hash(const Unique<_Type>& val)
{
    return hash(*reinterpret_cast<const intptr_t*>(&val._ptr));
}

// Shared

template<typename _Type>
class Shared
{
protected:
    struct RefCountedObject
    {
        int refCount;
        _Type object;

        template<typename... _Args>
        RefCountedObject(_Args&&... args) : refCount(1), object(static_cast<_Args&&>(args)...)
        {
        }
    };

public:
    Shared() : _sharedPtr(nullptr)
    {
    }

    Shared(const Shared<_Type>& other) : _sharedPtr(other._sharedPtr)
    {
        if (_sharedPtr)
            addRef();
    }

    template<typename _T>
    Shared(const Shared<_T>& other) : _sharedPtr(reinterpret_cast<RefCountedObject*>(other.refCountedObject()))
    {
        if (_sharedPtr)
            addRef();
    }

    Shared(Shared<_Type>&& other) : _sharedPtr(other._sharedPtr)
    {
        other._sharedPtr = nullptr;
    }

    template<typename _T>
    Shared(Shared<_T>&& other) : _sharedPtr(reinterpret_cast<RefCountedObject*>(other.release()))
    {
    }

    ~Shared()
    {
        if (_sharedPtr)
            releaseRef();
    }

    Shared<_Type>& operator=(const Shared<_Type>& other)
    {
        Shared<_Type> tmp(other);
        swap(*this, tmp);
        return *this;
    }

    template<typename _T>
    Shared<_Type>& operator=(const Shared<_T>& other)
    {
        Shared<_Type> tmp(other);
        swap(*this, tmp);
        return *this;
    }

    Shared<_Type>& operator=(Shared<_Type>&& other)
    {
        Shared<_Type> tmp(static_cast<Shared<_Type>&&>(other));
        swap(*this, tmp);
        return *this;
    }

    template<typename _T>
    Shared<_Type>& operator=(Shared<_T>&& other)
    {
        Shared<_Type> tmp(static_cast<Shared<_Type>&&>(other));
        swap(*this, tmp);
        return *this;
    }

    _Type& operator*() const
    {
        if (_sharedPtr)
            return _sharedPtr->object;
        else
            throw NullPointerException();
    }

    _Type* operator->() const
    {
        if (_sharedPtr)
            return &_sharedPtr->object;
        else
            throw NullPointerException();
    }

    operator _Type&() const
    {
        if (_sharedPtr)
            return _sharedPtr->object;
        else
            throw NullPointerException();
    }

    bool empty() const
    {
        return _sharedPtr == nullptr;
    }

    _Type* ptr() const
    {
        if (_sharedPtr)
            return &_sharedPtr->object;
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

    RefCountedObject* refCountedObject() const
    {
        return _sharedPtr;
    }

    template<typename... _Args>
    void create(_Args&&... args)
    {
        reset();
        _sharedPtr = Memory::create<RefCountedObject>(static_cast<_Args&&>(args)...);
    }

    void reset()
    {
        if (_sharedPtr)
            releaseRef();

        _sharedPtr = nullptr;
    }

    RefCountedObject* release()
    {
        RefCountedObject* ptr = _sharedPtr;
        _sharedPtr = nullptr;
        return ptr;
    }

    friend void swap(Shared<_Type>& left, Shared<_Type>& right)
    {
        swap(left._sharedPtr, right._sharedPtr);
    }

    template<typename _T, typename... _Args>
    friend Shared<_T> createShared(_Args&&... args);

    template<typename _T>
    friend int hash(const Shared<_T>& val);

protected:
    Shared(RefCountedObject* sharedPtr) : _sharedPtr(sharedPtr)
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

protected:
    RefCountedObject* _sharedPtr;
};

template<typename _Type, typename... _Args>
inline Shared<_Type> createShared(_Args&&... args)
{
    return Shared<_Type>(Memory::create<typename Shared<_Type>::RefCountedObject>(static_cast<_Args&&>(args)...));
}

template<typename _Type>
inline int hash(const Shared<_Type>& val)
{
    return hash(*reinterpret_cast<const intptr_t*>(&val._sharedPtr));
}

// Buffer

template<typename _Type>
class Buffer
{
public:
    Buffer() : _size(0), _values(nullptr)
    {
    }

    Buffer(int size)
    {
        ASSERT(size >= 0);
        _size = size;
        _values = Memory::allocate<_Type>(_size);
    }

    Buffer(int size, const _Type& value)
    {
        ASSERT(size >= 0);
        _size = size;
        _values = Memory::allocate<_Type>(_size);

        for (int i = 0; i < _size; ++i)
            _values[i] = value;
    }

    Buffer(int size, const _Type* values)
    {
        ASSERT(values ? size >= 0 : size == 0);
        _size = size;
        _values = Memory::allocate<_Type>(_size);
        memcpy(_values, values, _size * sizeof(_Type));
    }

    Buffer(const Buffer<_Type>& other)
    {
        _size = other._size;
        _values = Memory::allocate<_Type>(_size);
        memcpy(_values, other._values, _size * sizeof(_Type));
    }

    Buffer(Buffer<_Type>&& other)
    {
        _size = other._size;
        _values = other._values;
        other._size = 0;
        other._values = nullptr;
    }

    ~Buffer()
    {
        Memory::deallocate(_values);
    }

    Buffer<_Type>& operator=(const Buffer<_Type>& other)
    {
        assign(other);
        return *this;
    }

    Buffer<_Type>& operator=(Buffer<_Type>&& other)
    {
        Buffer<_Type> tmp(static_cast<Buffer<_Type>&&>(other));
        swap(*this, tmp);
        return *this;
    }

    _Type& operator[](int index)
    {
        return value(index);
    }

    const _Type& operator[](int index) const
    {
        return value(index);
    }

    _Type& value(int index)
    {
        ASSERT(index >= 0 && index < _size);
        return _values[index];
    }

    const _Type& value(int index) const
    {
        ASSERT(index >= 0 && index < _size);
        return _values[index];
    }

    int size() const
    {
        return _size;
    }

    bool empty() const
    {
        return _size == 0;
    }

    _Type* values()
    {
        return _values;
    }

    const _Type* values() const
    {
        return _values;
    }

    void resize(int size)
    {
        ASSERT(size >= 0);
        _size = size;
        _values = Memory::reallocate(_values, _size);
    }

    void assign(const Buffer<_Type>& other)
    {
        _size = other._size;
        _values = Memory::reallocate(_values, _size);
        memcpy(_values, other._values, _size * sizeof(_Type));
    }

    void assign(int size, const _Type& value)
    {
        ASSERT(size >= 0);
        _size = size;
        _values = Memory::reallocate(_values, _size);

        for (int i = 0; i < _size; ++i)
            _values[i] = value;
    }

    void assign(int size, const _Type* values)
    {
        ASSERT(values ? size >= 0 : size == 0);
        _size = size;
        _values = Memory::reallocate(_values, _size);
        memcpy(_values, values, _size * sizeof(_Type));
    }

    void append(const Buffer<_Type>& other)
    {
        _values = Memory::reallocate(_values, _size + other._size);
        memcpy(_values + _size, other._values, other._size * sizeof(_Type));
        _size += other._size;
    }

    void append(int size, const _Type& value)
    {
        ASSERT(size >= 0);
        _values = Memory::reallocate(_values, _size + size);

        for (int i = _size; i < _size + size; ++i)
            _values[i] = value;

        _size += size;
    }

    void append(int size, const _Type* values)
    {
        ASSERT(values ? size >= 0 : size == 0);
        _values = Memory::reallocate(_values, _size + size);
        memcpy(_values + _size, values, size * sizeof(_Type));
        _size += size;
    }

    void clear()
    {
        if (_size > 0)
            memset(_values, 0, _size * sizeof(_Type));
    }

    void reset()
    {
        Memory::deallocate(_values);
        _size = 0;
        _values = nullptr;
    }

    static Buffer<_Type> acquire(int size, _Type* values)
    {
        return Buffer<_Type>(size, values);
    }

    _Type* release()
    {
        _Type* values = _values;
        _size = 0;
        _values = nullptr;
        return values;
    }

    friend void swap(Buffer<_Type>& left, Buffer<_Type>& right)
    {
        swap(left._size, right._size);
        swap(left._values, right._values);
    }

protected:
    Buffer(int size, _Type* values) : _size(size), _values(values)
    {
        ASSERT(values ? size >= 0 : size == 0);
    }

protected:
    int _size;
    _Type* _values;
};

typedef Buffer<byte_t> ByteBuffer;
typedef Buffer<char_t> CharBuffer;
typedef Buffer<unichar_t> UniCharBuffer;

// string support

int strLen(const char_t* str);
char_t* strSet(char_t* str, unichar_t ch, int len);
char_t* strCopy(char_t* destStr, const char_t* srcStr);
char_t* strCopyLen(char_t* destStr, const char_t* srcStr, int len);
char_t* strMove(char_t* destStr, const char_t* srcStr, int len);

int strCompare(const char_t* left, const char_t* right);
int strCompareLen(const char_t* left, const char_t* right, int len);
int strCompareNoCase(const char_t* left, const char_t* right);
int strCompareLenNoCase(const char_t* left, const char_t* right, int len);

char_t* strFind(char_t* str, const char_t* searchStr);
const char_t* strFind(const char_t* str, const char_t* searchStr);
char_t* strFindNoCase(char_t* str, const char_t* searchStr);
const char_t* strFindNoCase(const char_t* str, const char_t* searchStr);

long strToLong(const char_t* str, char_t** end, int base);
unsigned long strToULong(const char_t* str, char_t** end, int base);
long long strToLLong(const char_t* str, char_t** end, int base);
unsigned long long strToULLong(const char_t* str, char_t** end, int base);
float strToFloat(const char_t* str, char_t** end);
double strToDouble(const char_t* str, char_t** end);

bool charIsSpace(unichar_t ch);
bool charIsPrint(unichar_t ch);
bool charIsAlpha(unichar_t ch);
bool charIsDigit(unichar_t ch);
bool charIsAlphaNum(unichar_t ch);
bool charIsUpper(unichar_t ch);
bool charIsLower(unichar_t ch);
unichar_t charToLower(unichar_t ch);
unichar_t charToUpper(unichar_t ch);

void formatString(char_t* str, const char_t* format, ...);
void formatAllocStringArgs(char_t** str, const char_t* format, va_list args);
void formatAllocString(char_t** str, const char_t* format, ...);

// ConstStringIterator

class String;

class ConstStringIterator
{
public:
    ConstStringIterator(const String& str) : _str(str), _pos(nullptr)
    {
    }

    unichar_t value() const;
    bool moveNext();
    bool movePrev();

    void reset()
    {
        _pos = nullptr;
    }

private:
    const String& _str;
    const char_t* _pos;
};

// String

class String
{
public:
    friend class ConstStringIterator;
    typedef ConstStringIterator ConstIterator;

public:
    String() : _length(0), _capacity(0), _chars(nullptr)
    {
    }

    String(const String& other);
    String(const char_t* chars, int len = -1);
    String(unichar_t ch, int n = 1);

    String(String&& other);

    ~String()
    {
        Memory::deallocate(_chars);
    }

    String& operator=(const String& other)
    {
        assign(other);
        return *this;
    }

    String& operator=(const char_t* chars)
    {
        assign(chars);
        return *this;
    }

    String& operator=(String&& other)
    {
        String tmp(static_cast<String&&>(other));
        swap(*this, tmp);
        return *this;
    }

    String& operator+=(const String& str)
    {
        append(str);
        return *this;
    }

    String& operator+=(const char_t* chars)
    {
        append(chars);
        return *this;
    }

    String& operator+=(unichar_t ch)
    {
        append(ch);
        return *this;
    }

    int length() const
    {
        return _length;
    }

    int charLength() const;

    int capacity() const
    {
        return _capacity;
    }

    const char_t* chars() const
    {
        return _chars ? _chars : STR("");
    }

    bool empty() const
    {
        return _length == 0;
    }

    ConstIterator constIterator() const
    {
        return ConstIterator(*this);
    }

    unichar_t charAt(int pos) const;
    int charForward(int pos, int n = 1) const;
    int charBack(int pos, int n = 1) const;

    String substr(int pos, int len = -1) const;

    int compare(const String& str, bool caseSensitive = true) const
    {
        return caseSensitive ? strCompare(this->chars(), str.chars()) : strCompareNoCase(this->chars(), str.chars());
    }

    int compare(const char_t* chars, bool caseSensitive = true) const
    {
        return caseSensitive ? strCompare(this->chars(), chars ? chars : STR("")) :
                               strCompareNoCase(this->chars(), chars ? chars : STR(""));
    }

    int find(const String& str, bool caseSensitive = true, int pos = 0) const;
    int find(const char_t* chars, bool caseSensitive = true, int pos = 0) const;
    int find(unichar_t ch, bool caseSensitive = true, int pos = 0) const;

    bool startsWith(const String& str, bool caseSensitive = true) const;
    bool startsWith(const char_t* chars, bool caseSensitive = true) const;

    bool endsWith(const String& str, bool caseSensitive = true) const;
    bool endsWith(const char_t* chars, bool caseSensitive = true) const;

    bool contains(const String& str, bool caseSensitive = true) const;
    bool contains(const char_t* chars, bool caseSensitive = true) const;

    void ensureCapacity(int capacity);
    void shrinkToLength();

    void assign(const String& other);
    void assign(const char_t* chars, int len = -1);
    void assign(unichar_t ch, int n = 1);

    void append(const String& str);
    void append(const char_t* chars, int len = -1);
    void append(unichar_t ch, int n = 1);

    void appendFormat(const char_t* format, ...);
    void appendFormat(const char_t* format, va_list args);

    void insert(int pos, const String& str);
    void insert(int pos, const char_t* chars, int len = -1);
    void insert(int pos, unichar_t ch, int n = 1);

    void erase(int pos, int len = -1);
    void eraseString(const String& str, bool caseSensitive = true);
    void eraseString(const char_t* chars, bool caseSensitive = true);

    void replace(int pos, const String& str, int len = -1);
    void replace(int pos, const char_t* chars, int len = -1);
    void replaceString(const String& searchStr, const String& replaceStr, bool caseSensitive = true);
    void replaceString(const char_t* searchChars, const char_t* replaceChars, bool caseSensitive = true);

    void trim();
    void trimRight();
    void trimLeft();
    void toUpper();
    void toLower();

    void clear();
    void reset();

    static String acquire(char_t* chars)
    {
        return String(chars);
    }

    char_t* release();

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
        return hash(val.chars());
    }

protected:
    explicit String(char_t* chars);

    template<typename... _Args>
    static char_t* concatInternal(int totalLen, const char_t* chars, _Args&&... args)
    {
        int len = strLen(chars);
        char_t* destChars = concatInternal(totalLen + len, args...);
        strCopyLen(destChars + totalLen, chars, len);
        return destChars;
    }

    static char_t* concatInternal(int totalLen, const char_t* chars)
    {
        int len = strLen(chars);
        char_t* destChars = Memory::allocate<char_t>(totalLen + len + 1);
        destChars[totalLen + len] = 0;
        strCopyLen(destChars + totalLen, chars, len);
        return destChars;
    }

    template<typename... _Args>
    static char_t* concatInternal(int totalLen, const String& str, _Args&&... args)
    {
        if (str._chars)
        {
            int len = strLen(str._chars);
            char_t* destChars = concatInternal(totalLen + len, args...);
            strCopyLen(destChars + totalLen, str._chars, len);
            return destChars;
        }
        else
            return concatInternal(totalLen, args...);
    }

    static char_t* concatInternal(int totalLen, const String& str)
    {
        if (str._chars)
        {
            int len = strLen(str._chars);
            char_t* destChars = Memory::allocate<char_t>(totalLen + len + 1);
            destChars[totalLen + len] = 0;
            strCopyLen(destChars + totalLen, str._chars, len);
            return destChars;
        }
        else
        {
            char_t* destChars = Memory::allocate<char_t>(totalLen + 1);
            destChars[totalLen] = 0;
            return destChars;
        }
    }

protected:
    int _length;
    int _capacity;
    char_t* _chars;
};

// string concatenation

inline String operator+(const String& left, const String& right)
{
    return String::concat(left, right);
}

inline String operator+(const String& left, const char_t* right)
{
    return String::concat(left, right);
}

inline String operator+(const char_t* left, const String& right)
{
    return String::concat(left, right);
}

// string compare

inline bool operator==(const String& left, const String& right)
{
    return strCompare(left.chars(), right.chars()) == 0;
}

inline bool operator!=(const String& left, const String& right)
{
    return strCompare(left.chars(), right.chars()) != 0;
}

inline bool operator<(const String& left, const String& right)
{
    return strCompare(left.chars(), right.chars()) < 0;
}

inline bool operator<=(const String& left, const String& right)
{
    return strCompare(left.chars(), right.chars()) <= 0;
}

inline bool operator>(const String& left, const String& right)
{
    return strCompare(left.chars(), right.chars()) > 0;
}

inline bool operator>=(const String& left, const String& right)
{
    return strCompare(left.chars(), right.chars()) >= 0;
}

inline bool operator==(const char_t* left, const String& right)
{
    return strCompare(left, right.chars()) == 0;
}

inline bool operator!=(const char_t* left, const String& right)
{
    return strCompare(left, right.chars()) != 0;
}

inline bool operator<(const char_t* left, const String& right)
{
    return strCompare(left, right.chars()) < 0;
}

inline bool operator<=(const char_t* left, const String& right)
{
    return strCompare(left, right.chars()) <= 0;
}

inline bool operator>(const char_t* left, const String& right)
{
    return strCompare(left, right.chars()) > 0;
}

inline bool operator>=(const char_t* left, const String& right)
{
    return strCompare(left, right.chars()) >= 0;
}

inline bool operator==(const String& left, const char_t* right)
{
    return strCompare(left.chars(), right) == 0;
}

inline bool operator!=(const String& left, const char_t* right)
{
    return strCompare(left.chars(), right) != 0;
}

inline bool operator<(const String& left, const char_t* right)
{
    return strCompare(left.chars(), right) < 0;
}

inline bool operator<=(const String& left, const char_t* right)
{
    return strCompare(left.chars(), right) <= 0;
}

inline bool operator>(const String& left, const char_t* right)
{
    return strCompare(left.chars(), right) > 0;
}

inline bool operator>=(const String& left, const char_t* right)
{
    return strCompare(left.chars(), right) >= 0;
}

// Unicode support

int utf8CharToUnicode(const char* in, char32_t& ch);
int unicodeCharToUtf8(char32_t ch, char* out);

int utf16CharToUnicode(const char16_t* in, char32_t& ch);
int utf16CharToUnicodeSwapBytes(const char16_t* in, char32_t& ch);
int unicodeCharToUtf16(char32_t ch, char16_t* out);

void utf8StringToUnicode(const char* in, char32_t* out);
void unicodeStringToUtf8(const char32_t* in, char* out);

void utf16StringToUnicode(const char16_t* in, char32_t* out);
void unicodeStringToUtf16(const char32_t* in, char16_t* out);

void utf8StringToUtf16(const char* in, char16_t* out);
void utf16StringToUtf8(const char16_t* in, char* out);

char32_t utf8CharAt(const char* pos);
const char* utf8CharForward(const char* pos);
const char* utf8CharBack(const char* pos);

int utf8CharLength(char32_t ch);
int utf8StringLength(const char* str);

char32_t utf16CharAt(const char16_t* pos);
const char16_t* utf16CharForward(const char16_t* pos);
const char16_t* utf16CharBack(const char16_t* pos);

int utf16CharLength(char32_t ch);
int utf16StringLength(const char16_t* str);

#ifdef CHAR_ENCODING_UTF8

#define UTF_CHAR_TO_UNICODE utf8CharToUnicode
#define UNICODE_CHAR_TO_UTF unicodeCharToUtf8
#define UTF_CHAR_AT utf8CharAt
#define UTF_CHAR_FORWARD utf8CharForward
#define UTF_CHAR_BACK utf8CharBack
#define UTF_CHAR_LENGTH utf8CharLength
#define UTF_STRING_LENGTH utf8StringLength

#else

#define UTF_CHAR_TO_UNICODE utf16CharToUnicode
#define UNICODE_CHAR_TO_UTF unicodeCharToUtf16
#define UTF_CHAR_AT utf16CharAt
#define UTF_CHAR_FORWARD utf16CharForward
#define UTF_CHAR_BACK utf16CharBack
#define UTF_CHAR_LENGTH utf16CharLength
#define UTF_STRING_LENGTH utf16StringLength

#endif

// Unicode

enum TextEncoding
{
    TEXT_ENCODING_UTF8,
    TEXT_ENCODING_UTF16_LE,
    TEXT_ENCODING_UTF16_BE
};

struct Unicode
{
    static String bytesToString(const ByteBuffer& bytes, TextEncoding& encoding, bool& bom, bool& crLf);
    static String bytesToString(int size, const byte_t* bytes, TextEncoding& encoding, bool& bom, bool& crLf);
    static ByteBuffer stringToBytes(const String& str, TextEncoding encoding, bool bom, bool crLf);
};

// ArrayIterator

template<typename _Type>
class Array;

template<typename _Type>
class ArrayIterator
{
public:
    ArrayIterator(Array<_Type>& array) : _array(array), _index(-1)
    {
    }

    _Type& value()
    {
        ASSERT(_index >= 0);
        return _array._values[_index];
    }

    bool moveNext()
    {
        if (++_index < _array._size)
            return true;

        _index = -1;
        return false;
    }

    bool movePrev()
    {
        if (_index < 0)
            _index = _array._size;

        if (--_index >= 0)
            return true;

        _index = -1;
        return false;
    }

    void reset()
    {
        _index = -1;
    }

protected:
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
        ASSERT(_index >= 0);
        return _array._values[_index];
    }

    bool moveNext()
    {
        if (++_index < _array._size)
            return true;

        _index = -1;
        return false;
    }

    bool movePrev()
    {
        if (_index < 0)
            _index = _array._size;

        if (--_index >= 0)
            return true;

        _index = -1;
        return false;
    }

    void reset()
    {
        _index = -1;
    }

protected:
    const Array<_Type>& _array;
    int _index;
};

// Array

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
    Array() : _size(0), _capacity(0), _values(nullptr)
    {
    }

    Array(int size)
    {
        ASSERT(size >= 0);

        _size = size;
        _capacity = size;
        _values = Memory::createArrayFill<_Type>(size, size);
    }

    Array(int size, const _Type& value)
    {
        ASSERT(size >= 0);

        _size = size;
        _capacity = size;
        _values = Memory::createArrayFill<_Type>(size, size, value);
    }

    Array(int size, const _Type* values)
    {
        ASSERT(values ? size >= 0 : size == 0);

        _size = size;
        _capacity = size;
        _values = Memory::createArrayCopy(size, size, values);
    }

    Array(const Array<_Type>& other)
    {
        _size = other._size;
        _capacity = other._size;
        _values = Memory::createArrayCopy(other._size, other._size, other._values);
    }

    Array(Array<_Type>&& other)
    {
        _size = other._size;
        _capacity = other._capacity;
        _values = other._values;

        other._size = 0;
        other._capacity = 0;
        other._values = nullptr;
    }

    ~Array()
    {
        Memory::destroyArray(_size, _values);
    }

    Array<_Type>& operator=(const Array<_Type>& other)
    {
        assign(other);
        return *this;
    }

    Array<_Type>& operator=(Array<_Type>&& other)
    {
        Array<_Type> tmp(static_cast<Array<_Type>&&>(other));
        swap(*this, tmp);
        return *this;
    }

    _Type& operator[](int index)
    {
        return value(index);
    }

    const _Type& operator[](int index) const
    {
        return value(index);
    }

    _Type& value(int index)
    {
        ASSERT(index >= 0 && index < _size);
        return _values[index];
    }

    const _Type& value(int index) const
    {
        ASSERT(index >= 0 && index < _size);
        return _values[index];
    }

    int size() const
    {
        return _size;
    }

    int capacity() const
    {
        return _capacity;
    }

    bool empty() const
    {
        return _size == 0;
    }

    const _Type* values() const
    {
        return _values;
    }

    _Type& first()
    {
        ASSERT(_size > 0);
        return _values[0];
    }

    const _Type& first() const
    {
        ASSERT(_size > 0);
        return _values[0];
    }

    _Type& last()
    {
        ASSERT(_size > 0);
        return _values[_size - 1];
    }

    const _Type& last() const
    {
        ASSERT(_size > 0);
        return _values[_size - 1];
    }

    Iterator iterator()
    {
        return Iterator(*this);
    }

    ConstIterator constIterator() const
    {
        return ConstIterator(*this);
    }

    int find(const _Type& value) const
    {
        for (int i = 0; i < _size; ++i)
            if (_values[i] == value)
                return i;

        return INVALID_POSITION;
    }

    void ensureCapacity(int capacity)
    {
        ASSERT(capacity >= 0);

        if (capacity > _capacity)
            reallocate(capacity);
    }

    void shrinkToLength()
    {
        if (_capacity > _size)
            reallocate(_size);
    }

    void resize(int size)
    {
        ASSERT(size >= 0);

        ensureCapacity(size);

        if (size > _size)
        {
            for (; _size < size; ++_size)
                Memory::construct<_Type>(_values + _size);
        }
        else
        {
            for (; _size > size; --_size)
                Memory::destruct(_values + _size);
        }
    }

    void resize(int size, const _Type& value)
    {
        ASSERT(size >= 0);

        ensureCapacity(size);

        if (size > _size)
        {
            for (; _size < size; ++_size)
                Memory::construct<_Type>(_values + _size, value);
        }
        else
        {
            for (; _size > size; --_size)
                Memory::destruct(_values + _size);
        }
    }

    void assign(int size, const _Type& value)
    {
        ASSERT(size >= 0);

        if (size <= _capacity)
        {
            clear();

            while (_size < size)
            {
                Memory::construct<_Type>(_values + _size, value);
                ++_size;
            }
        }
        else
        {
            Array<_Type> tmp(size, value);
            swap(*this, tmp);
        }
    }

    void assign(int size, const _Type* values)
    {
        ASSERT(values ? (size >= 0) && (values != _values) : size == 0);

        if (size <= _capacity)
        {
            clear();

            while (_size < size)
            {
                Memory::construct<_Type>(_values + _size, values[_size]);
                ++_size;
            }
        }
        else
        {
            Array<_Type> tmp(size, values);
            swap(*this, tmp);
        }
    }

    void assign(const Array<_Type>& other)
    {
        ASSERT(this != &other);

        if (other._size <= _capacity)
        {
            clear();

            while (_size < other._size)
            {
                Memory::construct<_Type>(_values + _size, other[_size]);
                ++_size;
            }
        }
        else
        {
            Array<_Type> tmp(other);
            swap(*this, tmp);
        }
    }

    void removeLast()
    {
        ASSERT(_size > 0);

        Memory::destruct(_values + _size - 1);
        --_size;
    }

    void addLast(const _Type& value)
    {
        if (_size >= _capacity)
            reallocate(_capacity * 2 + 1);

        Memory::construct<_Type>(_values + _size, value);
        ++_size;
    }

    void addLast(_Type&& value)
    {
        if (_size >= _capacity)
            reallocate(_capacity * 2 + 1);

        Memory::construct<_Type>(_values + _size, static_cast<_Type&&>(value));
        ++_size;
    }

    void insert(int index, const _Type& value)
    {
        ASSERT(index >= 0 && index <= _size);

        if (_size >= _capacity)
            reallocate(_capacity * 2 + 1);

        Memory::construct<_Type>(_values + _size, value);

        for (int i = _size; i > index; --i)
            swap(_values[i], _values[i - 1]);

        ++_size;
    }

    void insert(int index, _Type&& value)
    {
        ASSERT(index >= 0 && index <= _size);

        if (_size >= _capacity)
            reallocate(_capacity * 2 + 1);

        Memory::construct<_Type>(_values + _size, static_cast<_Type&&>(value));

        for (int i = _size; i > index; --i)
            swap(_values[i], _values[i - 1]);

        ++_size;
    }

    void remove(int index)
    {
        ASSERT(index >= 0 && index < _size);

        --_size;

        for (int i = index; i < _size; ++i)
            swap(_values[i], _values[i + 1]);

        Memory::destruct(_values + _size);
    }

    void sort()
    {
        if (_size > 0)
            quicksort(0, _size - 1);
    }

    void clear()
    {
        Memory::destructArray(_size, _values);
        _size = 0;
    }

    void reset()
    {
        Memory::destroyArray(_size, _values);

        _size = 0;
        _capacity = 0;
        _values = nullptr;
    }

    static Array<_Type> acquire(int size, _Type* values)
    {
        return Array<_Type>(size, size, values);
    }

    _Type* release()
    {
        _Type* values = _values;
        _size = 0;
        _capacity = 0;
        _values = nullptr;

        return values;
    }

    friend void swap(Array<_Type>& left, Array<_Type>& right)
    {
        swap(left._size, right._size);
        swap(left._capacity, right._capacity);
        swap(left._values, right._values);
    }

protected:
    Array(int size, int capacity, _Type* values) : _size(size), _capacity(capacity), _values(values)
    {
        ASSERT(values ? size >= 0 : size == 0);
        ASSERT(capacity >= 0 && size <= capacity);
    }

    void reallocate(int capacity)
    {
        ASSERT(capacity >= 0);

        _Type* values = Memory::createArrayMove(_size, capacity, _values);
        Memory::destroyArray(_size, _values);
        _values = values;
        _capacity = capacity;
    }

    void quicksort(int low, int high)
    {
        if (low < high)
        {
            int p = partition(low, high);
            quicksort(low, p);
            quicksort(p + 1, high);
        }
    }

    int partition(int low, int high)
    {
        _Type& pivot = _values[low];
        int i = low - 1;
        int j = high + 1;

        while (true)
        {
            do
                ++i;
            while (_values[i] < pivot);

            do
                --j;
            while (pivot < _values[j]);

            if (i >= j)
                return j;

            swap(_values[i], _values[j]);
        }
    }

protected:
    int _size;
    int _capacity;
    _Type* _values;
};

// ListNode

template<typename _Type>
struct ListNode
{
    _Type value;
    ListNode<_Type>* prev;
    ListNode<_Type>* next;

    ListNode(const _Type& value, ListNode<_Type>* prev, ListNode<_Type>* next) : value(value), prev(prev), next(next)
    {
    }

    ListNode(_Type&& value, ListNode<_Type>* prev, ListNode<_Type>* next) :
        value(static_cast<_Type&&>(value)), prev(prev), next(next)
    {
    }
};

// ListIterator

template<typename _Type>
class List;

template<typename _Type>
class ListIterator
{
public:
    ListIterator(List<_Type>& list) : _list(list), _node(nullptr)
    {
    }

    _Type& value()
    {
        ASSERT(_node);
        return _node->value;
    }

    bool moveNext()
    {
        if (_node)
            _node = _node->next;
        else
            _node = _list.first();

        return _node != nullptr;
    }

    bool movePrev()
    {
        if (_node)
            _node = _node->prev;
        else
            _node = _list.last();

        return _node != nullptr;
    }

    void reset()
    {
        _node = nullptr;
    }

protected:
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
        ASSERT(_node);
        return _node->value;
    }

    bool moveNext()
    {
        if (_node)
            _node = _node->next;
        else
            _node = _list.first();

        return _node != nullptr;
    }

    bool movePrev()
    {
        if (_node)
            _node = _node->prev;
        else
            _node = _list.last();

        return _node != nullptr;
    }

    void reset()
    {
        _node = nullptr;
    }

protected:
    const List<_Type>& _list;
    const ListNode<_Type>* _node;
};

// List

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
    List() : _first(nullptr), _last(nullptr)
    {
    }

    List(int size) : _first(nullptr), _last(nullptr)
    {
        ASSERT(size >= 0);

        try
        {
            for (int i = 0; i < size; ++i)
                addLast(_Type());
        }
        catch (...)
        {
            destroyNodes();
            throw;
        }
    }

    List(int size, const _Type& value) : _first(nullptr), _last(nullptr)
    {
        ASSERT(size >= 0);

        try
        {
            for (int i = 0; i < size; ++i)
                addLast(value);
        }
        catch (...)
        {
            destroyNodes();
            throw;
        }
    }

    List(int size, const _Type* values) : _first(nullptr), _last(nullptr)
    {
        ASSERT(values ? size >= 0 : size == 0);

        try
        {
            for (int i = 0; i < size; ++i)
                addLast(values[i]);
        }
        catch (...)
        {
            destroyNodes();
            throw;
        }
    }

    List(const List<_Type>& other) : _first(nullptr), _last(nullptr)
    {
        try
        {
            for (auto node = other.first(); node; node = node->next)
                addLast(node->value);
        }
        catch (...)
        {
            destroyNodes();
            throw;
        }
    }

    List(List<_Type>&& other)
    {
        _first = other._first;
        _last = other._last;
        other._first = nullptr;
        other._last = nullptr;
    }

    ~List()
    {
        destroyNodes();
    }

    List<_Type>& operator=(const List<_Type>& other)
    {
        assign(other);
        return *this;
    }

    List<_Type>& operator=(List<_Type>&& other)
    {
        List<_Type> tmp(static_cast<List<_Type>&&>(other));
        swap(*this, tmp);
        return *this;
    }

    int size() const
    {
        int cnt = 0;

        for (auto node = _first; node; node = node->next)
            ++cnt;

        return cnt;
    }

    bool empty() const
    {
        return !_first;
    }

    ListNode<_Type>* first()
    {
        return _first;
    }

    const ListNode<_Type>* first() const
    {
        return _first;
    }

    ListNode<_Type>* last()
    {
        return _last;
    }

    const ListNode<_Type>* last() const
    {
        return _last;
    }

    Iterator iterator()
    {
        return Iterator(*this);
    }

    ConstIterator constIterator() const
    {
        return ConstIterator(*this);
    }

    ListNode<_Type>* find(const _Type& value)
    {
        for (auto node = _first; node; node = node->next)
            if (node->value == value)
                return node;

        return nullptr;
    }

    const ListNode<_Type>* find(const _Type& value) const
    {
        for (auto node = _first; node; node = node->next)
            if (node->value == value)
                return node;

        return nullptr;
    }

    void assign(int size, const _Type& value)
    {
        List<_Type> tmp(size, value);
        swap(*this, tmp);
    }

    void assign(int size, const _Type* values)
    {
        List<_Type> tmp(size, values);
        swap(*this, tmp);
    }

    void assign(const List<_Type>& other)
    {
        List<_Type> tmp(other);
        swap(*this, tmp);
    }

    void removeFirst()
    {
        ASSERT(_first);

        auto node = _first;
        _first = _first->next;

        if (_first)
            _first->prev = nullptr;
        else
            _last = nullptr;

        Memory::destroy(node);
    }

    void removeLast()
    {
        ASSERT(_last);

        auto node = _last;
        _last = _last->prev;

        if (_last)
            _last->next = nullptr;
        else
            _first = nullptr;

        Memory::destroy(node);
    }

    void addFirst(const _Type& value)
    {
        if (_first)
        {
            auto node = createListNode(value, nullptr, _first);
            _first->prev = node;
            _first = node;
        }
        else
        {
            auto node = createListNode(value, nullptr, nullptr);
            _first = _last = node;
        }
    }

    void addFirst(_Type&& value)
    {
        if (_first)
        {
            auto node = createListNode(static_cast<_Type&&>(value), nullptr, _first);
            _first->prev = node;
            _first = node;
        }
        else
        {
            auto node = createListNode(static_cast<_Type&&>(value), nullptr, nullptr);
            _first = _last = node;
        }
    }

    void addLast(const _Type& value)
    {
        if (_first)
        {
            auto node = createListNode(value, _last, nullptr);
            _last->next = node;
            _last = node;
        }
        else
        {
            auto node = createListNode(value, nullptr, nullptr);
            _first = _last = node;
        }
    }

    void addLast(_Type&& value)
    {
        if (_first)
        {
            auto node = createListNode(static_cast<_Type&&>(value), _last, nullptr);
            _last->next = node;
            _last = node;
        }
        else
        {
            auto node = createListNode(static_cast<_Type&&>(value), nullptr, nullptr);
            _first = _last = node;
        }
    }

    ListNode<_Type>* insertBefore(ListNode<_Type>* pos, const _Type& value)
    {
        ASSERT(pos);

        auto node = createListNode(value, pos->prev, pos);

        if (pos->prev)
            pos->prev->next = node;
        else
            _first = node;

        pos->prev = node;
        return node;
    }

    ListNode<_Type>* insertBefore(ListNode<_Type>* pos, _Type&& value)
    {
        ASSERT(pos);

        auto node = createListNode(static_cast<_Type&&>(value), pos->prev, pos);

        if (pos->prev)
            pos->prev->next = node;
        else
            _first = node;

        pos->prev = node;
        return node;
    }

    ListNode<_Type>* insertAfter(ListNode<_Type>* pos, const _Type& value)
    {
        ASSERT(pos);

        auto node = createListNode(value, pos, pos->next);

        if (pos->next)
            pos->next->prev = node;
        else
            _last = node;

        pos->next = node;
        return node;
    }

    ListNode<_Type>* insertAfter(ListNode<_Type>* pos, _Type&& value)
    {
        ASSERT(pos);

        auto node = createListNode(static_cast<_Type&&>(value), pos, pos->next);

        if (pos->next)
            pos->next->prev = node;
        else
            _last = node;

        pos->next = node;
        return node;
    }

    void remove(ListNode<_Type>* pos)
    {
        ASSERT(pos);

        if (pos->prev)
            pos->prev->next = pos->next;
        else
            _first = pos->next;

        if (pos->next)
            pos->next->prev = pos->prev;
        else
            _last = pos->prev;

        Memory::destroy(pos);
    }

    void clear()
    {
        destroyNodes();
        _first = _last = nullptr;
    }

    friend void swap(List<_Type>& left, List<_Type>& right)
    {
        swap(left._first, right._first);
        swap(left._last, right._last);
    }

protected:
    ListNode<_Type>* createListNode(const _Type& value, ListNode<_Type>* prev, ListNode<_Type>* next)
    {
        auto ptr = Memory::allocate<ListNode<_Type>>();

        try
        {
            ::new (ptr) ListNode<_Type>(value, prev, next);
        }
        catch (...)
        {
            Memory::deallocate(ptr);
            throw;
        }

        return ptr;
    }

    ListNode<_Type>* createListNode(_Type&& value, ListNode<_Type>* prev, ListNode<_Type>* next)
    {
        auto ptr = Memory::allocate<ListNode<_Type>>();

        try
        {
            ::new (ptr) ListNode<_Type>(static_cast<_Type&&>(value), prev, next);
        }
        catch (...)
        {
            Memory::deallocate(ptr);
            throw;
        }

        return ptr;
    }

    void destroyNodes()
    {
        for (auto node = _first; node;)
        {
            auto n = node;
            node = node->next;
            Memory::destroy(n);
        }
    }

protected:
    ListNode<_Type>* _first;
    ListNode<_Type>* _last;
};

// KeyValue

template<typename _Key, typename _Value>
struct KeyValue
{
    const _Key key;
    _Value value;

    KeyValue(const _Key& key) : key(key), value()
    {
    }

    KeyValue(_Key&& key) : key(static_cast<_Key&&>(key)), value()
    {
    }

    KeyValue(const _Key& key, const _Value& value) : key(key), value(value)
    {
    }

    KeyValue(_Key&& key, _Value&& value) : key(static_cast<_Key&&>(key)), value(static_cast<_Value&&>(value))
    {
    }

    KeyValue(const KeyValue<_Key, _Value>& other) : key(other.key), value(other.value)
    {
    }

    KeyValue(KeyValue<_Key, _Value>&& other) :
        key(const_cast<_Key&&>(other.key)), value(static_cast<_Value&&>(other.value))
    {
    }
};

// MapIterator

template<typename _Key, typename _Value>
class Map;

template<typename _Key, typename _Value>
class MapIterator
{
public:
    MapIterator(Map<_Key, _Value>& map) : _map(map), _index(0), _node(nullptr)
    {
    }

    KeyValue<_Key, _Value>& value()
    {
        ASSERT(_node);
        return _node->value;
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
                    _node = _map._keyValues[_index].first();

                    if (_node)
                        return true;
                }
            }
        }
        else
        {
            for (_index = 0; _index < _map._keyValues.size(); ++_index)
            {
                _node = _map._keyValues[_index].first();

                if (_node)
                    return true;
            }
        }

        _index = 0;
        _node = nullptr;
        return false;
    }

    void reset()
    {
        _index = 0;
        _node = nullptr;
    }

protected:
    Map<_Key, _Value>& _map;
    int _index;
    ListNode<KeyValue<_Key, _Value>>* _node;
};

// ConstMapIterator

template<typename _Key, typename _Value>
class ConstMapIterator
{
public:
    ConstMapIterator(const Map<_Key, _Value>& map) : _map(map), _index(0), _node(nullptr)
    {
    }

    const KeyValue<_Key, _Value>& value() const
    {
        ASSERT(_node);
        return _node->value;
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
                    _node = _map._keyValues[_index].first();

                    if (_node)
                        return true;
                }
            }
        }
        else
        {
            for (_index = 0; _index < _map._keyValues.size(); ++_index)
            {
                _node = _map._keyValues[_index].first();

                if (_node)
                    return true;
            }
        }

        _index = 0;
        _node = nullptr;
        return false;
    }

    void reset()
    {
        _index = 0;
        _node = nullptr;
    }

protected:
    const Map<_Key, _Value>& _map;
    int _index;
    const ListNode<KeyValue<_Key, _Value>>* _node;
};

// Map

template<typename _Key, typename _Value>
class Map
{
public:
    template<typename, typename>
    friend class MapIterator;

    template<typename, typename>
    friend class ConstMapIterator;

    typedef MapIterator<_Key, _Value> Iterator;
    typedef ConstMapIterator<_Key, _Value> ConstIterator;

public:
    Map(int numBuckets = 0) : _keyValues(numBuckets), _size(0), _maxLoadFactor(0.75f)
    {
        ASSERT(numBuckets >= 0);
    }

    Map(const Map<_Key, _Value>& other) :
        _keyValues(other._keyValues), _size(other._size), _maxLoadFactor(other._maxLoadFactor)
    {
    }

    Map(Map<_Key, _Value>&& other) : _keyValues(static_cast<Array<List<KeyValue<_Key, _Value>>>&&>(other._keyValues))
    {
        _size = other._size;
        other._size = 0;
        _maxLoadFactor = other._maxLoadFactor;
    }

    _Value& operator[](const _Key& key)
    {
        return value(key);
    }

    _Value& operator[](_Key&& key)
    {
        return value(static_cast<_Key&&>(key));
    }

    const _Value& operator[](const _Key& key) const
    {
        return value(key);
    }

    Map<_Key, _Value>& operator=(const Map<_Key, _Value>& other)
    {
        assign(other);
        return *this;
    }

    Map<_Key, _Value>& operator=(Map<_Key, _Value>&& other)
    {
        Map<_Key, _Value> tmp(static_cast<Map<_Key, _Value>&&>(other));
        swap(*this, tmp);
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

    float maxLoadFactor() const
    {
        return _maxLoadFactor;
    }

    void maxLoadFactor(float loadFactor)
    {
        ASSERT(loadFactor > 0);
        _maxLoadFactor = loadFactor;
    }

    Iterator iterator()
    {
        return Iterator(*this);
    }

    ConstIterator constIterator() const
    {
        return ConstIterator(*this);
    }

    _Value& value(const _Key& key)
    {
        if (_keyValues.empty() || loadFactor() > _maxLoadFactor)
            rehash(_size * 2 / _maxLoadFactor + 1);

        auto& bucket = getBucket(key);

        for (auto node = bucket.first(); node; node = node->next)
        {
            if (node->value.key == key)
                return node->value.value;
        }

        bucket.addLast(KeyValue<_Key, _Value>(key));
        ++_size;

        return bucket.last()->value.value;
    }

    _Value& value(_Key&& key)
    {
        if (_keyValues.empty() || loadFactor() > _maxLoadFactor)
            rehash(_size * 2 / _maxLoadFactor + 1);

        auto& bucket = getBucket(key);

        for (auto node = bucket.first(); node; node = node->next)
        {
            if (node->value.key == key)
                return node->value.value;
        }

        bucket.addLast(KeyValue<_Key, _Value>(static_cast<_Key&&>(key)));
        ++_size;

        return bucket.last()->value.value;
    }

    const _Value& value(const _Key& key) const
    {
        const _Value* value = find(key);
        if (value)
            return *value;
        else
            throw Exception(STR("not found"));
    }

    _Value* find(const _Key& key)
    {
        if (!_keyValues.empty())
        {
            auto& bucket = getBucket(key);

            for (auto node = bucket.first(); node; node = node->next)
            {
                if (node->value.key == key)
                    return &node->value.value;
            }
        }

        return nullptr;
    }

    const _Value* find(const _Key& key) const
    {
        if (!_keyValues.empty())
        {
            auto& bucket = getBucket(key);

            for (auto node = bucket.first(); node; node = node->next)
            {
                if (node->value.key == key)
                    return &node->value.value;
            }
        }

        return nullptr;
    }

    void assign(const Map<_Key, _Value>& other)
    {
        Map<_Key, _Value> tmp(other);
        swap(*this, tmp);
    }

    void add(const _Key& key, const _Value& value)
    {
        if (_keyValues.empty() || loadFactor() > _maxLoadFactor)
            rehash(_size * 2 / _maxLoadFactor + 1);

        auto& bucket = getBucket(key);

        for (auto node = bucket.first(); node; node = node->next)
        {
            if (node->value.key == key)
            {
                node->value.value = value;
                return;
            }
        }

        bucket.addLast(KeyValue<_Key, _Value>(key, value));
        ++_size;
    }

    void add(_Key&& key, _Value&& value)
    {
        if (_keyValues.empty() || loadFactor() > _maxLoadFactor)
            rehash(_size * 2 / _maxLoadFactor + 1);

        auto& bucket = getBucket(key);

        for (auto node = bucket.first(); node; node = node->next)
        {
            if (node->value.key == key)
            {
                node->value.value = static_cast<_Value&&>(value);
                return;
            }
        }

        bucket.addLast(KeyValue<_Key, _Value>(static_cast<_Key&&>(key), static_cast<_Value&&>(value)));

        ++_size;
    }

    bool remove(const _Key& key)
    {
        if (!_keyValues.empty())
        {
            auto& bucket = getBucket(key);

            for (auto node = bucket.first(); node; node = node->next)
            {
                if (node->value.key == key)
                {
                    bucket.remove(node);
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
        ASSERT(numBuckets >= 0);

        Map<_Key, _Value> tmp(numBuckets);

        for (int i = 0; i < _keyValues.size(); ++i)
        {
            for (auto node = _keyValues[i].first(); node; node = node->next)
                tmp.add(const_cast<_Key&&>(node->value.key), static_cast<_Value&&>(node->value.value));
        }

        swap(*this, tmp);
    }

    friend void swap(Map<_Key, _Value>& left, Map<_Key, _Value>& right)
    {
        swap(left._keyValues, right._keyValues);
        swap(left._size, right._size);
    }

protected:
    List<KeyValue<_Key, _Value>>& getBucket(const _Key& key)
    {
        return _keyValues[getBucketIndex(key)];
    }

    const List<KeyValue<_Key, _Value>>& getBucket(const _Key& key) const
    {
        return _keyValues[getBucketIndex(key)];
    }

    int getBucketIndex(const _Key& key) const
    {
        int h = hash(key);
        return abs(h) % _keyValues.size();
    }

protected:
    Array<List<KeyValue<_Key, _Value>>> _keyValues;
    int _size;
    float _maxLoadFactor;
};

// ConstSetIterator

template<typename _Type>
class Set;

template<typename _Type>
class ConstSetIterator
{
public:
    ConstSetIterator(const Set<_Type>& set) : _set(set), _index(0), _node(nullptr)
    {
    }

    const _Type& value() const
    {
        ASSERT(_node);
        return _node->value;
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
                    _node = _set._values[_index].first();

                    if (_node)
                        return true;
                }
            }
        }
        else
        {
            for (_index = 0; _index < _set._values.size(); ++_index)
            {
                _node = _set._values[_index].first();

                if (_node)
                    return true;
            }
        }

        _index = 0;
        _node = nullptr;
        return false;
    }

    void reset()
    {
        _index = 0;
        _node = nullptr;
    }

protected:
    const Set<_Type>& _set;
    int _index;
    const ListNode<_Type>* _node;
};

// Set

template<typename _Type>
class Set
{
public:
    template<typename>
    friend class ConstSetIterator;

    typedef ConstSetIterator<_Type> ConstIterator;

public:
    Set(int numBuckets = 0) : _values(numBuckets), _size(0), _maxLoadFactor(0.75f)
    {
        ASSERT(numBuckets >= 0);
    }

    Set(const Set<_Type>& other) : _values(other._values), _size(other._size), _maxLoadFactor(other._maxLoadFactor)
    {
    }

    Set(Set<_Type>&& other) : _values(static_cast<Array<List<_Type>>&&>(other._values))
    {
        _size = other._size;
        other._size = 0;
        _maxLoadFactor = other._maxLoadFactor;
    }

    Set<_Type>& operator=(const Set<_Type>& other)
    {
        assign(other);
        return *this;
    }

    Set<_Type>& operator=(Set<_Type>&& other)
    {
        Set<_Type> tmp(static_cast<Set<_Type>&&>(other));
        swap(*this, tmp);
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

    ConstIterator constIterator() const
    {
        return ConstIterator(*this);
    }

    float loadFactor() const
    {
        return static_cast<float>(_size) / _values.size();
    }

    float maxLoadFactor() const
    {
        return _maxLoadFactor;
    }

    void maxLoadFactor(float loadFactor)
    {
        ASSERT(loadFactor > 0);
        _maxLoadFactor = loadFactor;
    }

    bool contains(const _Type& value) const
    {
        auto& bucket = getBucket(value);

        for (auto node = bucket.first(); node; node = node->next)
        {
            if (node->value == value)
                return true;
        }

        return false;
    }

    void assign(const Set<_Type>& other)
    {
        Set<_Type> tmp(other);
        swap(*this, tmp);
    }

    void add(const _Type& value)
    {
        if (_values.empty() || loadFactor() > _maxLoadFactor)
            rehash(_size * 2 / _maxLoadFactor + 1);

        auto& bucket = getBucket(value);

        for (auto node = bucket.first(); node; node = node->next)
        {
            if (node->value == value)
                return;
        }

        bucket.addLast(value);
        ++_size;
    }

    void add(_Type&& value)
    {
        if (_values.empty() || loadFactor() > _maxLoadFactor)
            rehash(_size * 2 / _maxLoadFactor + 1);

        auto& bucket = getBucket(value);

        for (auto node = bucket.first(); node; node = node->next)
        {
            if (node->value == value)
                return;
        }

        bucket.addLast(static_cast<_Type&&>(value));
        ++_size;
    }

    _Type remove()
    {
        for (int i = 0; i < _values.size(); ++i)
        {
            auto& bucket = _values[i];

            if (!bucket.empty())
            {
                _Type value = static_cast<_Type&&>(bucket.first()->value);
                bucket.removeFirst();
                --_size;
                return value;
            }
        }

        throw Exception(STR("set is empty"));
    }

    bool remove(const _Type& value)
    {
        auto& bucket = getBucket(value);

        for (auto node = bucket.first(); node; node = node->next)
        {
            if (node->value == value)
            {
                bucket.remove(node);
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
        ASSERT(numBuckets >= 0);

        Set<_Type> tmp(numBuckets);

        for (int i = 0; i < _values.size(); ++i)
        {
            for (auto node = _values[i].first(); node; node = node->next)
                tmp.add(static_cast<_Type&&>(node->value));
        }

        swap(*this, tmp);
    }

    friend void swap(Set<_Type>& left, Set<_Type>& right)
    {
        swap(left._values, right._values);
        swap(left._size, right._size);
    }

protected:
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
        return abs(h) % _values.size();
    }

protected:
    Array<List<_Type>> _values;
    int _size;
    float _maxLoadFactor;
};

#endif
