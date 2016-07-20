#include <Foundation.hpp>

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
    ASSERT(count >= 0);

    if (count > 0)
    {
        _length = count;
        _capacity = _length + 1;
        _chars = allocate(_capacity);
        STRSET(_chars, c, _length);
        _chars[_length] = 0;
    }
    else
    {
        _length = 0;
        _capacity = 0;
        _chars = nullptr;
    }
}

String::String(const String& other)
{
    if (other.empty())
    {
        _length = 0;
        _capacity = 0;
        _chars = nullptr;
    }
    else
    {
        _length = other._length;
        _capacity = _length + 1;
        _chars = allocate(_capacity);
        STRCPY(_chars, other._chars);
    }
}

String::String(const String& other, int pos, int len)
{
    ASSERT(pos >= 0 && pos <= other._length);
    ASSERT(len >= 0 && pos + len <= other._length);

    if (other.empty())
    {
        _length = 0;
        _capacity = 0;
        _chars = nullptr;
    }
    else
    {
        _length = len;
        _capacity = _length + 1;
        _chars = allocate(_capacity);
        STRNCPY(_chars, other._chars + pos, _length);
        _chars[_length] = 0;
    }
}

String::String(const char_t* chars)
{
    ASSERT(chars);

    if (*chars)
    {
        _length = STRLEN(chars);
        _capacity = _length + 1;
        _chars = allocate(_capacity);
        STRCPY(_chars, chars);
    }
    else
    {
        _length = 0;
        _capacity = 0;
        _chars = nullptr;
    }
}

String::String(const char_t* chars, int pos, int len)
{
    ASSERT(chars);
    ASSERT(pos >= 0);
    ASSERT(len >= 0);

    if (*chars)
    {
        _length = len;
        _capacity = _length + 1;
        _chars = allocate(_capacity);
        STRNCPY(_chars, chars + pos, _length);
        _chars[_length] = 0;
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
    ASSERT(chars);

    _length = STRLEN(chars);
    _capacity = _length + 1;
    _chars = chars;
}

String::String(int capacity)
{
    ASSERT(capacity >= 0);

    if (capacity > 0)
    {
        _length = 0;
        _capacity = capacity;
        _chars = allocate(_capacity);
        *_chars = 0;
    }
    else
    {
        _length = 0;
        _capacity = 0;
        _chars = nullptr;
    }
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

    _length = other._length;
    _capacity = other._capacity;
    _chars = other._chars;

    other._length = 0;
    other._capacity = 0;
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

String String::substr(int pos, int len) const
{
    ASSERT(pos >= 0 && pos <= _length);
    ASSERT(len >= 0 && len <= _length);

    return String(*this, pos, len);
}

void String::ensureCapacity(int capacity)
{
    ASSERT(capacity >= 0);

    if (capacity > _capacity)
    {
        String tmp(capacity);
        tmp.assign(*this);
        swap(*this, tmp);
    }
}

void String::shrinkToLength()
{
    if (_capacity > _length + 1)
    {
        String tmp(*this);
        swap(*this, tmp);
    }
}

void String::assign(const String& str)
{
    if (str.empty())
    {
        if (_chars)
        {
            _length = 0;
            *_chars = 0;
        }
    }
    else
    {
        int capacity = str._length + 1;

        if (capacity > _capacity)
        {
            String tmp(str);
            swap(*this, tmp);
        }
        else
        {
            _length = str._length;
            STRCPY(_chars, str._chars);
        }
    }
}

void String::assign(const char_t* chars)
{
    ASSERT(chars);

    int len = STRLEN(chars);
    int capacity = len + 1;

    if (capacity > _capacity)
    {
        String tmp(chars);
        swap(*this, tmp);
    }
    else
    {
        _length = len;
        STRCPY(_chars, chars);
    }
}

void String::append(const String& str)
{
    if (_chars)
    {
        if (!str.empty())
        {
            int capacity = _length + str._length + 1;

            if (capacity > _capacity)
                ensureCapacity(capacity * 2);

            STRCPY(_chars + _length, str._chars);
            _length += str._length;
        }
    }
    else
    {
        if (!str.empty())
            this->assign(str);
    }
}

void String::append(const char_t* chars)
{
    ASSERT(chars);

    if (_chars)
    {
        int len = STRLEN(chars);
        int capacity = _length + len + 1;

        if (capacity > _capacity)
            ensureCapacity(capacity * 2);

        STRCPY(_chars + _length, chars);
        _length += len;
    }
    else
        this->assign(chars);
}

void String::insert(int pos, const String& str)
{
    ASSERT(pos >= 0 && pos <= _length);

    if (_chars)
    {
        if (!str.empty())
        {
            int capacity = _length + str._length + 1;

            if (capacity > _capacity)
                ensureCapacity(capacity * 2);

            char_t* p = _chars + pos;
            STRMOVE(p + str._length, p, _length - pos + 1);
            STRNCPY(p, str._chars, str._length);
            _length += str._length;
        }
    }
    else
    {
        if (!str.empty())
            this->assign(str);
    }
}

void String::insert(int pos, const char_t* chars)
{
    ASSERT(pos >= 0 && pos <= _length);
    ASSERT(chars);

    if (_chars)
    {
        int len = STRLEN(chars);
        int capacity = _length + len + 1;

        if (capacity > _capacity)
            ensureCapacity(capacity * 2);

        char_t* p = _chars + pos;
        STRMOVE(p + len, p, _length - pos + 1);
        STRNCPY(p, chars, len);
        _length += len;
    }
    else
        this->assign(chars);
}

void String::erase(int pos, int len)
{
    ASSERT(pos >= 0 && pos <= _length);
    ASSERT(len >= 0 && len <= _length);

    if (_chars && len > 0)
    {
        char_t* p = _chars + pos;
        STRMOVE(p, p + len, _length - pos - len + 1);
        _length -= len;
    }
}

void String::erase(const String& str)
{
    if (_chars && !str.empty())
    {
        char_t* from = _chars;
        char_t* found;
        int foundCnt = 0;

        while ((found = STRSTR(from, str._chars)) != nullptr)
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
                while ((found = STRSTR(from, str._chars)) != nullptr)
                {
                    from = found + str._length;
                    STRMOVE(found, from, _chars + _length - from + 1);
                    _length -= str._length;
                }
            }
            else
                clear();
        }
    }
}

