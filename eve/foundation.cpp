#include <foundation.h>

// string support

char_t* strSet(char_t* str, unichar_t ch, int len)
{
    char_t s[4];
    int n = UNICODE_CHAR_TO_UTF(ch, s);

    for (int i = 0; i < len; ++i)
        for (int j = 0; j < n; ++j)
            *str++ = s[j];

    return str;
}

char_t* strCopyLen(char_t* destStr, const char_t* srcStr, int len)
{
    memcpy(destStr, srcStr, len * sizeof(char_t));
    return destStr + len;
}

char_t* strMove(char_t* destStr, const char_t* srcStr, int len)
{
    memmove(destStr, srcStr, len * sizeof(char_t));
    return destStr + len;
}

#ifdef PLATFORM_WINDOWS

int strLen(const char_t* str)
{
    return wcslen(reinterpret_cast<const wchar_t*>(str));
}

char_t* strCopy(char_t* destStr, const char_t* srcStr)
{
    return reinterpret_cast<char_t*>(
        wcscpy(reinterpret_cast<wchar_t*>(destStr), 
            reinterpret_cast<const wchar_t*>(srcStr)));
}

int strCompare(const char_t* left, const char_t* right)
{
    return wcscmp(reinterpret_cast<const wchar_t*>(left), 
        reinterpret_cast<const wchar_t*>(right));
}

int strCompareLen(const char_t* left, const char_t* right, int len)
{
    return wcsncmp(reinterpret_cast<const wchar_t*>(left), 
        reinterpret_cast<const wchar_t*>(right), len);
}

int strCompareNoCase(const char_t* left, const char_t* right)
{
    return _wcsicmp(reinterpret_cast<const wchar_t*>(left), 
        reinterpret_cast<const wchar_t*>(right));
}

int strCompareLenNoCase(const char_t* left, const char_t* right, int len)
{
    return _wcsnicmp(reinterpret_cast<const wchar_t*>(left), 
        reinterpret_cast<const wchar_t*>(right), len);
}

char_t* strFind(const char_t* str, const char_t* searchStr)
{
    return reinterpret_cast<char_t*>(const_cast<wchar_t*>(
        wcsstr(reinterpret_cast<const wchar_t*>(str), 
            reinterpret_cast<const wchar_t*>(searchStr))));
}

char_t* strFindNoCase(const char_t* str, const char_t* searchStr)
{
    int index = FindNLSStringEx(LOCALE_NAME_USER_DEFAULT, 
        FIND_FROMSTART | LINGUISTIC_IGNORECASE, 
        reinterpret_cast<const wchar_t*>(str), -1, 
        reinterpret_cast<const wchar_t*>(searchStr),
        -1, nullptr, nullptr, nullptr, 0);

    return const_cast<char_t*>(index >= 0 ? str + index : nullptr);
}

long strToLong(const char_t* str, char_t** end, int base)
{
    return wcstol(reinterpret_cast<const wchar_t*>(str), 
        reinterpret_cast<wchar_t**>(end), base);
}

unsigned long strToULong(const char_t* str, char_t** end, int base)
{
    return wcstoul(reinterpret_cast<const wchar_t*>(str), 
        reinterpret_cast<wchar_t**>(end), base);
}

long long strToLLong(const char_t* str, char_t** end, int base)
{
    return wcstoll(reinterpret_cast<const wchar_t*>(str), 
        reinterpret_cast<wchar_t**>(end), base);
}

unsigned long long strToULLong(const char_t* str, char_t** end, int base)
{
    return wcstoull(reinterpret_cast<const wchar_t*>(str), 
        reinterpret_cast<wchar_t**>(end), base);
}

float strToFloat(const char_t* str, char_t** end)
{
    return wcstof(reinterpret_cast<const wchar_t*>(str), 
        reinterpret_cast<wchar_t**>(end));
}

double strToDouble(const char_t* str, char_t** end)
{
    return wcstod(reinterpret_cast<const wchar_t*>(str), 
        reinterpret_cast<wchar_t**>(end));
}

bool charIsSpace(unichar_t ch)
{
    return iswspace(ch) != 0;
}

bool charIsPrint(unichar_t ch)
{
    return iswprint(ch) != 0;
}

bool charIsAlphaNum(unichar_t ch)
{
    return iswalnum(ch) != 0;
}

unichar_t charToLower(unichar_t ch)
{
    return towlower(ch);
}

unichar_t charToUpper(unichar_t ch)
{
    return towupper(ch);
}

void print(const char_t* str)
{
    _putws(reinterpret_cast<const wchar_t*>(str));
}

