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

#ifdef CHAR_ENCODING_UTF8

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

char_t* strFind(char_t* str, const char_t* searchStr)
{
    return strstr(str, searchStr);
}

const char_t* strFind(const char_t* str, const char_t* searchStr)
{
    return strstr(str, searchStr);
}

char_t* strFindNoCase(char_t* str, const char_t* searchStr)
{
#ifdef PLATFORM_AIX
    int len = strlen(searchStr);
	for (; *str; ++str)
        if (!strncasecmp(str, searchStr, len))
            return str;

    return NULL;
#else
    return strcasestr(str, searchStr);
#endif
}

const char_t* strFindNoCase(const char_t* str, const char_t* searchStr)
{
#ifdef PLATFORM_AIX
    int len = strlen(searchStr);
	for (; *str; ++str)
        if (!strncasecmp(str, searchStr, len))
            return str;

    return NULL;
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

void formatString(char_t* str, const char_t* format, ...)
{
    va_list args;
    va_start(args, format);
    vsprintf(str, format, args);
    va_end(args);
}

void formatAllocStringArgs(char_t** str, const char_t* format, va_list args)
{
#ifdef PLATFORM_AIX
    va_list args2;
	va_copy(args2, args);
	int len = vsnprintf(0, 0, format, args2);
	va_end(args2);

    if (len > 0)
    {
        *str = Memory::allocate<char_t>(len + 1);
        vsnprintf(*str, len + 1, format, args);
    }
    else
        *str = 0;
#else
    vasprintf(str, format, args);
#endif
}

void formatAllocString(char_t** str, const char_t* format, ...)
{
    va_list args;
    va_start(args, format);
    formatAllocStringArgs(str, format, args);
    va_end(args);
}

#else

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

char_t* strFind(char_t* str, const char_t* searchStr)
{
    return reinterpret_cast<char_t*>(
        wcsstr(reinterpret_cast<wchar_t*>(str),
            reinterpret_cast<const wchar_t*>(searchStr)));
}

const char_t* strFind(const char_t* str, const char_t* searchStr)
{
    return reinterpret_cast<const char_t*>(
        wcsstr(reinterpret_cast<const wchar_t*>(str),
            reinterpret_cast<const wchar_t*>(searchStr)));
}

char_t* strFindNoCase(char_t* str, const char_t* searchStr)
{
    int index = FindNLSStringEx(LOCALE_NAME_USER_DEFAULT,
        FIND_FROMSTART | LINGUISTIC_IGNORECASE,
        reinterpret_cast<const wchar_t*>(str), -1,
        reinterpret_cast<const wchar_t*>(searchStr),
        -1, NULL, NULL, NULL, 0);

    return index >= 0 ? str + index : NULL;
}

const char_t* strFindNoCase(const char_t* str, const char_t* searchStr)
{
    int index = FindNLSStringEx(LOCALE_NAME_USER_DEFAULT,
        FIND_FROMSTART | LINGUISTIC_IGNORECASE,
        reinterpret_cast<const wchar_t*>(str), -1,
        reinterpret_cast<const wchar_t*>(searchStr),
        -1, NULL, NULL, NULL, 0);

    return index >= 0 ? str + index : NULL;
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

void formatString(char_t* str, const char_t* format, ...)
{
    va_list args;
    va_start(args, format);
    vswprintf(reinterpret_cast<wchar_t*>(str),
        reinterpret_cast<const wchar_t*>(format), args);
    va_end(args);
}

void formatAllocStringArgs(char_t** str, const char_t* format, va_list args)
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

void formatAllocString(char_t** str, const char_t* format, ...)
{
    va_list args;
    va_start(args, format);
    formatAllocStringArgs(str, format, args);
    va_end(args);
}

#endif

bool charIsSpace(unichar_t ch)
{
    return iswspace(ch);
}

bool charIsPrint(unichar_t ch)
{
    return iswprint(ch);
}

bool charIsAlphaNum(unichar_t ch)
{
    return iswalnum(ch);
}

bool charIsUpper(unichar_t ch)
{
    return iswupper(ch);
}

bool charIsLower(unichar_t ch)
{
    return iswlower(ch);
}

unichar_t charToLower(unichar_t ch)
{
    return towlower(ch);
}

unichar_t charToUpper(unichar_t ch)
{
    return towupper(ch);
}

// Unicode support

int utf8CharToUnicode(const char* in, char32_t& ch)
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

int unicodeCharToUtf8(char32_t ch, char* out)
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

int utf16CharToUnicode(const char16_t* in, char32_t& ch)
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

int unicodeCharToUtf16(char32_t ch, char16_t* out)
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

void utf8StringToUnicode(const char* in, char32_t* out)
{
    while (*in)
        in += utf8CharToUnicode(in, *out++);

    *out = 0;
}

void unicodeStringToUtf8(const char32_t* in, char* out)
{
    while (*in)
        out += unicodeCharToUtf8(*in++, out);

    *out = 0;
}

void utf16StringToUnicode(const char16_t* in, char32_t* out)
{
    while (*in)
        in += utf16CharToUnicode(in, *out++);

    *out = 0;
}

void unicodeStringToUtf16(const char32_t* in, char16_t* out)
{
    while (*in)
        out += unicodeCharToUtf16(*in++, out);

    *out = 0;
}

void utf8StringToUtf16(const char* in, char16_t* out)
{
    char32_t ch;

    while (*in)
    {
        in += utf8CharToUnicode(in, ch);
        out += unicodeCharToUtf16(ch, out);
    }

    *out = 0;
}

void utf16StringToUtf8(const char16_t* in, char* out)
{
    char32_t ch;

    while (*in)
    {
        in += utf16CharToUnicode(in, ch);
        out += unicodeCharToUtf8(ch, out);
    }

    *out = 0;
}

char32_t utf8CharAt(const char* pos)
{
    char32_t ch;
    utf8CharToUnicode(pos, ch);
    return ch;
}

const char* utf8CharForward(const char* pos)
{
    do
    {
        ++pos;
    }
    while ((*pos & 0xc0) == 0x80);

    return pos;
}

const char* utf8CharBack(const char* pos)
{
    do
    {
        --pos;
    }
    while ((*pos & 0xc0) == 0x80);

    return pos;
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

    while (*str)
    {
        str = utf8CharForward(str);
        ++len;
    }

    return len;
}

char32_t utf16CharAt(const char16_t* pos)
{
    char32_t ch;
    utf16CharToUnicode(pos, ch);
    return ch;
}

const char16_t* utf16CharForward(const char16_t* pos)
{
    if ((*pos & 0xfc00) == 0xd800)
        pos += 2;
    else
        ++pos;

    return pos;
}

const char16_t* utf16CharBack(const char16_t* pos)
{
    --pos;
    if ((*pos & 0xfc00) == 0xdc00)
        --pos;

    return pos;
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

    while (*str)
    {
        str = utf16CharForward(str);
        ++len;
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
        _chars = NULL;
    }
}

String::String(const char_t* chars, int len)
{
    if (chars)
    {
        _length = len < 0 ? strLen(chars) : len;

        if (_length > 0)
        {
            _capacity = _length + 1;
            _chars = Memory::allocate<char_t>(_capacity);
            *strCopyLen(_chars, chars, _length) = 0;
        }
        else
        {
            _capacity = 0;
            _chars = NULL;
        }
    }
    else
    {
        ASSERT(len <= 0);
        _length = 0;
        _capacity = 0;
        _chars = NULL;
    }
}

String::String(unichar_t ch, int n)
{
    ASSERT(ch != 0);
    ASSERT(n >= 0);

    if (n > 0)
    {
        _length = UTF_CHAR_LENGTH(ch) * n;
        _capacity = _length + 1;
        _chars = Memory::allocate<char_t>(_capacity);
        *strSet(_chars, ch, n) = 0;
    }
    else
    {
        _length = 0;
        _capacity = 0;
        _chars = NULL;
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
        _chars = NULL;
    }
}

String::String(String&& other)
{
    _length = other._length;
    _capacity = other._capacity;
    _chars = other._chars;

    other._length = 0;
    other._capacity = 0;
    other._chars = NULL;
}

unichar_t String::charAt(int pos) const
{
    ASSERT(pos >= 0 && pos <= _length);
    return _chars ? UTF_CHAR_AT(_chars + pos) : 0;
}

int String::charForward(int pos, int n) const
{
    ASSERT(pos >= 0 && pos <= _length);
    ASSERT(n >= 0);

    if (_chars)
    {
        const char_t* p = _chars + pos;

        while (*p && n > 0)
        {
            p = UTF_CHAR_FORWARD(p);
            --n;
        }

        return p - _chars;
    }

    return 0;
}

int String::charBack(int pos, int n) const
{
    ASSERT(pos >= 0 && pos <= _length);
    ASSERT(n >= 0);

    if (_chars)
    {
        const char_t* p = _chars + pos;

        while (p > _chars && n > 0)
        {
            p = UTF_CHAR_BACK(p);
            --n;
        }

        return p - _chars;
    }

    return 0;
}

String String::substr(int pos, int len) const
{
    ASSERT(pos >= 0 && pos <= _length);

    if (_length > 0)
    {
        if (len >= 0)
            ASSERT(pos + len >= 0 && pos + len <= _length);

        return String(_chars + pos, len);
    }
    else
    {
        ASSERT(len <= 0);
        return String();
    }
}

int String::find(const String& str, int pos) const
{
    ASSERT(pos >= 0 && pos <= _length);

    if (_length > 0 && str._length > 0)
    {
        const char_t* p = strFind(_chars + pos, str._chars);
        if (p)
            return p - _chars;
    }

    return INVALID_POSITION;
}

int String::find(const char_t* chars, int pos) const
{
    ASSERT(pos >= 0 && pos <= _length);

    if (_length > 0 && chars && *chars)
    {
        const char_t* p = strFind(_chars + pos, chars);
        if (p)
            return p - _chars;
    }

    return INVALID_POSITION;
}

int String::find(unichar_t ch, int pos) const
{
    ASSERT(ch != 0);
    ASSERT(pos >= 0 && pos <= _length);

    if (_length > 0)
    {
        while (pos < _length)
        {
            if (charAt(pos) == ch)
                return pos;
            pos = charForward(pos);
        }
    }

    return INVALID_POSITION;
}

int String::findNoCase(const String& str, int pos) const
{
    ASSERT(pos >= 0 && pos <= _length);

    if (_length > 0 && str._length > 0)
    {
        const char_t* p = strFindNoCase(_chars + pos, str._chars);
        if (p)
            return p - _chars;
    }

    return INVALID_POSITION;
}

int String::findNoCase(const char_t* chars, int pos) const
{
    ASSERT(pos >= 0 && pos <= _length);

    if (_length > 0 && chars && *chars)
    {
        const char_t* p = strFindNoCase(_chars + pos, chars);
        if (p)
            return p - _chars;
    }

    return INVALID_POSITION;
}

int String::findNoCase(unichar_t ch, int pos) const
{
    ASSERT(ch != 0);
    ASSERT(pos >= 0 && pos <= _length);

    if (_length > 0)
    {
        ch = charToLower(ch);

        while (pos < _length)
        {
            if (charToLower(charAt(pos)) == ch)
                return pos;
            pos = charForward(pos);
        }
    }

    return INVALID_POSITION;
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
    return find(str) >= 0;
}

bool String::contains(const char_t* chars) const
{
    return find(chars) >= 0;
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

        ensureCapacity(other._length + 1);

        _length = other._length;
        strCopyLen(_chars, other._chars, _length + 1);
    }
    else
        clear();
}

void String::assign(const char_t* chars, int len)
{
    if (chars && *chars)
    {
        ASSERT(_chars != chars);

        if (len < 0)
            len = strLen(chars);

        ensureCapacity(len + 1);

        _length = len;
        *strCopyLen(_chars, chars, _length) = 0;
    }
    else
    {
        ASSERT(len <= 0);
        clear();
    }
}

void String::assign(unichar_t ch, int n)
{
    ASSERT(ch != 0);
    ASSERT(n >= 0);

    if (n > 0)
    {
        int l = UTF_CHAR_LENGTH(ch) * n;

        ensureCapacity(l + 1);

        _length = l;
        *strSet(_chars, ch, n) = 0;
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

void String::append(const char_t* chars, int len)
{
    if (chars && *chars)
    {
        ASSERT(_chars != chars);

        if (len < 0)
            len = strLen(chars);

        int capacity = _length + len + 1;
        if (capacity > _capacity)
            ensureCapacity(capacity * 2);

        *strCopyLen(_chars + _length, chars, len) = 0;
        _length += len;
    }
    else
        ASSERT(len <= 0);
}

void String::append(unichar_t ch, int n)
{
    ASSERT(ch != 0);
    ASSERT(n >= 0);

    if (n > 0)
    {
        int l = UTF_CHAR_LENGTH(ch) * n;

        int capacity = _length + l + 1;
        if (capacity > _capacity)
            ensureCapacity(capacity * 2);

        *strSet(_chars + _length, ch, n) = 0;
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

void String::insert(int pos, const String& str)
{
    ASSERT(pos >= 0 && pos <= _length);

    if (_length > 0)
    {
        if (str._length > 0)
        {
            ASSERT(this != &str);

            int capacity = _length + str._length + 1;
            if (capacity > _capacity)
                ensureCapacity(capacity * 2);

            strMove(_chars + pos + str._length, _chars + pos, _length - pos + 1);
            strCopyLen(_chars + pos, str._chars, str._length);
            _length += str._length;
        }
    }
    else
        assign(str);
}

void String::insert(int pos, const char_t* chars, int len)
{
    ASSERT(pos >= 0 && pos <= _length);

    if (_length > 0)
    {
        if (chars && *chars)
        {
            ASSERT(_chars != chars);

            if (len < 0)
                len = strLen(chars);

            int capacity = _length + len + 1;
            if (capacity > _capacity)
                ensureCapacity(capacity * 2);

            strMove(_chars + pos + len, _chars + pos, _length - pos + 1);
            strCopyLen(_chars + pos, chars, len);
            _length += len;
        }
        else
            ASSERT(len <= 0);
    }
    else
        assign(chars, len);
}

void String::insert(int pos, unichar_t ch, int n)
{
    ASSERT(pos >= 0 && pos <= _length);

    if (_length > 0)
    {
        ASSERT(ch != 0);
        ASSERT(n >= 0);

        if (n > 0)
        {
            int l = UTF_CHAR_LENGTH(ch) * n;

            int capacity = _length + l + 1;
            if (capacity > _capacity)
                ensureCapacity(capacity * 2);

            strMove(_chars + pos + l, _chars + pos, _length - pos + 1);
            strSet(_chars + pos, ch, n);
            _length += l;
        }
    }
    else
        assign(ch, n);
}

void String::erase(int pos, int len)
{
    ASSERT(pos >= 0 && pos <= _length);

    if (_length > 0)
    {
        if (len < 0)
        {
            *(_chars + pos) = 0;
            _length = pos;
        }
        else if (len > 0)
        {
            ASSERT(pos + len >= 0 && pos + len <= _length);
            strMove(_chars + pos, _chars + pos + len, _length - pos - len + 1);
            _length -= len;
        }
    }
    else
        ASSERT(len <= 0);
}

void String::eraseString(const String& str)
{
    if (_length > 0)
    {
        if (str._length > 0)
        {
            ASSERT(this != &str);

            char_t* from = _chars;
            char_t* found;
            int foundCnt = 0;

            while ((found = strFind(from, str._chars)) != NULL)
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
                    while ((found = strFind(from, str._chars)) != NULL)
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

void String::eraseString(const char_t* chars)
{
    if (_length > 0)
    {
        if (chars && *chars)
        {
            ASSERT(_chars != chars);

            char_t* from = _chars;
            char_t* found;
            int foundCnt = 0, len = strLen(chars);

            while ((found = strFind(from, chars)) != NULL)
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
                    while ((found = strFind(from, chars)) != NULL)
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

void String::replace(int pos, const String& str, int len)
{
    ASSERT(pos >= 0 && pos <= _length);

    if (_length > 0)
    {
        if (str._length > 0)
        {
            ASSERT(this != &str);

            if (len < 0)
                len = _length - pos;
            else
                ASSERT(pos + len >= 0 && pos + len <= _length);

            int newLen = _length - len + str._length;

            if (newLen > 0)
            {
                ensureCapacity(newLen + 1);

                strMove(_chars + pos + str._length, _chars + pos + len, _length - pos - len + 1);
                strCopyLen(_chars + pos, str._chars, str._length);
                _length = newLen;
            }
            else
                assign(str);
        }
        else
            erase(pos, len);
    }
    else
    {
        ASSERT(len <= 0);
        assign(str);
    }
}

void String::replace(int pos, const char_t* chars, int len)
{
    ASSERT(pos >= 0 && pos <= _length);

    if (_length > 0)
    {
        if (chars && *chars)
        {
            ASSERT(_chars != chars);

            if (len < 0)
                len = _length - pos;
            else
                ASSERT(pos + len >= 0 && pos + len <= _length);

            int replaceLen = strLen(chars);
            int newLen = _length - len + replaceLen;

            if (newLen > 0)
            {
                ensureCapacity(newLen + 1);

                strMove(_chars + pos + replaceLen, _chars + pos + len, _length - pos - len + 1);
                strCopyLen(_chars + pos, chars, replaceLen);
                _length = newLen;
            }
            else
                assign(chars);
        }
        else
            erase(pos, len);
    }
    else
    {
        ASSERT(len <= 0);
        assign(chars);
    }
}

void String::replaceString(const String& searchStr, const String& replaceStr)
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

                while ((found = strFind(from, searchStr._chars)) != NULL)
                {
                    ++foundCnt;
                    from = found + searchStr._length;
                }

                if (foundCnt > 0)
                {
                    int capacity = _length + foundCnt * (replaceStr._length - searchStr._length) + 1;
                    ensureCapacity(capacity);

                    from = _chars;
                    while ((found = strFind(from, searchStr._chars)) != NULL)
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
                eraseString(searchStr);
        }
    }
}

void String::replaceString(const char_t* searchChars, const char_t* replaceChars)
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

                while ((found = strFind(from, searchChars)) != NULL)
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
                    while ((found = strFind(from, searchChars)) != NULL)
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
                eraseString(searchChars);
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
            int l = UTF_CHAR_TO_UNICODE(UTF_CHAR_BACK(q), ch);
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
            int l = UTF_CHAR_TO_UNICODE(UTF_CHAR_BACK(q), ch);
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
        String tmp;
        tmp.ensureCapacity(_length + 1);

        while (*p)
        {
            unichar_t ch;
            p += UTF_CHAR_TO_UNICODE(p, ch);
            tmp += charToUpper(ch);
        }

        swap(*this, tmp);
    }
}

void String::toLower()
{
    char_t* p = _chars;

    if (p)
    {
        String tmp;
        tmp.ensureCapacity(_length + 1);

        while (*p)
        {
            unichar_t ch;
            p += UTF_CHAR_TO_UNICODE(p, ch);
            tmp += charToLower(ch);
        }

        swap(*this, tmp);
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
    _chars = NULL;
}

char_t* String::release()
{
    char_t* chars = _chars;
    _length = 0;
    _capacity = 0;
    _chars = NULL;

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
    const char_t* s = chars();
    char_t* e;

    int32_t val = strToLong(s, &e, 10);
    if (s < e && *e == '\0')
        return val;
    else
        throw Exception(STR("invalid number"));
}

uint32_t String::toUInt32() const
{
    const char_t* s = chars();
    char_t* e;

    uint32_t val = strToULong(s, &e, 10);
    if (s < e && *e == '\0')
        return val;
    else
        throw Exception(STR("invalid number"));
}

int64_t String::toInt64() const
{
    const char_t* s = chars();
    char_t* e;

    int64_t val = strToLLong(s, &e, 10);
    if (s < e && *e == '\0')
        return val;
    else
        throw Exception(STR("invalid number"));
}

uint64_t String::toUInt64() const
{
    const char_t* s = chars();
    char_t* e;

    uint64_t val = strToULLong(s, &e, 10);
    if (s < e && *e == '\0')
        return val;
    else
        throw Exception(STR("invalid number"));
}

float String::toFloat() const
{
    const char_t* s = chars();
    char_t* e;

    float val = strToFloat(s, &e);
    if (s < e && *e == '\0')
        return val;
    else
        throw Exception(STR("invalid number"));
}

double String::toDouble() const
{
    const char_t* s = chars();
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
    formatAllocString(&chars, STR("%d"), value);
    return String(chars);
}

String String::from(unsigned value)
{
    char_t* chars;
    formatAllocString(&chars, STR("%u"), value);
    return String(chars);
}

String String::from(long value)
{
    char_t* chars;
    formatAllocString(&chars, STR("%ld"), value);
    return String(chars);
}

String String::from(unsigned long value)
{
    char_t* chars;
    formatAllocString(&chars, STR("%lu"), value);
    return String(chars);
}

String String::from(long long value)
{
    char_t* chars;
    formatAllocString(&chars, STR("%lld"), value);
    return String(chars);
}

String String::from(unsigned long long value)
{
    char_t* chars;
    formatAllocString(&chars, STR("%llu"), value);
    return String(chars);
}

String String::from(float value, int precision)
{
    char_t* chars;
    formatAllocString(&chars, STR("%.*f"), precision, static_cast<double>(value));
    return String(chars);
}

String String::from(double value, int precision)
{
    char_t* chars;
    formatAllocString(&chars, STR("%.*f"), precision, value);
    return String(chars);
}

// formatting

String String::format(const char_t* format, ...)
{
    ASSERT(format);

    char_t* chars;
    va_list args;

    va_start(args, format);
    formatAllocStringArgs(&chars, format, args);
    va_end(args);

    return String(chars);
}

String String::format(const char_t* format, va_list args)
{
    ASSERT(format);

    char_t* chars;
    formatAllocStringArgs(&chars, format, args);
    return String(chars);
}

// ConstStringIterator

unichar_t ConstStringIterator::value() const
{
    ASSERT(_pos);

    unichar_t ch;
    UTF_CHAR_TO_UNICODE(_pos, ch);
    return ch;
}

bool ConstStringIterator::moveNext()
{
    _pos = _pos ? UTF_CHAR_FORWARD(_pos) : _str.chars();

    if (*_pos)
        return true;
    else
    {
        _pos = NULL;
        return false;
    }
}

bool ConstStringIterator::movePrev()
{
    if (!_pos)
        _pos = _str.chars() + _str.length();

    if (_pos > _str.chars())
    {
        _pos = UTF_CHAR_BACK(_pos);
        return true;
    }
    else
    {
        _pos = NULL;
        return false;
    }
}
