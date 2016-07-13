#include <Foundation.hpp>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef PLATFORM_WINDOWS
#include <io.h>
#else
#include <unistd.h>
#endif

// string formatting

int vasprintf(char_t** str, const char_t* format, va_list args)
{
    va_list args2;

    va_copy(args2, args);
#ifdef PLATFORM_WINDOWS
    int len = _vscwprintf(format, args2);
#else
    int len = vsnprintf(nullptr, 0, format, args2);
#endif
    va_end(args2);

    if (len < 0 || !(*str = Memory::allocateArray<char_t>(len + 1)))
        return -1;

#ifdef PLATFORM_WINDOWS
    return _vsnwprintf(*str, len + 1, format, args);
#else
    return vsnprintf(*str, len + 1, format, args);
#endif
}

int asprintf(char_t** str, const char_t* format, ...)
{
    va_list args;

    va_start(args, format);
    int rc = vasprintf(str, format, args);
    va_end(args);

    return rc;
}

// String

String::String(int count, char_t c)
{
    if (count > 0)
    {
        _chars = allocate(count + 1);
        STRSET(_chars, c, count);
        _chars[count] = 0;
    }
    else
        _chars = nullptr;
}

String::String(const String& other)
{
    if (other._chars)
    {
        int len = STRLEN(other._chars);
        _chars = allocate(len + 1);
        STRNCPY(_chars, other._chars, len);
        _chars[len] = 0;
    }
    else
        _chars = nullptr;
}

String::String(const String& other, int pos, int len)
{
    if (other._chars && len > 0)
    {
        _chars = allocate(len + 1);
        STRNCPY(_chars, other._chars + pos, len);
        _chars[len] = 0;
    }
    else
        _chars = nullptr;
}

String::String(const char_t* chars)
{
    if (*chars)
    {
        int len = STRLEN(chars);
        _chars = allocate(len + 1);
        STRNCPY(_chars, chars, len);
        _chars[len] = 0;
    }
    else
        _chars = nullptr;
}

String::String(const char_t* chars, int pos, int len)
{
    if (*chars && len > 0)
    {
        _chars = allocate(len + 1);
        STRNCPY(_chars, chars + pos, len);
        _chars[len] = 0;
    }
    else
        _chars = nullptr;
}

String::~String()
{
    Memory::deallocate(_chars);
}

String& String::operator=(const String& other)
{
    assign(other);
    return *this;
}

String& String::operator=(const char_t* chars)
{
    assign(chars);
    return *this;
}

String& String::operator=(String&& other)
{
    Memory::deallocate(_chars);
    _chars = other._chars;
    other._chars = nullptr;
    return *this;
}

String& String::operator+=(const String& str)
{
    append(str);
    return *this;
}

String& String::operator+=(const char_t* chars)
{
    append(chars);
    return *this;
}

int String::length() const
{
    return _chars ? STRLEN(_chars) : 0;
}

String String::substr(int pos, int len) const
{
    return String(*this, pos, len);
}

void String::assign(const String& str)
{
    String tmp(str);
    swap(*this, tmp);
}

void String::assign(const char_t* chars)
{
    String tmp(chars);
    swap(*this, tmp);
}

void String::append(const String& str)
{
    if (_chars)
    {
        if (str._chars)
        {
            int curLen = STRLEN(_chars);
            int appLen = STRLEN(str._chars);
            char_t* newChars = allocate(curLen + appLen + 1);

            *STRNCPY(STRNCPY(newChars, _chars, curLen),
                    str._chars, appLen) = 0;

            Memory::deallocate(_chars);
            _chars = newChars;
        }
    }
    else
    {
        if (str._chars)
            *this = str;
    }
}

void String::append(const char_t* chars)
{
    if (_chars)
    {
        int curLen = STRLEN(_chars);
        int appLen = STRLEN(chars);
        char_t* newChars = allocate(curLen + appLen + 1);

        *STRNCPY(STRNCPY(newChars, _chars, curLen),
                chars, appLen) = 0;

        Memory::deallocate(_chars);
        _chars = newChars;
    }
    else
        *this = chars;
}