void printFormatted(const char_t* format, ...)
{
    va_list args;
    va_start(args, format);
    wprintf(reinterpret_cast<const wchar_t*>(format), args);
    va_end(args);
}

void printString(char_t* str, const char_t* format, ...)
{
    va_list args;
    va_start(args, format);
    swprintf(reinterpret_cast<wchar_t*>(str), 
        reinterpret_cast<const wchar_t*>(format), args);
    va_end(args);
}


void printArgs(const char_t* format, va_list args)
{
    vwprintf(reinterpret_cast<const wchar_t*>(format), args);
}

void printArgsAlloc(char_t** str, const char_t* format, va_list args)
{
    va_list args2;

    va_copy(args2, args);
    int len = _vscwprintf(reinterpret_cast<const wchar_t*>(format), args2);
    va_end(args2);

    if (len > 0)
    {
        *str = Memory::allocate<char_t>(len + 1);
        _vsnwprintf(reinterpret_cast<wchar_t*>(*str), 
            len + 1, reinterpret_cast<const wchar_t*>(format), args);
    }
    else
        *str = 0;
}

void printAlloc(char_t** str, const char_t* format, ...)
{
    va_list args;
    va_start(args, format);
    printArgsAlloc(str, format, args);
    va_end(args);
}

void putChar(unichar_t ch)
{
    putwchar(ch);
}

unichar_t getChar()
{
    return getwchar();
}

#else

int strLen(const char_t* str)
{
    return strlen(str);
}

char_t* strCopy(char_t* destStr, const char_t* srcStr)
{
    return strcpy(destStr, srcStr);
}

int strCompare(const char_t* left, const char_t* right)
{
    return strcmp(left, right);
}

int strCompareLen(const char_t* left, const char_t* right, int len)
{
    return strncmp(left, right, len);
}

int strCompareNoCase(const char_t* left, const char_t* right)
{
    return strcasecmp(left, right);
}

int strCompareLenNoCase(const char_t* left, const char_t* right, int len)
{
    return strncasecmp(left, right, len);
}

char_t* strFind(const char_t* str, const char_t* searchStr)
{
    return strstr(str, searchStr);
}

char_t* strFindNoCase(const char_t* str, const char_t* searchStr)
{
#ifdef PLATFORM_AIX
    for (; *str; ++str)
        if (!strcasecmp(str, searchStr))
            return str;

    return nullptr;
#else
    return strcasestr(str, searchStr);
#endif
}

long strToLong(const char_t* str, char_t** end, int base)
{
    return strtol(str, end, base);
}

unsigned long strToULong(const char_t* str, char_t** end, int base)
{
    return strtoul(str, end, base);
}

long long strToLLong(const char_t* str, char_t** end, int base)
{
    return strtoll(str, end, base);
}

unsigned long long strToULLong(const char_t* str, char_t** end, int base)
{
    return strtoull(str, end, base);
}

float strToFloat(const char_t* str, char_t** end)
{
    return strtof(str, end);
}

double strToDouble(const char_t* str, char_t** end)
{
    return strtod(str, end);
}

bool charIsSpace(unichar_t ch)
{
    return isspace(ch);
}

bool charIsPrint(unichar_t ch)
{
    return isprint(ch);
}

bool charIsAlphaNum(unichar_t ch)
{
    return isalnum(ch);
}

unichar_t charToLower(unichar_t ch)
{
    return tolower(ch);
}

unichar_t charToUpper(unichar_t ch)
{
    return toupper(ch);
}

void print(const char_t* str)
{
    puts(str);
}

void printFormatted(const char_t* format, ...)
{
    va_list args;
    va_start(args, format);
    printf(format, args);
    va_end(args);
}

void printString(char_t* str, const char_t* format, ...)
{
    va_list args;
    va_start(args, format);
    sprintf(str, format, args);
    va_end(args);
}


void printArgs(const char_t* format, va_list args)
{
    vprintf(format, args);
}

void printArgsAlloc(char_t** str, const char_t* format, va_list args)
{
    printArgsAlloc(str, format, args);
}

void printAlloc(char_t** str, const char_t* format, ...)
{
    va_list args;
    va_start(args, format);
    vasprintf(str, format, args);
    va_end(args);
}

void putChar(unichar_t ch)
{
    putchar(ch);
}

unichar_t getChar()
{
    return getchar();
}

#endif

// Unicode support

