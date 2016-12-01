#include <console.h>

// Console

#ifdef PLATFORM_WINDOWS
Array<INPUT_RECORD> Console::_input(10);
#else
CharArray Console::_input(10);
#endif

Array<Key> Console::_keys;

void Console::setMode(int mode)
{
#ifdef PLATFORM_WINDOWS

    if (mode & CONSOLE_MODE_UNICODE)
        _setmode(_fileno(stdout), _O_U16TEXT);
    else
        _setmode(_fileno(stdout), _O_U8TEXT);

#else

    if (mode & CONSOLE_MODE_LINE_INPUT)
    {
        termios ta;
        tcgetattr(STDIN_FILENO, &ta);
        ta.c_lflag |= ECHO | ICANON;
        tcsetattr(STDIN_FILENO, TCSANOW, &ta);    
    }
    else
    {
        termios ta;
        tcgetattr(STDIN_FILENO, &ta);
        ta.c_lflag &= ~(ECHO | ICANON);
        ta.c_cc[VTIME] = 0;
        ta.c_cc[VMIN] = 1;
        tcsetattr(STDIN_FILENO, TCSANOW, &ta);    
    }

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

void Console::write(int line, int column, const char_t* chars, int len)
{
#ifdef PLATFORM_WINDOWS
    DWORD written;
    COORD pos;
    pos.X = column - 1;
    pos.Y = line - 1;
    
    WriteConsoleOutputCharacter(GetStdHandle(STD_OUTPUT_HANDLE),  
        chars, len, pos, &written);
#else
    setCursorPosition(line, column);
    ::write(STDOUT_FILENO, chars, len);
#endif
}

String Console::readLine()
{
    String line;
    int ch = GETCHAR();

    while (!(ch == '\n' || ch == CHAR_EOF))
    {
        line += ch;
        ch = GETCHAR();
    }

    return line;
}

#ifdef PLATFORM_WINDOWS

void Console::getSize(int& width, int& height)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
}

void Console::clear()
{
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    
    COORD pos = { 0, 0 };
    DWORD size = csbi.dwSize.X * csbi.dwSize.Y, written;
    
    FillConsoleOutputCharacter(handle, ' ', size, pos, &written);
    FillConsoleOutputAttribute(handle, csbi.wAttributes, size, pos, &written);
    SetConsoleCursorPosition(handle, pos);
}

void Console::showCursor(bool show)
{
    CONSOLE_CURSOR_INFO cci;
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);

    GetConsoleCursorInfo(handle, &cci);
    cci.bVisible = show;
    SetConsoleCursorInfo(handle, &cci);
}

void Console::setCursorPosition(int line, int column)
{
    COORD pos;
    pos.X = column - 1;
    pos.Y = line - 1;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

#else

void Console::getSize(int& width, int& height)
{
    struct winsize ws;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
    width = ws.ws_col;
    height = ws.ws_row;
}

void Console::clear()
{
    ::write(STDOUT_FILENO, "\x1b[2J", 4);
    setCursorPosition(1, 1);
}

void Console::showCursor(bool show)
{
    ::write(STDOUT_FILENO, show ? "\x1b[?25h" : "\x1b[?25l", 6);
}

void Console::setCursorPosition(int line, int column)
{
    char_t cmd[30];
    sprintf(cmd, "\x1b[%d;%dH", line, column);
    ::write(STDOUT_FILENO, cmd, strlen(cmd));
}

#endif

#ifdef PLATFORM_UNIX

void Console::readRegularKey(const char_t ch, Key& key)
{
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
}

const char_t* Console::readSpecialKey(const char_t* p, Key& key)
{
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
        key.code = KEY_DELETE;
    else if (*p == 0x34)
        key.code = KEY_END;
    else if (*p == 0x35)
        key.code = KEY_PGUP;
    else if (*p == 0x36)
        key.code = KEY_PGDN;
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
    else
        return p;

    ++p;
    if (read7e && *p == 0x7e)
        ++p;

    return p;
}

#endif

const Array<Key>& Console::readKeys()
{
    _keys.clear();

#ifdef PLATFORM_WINDOWS

    HANDLE handle = GetStdHandle(STD_INPUT_HANDLE);
    
    if (WaitForSingleObject(handle, INFINITE) == WAIT_OBJECT_0)
    {
        DWORD numInputRec = 0;
        GetNumberOfConsoleInputEvents(handle, &numInputRec);
        
        _input.resize(numInputRec);
        ReadConsoleInput(handle, _input.values(), numInputRec, &numInputRec);

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
                default:
                    key.code = KEY_NONE;
                    key.ch = inputRec.Event.KeyEvent.uChar.UnicodeChar;
                    break;
                }

                DWORD controlKeys = inputRec.Event.KeyEvent.dwControlKeyState;
                key.ctrl = (controlKeys & (LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED)) != 0;
                key.alt = (controlKeys & (LEFT_ALT_PRESSED | RIGHT_ALT_PRESSED)) != 0;
                key.shift = (controlKeys & SHIFT_PRESSED) != 0;

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
        ioctl(STDIN_FILENO, FIONREAD, &len);

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
                        readRegularKey(*p, key);
                        ++p;
                    }
                    else
                        key.code = KEY_ESC;
                }
                else
                {
                    readRegularKey(*p, key);
                    ++p;
                }

                _keys.pushBack(key);
            }
        }
    }

#endif

    return _keys;
}