void String::insert(int pos, const String& str)
{
    if (_chars)
    {
        if (str._chars)
        {
            int curLen = STRLEN(_chars);
            int insLen = STRLEN(str._chars);
            char_t* newChars = allocate(curLen + insLen + 1);

            *STRNCPY(STRNCPY(STRNCPY(newChars, _chars, pos),
                    str._chars, insLen),
                    _chars + pos, curLen - pos) = 0;

            Memory::deallocate(_chars);
            _chars = newChars;
        }
    }
    else
    {
        if (str._chars)
            *this = str;
    }
}

void String::insert(int pos, const char_t* chars)
{
    if (_chars)
    {
        int curLen = STRLEN(_chars);
        int insLen = STRLEN(chars);
        char_t* newChars = allocate(curLen + insLen + 1);

        *STRNCPY(STRNCPY(STRNCPY(newChars, _chars, pos),
                chars, insLen),
                _chars + pos, curLen - pos) = 0;

        Memory::deallocate(_chars);
        _chars = newChars;
    }
    else
        *this = chars;
}

void String::erase(int pos, int len)
{
    if (_chars && len > 0)
    {
        int curLen = STRLEN(_chars);
        int newLen = curLen - len;

        if (newLen > 0)
        {
            char_t* newChars = allocate(newLen + 1);

            *STRNCPY(STRNCPY(newChars, _chars, pos),
                    _chars + pos + len, curLen - pos - len) = 0;

            Memory::deallocate(_chars);
            _chars = newChars;
        }
        else
            clear();
    }
}

void String::erase(const String& str)
{
    if (_chars && str._chars)
    {
        const char_t* from = _chars;
        const char_t* found;

        int curLen = STRLEN(_chars);
        int len = STRLEN(str._chars), foundCnt = 0;

        while ((found = STRSTR(from, str._chars)) != nullptr)
        {
            ++foundCnt;
            from = found + len;
        }
        
        if (foundCnt > 0)
        {
            int newLen = curLen - foundCnt * len;
            if (newLen > 0)
            {
                from = _chars;
                char_t* newChars = allocate(newLen + 1);
                char_t* to = newChars;

                while ((found = STRSTR(from, str._chars)) != nullptr)
                {
                    to = STRNCPY(to, from, found - from);
                    from = found + len;
                }

                *STRNCPY(to, from, _chars + curLen - from) = 0;
                Memory::deallocate(_chars);
                _chars = newChars;
            }
            else
                clear();
        }
    }
}

void String::erase(const char_t* chars)
{
    if (_chars)
    {
        const char_t* from = _chars;
        const char_t* found;

        int curLen = STRLEN(_chars);
        int len = STRLEN(chars), foundCnt = 0;

        while ((found = STRSTR(from, chars)) != nullptr)
        {
            ++foundCnt;
            from = found + len;
        }
        
        if (foundCnt > 0)
        {
            int newLen = curLen - foundCnt * len;
            if (newLen > 0)
            {
                from = _chars;
                char_t* newChars = allocate(newLen + 1);
                char_t* to = newChars;

                while ((found = STRSTR(from, chars)) != nullptr)
                {
                    to = STRNCPY(to, from, found - from);
                    from = found + len;
                }

                *STRNCPY(to, from, _chars + curLen - from) = 0;
                Memory::deallocate(_chars);
                _chars = newChars;
            }
            else
                clear();
        }
    }
}

void String::clear()
{
    Memory::deallocate(_chars);
    _chars = nullptr;
}

void String::replace(int pos, int len, const String& str)
{
    if (!str._chars)
    {
        erase(pos, len);
    }
    else if (_chars && len > 0)
    {
        int curLen = STRLEN(_chars);
        int replLen = STRLEN(str._chars);
        int newLen = curLen - len + replLen;
        
        if (newLen > 0)
        {
            char_t* newChars = allocate(newLen + 1);

            *STRNCPY(STRNCPY(STRNCPY(newChars, _chars, pos),
                    str._chars, replLen),
                    _chars + pos + len, curLen - pos - len) = 0;

            Memory::deallocate(_chars);
            _chars = newChars;
        }
        else
            clear();
    }
}

void String::replace(int pos, int len, const char_t* chars)
{
    if (_chars && len > 0)
    {
        int curLen = STRLEN(_chars);
        int replLen = STRLEN(chars);
        int newLen = curLen - len + replLen;
        
        if (newLen > 0)
        {
            char_t* newChars = allocate(newLen + 1);

            *STRNCPY(STRNCPY(STRNCPY(newChars, _chars, pos),
                    chars, replLen),
                    _chars + pos + len, curLen - pos - len) = 0;

            Memory::deallocate(_chars);
            _chars = newChars;
        }
        else
            clear();
    }
}