int utf8CharToUtf32(const char* in, char32_t& ch)
{
    uint8_t ch1 = *in++;

    if (ch1 < 0x80)
    {
        ch = ch1;
        return 1;
    }
    else if (ch1 < 0xe0)
    {
        uint8_t ch2 = *in++;
        ch = ((ch1 & 0x1f) << 6) | (ch2 & 0x3f);
        return 2;
    }
    else if (ch1 < 0xf0)
    {
        uint8_t ch2 = *in++;
        uint8_t ch3 = *in++;
        ch = ((ch1 & 0x0f) << 12) | 
            ((ch2 & 0x3f) << 6) | (ch3 & 0x3f);
        return 3;
    }
    else
    {
        uint8_t ch2 = *in++;
        uint8_t ch3 = *in++;
        uint8_t ch4 = *in++;
        ch = ((ch1 & 0x07) << 18) | 
            ((ch2 & 0x3f) << 12) | ((ch3 & 0x3f) << 6) | (ch4 & 0x3f);
        return 4;
    }
}

void utf8StringToUtf32(const char* in, char32_t* out)
{
    while (*in)
        in += utf8CharToUtf32(in, *out++);

    *out = 0;
}

int utf32CharToUtf8(char32_t ch, char* out)
{
    if (ch < 0x80)
    {
        *out++ = ch;
        return 1;
    }
    else if (ch < 0x800)
    {
        *out++ = 0xc0 | (ch >> 6);
        *out++ = 0x80 | (ch & 0x03f);
        return 2;
    }
    else if (ch < 0x10000)
    {
        *out++ = 0xe0 | (ch >> 12);
        *out++ = 0x80 | ((ch & 0x0fc0) >> 6);
        *out++ = 0x80 | (ch & 0x003f);
        return 3;
    }
    else
    {
        *out++ = 0xf0 | (ch >> 18);
        *out++ = 0x80 | ((ch & 0x03f000) >> 12);
        *out++ = 0x80 | ((ch & 0x000fc0) >> 6);
        *out++ = 0x80 | (ch & 0x00003f);
        return 4;
    }
}

void utf32StringToUtf8(const char32_t* in, char* out)
{
    while (*in)
        out += utf32CharToUtf8(*in++, out);

    *out = 0;
}

int utf16CharToUtf32(const char16_t* in, char32_t& ch)
{
    char16_t ch1 = *in++;

    if ((ch1 & 0xfc00) != 0xd800)
    {
        ch = ch1;
        return 1;
    }
    else
    {
        char16_t ch2 = *in++;
        ch = ((((ch1 & 0x03c0) >> 6) + 1) << 16) | 
            ((ch1 & 0x003f) << 10) | (ch2 & 0x03ff);
        return 2;
    }
}

void utf16StringToUtf32(const char16_t* in, char32_t* out)
{
    while (*in)
        in += utf16CharToUtf32(in, *out++);

    *out = 0;
}

int utf32CharToUtf16(char32_t ch, char16_t* out)
{
    if (ch < 0x010000)
    {
        *out++ = ch;
        return 1;
    }
    else
    {
        *out++ = 0xd800 | 
            ((((ch & 0x1f0000) >> 16) - 1) << 6) | ((ch & 0x00fc00) >> 10);
        *out++ = 0xdc00 | (ch & 0x03ff);
        return 2;
    }
}

void utf32StringToUtf16(const char32_t* in, char16_t* out)
{
    while (*in)
        out += utf32CharToUtf16(*in++, out);

    *out = 0;
}

void utf8StringToUtf16(const char* in, char16_t* out)
{
    char32_t ch;

    while (*in)
    {
        in += utf8CharToUtf32(in, ch);
        out += utf32CharToUtf16(ch, out);
    }

    *out = 0;
}

void utf16StringToUtf8(const char16_t* in, char* out)
{
    char32_t ch;

    while (*in)
    {
        in += utf16CharToUtf32(in, ch);
        out += utf32CharToUtf8(ch, out);
    }

    *out = 0;
}

char32_t utf8CharAt(const char* pos)
{
    char32_t ch;
    utf8CharToUtf32(pos, ch);
    return ch;
}

char* utf8CharForward(const char* pos, int n)
{
    while (*pos && n--)
    {
        do
        {
            ++pos;
        }
        while ((*pos & 0xc0) == 0x80);
    }

    return const_cast<char*>(pos);
}

char* utf8CharBack(const char* pos, const char* start, int n)
{
    while (pos > start && n--)
    {
        do
        {
            --pos;
        }
        while ((*pos & 0xc0) == 0x80);
    }

    return const_cast<char*>(pos);
}

int utf8CharLength(char32_t ch)
{
    if (ch < 0x80)
        return 1;
    else if (ch < 0x800)
        return 2;
    else if (ch < 0x10000)
        return 3;
    else
        return 4;
}

