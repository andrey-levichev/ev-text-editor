#include <console.h>

// Console

#ifdef PLATFORM_WINDOWS
Array<INPUT_RECORD> Console::_input(10);
#else
Array<char> Console::_input(10);
#endif

Array<Key> Console::_keys;

void Console::setMode(int mode)
{
    if (mode & CONSOLE_MODE_NOTBUFFERED)
        ASSERT(setvbuf(stdout, NULL, _IONBF, 0) == 0);
    else
        ASSERT(setvbuf(stdout, NULL, _IOFBF, BUFSIZ) == 0);

#ifdef PLATFORM_WINDOWS

    ASSERT(_setmode(_fileno(stdout), 
            mode & CONSOLE_MODE_UNICODE ? _O_U16TEXT : _O_TEXT) >= 0);

#else

    if (mode & CONSOLE_MODE_NONCANONICAL)
    {
        termios ta;
        ASSERT(tcgetattr(STDIN_FILENO, &ta) >= 0);
        ta.c_lflag &= ~(ECHO | ICANON);
        ta.c_cc[VTIME] = 0;
        ta.c_cc[VMIN] = 1;
        ASSERT(tcsetattr(STDIN_FILENO, TCSANOW, &ta) >= 0);
    }
    else
    {
        termios ta;
        ASSERT(tcgetattr(STDIN_FILENO, &ta) >= 0);
        ta.c_lflag |= ECHO | ICANON;
        ASSERT(tcsetattr(STDIN_FILENO, TCSANOW, &ta) >= 0);
    }

#endif
}

void Console::write(const String& str)
{
    write(str.str(), str.length());
}

void Console::write(const char_t* chars)
{
    ASSERT(chars);
    write(chars, strLen(chars));
}

void Console::write(unichar_t ch, int len)
{
    ASSERT(len >= 0);

    int l = UTF_CHAR_LENGTH(ch) * len;
    char_t* chars = ALLOCATE_STACK(char_t, l);
    strSet(chars, ch, len);
    write(chars, l);
}

void Console::writeLine(const String& str)
{
    write(str);
    writeLine();
}

void Console::writeLine(const char_t* chars)
{
    ASSERT(chars);
    write(chars, strLen(chars));
    writeLine();
}

void Console::writeLine(unichar_t ch, int len)
{
    ASSERT(len >= 0);
    write(ch, len);
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

void Console::write(int line, int column, const char_t* chars)
{
    ASSERT(chars);
    write(line, column, chars, strLen(chars));
}

void Console::write(int line, int column, unichar_t ch, int len)
{
    ASSERT(len >= 0);

    int l = UTF_CHAR_LENGTH(ch) * len;
    char_t* chars = ALLOCATE_STACK(char_t, l);
    strSet(chars, ch, len);
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
    putChar('\n');
}

void Console::writeLineFormatted(const char_t* format, va_list args)
{
    ASSERT(format);
    printArgs(format, args);
    putChar('\n');
}

String Console::readLine()
{
    String line;
    unichar_t ch = getChar();

    while (ch && ch != '\n')
    {
        line += ch;
        ch = getChar();
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
    ASSERT(::write(STDOUT_FILENO, "\x1b[2J", 4) >= 0);
    setCursorPosition(1, 1);
}

void Console::showCursor(bool show)
{
    ASSERT(::write(STDOUT_FILENO, show ? "\x1b[?25h" : "\x1b[?25l", 6) >= 0);
}

void Console::setCursorPosition(int line, int column)
{
    ASSERT(line > 0);
    ASSERT(column > 0);

    char_t cmd[30];
    ASSERT(sprintf(cmd, "\x1b[%d;%dH", line, column) >= 0);
    ASSERT(::write(STDOUT_FILENO, cmd, strlen(cmd)) >= 0);
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
            const char_t* p = _input.values();

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

void Console::write(const char_t* chars, int len)
{
    ASSERT(chars);
    ASSERT(len >= 0);

#ifdef PLATFORM_WINDOWS
    DWORD written;

    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    ASSERT(handle);

    ASSERT(WriteConsole(handle, chars, len, &written, NULL));
#else
    ASSERT(::write(STDOUT_FILENO, chars, len) >= 0);
#endif
}

void Console::write(int line, int column, const char_t* chars, int len)
{
    ASSERT(line > 0);
    ASSERT(column > 0);
    ASSERT(chars);
    ASSERT(len >= 0);

#ifdef PLATFORM_WINDOWS
    DWORD written;
    COORD pos;
    pos.X = column - 1;
    pos.Y = line - 1;
    
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    ASSERT(handle);

    ASSERT(WriteConsoleOutputCharacter(handle, 
        reinterpret_cast<const wchar_t*>(chars), len, pos, &written));
#else
    setCursorPosition(line, column);
    ASSERT(::write(STDOUT_FILENO, chars, len) >= 0);
#endif
}

#ifdef PLATFORM_UNIX

const char_t* Console::readRegularKey(const char_t* p, Key& key)
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
    else if (iscntrl(ch))
        key.ctrl = true;
    else
        key.shift = isupper(ch);

    return p;
}

const char_t* Console::readSpecialKey(const char_t* p, Key& key)
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
        key.code = KEY_END;
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
    else
        return p;

    ++p;
    if (read7e && *p == 0x7e)
        ++p;

    return p;
}

#endif