void String::replace(const String& searchStr, const String& replaceStr)
{
    if (!replaceStr._chars)
    {
        erase(searchStr);
    }
    else if (_chars && searchStr._chars)
    {
        int searchLen = STRLEN(searchStr._chars);
        
        if (searchLen > 0)
        {
            const char_t* from = _chars;
            const char_t* found;
            int foundCnt = 0;

            while ((found = STRSTR(from, searchStr._chars)) != nullptr)
            {
                ++foundCnt;
                from = found + searchLen;
            }

            if (foundCnt > 0)
            {
                from = _chars;
                int curLen = STRLEN(_chars);
                int replLen = STRLEN(replaceStr._chars);
                int newLen = curLen - foundCnt * searchLen + foundCnt * replLen;
                
                if (newLen > 0)
                {
                    char_t* newChars = allocate(newLen + 1);
                    char_t* to = newChars;

                    while ((found = STRSTR(from, searchStr._chars)) != nullptr)
                    {
                        to = STRNCPY(STRNCPY(to, from, found - from), replaceStr._chars, replLen);
                        from = found + searchLen;
                    }

                    *STRNCPY(to, from, _chars + curLen - from) = 0;
                    Memory::deallocate(_chars);
                    _chars = newChars;
                }
                else
                    clear();
            }
        }
    }
}

void String::replace(const char_t* searchChars, const char_t* replaceChars)
{
    int searchLen = STRLEN(searchChars);
        
    if (searchLen > 0)
    {
        const char_t* from = _chars;
        const char_t* found;
        int foundCnt = 0;

        while ((found = STRSTR(from, searchChars)) != nullptr)
        {
            ++foundCnt;
            from = found + searchLen;
        }

        if (foundCnt > 0)
        {
            from = _chars;
            int curLen = STRLEN(_chars);
            int replLen = STRLEN(replaceChars);
            int newLen = curLen - foundCnt * searchLen + foundCnt * replLen;
            
            if (newLen > 0)
            {
                char_t* newChars = allocate(newLen + 1);
                char_t* to = newChars;

                while ((found = STRSTR(from, searchChars)) != nullptr)
                {
                    to = STRNCPY(STRNCPY(to, from, found - from), replaceChars, replLen);
                    from = found + searchLen;
                }

                *STRNCPY(to, from, _chars + curLen - from) = 0;
                Memory::deallocate(_chars);
                _chars = newChars;
            }
            else
                clear();
        }
    }
}

void String::trim()
{
    if (_chars)
    {
        char_t* p = _chars;
        while (ISBLANK(*p))
            ++p;

        char_t* q = p + STRLEN(p);
        while (p < q && ISBLANK(*(q - 1)))
            --q;

        if (p < q)
        {
            int len = q - p;
            char_t* newChars = allocate(len + 1);
            *STRNCPY(newChars, p, len) = 0;
            Memory::deallocate(_chars);
            _chars = newChars;
        }
        else
            clear();
    }
}

void String::trimRight()
{
    if (_chars)
    {
        char_t* p = _chars + STRLEN(_chars);
        while (_chars < p && ISBLANK(*(p - 1)))
            --p;

        if (_chars < p)
        {
            int len = p - _chars;
            char_t* newChars = allocate(len + 1);
            *STRNCPY(newChars, _chars, len) = 0;
            Memory::deallocate(_chars);
            _chars = newChars;
        }
        else
            clear();
    }
}

void String::trimLeft()
{
    if (_chars)
    {
        char_t* p = _chars;
        while (ISBLANK(*p))
            ++p;

        int len = STRLEN(p);
        if (len > 0)
        {
            char_t* newChars = allocate(len + 1);
            *STRNCPY(newChars, p, len) = 0;
            Memory::deallocate(_chars);
            _chars = newChars;
        }
        else
            clear();
    }
}

void String::reverse()
{
    if (_chars)
    {
        char_t* p = _chars;
        char_t* q = _chars + STRLEN(_chars) - 1;

        while (p < q)
        {
            char_t c = *p; *p = *q; *q = c;
            ++p; --q;
        }
    }
}