int utf8StringLength(const char* str)
{
    int len = 0;

    if (str)
    {
        while (*str)
        {
            str = utf8CharForward(str);
            ++len;
        }
    }

    return len;
}

char32_t utf16CharAt(const char16_t* pos)
{
    char32_t ch;
    utf16CharToUtf32(pos, ch);
    return ch;
}

char16_t* utf16CharForward(const char16_t* pos, int n)
{
    while (*pos && n--)
    {
        if ((*pos & 0xfc00) == 0xd800)
            pos += 2;
        else
            ++pos;
    }

    return const_cast<char16_t*>(pos);
}

char16_t* utf16CharBack(const char16_t* pos, const char16_t* start, int n)
{
    while (pos > start && n--)
    {
        --pos;
        if ((*pos & 0xfc00) == 0xdc00)
            --pos;
    }

    return const_cast<char16_t*>(pos);
}

int utf16CharLength(char32_t ch)
{
    if (ch < 0x010000)
        return 1;
    else
        return 2;
}

int utf16StringLength(const char16_t* str)
{
    int len = 0;

    if (str)
    {
        while (*str)
        {
            str = utf16CharForward(str);
            ++len;
        }
    }

    return len;
}

// String

String::String(const String& other)
{
    if (other._length > 0)
    {
        _length = other._length;
        _capacity = _length + 1;
        _chars = Memory::allocate<char_t>(_capacity);
        strCopyLen(_chars, other._chars, _length + 1);
    }
    else
    {
        _length = 0;
        _capacity = 0;
        _chars = nullptr;
    }
}

String::String(const char_t* pos, int len)
{
    if (pos)
    {
        const char_t* end = len < 0 ? 
            pos + strLen(pos) : UTF_CHAR_FORWARD(pos, len);

        if (end > pos)
        {
            _length = end - pos;
            _capacity = _length + 1;
            _chars = Memory::allocate<char_t>(_capacity);
            *strCopyLen(_chars, pos, _length) = 0;
            return;
        }
    }
    else
        ASSERT(len == 0);

    _length = 0;
    _capacity = 0;
    _chars = nullptr;
}

String::String(unichar_t ch, int len)
{
    ASSERT(ch != 0);
    ASSERT(len >= 0);

    if (len > 0)
    {
        _length = UTF_CHAR_LENGTH(ch) * len;
        _capacity = _length + 1;
        _chars = Memory::allocate<char_t>(_capacity);
        *strSet(_chars, ch, len) = 0;
    }
    else
    {
        _length = 0;
        _capacity = 0;
        _chars = nullptr;
    }
}

String::String(char_t* chars)
{
    if (chars)
    {
        _length = strLen(chars);
        _capacity = _length + 1;
        _chars = chars;
    }
    else
    {
        _length = 0;
        _capacity = 0;
        _chars = nullptr;
    }
}

String::String(String&& other)
{
    _length = other._length;
    _capacity = other._capacity;
    _chars = other._chars;

    other._length = 0;
    other._capacity = 0;
    other._chars = nullptr;
}

const char_t* String::find(const String& str, const char_t* pos) const
{
    if (pos)
        ASSERT(pos >= _chars && pos <= _chars + _length);
    else
        pos = _chars;

    if (str._chars)
        return strFind(pos, str._chars);
    else
        return nullptr;
}

const char_t* String::find(const char_t* chars, const char_t* pos) const
{
    if (pos)
        ASSERT(pos >= _chars && pos <= _chars + _length);
    else
        pos = _chars;

    if (chars)
        return strFind(pos, chars);
    else
        return nullptr;
}

const char_t* String::find(unichar_t ch, const char_t* pos) const
{
    if (pos)
        ASSERT(pos >= _chars && pos <= _chars + _length);
    else
        pos = _chars;

    char_t s[5];
    int n = UNICODE_CHAR_TO_UTF(ch, s);
    s[n] = 0;

    return find(s, pos);
}

const char_t* String::findNoCase(const String& str, const char_t* pos) const
{
    if (pos)
        ASSERT(pos >= _chars && pos <= _chars + _length);
    else
        pos = _chars;

    if (str._chars)
        return strFindNoCase(pos, str._chars);
    else
        return nullptr;
}

const char_t* String::findNoCase(const char_t* chars, const char_t* pos) const
{
    if (pos)
        ASSERT(pos >= _chars && pos <= _chars + _length);
    else
        pos = _chars;

    if (chars)
        return strFindNoCase(pos, chars);
    else
        return nullptr;
}

const char_t* String::findNoCase(unichar_t ch, const char_t* pos) const
{
    if (pos)
        ASSERT(pos >= _chars && pos <= _chars + _length);
    else
        pos = _chars;

    char_t s[5];
    int n = UNICODE_CHAR_TO_UTF(ch, s);
    s[n] = 0;

    return find(s, pos);
}