void String::erase(const char_t* chars)
{
    ASSERT(chars);

    if (_chars && !*chars)
    {
        char_t* from = _chars;
        char_t* found;
        int foundCnt = 0, len = STRLEN(chars);

        while ((found = STRSTR(from, chars)) != nullptr)
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
                while ((found = STRSTR(from, chars)) != nullptr)
                {
                    from = found + len;
                    STRMOVE(found, from, _chars + _length - from + 1);
                    _length -= len;
                }
            }
            else
                clear();
        }
    }
}

void String::clear()
{
    _length = 0;
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

void String::replace(int pos, int len, const String& str)
{
    ASSERT(pos >= 0 && pos <= _length);
    ASSERT(len >= 0 && len <= _length);

    if (str.empty())
    {
        erase(pos, len);
    }
    else if (_chars)
    {
        int newLen = _length - len + str._length;
        int capacity = newLen + 1;

        if (capacity > _capacity)
            ensureCapacity(capacity * 2);
        
        if (newLen > 0)
        {
            char_t* p = _chars + pos;
            STRMOVE(p + str._length, p + len, _length - pos - len + 1);
            STRNCPY(p, str._chars, str._length);
            _length = newLen;
        }
        else
            clear();
    }
}

void String::replace(int pos, int len, const char_t* chars)
{
    ASSERT(pos >= 0 && pos <= _length);
    ASSERT(len >= 0 && len <= _length);
    ASSERT(chars);

    if (!*chars)
    {
        erase(pos, len);
    }
    else if (_chars)
    {
        int replLen = STRLEN(chars);
        int newLen = _length - len + replLen;
        int capacity = newLen + 1;

        if (capacity > _capacity)
            ensureCapacity(capacity * 2);
        
        if (newLen > 0)
        {
            char_t* p = _chars + pos;
            STRMOVE(p + replLen, p + len, _length - pos - len + 1);
            STRNCPY(p, chars, replLen);
            _length = newLen;
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

#else

void Console::setCharInputMode()
{
    struct termios ta;

    tcgetattr(STDIN_FILENO, &ta);
    ta.c_lflag &= ~(ECHO|ICANON);
    ta.c_cc[VTIME] = 0;
    ta.c_cc[VMIN] = 1;

    tcsetattr(STDIN_FILENO, TCSANOW, &ta);
}

void Console::setLineInputMode()
{
    struct termios ta;

    tcgetattr(STDIN_FILENO, &ta);
    ta.c_lflag |= ECHO|ICANON;

    tcsetattr(STDIN_FILENO, TCSANOW, &ta);
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

#ifdef PLATFORM_WINDOWS

void Console::getSize(int& width, int& height)
{
}

void Console::clearScreen()
{
}

void Console::showCursor()
{
}

void Console::hideCursor()
{
}

void Console::setCursorPosition(int line, int column)
{
}

#else

void Console::getSize(int& width, int& height)
{
    struct winsize ws;

    ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
    width = ws.ws_col;
    height = ws.ws_row;
}

void Console::clearScreen()
{
    ::write(STDOUT_FILENO, "\x1b[2J", 4);
    setCursorPosition(1, 1);
}

void Console::showCursor()
{
    ::write(STDOUT_FILENO, "\x1b[?25h", 6);
}

void Console::hideCursor()
{
    ::write(STDOUT_FILENO, "\x1b[?25l", 6);
}

void Console::setCursorPosition(int line, int column)
{
    char cmd[30];

    sprintf(cmd, "\x1b[%d;%dH", line, column);
    ::write(STDOUT_FILENO, cmd, strlen(cmd));
}

#endif

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
    if (!open(fileName, openMode))
        throw Exception(STR("failed to open file"));
}

File::~File()
{
    close();
}

bool File::open(const String& fileName, FileMode openMode)
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
    
    return _handle != INVALID_HANDLE_VALUE;
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