void String::toUpper()
{
    if (_chars)
    {
        for (char_t* p = _chars; *p; ++p)
            *p = TOUPPER(*p);
    }
}

void String::toLower()
{
    if (_chars)
    {
        for (char_t* p = _chars; *p; ++p)
            *p = TOLOWER(*p);
    }
}

int String::compare(const String& str) const
{
    return STRCMP(this->str(), str.str());
}

int String::compare(const char_t* chars) const
{
    return STRCMP(str(), chars);
}

int String::compare(int pos, int len, const String& str) const
{
    return STRNCMP(this->str() + pos, str.str(), len);
}

int String::compare(int pos, int len, const char_t* chars) const
{
    return STRNCMP(str() + pos, chars, len);
}

int String::compareNoCase(const String& str) const
{
    return STRICMP(this->str(), str.str());
}

int String::compareNoCase(const char_t* chars) const
{
    return STRICMP(str(), chars);
}

int String::compareNoCase(int pos, int len, const String& str) const
{
    return STRNICMP(this->str() + pos, str.str(), len);
}

int String::compareNoCase(int pos, int len, const char_t* chars) const
{
    return STRNICMP(str() + pos, chars, len);
}

int String::find(const String& str, int pos) const
{
    if (_chars)
    {
        const char_t* found = STRSTR(_chars + pos, str.str());
        if (found)
            return found - _chars;
    }

    return INVALID_POS;
}

int String::find(const char_t* chars, int pos) const
{
    if (_chars)
    {
        const char_t* found = STRSTR(_chars + pos, chars);
        if (found)
            return found - _chars;
    }

    return INVALID_POS;
}

bool String::startsWith(const String& str) const
{
    return compare(0, str.length(), str) == 0;
}

bool String::startsWith(const char_t* chars) const
{
    return compare(0, STRLEN(chars), chars) == 0;
}

bool String::endsWith(const String& str) const
{
    int len1 = length(), len2 = str.length();
    if (len1 < len2)
        return false;

    return compare(len1 - len2, len2, str) == 0;
}

bool String::endsWith(const char_t* chars) const
{
    int len1 = length(), len2 = STRLEN(chars);
    if (len1 < len2)
        return false;

    return compare(len1 - len2, len2, chars) == 0;
}

bool String::contains(const String& str) const
{
    return find(str) != INVALID_POS;
}

bool String::contains(const char_t* chars) const
{
    return find(chars) != INVALID_POS;
}

// conversion from string

bool String::toBool() const
{
    return compareNoCase(STR("true")) == 0;
}

int String::toInt() const
{
    return STRTOL(str(), nullptr, 10);
}

int32_t String::toInt32() const
{
    return STRTOL(str(), nullptr, 10);
}

uint32_t String::toUInt32() const
{
    return STRTOUL(str(), nullptr, 10);
}

int64_t String::toInt64() const
{
    return STRTOLL(str(), nullptr, 10);
}

uint64_t String::toUInt64() const
{
    return STRTOULL(str(), nullptr, 10);
}

float String::toFloat() const
{
    return STRTOF(str(), nullptr);
}

double String::toDouble() const
{
    return STRTOD(str(), nullptr);
}

// conversion to string

String String::from(bool value)
{
    return String(value ? STR("true") : STR("false"));
}

String String::from(int value)
{
    char_t* chars;

    if (asprintf(&chars, STR("%d"), value) < 0)
        throw Exception(STR("failed to convert to string"));

    return String(chars);
}

String String::from(unsigned value)
{
    char_t* chars;

    if (asprintf(&chars, STR("%u"), value) < 0)
        throw Exception(STR("failed to convert to string"));

    return String(chars);
}

String String::from(long value)
{
    char_t* chars;

    if (asprintf(&chars, STR("%ld"), value) < 0)
        throw Exception(STR("failed to convert to string"));

    return String(chars);
}

String String::from(unsigned long value)
{
    char_t* chars;

    if (asprintf(&chars, STR("%lu"), value) < 0)
        throw Exception(STR("failed to convert to string"));

    return String(chars);
}

String String::from(long long value)
{
    char_t* chars;

    if (asprintf(&chars, STR("%lld"), value) < 0)
        throw Exception(STR("failed to convert to string"));

    return String(chars);
}