bool String::startsWith(const String& str) const
{
    if (str._length > 0 && _length > 0)
        return strCompareLen(_chars, str._chars, str._length) == 0;
    else
        return false;
}

bool String::startsWith(const char_t* chars) const
{
    if (chars)
    {
        int len = strLen(chars);
        if (len > 0 && _length > 0)
            return strCompareLen(_chars, chars, len) == 0;
    }

    return false;
}

bool String::endsWith(const String& str) const
{
    if (str._length > 0 && _length > 0 && str._length <= _length)
        return strCompare(_chars + _length - str._length, str._chars) == 0;
    else
        return false;
}

bool String::endsWith(const char_t* chars) const
{
    if (chars)
    {
        int len = strLen(chars);
        if (len > 0 && _length > 0 && len <= _length)
            return strCompare(_chars + _length - len, chars) == 0;
    }

    return false;
}

bool String::contains(const String& str) const
{
    return find(str) != nullptr;
}

bool String::contains(const char_t* chars) const
{
    return find(chars) != nullptr;
}

void String::ensureCapacity(int capacity)
{
    ASSERT(capacity >= 0);

    if (capacity > _capacity)
    {
        if (_chars)
            _chars = Memory::reallocate(_chars, capacity);
        else
        {
            _chars = Memory::allocate<char_t>(capacity);
            *_chars = 0;
        }

        _capacity = capacity;
    }
}

void String::shrinkToLength()
{
    if (_length > 0)
    {
        int capacity = _length + 1;

        if (_capacity > capacity)
        {
            _chars = Memory::reallocate(_chars, capacity);
            _capacity = capacity;
        }
    }
    else
        reset();
}

void String::assign(const String& other)
{
    if (other._length > 0)
    {
        ASSERT(this != &other);

        int capacity = other._length + 1;

        if (capacity > _capacity)
        {
            String tmp(other);
            swap(*this, tmp);
        }
        else
        {
            _length = other._length;
            strCopyLen(_chars, other._chars, _length + 1);
        }
    }
    else
        clear();
}

void String::assign(const char_t* chars)
{
    if (chars && *chars)
    {
        ASSERT(_chars != chars);

        int len = strLen(chars);
        int capacity = len + 1;

        if (capacity > _capacity)
        {
            String tmp(chars);
            swap(*this, tmp);
        }
        else
        {
            _length = len;
            strCopyLen(_chars, chars, _length + 1);
        }
    }
    else
        clear();
}

void String::assign(unichar_t ch, int len)
{
    ASSERT(ch != 0);
    ASSERT(len >= 0);

    if (len > 0)
    {
        int l = UTF_CHAR_LENGTH(ch) * len;
        int capacity = l + 1;

        if (capacity > _capacity)
        {
            String tmp(ch, len);
            swap(*this, tmp);
        }
        else
        {
            *strSet(_chars, ch, len) = 0;
            _length = l;
        }
    }
    else
        clear();
}

void String::append(const String& str)
{
    if (str._length > 0)
    {
        ASSERT(this != &str);

        int capacity = _length + str._length + 1;
        if (capacity > _capacity)
            ensureCapacity(capacity * 2);

        *strCopyLen(_chars + _length, str._chars, str._length) = 0;
        _length += str._length;
    }
}

void String::append(const char_t* chars)
{
    if (chars && *chars)
    {
        ASSERT(_chars != chars);

        int len = strLen(chars), capacity = _length + len + 1;
        if (capacity > _capacity)
            ensureCapacity(capacity * 2);

        *strCopyLen(_chars + _length, chars, len) = 0;
        _length += len;
    }
}

void String::append(unichar_t ch, int len)
{
    ASSERT(ch != 0);
    ASSERT(len >= 0);

    if (len > 0)
    {
        int l = UTF_CHAR_LENGTH(ch) * len;
        int capacity = _length + l + 1;

        if (capacity > _capacity)
            ensureCapacity(capacity * 2);

        *strSet(_chars + _length, ch, len) = 0;
        _length += l;
    }
}

void String::appendFormat(const char_t* format, ...)
{
    va_list args;

    va_start(args, format);
    *this += String::format(format, args);
    va_end(args);
}

void String::appendFormat(const char_t* format, va_list args)
{
    *this += String::format(format, args);
}

void String::insert(char_t* pos, const String& str)
{
    ASSERT(pos >= _chars && pos <= _chars + _length);

    if (str._length > 0)
    {
        ASSERT(this != &str);

        int capacity = _length + str._length + 1;
        if (capacity > _capacity)
            ensureCapacity(capacity * 2);

        strMove(pos + str._length, pos, _chars + _length - pos + 1);
        strCopyLen(pos, str._chars, str._length);
        _length += str._length;
    }
}

