#include <foundation.h>

// string formatting

int vasprintf(char_t** str, const char_t* format, va_list args)
{
    va_list args2;

    va_copy(args2, args);
#ifdef CHAR_ENCODING_UTF16
    int len = _vscwprintf(format, args2);
#else
    int len = vsnprintf(nullptr, 0, format, args2);
#endif
    va_end(args2);

    if (len < 0)
        return -1;

    *str = Memory::allocate<char_t>(len + 1);

#ifdef CHAR_ENCODING_UTF16
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

String::String(int count, char_t ch)
{
    ASSERT(count >= 0);

    if (count > 0)
    {
        _length = count;
        _capacity = _length + 1;
        _chars = Memory::allocate<char_t>(_capacity);
        STRSET(_chars, ch, _length);
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
    if (other._length > 0)
    {
        _length = other._length;
        _capacity = _length + 1;
        _chars = Memory::allocate<char_t>(_capacity);
        STRNCPY(_chars, other._chars, _length + 1);
    }
    else
    {
        _length = 0;
        _capacity = 0;
        _chars = nullptr;
    }
}

String::String(const String& other, int pos, int len)
{
    ASSERT(pos >= 0 && pos <= other._length);
    ASSERT(len >= 0 && pos + len <= other._length);

    if (len > 0)
    {
        _length = len;
        _capacity = _length + 1;
        _chars = Memory::allocate<char_t>(_capacity);
        STRNCPY(_chars, other._chars + pos, _length);
        _chars[_length] = 0;
    }
    else
    {
        _length = 0;
        _capacity = 0;
        _chars = nullptr;
    }
}

String::String(const char_t* chars)
{
    ASSERT(chars);

    if (*chars)
    {
        _length = STRLEN(chars);
        _capacity = _length + 1;
        _chars = Memory::allocate<char_t>(_capacity);
        STRNCPY(_chars, chars, _length + 1);
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

    if (len > 0)
    {
        _length = len;
        _capacity = _length + 1;
        _chars = Memory::allocate<char_t>(_capacity);
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
        _chars = Memory::allocate<char_t>(_capacity);
        *_chars = 0;
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
    String tmp(static_cast<String&&>(other));
    swap(*this, tmp);
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

String& String::operator+=(const char_t ch)
{
    append(ch);
    return *this;
}

String String::substr(int pos, int len) const
{
    return String(*this, pos, len);
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

void String::assign(const String& str)
{
    if (this != &str)
    {
        if (str._length > 0)
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
                STRNCPY(_chars, str._chars, _length + 1);
            }
        }
        else
            clear();
    }
}

void String::assign(const char_t* chars)
{
    ASSERT(chars);

    if (_chars != chars)
    {
        if (*chars)
        {
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
                STRNCPY(_chars, chars, _length + 1);
            }
        }
        else
            clear();
    }
}

void String::append(const String& str)
{
    ASSERT(this != &str);

    if (str._length > 0)
    {
        ensureCapacity(_length + str._length + 1);
        STRNCPY(_chars + _length, str._chars, str._length);
        _length += str._length;
        _chars[_length] = 0;
    }
}

void String::append(const char_t* chars)
{
    ASSERT(chars);
    ASSERT(_chars != chars);

    if (*chars)
    {
        int len = STRLEN(chars);

        ensureCapacity(_length + len + 1);
        STRNCPY(_chars + _length, chars, len);
        _length += len;
        _chars[_length] = 0;
    }
}

void String::append(const char_t ch)
{
    ensureCapacity(_length + 2);
    _chars[_length++] = ch;
    _chars[_length] = 0;
}

void String::appendFormat(const char_t* format, ...)
{
    va_list args;

    va_start(args, format);
    append(String::format(format, args));
    va_end(args);
}

void String::appendFormat(const char_t* format, va_list args)
{
    append(String::format(format, args));
}
    
void String::insert(int pos, const String& str)
{
    ASSERT(pos >= 0 && pos <= _length);
    ASSERT(this != &str);

    if (str._length > 0)
    {
        ensureCapacity(_length + str._length + 1);

        char_t* p = _chars + pos;
        STRMOVE(p + str._length, p, _length - pos + 1);
        STRNCPY(p, str._chars, str._length);
        _length += str._length;
    }
}

void String::insert(int pos, const char_t* chars)
{
    ASSERT(pos >= 0 && pos <= _length);
    ASSERT(chars);
    ASSERT(_chars != chars);

    if (*chars)
    {
        int len = STRLEN(chars);
        ensureCapacity(_length + len + 1);

        char_t* p = _chars + pos;
        STRMOVE(p + len, p, _length - pos + 1);
        STRNCPY(p, chars, len);
        _length += len;
    }
}

void String::erase(int pos, int len)
{
    ASSERT(pos >= 0 && pos <= _length);
    ASSERT(len >= 0 && pos + len <= _length);

    if (len > 0)
    {
        char_t* p = _chars + pos;
        STRMOVE(p, p + len, _length - pos - len + 1);
        _length -= len;
    }
}

void String::erase(const String& str)
{
    ASSERT(this != &str);

    if (str._length > 0)
    {
        if (_length > 0)
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
                        STRMOVE(found, found + str._length, 
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
    ASSERT(chars);
    ASSERT(_chars != chars);

    if (*chars)
    {
        if (_length > 0)
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
                        STRMOVE(found, found + len,
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

void String::replace(int pos, int len, const String& str)
{
    ASSERT(pos >= 0 && pos <= _length);
    ASSERT(len >= 0 && pos + len <= _length);
    ASSERT(this != &str);

    if (str._length > 0)
    {
        int newLen = _length - len + str._length;
        
        if (newLen > 0)
        {
            ensureCapacity(newLen + 1);

            char_t* p = _chars + pos;
            STRMOVE(p + str._length, p + len, _length - pos - len + 1);
            STRNCPY(p, str._chars, str._length);
            _length = newLen;
        }
        else
            this->assign(str);
    }
    else
        erase(pos, len);
}

void String::replace(int pos, int len, const char_t* chars)
{
    ASSERT(pos >= 0 && pos <= _length);
    ASSERT(len >= 0 && pos + len <= _length);
    ASSERT(chars);
    ASSERT(_chars != chars);

    if (*chars)
    {
        int replaceLen = STRLEN(chars);
        int newLen = _length - len + replaceLen;
        
        if (newLen > 0)
        {
            ensureCapacity(newLen + 1);

            char_t* p = _chars + pos;
            STRMOVE(p + replaceLen, p + len, _length - pos - len + 1);
            STRNCPY(p, chars, replaceLen);
            _length = newLen;
        }
        else
            this->assign(chars);
    }
    else
        erase(pos, len);
}

void String::replace(const String& searchStr, const String& replaceStr)
{
    ASSERT(this != &searchStr);
    ASSERT(this != &replaceStr);

    if (searchStr._length > 0)
    {
        if (replaceStr._length > 0)
        {
            if (_length > 0)
            {
                char_t* from = _chars;
                char_t* found;
                int foundCnt = 0;

                while ((found = STRSTR(from, searchStr._chars)) != nullptr)
                {
                    ++foundCnt;
                    from = found + searchStr._length;
                }

                if (foundCnt > 0)
                {
                    int capacity = _length + foundCnt * (replaceStr._length - searchStr._length) + 1;
                    ensureCapacity(capacity);

                    from = _chars;
                    while ((found = STRSTR(from, searchStr._chars)) != nullptr)
                    {
                        STRMOVE(found + replaceStr._length, found + searchStr._length,
                            _chars + _length - found - searchStr._length + 1);
                        STRNCPY(found, replaceStr._chars, replaceStr._length);

                        from = found + replaceStr._length;
                        _length += replaceStr._length - searchStr._length;
                    }
                }
            }
        }
        else
            erase(searchStr);
    }
}

void String::replace(const char_t* searchChars, const char_t* replaceChars)
{
    ASSERT(searchChars);
    ASSERT(_chars != searchChars);
    ASSERT(replaceChars);
    ASSERT(_chars != replaceChars);

    if (*searchChars)
    {
        if (*replaceChars)
        {
            if (_length > 0)
            {
                char_t* from = _chars;
                char_t* found;
                int foundCnt = 0;
                int searchLen = STRLEN(searchChars);

                while ((found = STRSTR(from, searchChars)) != nullptr)
                {
                    ++foundCnt;
                    from = found + searchLen;
                }

                if (foundCnt > 0)
                {
                    int replaceLen = STRLEN(replaceChars);

                    int capacity = _length + foundCnt * (replaceLen - searchLen) + 1;
                    ensureCapacity(capacity);

                    from = _chars;
                    while ((found = STRSTR(from, searchChars)) != nullptr)
                    {
                        STRMOVE(found + replaceLen, found + searchLen,
                            _chars + _length - found - searchLen + 1);
                        STRNCPY(found, replaceChars, replaceLen);

                        from = found + replaceLen;
                        _length += replaceLen - searchLen;
                    }
                }
            }
        }
        else
            erase(searchChars);
    }
}

void String::trim()
{
    if (_length > 0)
    {
        char_t* p = _chars;
        while (ISSPACE(*p))
            ++p;

        char_t* q = _chars + _length;
        while (p < q && ISSPACE(*(q - 1)))
            --q;

        if (p < q)
        {
            _length = q - p;
            STRMOVE(_chars, p, _length);
            _chars[_length] = 0;
        }
        else
            clear();
    }
}

void String::trimRight()
{
    if (_length > 0)
    {
        char_t* p = _chars;

        char_t* q = _chars + _length;
        while (p < q && ISSPACE(*(q - 1)))
            --q;

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
    if (_length > 0)
    {
        char_t* p = _chars;
        while (ISSPACE(*p))
            ++p;

        char_t* q = _chars + _length;

        if (p < q)
        {
            _length = q - p;
            STRMOVE(_chars, p, _length);
            _chars[_length] = 0;
        }
        else
            clear();
    }
}

void String::reverse()
{
    if (_length > 0)
    {
        char_t* p = _chars;
        char_t* q = _chars + _length - 1;

        while (p < q)
        {
            char_t ch = *p; *p = *q; *q = ch;
            ++p; --q;
        }
    }
}

void String::toUpper()
{
    for (int i = 0; i < _length; ++i)
        _chars[i] = TOUPPER(_chars[i]);
}

void String::toLower()
{
    for (int i = 0; i < _length; ++i)
        _chars[i] = TOLOWER(_chars[i]);
}

int String::compare(const String& str) const
{
    return STRCMP(this->str(), str.str());
}

int String::compare(const char_t* chars) const
{
    ASSERT(chars);
    return STRCMP(str(), chars);
}

int String::compareNoCase(const String& str) const
{
    return STRICMP(this->str(), str.str());
}

int String::compareNoCase(const char_t* chars) const
{
    ASSERT(chars);
    return STRICMP(str(), chars);
}

int String::find(const String& str, int pos) const
{
    ASSERT(pos >= 0 && pos <= _length);

    if (str._length > 0)
    {
        if (_length > 0)
        {
            const char_t* found = STRSTR(_chars + pos, str._chars);
            if (found)
                return found - _chars;
        }
    }

    return INVALID_POS;
}

int String::find(const char_t* chars, int pos) const
{
    ASSERT(pos >= 0 && pos <= _length);
    ASSERT(chars);

    if (*chars)
    {
        if (_length > 0)
        {
            const char_t* found = STRSTR(_chars + pos, chars);
            if (found)
                return found - _chars;
        }
    }

    return INVALID_POS;
}

int String::findNoCase(const String& str, int pos) const
{
    ASSERT(pos >= 0 && pos <= _length);

    if (str._length > 0)
    {
        if (_length > 0)
        {
            const char_t* found = STRISTR(_chars + pos, str._chars);
            if (found)
                return found - _chars;
        }
    }

    return INVALID_POS;
}

int String::findNoCase(const char_t* chars, int pos) const
{
    ASSERT(pos >= 0 && pos <= _length);
    ASSERT(chars);

    if (*chars)
    {
        if (_length > 0)
        {
            const char_t* found = STRISTR(_chars + pos, chars);
            if (found)
                return found - _chars;
        }
    }

    return INVALID_POS;
}

bool String::startsWith(const String& str) const
{
    if (str._length > 0 && _length > 0)
        return STRNCMP(_chars, str._chars, str._length) == 0;
    else
        return false;
}

bool String::startsWith(const char_t* chars) const
{
    ASSERT(chars);

    int len = STRLEN(chars);
    if (len > 0 && _length > 0)
        return STRNCMP(_chars, chars, len) == 0;
    else
        return false;
}

bool String::endsWith(const String& str) const
{
    if (str._length > 0 && _length > 0 && str._length <= _length)
        return STRCMP(_chars + _length - str._length, str._chars) == 0;
    else
        return false;
}

bool String::endsWith(const char_t* chars) const
{
    ASSERT(chars);

    int len = STRLEN(chars);
    if (len > 0 && _length > 0 && len <= _length)
        return STRCMP(_chars + _length - len, chars) == 0;
    else
        return false;
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
    ASSERT(format);

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
    ASSERT(format);

    char_t* chars;

    int rc = vasprintf(&chars, format, args);
    if (rc < 0)
        throw Exception(STR("failed to format string"));

    return String(chars);
}

// string concatenation

String operator+(const String& left, const String& right)
{
    return String::concat(left, right);
}

String operator+(const String& left, const char_t* right)
{
    return String::concat(left, right);
}

String operator+(const char_t* left, const String& right)
{
    return String::concat(left, right);
}

// string compare

bool operator==(const String& left, const String& right)
{
    return STRCMP(left.str(), right.str()) == 0;
}

bool operator!=(const String& left, const String& right)
{
    return STRCMP(left.str(), right.str()) != 0;
}

bool operator<(const String& left, const String& right)
{
    return STRCMP(left.str(), right.str()) < 0;
}

bool operator<=(const String& left, const String& right)
{
    return STRCMP(left.str(), right.str()) <= 0;
}

bool operator>(const String& left, const String& right)
{
    return STRCMP(left.str(), right.str()) > 0;
}

bool operator>=(const String& left, const String& right)
{
    return STRCMP(left.str(), right.str()) >= 0;
}

bool operator==(const char_t* left, const String& right)
{
    return STRCMP(left, right.str()) == 0;
}

bool operator==(const String& left, const char_t* right)
{
    return STRCMP(left.str(), right) == 0;
}

bool operator!=(const char_t* left, const String& right)
{
    return STRCMP(left, right.str()) != 0;
}

bool operator!=(const String& left, const char_t* right)
{
    return STRCMP(left.str(), right) != 0;
}

bool operator<(const char_t* left, const String& right)
{
    return STRCMP(left, right.str()) < 0;
}

bool operator<(const String& left, const char_t* right)
{
    return STRCMP(left.str(), right) < 0;
}

bool operator<=(const char_t* left, const String& right)
{
    return STRCMP(left, right.str()) <= 0;
}

bool operator<=(const String& left, const char_t* right)
{
    return STRCMP(left.str(), right) <= 0;
}

bool operator>(const char_t* left, const String& right)
{
    return STRCMP(left, right.str()) > 0;
}

bool operator>(const String& left, const char_t* right)
{
    return STRCMP(left.str(), right) > 0;
}

bool operator>=(const char_t* left, const String& right)
{
    return STRCMP(left, right.str()) >= 0;
}

bool operator>=(const String& left, const char_t* right)
{
    return STRCMP(left.str(), right) >= 0;
}

// Console

void Console::setConsoleMode(bool unicode, bool canonical)
{
#ifdef PLATFORM_WINDOWS
    _setmode(_fileno(stdout), unicode ? _O_U16TEXT : _O_U8TEXT);
#else
    termios ta;
    tcgetattr(STDIN_FILENO, &ta);

    if (canonical)
        ta.c_lflag |= ECHO | ICANON;
    else
    {
        ta.c_lflag &= ~(ECHO | ICANON);
        ta.c_cc[VTIME] = 0;
        ta.c_cc[VMIN] = 1;
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &ta);    
#endif
}

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
    String line;
    int ch = GETCHAR();

    while (!(ch == '\n' || ch == CHAR_EOF))
    {
        line.append(ch);
        ch = GETCHAR();
    }

    return line;
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
    }
    else
        throw Exception(STR("failed to read file"));

    return bytes;
}

void File::writeBytes(const Array<uint8_t>& bytes)
{
    if (_handle == INVALID_HANDLE_VALUE)
        throw Exception(STR("file not opened"));
    
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

String File::readString()
{
    if (_handle == INVALID_HANDLE_VALUE)
        throw Exception(STR("file not opened"));

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
    }
    else
        throw Exception(STR("failed to read file"));
            
    chars[charsSize] = 0;
    return String::acquire(chars.release());
}

void File::writeString(const String& str)
{
    if (_handle == INVALID_HANDLE_VALUE)
        throw Exception(STR("file not opened"));

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

int64_t File::size() const
{
    if (_handle == INVALID_HANDLE_VALUE)
        throw Exception(STR("file not opened"));

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