String String::from(unsigned long long value)
{
    char_t* chars;

    if (asprintf(&chars, STR("%lld"), value) < 0)
        throw Exception(STR("failed to convert to string"));

    return String(chars);
}

String String::from(float value, int precision)
{
    char_t* chars;

    if (asprintf(&chars, STR("%.*f"), precision, static_cast<double>(value)) < 0)
        throw Exception(STR("failed to convert to string"));

    return String(chars);
}

String String::from(double value, int precision)
{
    char_t* chars;

    if (asprintf(&chars, STR("%.*f"), precision, value) < 0)
        throw Exception(STR("failed to convert to string"));

    return String(chars);
}

// formatting

String String::format(const char_t* format, ...)
{
    char_t* chars;
    va_list args;

    va_start(args, format);
    int rc = vasprintf(&chars, format, args);
    va_end(args);

    if (rc < 0)
        throw Exception(STR("failed to format string"));

    return String(chars);
}

String String::format(const char_t* format, va_list args)
{
    char_t* chars;

    int rc = vasprintf(&chars, format, args);
    if (rc < 0)
        throw Exception(STR("failed to format string"));

    return String(chars);
}

char_t* String::allocate(int len)
{
    char_t* chars = Memory::allocateArray<char_t>(len);
    if (!chars)
        throw OutOfMemoryException();
    return chars;
}

// string concatenation

String operator+(const String& x, const String& y)
{
    return String::concat(x, y);
}

String operator+(const String& x, const char_t* y)
{
    return String::concat(x, y);
}

String operator+(const char_t* x, const String& y)
{
    return String::concat(x, y);
}

// string compare

bool operator==(const String& x, const String& y)
{
    return STRCMP(x.str(), y.str()) == 0;
}

bool operator!=(const String& x, const String& y)
{
    return STRCMP(x.str(), y.str()) != 0;
}

bool operator<(const String& x, const String& y)
{
    return STRCMP(x.str(), y.str()) < 0;
}

bool operator<=(const String& x, const String& y)
{
    return STRCMP(x.str(), y.str()) <= 0;
}

bool operator>(const String& x, const String& y)
{
    return STRCMP(x.str(), y.str()) > 0;
}

bool operator>=(const String& x, const String& y)
{
    return STRCMP(x.str(), y.str()) >= 0;
}

bool operator==(const char_t* x, const String& y)
{
    return STRCMP(x, y.str()) == 0;
}

bool operator==(const String& x, const char_t* y)
{
    return STRCMP(x.str(), y) == 0;
}

bool operator!=(const char_t* x, const String& y)
{
    return STRCMP(x, y.str()) != 0;
}

bool operator!=(const String& x, const char_t* y)
{
    return STRCMP(x.str(), y) != 0;
}

bool operator<(const char_t* x, const String& y)
{
    return STRCMP(x, y.str()) < 0;
}

bool operator<(const String& x, const char_t* y)
{
    return STRCMP(x.str(), y) < 0;
}

bool operator<=(const char_t* x, const String& y)
{
    return STRCMP(x, y.str()) <= 0;
}

bool operator<=(const String& x, const char_t* y)
{
    return STRCMP(x.str(), y) <= 0;
}

bool operator>(const char_t* x, const String& y)
{
    return STRCMP(x, y.str()) > 0;
}

bool operator>(const String& x, const char_t* y)
{
    return STRCMP(x.str(), y) > 0;
}

bool operator>=(const char_t* x, const String& y)
{
    return STRCMP(x, y.str()) >= 0;
}

bool operator>=(const String& x, const char_t* y)
{
    return STRCMP(x.str(), y) >= 0;
}

// Console

#ifdef PLATFORM_WINDOWS

void Console::enableUnicode()
{
    _setmode(_fileno(stdout), _O_U16TEXT);
}

void Console::openConsole()
{
    AllocConsole();
    _wfreopen(STR("CONOUT$"), STR("w"), stdout);
}

#endif

void Console::write(char_t ch)
{
    PUTCHAR(ch);
}

void Console::write(const String& str)
{
    FPUTS(str.str(), stdout);
}

void Console::write(const char_t* format, ...)
{
    va_list args;

    va_start(args, format);
    VPRINTF(format, args);
    va_end(args);
}

void Console::writeLine()
{
    PUTCHAR('\n');
}