void String::insert(char_t* pos, const char_t* chars)
{
    ASSERT(pos >= _chars && pos <= _chars + _length);

    if (chars && *chars)
    {
        ASSERT(_chars != chars);

        int len = strLen(chars), capacity = _length + len + 1;
        if (capacity > _capacity)
            ensureCapacity(capacity * 2);

        strMove(pos + len, pos, _chars + _length - pos + 1);
        strCopyLen(pos, chars, len);
        _length += len;
    }
}

void String::insert(char_t* pos, unichar_t ch, int len)
{
    ASSERT(pos >= _chars && pos <= _chars + _length);
    ASSERT(ch != 0);
    ASSERT(len >= 0);

    if (len > 0)
    {
        int l = UTF_CHAR_LENGTH(ch) * len;
        int capacity = _length + l + 1;

        if (capacity > _capacity)
            ensureCapacity(capacity * 2);

        strMove(pos + l, pos, _chars + _length - pos + 1);
        strSet(pos, ch, len);
        _length += l;
    }
}

void String::erase(char_t* pos, int len)
{
    ASSERT(pos >= _chars && pos <= _chars + _length);

    if (_length > 0)
    {
        if (len < 0)
        {
            *pos = 0;
            _length = pos - _chars;
        }
        else if (len > 0)
        {
            char_t* end = UTF_CHAR_FORWARD(pos, len);
            strMove(pos, end, _chars + _length - end + 1);
            _length -= end - pos;
        }
    }
}

void String::erase(const String& str)
{
    if (_length > 0)
    {
        if (str._length > 0)
        {
            ASSERT(this != &str);

            char_t* from = _chars;
            char_t* found;
            int foundCnt = 0;

            while ((found = strFind(from, str._chars)) != nullptr)
            {
                ++foundCnt;
                from = found + str._length;
            }

            if (foundCnt > 0)
            {
                int newLen = _length - foundCnt * str._length;
                if (newLen > 0)
                {
                    from = _chars;
                    while ((found = strFind(from, str._chars)) != nullptr)
                    {
                        strMove(found, found + str._length, 
                            _chars + _length - found - str._length + 1);

                        from = found;
                        _length -= str._length;
                    }
                }
                else
                    clear();
            }
        }
    }
}

void String::erase(const char_t* chars)
{
    if (_length > 0)
    {
        if (chars && *chars)
        {
            ASSERT(_chars != chars);

            char_t* from = _chars;
            char_t* found;
            int foundCnt = 0, len = strLen(chars);

            while ((found = strFind(from, chars)) != nullptr)
            {
                ++foundCnt;
                from = found + len;
            }

            if (foundCnt > 0)
            {
                int newLen = _length - foundCnt * len;
                if (newLen > 0)
                {
                    from = _chars;
                    while ((found = strFind(from, chars)) != nullptr)
                    {
                        strMove(found, found + len,
                            _chars + _length - found - len + 1);

                        from = found;
                        _length -= len;
                    }
                }
                else
                    clear();
            }
        }
    }
}

void String::replace(char_t* pos, const String& str, int len)
{
    ASSERT(pos >= _chars && pos <= _chars + _length);

    if (_length > 0)
    {
        if (str._length > 0)
        {
            ASSERT(this != &str);

            char_t* end = len < 0 ? _chars + _length :
                UTF_CHAR_FORWARD(pos, len);
            int newLen = _length - (end - pos) + str._length;

            if (newLen > 0)
            {
                ensureCapacity(newLen + 1);

                strMove(pos + str._length, end, _chars + _length - end + 1);
                strCopyLen(pos, str._chars, str._length);
                _length = newLen;
            }
            else
                *this = str;
        }
        else
            erase(pos, len);
    }
}

void String::replace(char_t* pos, const char_t* chars, int len)
{
    ASSERT(pos >= _chars && pos <= _chars + _length);

    if (_length > 0)
    {
        if (chars && *chars)
        {
            ASSERT(_chars != chars);

            char_t* end = len < 0 ? _chars + _length :
                UTF_CHAR_FORWARD(pos, len);
            int replaceLen = strLen(chars);
            int newLen = _length - (end - pos) + replaceLen;

            if (newLen > 0)
            {
                ensureCapacity(newLen + 1);

                strMove(pos + replaceLen, end, _chars + _length - end + 1);
                strCopyLen(pos, chars, replaceLen);
                _length = newLen;
            }
            else
                *this = chars;
        }
        else
            erase(pos, len);
    }
}

