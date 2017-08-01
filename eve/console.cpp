#include <console.h>

#ifdef PLATFORM_WINDOWS

void printLine(const char_t* str)
{
    _putws(reinterpret_cast<const wchar_t*>(str));
}

void print(const char_t* format, ...)
{
    va_list args;
    va_start(args, format);
    wprintf(reinterpret_cast<const wchar_t*>(format), args);
    va_end(args);
}

void printArgs(const char_t* format, va_list args)
{
    vwprintf(reinterpret_cast<const wchar_t*>(format), args);
}

#else

void printLine(const char_t* str)
{
    puts(str);
}

void print(const char_t* format, ...)
{
    va_list args;
    va_start(args, format);
    printf(format, args);
    va_end(args);
}

void printArgs(const char_t* format, va_list args)
{
    vprintf(format, args);
}

#endif

// Console

#ifdef PLATFORM_WINDOWS
Array<INPUT_RECORD> Console::_input(10);
#else
Array<char> Console::_input(10);
#endif

Array<Key> Console::_keys;

void Console::initialize()
{
    setlocale(LC_ALL, "");

#ifdef PLATFORM_WINDOWS
    ASSERT(_setmode(_fileno(stdin), _O_U16TEXT) >= 0);
    ASSERT(_setmode(_fileno(stdout), _O_U16TEXT) >= 0);
#else
    setvbuf(stdout, NULL, _IONBF, 0);
#endif
}

void Console::setLineMode(bool lineMode)
{
#ifndef PLATFORM_WINDOWS
    if (lineMode)
    {
        termios ta;
        ASSERT(tcgetattr(STDIN_FILENO, &ta) >= 0);
        ta.c_lflag |= ECHO | ICANON;
        ASSERT(tcsetattr(STDIN_FILENO, TCSANOW, &ta) >= 0);
    }
    else
    {
        termios ta;
        ASSERT(tcgetattr(STDIN_FILENO, &ta) >= 0);
        ta.c_lflag &= ~(ECHO | ICANON);
        ta.c_cc[VTIME] = 0;
        ta.c_cc[VMIN] = 1;
        ASSERT(tcsetattr(STDIN_FILENO, TCSANOW, &ta) >= 0);
    }
#endif
}

void Console::write(const String& str)
{
    write(str.str(), str.length());
}

void Console::write(const char_t* chars, int len)
{
    ASSERT(chars);
    int l = len < 0 ? strLen(chars) : len;

#ifdef PLATFORM_WINDOWS
    DWORD written;

    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    ASSERT(handle);

    ASSERT(WriteConsole(handle, chars, l, &written, NULL));
#else
    ASSERT(::write(STDOUT_FILENO, chars, l) >= 0);
#endif
}

void Console::write(unichar_t ch, int n)
{
    ASSERT(n >= 0);

    int l = UTF_CHAR_LENGTH(ch) * n;
    char_t* chars = ALLOCATE_STACK(char_t, l);
    strSet(chars, ch, n);
    write(chars, l);
}

void Console::writeLine(const String& str)
{
    write(str);
    writeLine();
}

void Console::writeLine(const char_t* chars, int len)
{
    write(chars, len);
    writeLine();
}

void Console::writeLine(unichar_t ch, int n)
{
    write(ch, n);
    writeLine();
}

void Console::writeLine()
{
    write(STR("\n"), 1);
}

void Console::write(int line, int column, const String& str)
{
    write(line, column, str.str(), str.length());
}

void Console::write(int line, int column, const char_t* chars, int len)
{
    ASSERT(line > 0);
    ASSERT(column > 0);
    ASSERT(chars);

    int l = len < 0 ? strLen(chars) : len;

#ifdef PLATFORM_WINDOWS
    DWORD written;
    COORD pos;
    pos.X = column - 1;
    pos.Y = line - 1;
    
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    ASSERT(handle);

    ASSERT(WriteConsoleOutputCharacter(handle, 
        reinterpret_cast<const wchar_t*>(chars), l, pos, &written));
#else
    setCursorPosition(line, column);
    ASSERT(::write(STDOUT_FILENO, chars, l) >= 0);
#endif
}

void Console::write(int line, int column, unichar_t ch, int n)
{
    ASSERT(n >= 0);

    int l = UTF_CHAR_LENGTH(ch) * n;
    char_t* chars = ALLOCATE_STACK(char_t, l);
    strSet(chars, ch, n);
    write(line, column, chars, l);
}

void Console::writeFormatted(const char_t* format, ...)
{
    ASSERT(format);

    va_list args;
    va_start(args, format);
    printArgs(format, args);
    va_end(args);
}