void Console::writeLine(const String& str)
{
    PUTS(str.str());
}

void Console::writeLine(const char_t* format, ...)
{
    va_list args;

    va_start(args, format);
    VPRINTF(format, args);
    va_end(args);
    PUTCHAR('\n');
}

String Console::readLine()
{
    char_t line[1024];
    int len = 0, maxlen = sizeof(line) - 1;

    do
    {
        int c = GETCHAR();

        if (c == '\n' || c == CHAR_EOF)
            break;

        line[len++] = c;
    }
    while (len < maxlen);

    line[len] = 0;
    return String(static_cast<const char_t*>(line));
}

// StringBuilder

StringBuilder::StringBuilder(int capacity)
{
    if (capacity > 0)
    {
        _chars = Memory::allocateArray<char_t>(capacity);

        if (!_chars)
            throw OutOfMemoryException();

        *_chars = '\0';
        _length = 0;
        _capacity = capacity;
    }
    else
        throw Exception(STR("capacity must be positive"));
}

StringBuilder::StringBuilder(const StringBuilder& other)
{
    assign(other._chars);
    _length = other._length;
    _capacity = _length + 1;
}

StringBuilder::~StringBuilder()
{
    Memory::deallocate(_chars);
}

StringBuilder& StringBuilder::operator=(const StringBuilder& other)
{
    StringBuilder tmp(other);
    swap(*this, tmp);
    return *this;
}

const char_t* StringBuilder::chars() const
{
    return _chars ? _chars : STR("");
}

int StringBuilder::length() const
{
    return _length;
}

int StringBuilder::capacity() const
{
    return _capacity;
}

bool StringBuilder::empty() const
{
    return _length == 0;
}

void StringBuilder::append(const char_t* chars)
{
    int len = STRLEN(chars);

    if (_length + len < _capacity)
    {
        MEMCPY(_chars + _length, chars, len);
        _length += len;
    }
    else
    {
        StringBuilder tmp((_capacity + len) * 2);
        tmp.append(chars);
        swap(*this, tmp);
    }
}

void StringBuilder::assign(const char_t* chars)
{
    if (chars)
    {
        _chars = strdup(chars);

        if (!_chars)
            throw OutOfMemoryException();
    }
    else
        _chars = nullptr;
}

// File

#ifndef PLATFORM_WINDOWS
#define INVALID_HANDLE_VALUE -1
#endif

File::File() :
    _handle(INVALID_HANDLE_VALUE)
{    
}

File::File(const String& fileName, FileMode openMode) :
    _handle(INVALID_HANDLE_VALUE)
{
    open(fileName, openMode);
}

File::~File()
{
    close();
}