void String::replace(const String& searchStr, const String& replaceStr)
{
    if (_length > 0)
    {
        if (searchStr._length > 0)
        {
            ASSERT(this != &searchStr);

            if (replaceStr._length > 0)
            {
                ASSERT(this != &replaceStr);

                char_t* from = _chars;
                char_t* found;
                int foundCnt = 0;

                while ((found = strFind(from, searchStr._chars)) != nullptr)
                {
                    ++foundCnt;
                    from = found + searchStr._length;
                }

                if (foundCnt > 0)
                {
                    int capacity = _length + foundCnt * (replaceStr._length - searchStr._length) + 1;
                    ensureCapacity(capacity);

                    from = _chars;
                    while ((found = strFind(from, searchStr._chars)) != nullptr)
                    {
                        strMove(found + replaceStr._length, found + searchStr._length,
                            _chars + _length - found - searchStr._length + 1);
                        strCopyLen(found, replaceStr._chars, replaceStr._length);

                        from = found + replaceStr._length;
                        _length += replaceStr._length - searchStr._length;
                    }
                }
            }
            else
                erase(searchStr);
        }
    }
}

void String::replace(const char_t* searchChars, const char_t* replaceChars)
{
    if (_length > 0)
    {
        if (searchChars && *searchChars)
        {
            ASSERT(_chars != searchChars);

            if (replaceChars && *replaceChars)
            {
                ASSERT(_chars != replaceChars);

                char_t* from = _chars;
                char_t* found;
                int foundCnt = 0;
                int searchLen = strLen(searchChars);

                while ((found = strFind(from, searchChars)) != nullptr)
                {
                    ++foundCnt;
                    from = found + searchLen;
                }

                if (foundCnt > 0)
                {
                    int replaceLen = strLen(replaceChars);

                    int capacity = _length + foundCnt * (replaceLen - searchLen) + 1;
                    ensureCapacity(capacity);

                    from = _chars;
                    while ((found = strFind(from, searchChars)) != nullptr)
                    {
                        strMove(found + replaceLen, found + searchLen,
                            _chars + _length - found - searchLen + 1);
                        strCopyLen(found, replaceChars, replaceLen);

                        from = found + replaceLen;
                        _length += replaceLen - searchLen;
                    }
                }
            }
            else
                erase(searchChars);
        }
    }
}

void String::trim()
{
    char_t* p = _chars;

    if (p)
    {
        char_t* q = _chars + _length;
        unichar_t ch;

        while (p < q)
        {
            int l = UTF_CHAR_TO_UNICODE(p, ch);
            if (charIsSpace(ch))
                p += l;
            else
                break;
        }

        while (p < q)
        {
            int l = UTF_CHAR_TO_UNICODE(UTF_CHAR_BACK(q, p), ch);
            if (charIsSpace(ch))
                q -= l;
            else
                break;
        }

        if (p < q)
        {
            _length = q - p;
            strMove(_chars, p, _length);
            _chars[_length] = 0;
        }
        else
            clear();
    }
}

void String::trimRight()
{
    char_t* p = _chars;

    if (p)
    {
        char_t* q = _chars + _length;
        unichar_t ch;
        
        while (p < q)
        {
            int l = UTF_CHAR_TO_UNICODE(UTF_CHAR_BACK(q, p), ch);
            if (charIsSpace(ch))
                q -= l;
            else
                break;
        }

        if (p < q)
        {
            _length = q - p;
            _chars[_length] = 0;
        }
        else
            clear();
    }
}

void String::trimLeft()
{
    char_t* p = _chars;

    if (p)
    {
        char_t* q = _chars + _length;
        unichar_t ch;

        while (p < q)
        {
            int l = UTF_CHAR_TO_UNICODE(p, ch);
            if (charIsSpace(ch))
                p += l;
            else
                break;
        }

        if (p < q)
        {
            _length = q - p;
            strMove(_chars, p, _length);
            _chars[_length] = 0;
        }
        else
            clear();
    }
}

void String::toUpper()
{
    char_t* p = _chars;

    if (p)
    {
        while (*p)
        {
            unichar_t ch;
            int l = UTF_CHAR_TO_UNICODE(p, ch);
            ch = charToUpper(ch);
            UNICODE_CHAR_TO_UTF(ch, p);
            p += l;
        }
    }
}

void String::toLower()
{
    char_t* p = _chars;

    if (p)
    {
        while (*p)
        {
            unichar_t ch;
            int l = UTF_CHAR_TO_UNICODE(p, ch);
            ch = charToLower(ch);
            UNICODE_CHAR_TO_UTF(ch, p);
            p += l;
        }
    }
}