void Console::writeFormatted(const char_t* format, va_list args)
{
    ASSERT(format);
    printArgs(format, args);
}

void Console::writeLineFormatted(const char_t* format, ...)
{
    ASSERT(format);

    va_list args;
    va_start(args, format);
    printArgs(format, args);
    va_end(args);
    write('\n');
}

void Console::writeLineFormatted(const char_t* format, va_list args)
{
    ASSERT(format);
    printArgs(format, args);
    write('\n');
}

char32_t Console::readChar()
{
#ifdef PLATFORM_WINDOWS
    DWORD written;

    HANDLE handle = GetStdHandle(STD_INPUT_HANDLE);
    ASSERT(handle);

    wchar_t ch;
    ASSERT(ReadConsole(handle, &ch, 1, &written, NULL));
    
    if (ch == '\r')
        ASSERT(ReadConsole(handle, &ch, 1, &written, NULL));
    
    return ch;
#else
    int ch = getchar();
    if (ch == EOF)
        return 0;

    uint8_t ch1 = ch;

    if (ch1 < 0x80)
    {
        return ch1;
    }
    else if (ch1 < 0xe0)
    {
        uint8_t ch2 = getchar();
        return ((ch1 & 0x1f) << 6) | (ch2 & 0x3f);
    }
    else if (ch1 < 0xf0)
    {
        uint8_t ch2 = getchar();
        uint8_t ch3 = getchar();
        return ((ch1 & 0x0f) << 12) | 
            ((ch2 & 0x3f) << 6) | (ch3 & 0x3f);
    }
    else
    {
        uint8_t ch2 = getchar();
        uint8_t ch3 = getchar();
        uint8_t ch4 = getchar();
        return ((ch1 & 0x07) << 18) | 
            ((ch2 & 0x3f) << 12) | ((ch3 & 0x3f) << 6) | (ch4 & 0x3f);
    }
#endif
}

String Console::readLine()
{
    String line;
    unichar_t ch = readChar();

    while (ch && ch != '\n')
    {
        line += ch;
        ch = readChar();
    }

    return line;
}

#ifdef PLATFORM_WINDOWS

void Console::getSize(int& width, int& height)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    ASSERT(handle);

    ASSERT(GetConsoleScreenBufferInfo(handle, &csbi));

    width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
}

void Console::clear()
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    ASSERT(handle);

    ASSERT(GetConsoleScreenBufferInfo(handle, &csbi));
    
    COORD pos = { 0, 0 };
    DWORD size = csbi.dwSize.X * csbi.dwSize.Y, written;
    
    ASSERT(FillConsoleOutputCharacter(handle, ' ', size, pos, &written));
    ASSERT(FillConsoleOutputAttribute(handle, csbi.wAttributes, size, pos, &written));
    ASSERT(SetConsoleCursorPosition(handle, pos));
}

void Console::showCursor(bool show)
{
    CONSOLE_CURSOR_INFO cci;

    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    ASSERT(handle);

    ASSERT(GetConsoleCursorInfo(handle, &cci));
    cci.bVisible = show;
    ASSERT(SetConsoleCursorInfo(handle, &cci));
}

void Console::setCursorPosition(int line, int column)
{
    ASSERT(line > 0);
    ASSERT(column > 0);

    COORD pos;
    pos.X = column - 1;
    pos.Y = line - 1;

    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    ASSERT(handle);

    ASSERT(SetConsoleCursorPosition(handle, pos));
}

#else

void Console::getSize(int& width, int& height)
{
    struct winsize ws;
    ASSERT(ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) >= 0);
    width = ws.ws_col;
    height = ws.ws_row;
}

void Console::clear()
{
    printf("\x1b[2J\x1b[1;1H");
}

void Console::showCursor(bool show)
{
    printf(show ? "\x1b[?25h" : "\x1b[?25l");
}

void Console::setCursorPosition(int line, int column)
{
    ASSERT(line > 0);
    ASSERT(column > 0);
    printf("\x1b[%d;%dH", line, column);
}

#endif