void File::open(const String& fileName, FileMode openMode)
{
    if (_handle != INVALID_HANDLE_VALUE)
        throw Exception(STR("file already open"));
    
    int mode;
    
#ifdef PLATFORM_WINDOWS
    switch (openMode)
    {
    case FILE_MODE_CREATE_ALWAYS:
        mode = CREATE_ALWAYS;
        break;
    case FILE_MODE_CREATE_NEW:
        mode = CREATE_NEW;
        break;
    case FILE_MODE_OPEN_ALWAYS:
        mode = OPEN_ALWAYS;
        break;
    default:
    case FILE_MODE_OPEN_EXISTING:
        mode = OPEN_EXISTING;
        break;
    case FILE_MODE_TRUNCATE_EXISTING:
        mode = TRUNCATE_EXISTING;
        break;
    }
    
    _handle = CreateFile(fileName.str(), GENERIC_READ | GENERIC_WRITE,
        0, nullptr, mode, FILE_ATTRIBUTE_NORMAL, nullptr);
#else
    switch (openMode)
    {
    case FILE_MODE_CREATE_ALWAYS:
        mode = O_CREAT | O_TRUNC;
        break;
    case FILE_MODE_CREATE_NEW:
        mode = O_CREAT | O_EXCL;
        break;
    case FILE_MODE_OPEN_ALWAYS:
        mode = O_CREAT;
        break;
    default:
    case FILE_MODE_OPEN_EXISTING:
        mode = 0;
        break;
    case FILE_MODE_TRUNCATE_EXISTING:
        mode = O_TRUNC;
        break;
    }
    
    _handle = ::open(fileName.str(), O_RDWR | mode, 
        S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
#endif
    
    if (_handle == INVALID_HANDLE_VALUE)
        throw Exception(STR("failed to open file"));
}

void File::close()
{
    if (_handle != INVALID_HANDLE_VALUE)
    {
#ifdef PLATFORM_WINDOWS
        CloseHandle(_handle);
#else
        ::close(_handle);
#endif
        _handle = INVALID_HANDLE_VALUE;
    }        
}

Array<uint8_t> File::readBytes()
{
    if (_handle == INVALID_HANDLE_VALUE)
        throw Exception(STR("file not opened"));
    else
    {
#ifdef PLATFORM_WINDOWS
        DWORD bytesSize = size(), bytesRead;
        Array<uint8_t> bytes(bytesSize);
        
        if (ReadFile(_handle, bytes.elements(), bytesSize, &bytesRead, nullptr))
#else
        ssize_t bytesSize = size(), bytesRead;
        Array<uint8_t> bytes(bytesSize);

        if ((bytesRead = read(_handle, bytes.elements(), bytesSize)) >= 0)
#endif
        {
            if (bytesSize != bytesRead)
                throw Exception(STR("failed to read entire file"));
            return bytes;
        }
        else
            throw Exception(STR("failed to read file"));
    }
}

void File::writeBytes(const Array<uint8_t>& bytes)
{
    if (_handle == INVALID_HANDLE_VALUE)
        throw Exception(STR("file not opened"));
    else
    {
#ifdef PLATFORM_WINDOWS
        DWORD bytesSize = bytes.size(), bytesWritten;
        
        if (WriteFile(_handle, bytes.elements(), bytesSize, &bytesWritten, nullptr))
#else
        ssize_t bytesSize = bytes.size(), bytesWritten;

        if ((bytesWritten = write(_handle, bytes.elements(), bytesSize)) >= 0)
#endif
        {
            if (bytesSize != bytesWritten)
                throw Exception(STR("failed to write entire file"));
        }
        else
            throw Exception(STR("failed to write file"));
    }
}

String File::readString()
{
    if (_handle == INVALID_HANDLE_VALUE)
        throw Exception(STR("file not opened"));
    else
    {
#ifdef PLATFORM_WINDOWS
        DWORD charsSize = size() / sizeof(char_t);
        DWORD bytesSize = charsSize * sizeof(char_t), bytesRead;
        Array<char_t> chars(charsSize + 1);
        
        if (ReadFile(_handle, chars.elements(), bytesSize, &bytesRead, nullptr))
#else
        ssize_t charsSize = size() / sizeof(char_t);
        ssize_t bytesSize = charsSize * sizeof(char_t), bytesRead;
        Array<char_t> chars(charsSize + 1);
        
        if ((bytesRead = read(_handle, chars.elements(), bytesSize)) >= 0)
#endif
        {
            if (bytesSize != bytesRead)
                throw Exception(STR("failed to read entire file"));
            
            chars[charsSize] = 0;
            return String::acquire(chars.release());
        }
        else
            throw Exception(STR("failed to read file"));
    }
}

void File::writeString(const String& str)
{
    if (_handle == INVALID_HANDLE_VALUE)
        throw Exception(STR("file not opened"));
    else
    {
#ifdef PLATFORM_WINDOWS
        DWORD bytesSize = str.length() * sizeof(char_t), bytesWritten;
        
        if (WriteFile(_handle, str.str(), bytesSize, &bytesWritten, nullptr))
#else
        ssize_t bytesSize = str.length() * sizeof(char_t), bytesWritten;
        
        if ((bytesWritten = write(_handle, str.str(), bytesSize)) >= 0)
#endif
        {
            if (bytesSize != bytesWritten)
                throw Exception(STR("failed to write entire file"));
        }
        else
            throw Exception(STR("failed to write file"));
    }
}

int64_t File::size() const
{
    if (_handle == INVALID_HANDLE_VALUE)
        throw Exception(STR("file not opened"));
    else
    {
#ifdef PLATFORM_WINDOWS
        int64_t sz;
        
        if (!GetFileSizeEx(_handle, reinterpret_cast<LARGE_INTEGER*>(&sz)))
            throw Exception(STR("failed to get file size"));
        
        return sz;
#else
        struct stat64 st;
        fstat64(_handle, &st);
        return st.st_size;
#endif
    }
}