void String::clear()
{
    _length = 0;
    if (_chars)
        *_chars = 0;
}

void String::reset()
{
    Memory::deallocate(_chars);

    _length = 0;
    _capacity = 0;
    _chars = nullptr;
}

char_t* String::release()
{
    char_t* chars = _chars;
    _length = 0;
    _capacity = 0;
    _chars = nullptr;
    
    return chars;
}

// conversion from string

bool String::toBool() const
{
    if (compareNoCase(STR("true")) == 0)
        return true;
    else if (compareNoCase(STR("false")) == 0)
        return false;
    else
        throw Exception(STR("invalid boolean value"));
}

int String::toInt() const
{
    return toInt32();
}

int32_t String::toInt32() const
{
    const char_t* s = str();
    char_t* e;
    
    int32_t val = strToLong(s, &e, 10);
    if (s < e && *e == '\0')
        return val;
    else
        throw Exception(STR("invalid number"));
}

uint32_t String::toUInt32() const
{
    const char_t* s = str();
    char_t* e;
    
    uint32_t val = strToULong(s, &e, 10);
    if (s < e && *e == '\0')
        return val;
    else
        throw Exception(STR("invalid number"));
}

int64_t String::toInt64() const
{
    const char_t* s = str();
    char_t* e;
    
    int64_t val = strToLLong(s, &e, 10);
    if (s < e && *e == '\0')
        return val;
    else
        throw Exception(STR("invalid number"));
}

uint64_t String::toUInt64() const
{
    const char_t* s = str();
    char_t* e;
    
    uint64_t val = strToULLong(s, &e, 10);
    if (s < e && *e == '\0')
        return val;
    else
        throw Exception(STR("invalid number"));
}

float String::toFloat() const
{
    const char_t* s = str();
    char_t* e;
    
    float val = strToFloat(s, &e);
    if (s < e && *e == '\0')
        return val;
    else
        throw Exception(STR("invalid number"));
}

double String::toDouble() const
{
    const char_t* s = str();
    char_t* e;
    
    double val = strToDouble(s, &e);
    if (s < e && *e == '\0')
        return val;
    else
        throw Exception(STR("invalid number"));
}

// conversion to string

String String::from(bool value)
{
    return String(value ? STR("true") : STR("false"));
}

String String::from(int value)
{
    char_t* chars;
    printAlloc(&chars, STR("%d"), value);
    return String(chars);
}

String String::from(unsigned value)
{
    char_t* chars;
    printAlloc(&chars, STR("%u"), value);
    return String(chars);
}

String String::from(long value)
{
    char_t* chars;
    printAlloc(&chars, STR("%ld"), value);
    return String(chars);
}

String String::from(unsigned long value)
{
    char_t* chars;
    printAlloc(&chars, STR("%lu"), value);
    return String(chars);
}

String String::from(long long value)
{
    char_t* chars;
    printAlloc(&chars, STR("%lld"), value);
    return String(chars);
}

String String::from(unsigned long long value)
{
    char_t* chars;
    printAlloc(&chars, STR("%llu"), value);
    return String(chars);
}

String String::from(float value, int precision)
{
    char_t* chars;
    printAlloc(&chars, STR("%.*f"), precision, static_cast<double>(value));
    return String(chars);
}

String String::from(double value, int precision)
{
    char_t* chars;
    printAlloc(&chars, STR("%.*f"), precision, value);
    return String(chars);
}

// formatting

String String::format(const char_t* format, ...)
{
    ASSERT(format);

    char_t* chars;
    va_list args;

    va_start(args, format);
    printArgsAlloc(&chars, format, args);
    va_end(args);

    return String(chars);
}

String String::format(const char_t* format, va_list args)
{
    ASSERT(format);

    char_t* chars;
    printArgsAlloc(&chars, format, args);
    return String(chars);
}

// ConstStringIterator

unichar_t ConstStringIterator::value() const
{
    unichar_t ch;
    UTF_CHAR_TO_UNICODE(_pos, ch);
    return ch;
}

bool ConstStringIterator::moveNext()
{
    _pos = _pos ? UTF_CHAR_FORWARD(_pos) : _str.str();

    if (*_pos)
        return true;
    else
    {
        _pos = nullptr;
        return false;
    }
}

bool ConstStringIterator::movePrev()
{
    if (!_pos)
        _pos = _str.str() + _str.length();

    if (_pos > _str.str())
    {
        _pos = UTF_CHAR_BACK(_pos, _str.str());
        return true;
    }
    else
    {
        _pos = nullptr;
        return false;
    }
}