const Array<Key>& Console::readKeys()
{
    _keys.clear();

#ifdef PLATFORM_WINDOWS

    HANDLE handle = GetStdHandle(STD_INPUT_HANDLE);
    ASSERT(handle);
    
    if (WaitForSingleObject(handle, INFINITE) == WAIT_OBJECT_0)
    {
        DWORD numInputRec = 0;
        ASSERT(GetNumberOfConsoleInputEvents(handle, &numInputRec));
        
        _input.resize(numInputRec);
        ASSERT(ReadConsoleInput(handle, _input.values(), numInputRec, &numInputRec));

        for (DWORD i = 0; i < numInputRec; ++i)
        {
            INPUT_RECORD& inputRec = _input[i];

            if (inputRec.EventType == KEY_EVENT && inputRec.Event.KeyEvent.bKeyDown)
            {
                Key key;

                switch (inputRec.Event.KeyEvent.wVirtualKeyCode)
                {
                case VK_ESCAPE:
                    key.code = KEY_ESC;
                    break;
                case VK_TAB:
                    key.code = KEY_TAB;
                    key.ch = '\t';
                    break;
                case VK_BACK:
                    key.code = KEY_BACKSPACE;
                    break;
                case VK_RETURN:
                    key.code = KEY_ENTER;
                    key.ch = '\n';
                    break;
                case VK_UP:
                    key.code = KEY_UP;
                    break;
                case VK_DOWN:
                    key.code = KEY_DOWN;
                    break;
                case VK_LEFT:
                    key.code = KEY_LEFT;
                    break;
                case VK_RIGHT:
                    key.code = KEY_RIGHT;
                    break;
                case VK_INSERT:
                    key.code = KEY_INSERT;
                    break;
                case VK_DELETE:
                    key.code = KEY_DELETE;
                    break;
                case VK_HOME:
                    key.code = KEY_HOME;
                    break;
                case VK_END:
                    key.code = KEY_END;
                    break;
                case VK_PRIOR:
                    key.code = KEY_PGUP;
                    break;
                case VK_NEXT:
                    key.code = KEY_PGDN;
                    break;
                case VK_F1:
                    key.code = KEY_F1;
                    break;
                case VK_F2:
                    key.code = KEY_F2;
                    break;
                case VK_F3:
                    key.code = KEY_F3;
                    break;
                case VK_F4:
                    key.code = KEY_F4;
                    break;
                case VK_F5:
                    key.code = KEY_F5;
                    break;
                case VK_F6:
                    key.code = KEY_F6;
                    break;
                case VK_F7:
                    key.code = KEY_F7;
                    break;
                case VK_F8:
                    key.code = KEY_F8;
                    break;
                case VK_F9:
                    key.code = KEY_F9;
                    break;
                case VK_F10:
                    key.code = KEY_F10;
                    break;
                case VK_F11:
                    key.code = KEY_F11;
                    break;
                case VK_F12:
                    key.code = KEY_F12;
                    break;
                default:
                    key.code = KEY_NONE;
                    key.ch = inputRec.Event.KeyEvent.uChar.UnicodeChar;
                    break;
                }

                DWORD controlKeys = inputRec.Event.KeyEvent.dwControlKeyState;
                key.ctrl = (controlKeys & (LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED)) != 0;
                key.alt = (controlKeys & (LEFT_ALT_PRESSED | RIGHT_ALT_PRESSED)) != 0;
                key.shift = (controlKeys & SHIFT_PRESSED) != 0;

                if (!(key.code == KEY_NONE && key.ch == 0))
                    _keys.pushBack(key);
            }
        }
    }
    
    return _keys;

#else

    pollfd pfd;
    pfd.fd = STDIN_FILENO;
    pfd.events = POLLIN;
    pfd.revents = 0;

    if (poll(&pfd, 1, -1) > 0)
    {
        int len;
        ASSERT(ioctl(STDIN_FILENO, FIONREAD, &len) >= 0);

        _input.resize(len + 1);
        len = read(STDIN_FILENO, _input.values(), len);

        if (len > 0)
        {
            _input[len] = 0;
            const char* p = _input.values();

            while (*p)
            {
                Key key;

                if (*p == 0x1b)
                {
                    ++p;

                    if (*p == 0x1b)
                    {
                        ++p;
                        key.alt = true;

                        if (*p == 0x5b)
                        {
                            p = readSpecialKey(p, key);
                        }
                        else if (*p == 0x4f)
                        {
                            key.ctrl = true;
                            p = readSpecialKey(p, key);
                        }
                        else
                            continue;
                    }
                    else if (*p == 0x5b)
                    {
                        p = readSpecialKey(p, key);
                    }
                    else if (*p == 0x4f)
                    {
                        key.ctrl = true;
                        p = readSpecialKey(p, key);
                    }
                    else if (*p)
                    {
                        key.alt = true;
                        p = readRegularKey(p, key);
                    }
                    else
                        key.code = KEY_ESC;
                }
                else
                    p = readRegularKey(p, key);

                _keys.pushBack(key);
            }
        }
    }

#endif

    return _keys;
}

#ifdef PLATFORM_UNIX

const char* Console::readRegularKey(const char* p, Key& key)
{
    ASSERT(p);

    unichar_t ch;
    p += UTF_CHAR_TO_UNICODE(p, ch);
    key.ch = ch;

    if (ch == '\t')
        key.code = KEY_TAB;
    else if (ch == '\n')
        key.code = KEY_ENTER;
    else if (ch == 0x7f)
        key.code = KEY_BACKSPACE;
    else if (iswcntrl(ch))
        key.ctrl = true;
    else
        key.shift = charIsUpper(ch);

    return p;
}

const char* Console::readSpecialKey(const char* p, Key& key)
{
    ASSERT(p);

    ++p;
    bool read7e = true;

    if (*p == 0x31)
    {
        ++p;

        if (*p == 0x31)
            key.code = KEY_F1;
        else if (*p == 0x32)
            key.code = KEY_F2;
        else if (*p == 0x33)
            key.code = KEY_F3;
        else if (*p == 0x34)
            key.code = KEY_F4;
        else if (*p == 0x35)
            key.code = KEY_F5;
        else if (*p == 0x37)
            key.code = KEY_F6;
        else if (*p == 0x38)
            key.code = KEY_F7;
        else if (*p == 0x39)
            key.code = KEY_F8;
        else if (*p == 0x3b)
        {
            ++p;
            read7e = false;

            if (*p == 0x33)
            {
                ++p;

                if (*p == 0x46)
                {
                    key.alt = true;
                    key.code = KEY_END;
                }
                else if (*p == 0x48)
                {
                    key.alt = true;
                    key.code = KEY_HOME;
                }
                else
                    return p;
            }
            else if (*p == 0x35)
            {
                ++p;

                if (*p == 0x43)
                {
                    key.ctrl = true;
                    key.code = KEY_RIGHT;
                }
                else if (*p == 0x44)
                {
                    key.ctrl = true;
                    key.code = KEY_LEFT;
                }
                else
                    return p;
            }
            else
                return p;
        }
        else if (*p == 0x7e)
        {
            read7e = false;
            key.code = KEY_HOME;
        }
        else
            return p;
    }
    else if (*p == 0x32)
    {
        ++p;

        if (*p == 0x30)
            key.code = KEY_F9;
        else if (*p == 0x31)
            key.code = KEY_F10;
        else if (*p == 0x33)
            key.code = KEY_F11;
        else if (*p == 0x34)
            key.code = KEY_F12;
        else if (*p == 0x7e)
        {
            read7e = false;
            key.code = KEY_INSERT;
        }
        else
            return p;
    }
    else if (*p == 0x33)
    {
        if (*(p + 1) == 0x3b)
        {
            p += 2;
            if (*p == 0x33)
            {
                key.alt = true;
                key.code = KEY_DELETE;
            }
            else
                return p;
        }
        else
            key.code = KEY_DELETE;
    }
    else if (*p == 0x34)
    {
        key.code = KEY_END;
    }
    else if (*p == 0x35)
    {
        if (*(p + 1) == 0x3b)
        {
            p += 2;
            if (*p == 0x33)
            {
                key.alt = true;
                key.code = KEY_PGUP;
            }
            else
                return p;
        }
        else
            key.code = KEY_PGUP;
    }
    else if (*p == 0x36)
    {
        if (*(p + 1) == 0x3b)
        {
            p += 2;
            if (*p == 0x33)
            {
                key.alt = true;
                key.code = KEY_PGDN;
            }
            else
                return p;
        }
        else
            key.code = KEY_PGDN;
    }
    else if (*p == 0x41)
    {
        read7e = false;
        key.code = KEY_UP;
    }
    else if (*p == 0x42)
    {
        read7e = false;
        key.code = KEY_DOWN;
    }
    else if (*p == 0x43)
    {
        read7e = false;
        key.code = KEY_RIGHT;
    }
    else if (*p == 0x44)
    {
        read7e = false;
        key.code = KEY_LEFT;
    }
    else if (*p == 0x46)
    {
        read7e = false;
        key.code = KEY_END;
    }
    else if (*p == 0x48)
    {
        read7e = false;
        key.code = KEY_HOME;
    }
    else if (*p == 0x5b)
    {
        ++p;
        read7e = false;

        if (*p == 0x41)
            key.code = KEY_F1;
        else if (*p == 0x42)
            key.code = KEY_F2;
        else if (*p == 0x43)
            key.code = KEY_F3;
        else if (*p == 0x44)
            key.code = KEY_F4;
        else if (*p == 0x45)
            key.code = KEY_F5;
        else
            return p;
    }
    else
        return p;

    ++p;
    if (read7e)
    {
        if (*p == 0x7e)
            ++p;
        else
            throw Exception(STR("0x7e expected in input stream"));
    }

    return p;
}

#endif

