#include <console.h>

#ifdef CHAR_ENCODING_UTF8

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

#else

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

#endif

#ifdef PLATFORM_UNIX

struct KeyMapping
{
    int len;
    const char* chars;
    Key key;
};

static KeyMapping keyMapping[] =
    {
        { 7, "\x1b[15;3~", Key(KEY_F5, false, true) },
        { 7, "\x1b[17;3~", Key(KEY_F6, false, true) },
        { 7, "\x1b[18;3~", Key(KEY_F7, false, true) },
        { 7, "\x1b[19;3~", Key(KEY_F8, false, true) },
        { 7, "\x1b[20;3~", Key(KEY_F9, false, true) },
        { 7, "\x1b[21;3~", Key(KEY_F10, false, true) },
        { 7, "\x1b[23;3~", Key(KEY_F11, false, true) },
        { 7, "\x1b[24;3~", Key(KEY_F12, false, true) },
        { 7, "\x1b[15;5~", Key(KEY_F5, true) },
        { 7, "\x1b[17;5~", Key(KEY_F6, true) },
        { 7, "\x1b[18;5~", Key(KEY_F7, true) },
        { 7, "\x1b[19;5~", Key(KEY_F8, true) },
        { 7, "\x1b[20;5~", Key(KEY_F9, true) },
        { 7, "\x1b[21;5~", Key(KEY_F10, true) },
        { 7, "\x1b[23;5~", Key(KEY_F11, true) },
        { 7, "\x1b[24;5~", Key(KEY_F12, true) },
        { 6, "\x1b\x1b[11~", Key(KEY_F1, false, true) },
        { 6, "\x1b\x1b[12~", Key(KEY_F2, false, true) },
        { 6, "\x1b\x1b[13~", Key(KEY_F3, false, true) },
        { 6, "\x1b\x1b[14~", Key(KEY_F4, false, true) },
        { 6, "\x1b\x1b[15~", Key(KEY_F5, false, true) },
        { 6, "\x1b\x1b[17~", Key(KEY_F6, false, true) },
        { 6, "\x1b\x1b[18~", Key(KEY_F7, false, true) },
        { 6, "\x1b\x1b[19~", Key(KEY_F8, false, true) },
        { 6, "\x1b\x1b[20~", Key(KEY_F9, false, true) },
        { 6, "\x1b\x1b[21~", Key(KEY_F10, false, true) },
        { 6, "\x1b\x1b[23~", Key(KEY_F11, false, true) },
        { 6, "\x1b\x1b[24~", Key(KEY_F12, false, true) },
        { 5, "\x1b[11~", Key(KEY_F1) },
        { 5, "\x1b[12~", Key(KEY_F2) },
        { 5, "\x1b[13~", Key(KEY_F3) },
        { 5, "\x1b[14~", Key(KEY_F4) },
        { 5, "\x1b[15~", Key(KEY_F5) },
        { 5, "\x1b[17~", Key(KEY_F6) },
        { 5, "\x1b[18~", Key(KEY_F7) },
        { 5, "\x1b[19~", Key(KEY_F8) },
        { 5, "\x1b[20~", Key(KEY_F9) },
        { 5, "\x1b[21~", Key(KEY_F10) },
        { 5, "\x1b[23~", Key(KEY_F11) },
        { 5, "\x1b[24~", Key(KEY_F12) },
        { 5, "\x1b[23~", Key(KEY_F1, false, false, true) },
        { 5, "\x1b[24~", Key(KEY_F2, false, false, true) },
        { 5, "\x1b[25~", Key(KEY_F3, false, false, true) },
        { 5, "\x1b[26~", Key(KEY_F4, false, false, true) },
        { 5, "\x1b[28~", Key(KEY_F5, false, false, true) },
        { 5, "\x1b[29~", Key(KEY_F6, false, false, true) },
        { 5, "\x1b[31~", Key(KEY_F7, false, false, true) },
        { 5, "\x1b[32~", Key(KEY_F8, false, false, true) },
        { 5, "\x1b[33~", Key(KEY_F9, false, false, true) },
        { 5, "\x1b[34~", Key(KEY_F10, false, false, true) },
        { 5, "\x1b[23~", Key(KEY_F11, false, false, true) },
        { 5, "\x1b[24~", Key(KEY_F12, false, false, true) },
        { 5, "\x1b\x1b[2~", Key(KEY_INSERT, false, true) },
        { 5, "\x1b\x1b[3~", Key(KEY_DELETE, false, true) },
        { 5, "\x1b\x1b[1~", Key(KEY_HOME, false, true) },
        { 5, "\x1b\x1b[4~", Key(KEY_END, false, true) },
        { 5, "\x1b\x1b[5~", Key(KEY_PGUP, false, true) },
        { 5, "\x1b\x1b[6~", Key(KEY_PGDN, false, true) },
        { 4, "\x1b[2~", Key(KEY_INSERT) },
        { 4, "\x1b[3~", Key(KEY_DELETE) },
        { 4, "\x1b[1~", Key(KEY_HOME) },
        { 4, "\x1b[4~", Key(KEY_END) },
        { 4, "\x1b[5~", Key(KEY_PGUP) },
        { 4, "\x1b[6~", Key(KEY_PGDN) },
        { 4, "\x1b\x1b[A", Key(KEY_UP, false, true) },
        { 4, "\x1b\x1b[B", Key(KEY_DOWN, false, true) },
        { 4, "\x1b\x1b[C", Key(KEY_RIGHT, false, true) },
        { 4, "\x1b\x1b[D", Key(KEY_LEFT, false, true) },
        { 4, "\x1b\x1bOA", Key(KEY_UP, true, true) },
        { 4, "\x1b\x1bOB", Key(KEY_DOWN, true, true) },
        { 4, "\x1b\x1bOC", Key(KEY_RIGHT, true, true) },
        { 4, "\x1b\x1bOD", Key(KEY_LEFT, true, true) },
        { 4, "\x1bO3P", Key(KEY_F1, false, true) },
        { 4, "\x1bO3Q", Key(KEY_F2, false, true) },
        { 4, "\x1bO3R", Key(KEY_F3, false, true) },
        { 4, "\x1bO3S", Key(KEY_F4, false, true) },
        { 4, "\x1bO5P", Key(KEY_F1, true) },
        { 4, "\x1bO5Q", Key(KEY_F2, true) },
        { 4, "\x1bO5R", Key(KEY_F3, true) },
        { 4, "\x1bO5S", Key(KEY_F4, true) },
        { 3, "\x1b[A", Key(KEY_UP) },
        { 3, "\x1b[B", Key(KEY_DOWN) },
        { 3, "\x1b[C", Key(KEY_RIGHT) },
        { 3, "\x1b[D", Key(KEY_LEFT) },
        { 3, "\x1bOA", Key(KEY_UP, true) },
        { 3, "\x1bOB", Key(KEY_DOWN, true) },
        { 3, "\x1bOC", Key(KEY_RIGHT, true) },
        { 3, "\x1bOD", Key(KEY_LEFT, true) },
        { 3, "\x1b[Z", Key(KEY_TAB, false, false, true) },
        { 3, "\x1bOP", Key(KEY_F1) },
        { 3, "\x1bOQ", Key(KEY_F2) },
        { 3, "\x1bOR", Key(KEY_F3) },
        { 3, "\x1bOS", Key(KEY_F4) }
    };

#endif

// Console

#ifdef PLATFORM_WINDOWS
Array<INPUT_RECORD> Console::_input(16);
#else
Array<char> Console::_input(16);
#endif

Array<Key> Console::_keys;

void Console::initialize()
{
    setlocale(LC_ALL, "");

#ifdef PLATFORM_WINDOWS

#ifdef CHAR_ENCODING_UTF16
    int rc = _setmode(_fileno(stdin), _O_U16TEXT);
    ASSERT(rc >= 0);
    rc = _setmode(_fileno(stdout), _O_U16TEXT);
    ASSERT(rc >= 0);
#endif

#else
    int rc = setvbuf(stdout, NULL, _IONBF, 0);
    ASSERT(rc == 0);
#endif
}

void Console::setLineMode(bool lineMode)
{
#ifndef PLATFORM_WINDOWS
    if (lineMode)
    {
        termios ta;

        int rc = tcgetattr(STDIN_FILENO, &ta);
        ASSERT(rc == 0);

        ta.c_lflag |= ECHO | ICANON;
        rc = tcsetattr(STDIN_FILENO, TCSANOW, &ta);
        ASSERT(rc == 0);
    }
    else
    {
        termios ta;

        int rc = tcgetattr(STDIN_FILENO, &ta);
        ASSERT(rc == 0);

        ta.c_lflag &= ~(ECHO | ICANON);
        ta.c_cc[VTIME] = 0;
        ta.c_cc[VMIN] = 0;

        rc = tcsetattr(STDIN_FILENO, TCSANOW, &ta);
        ASSERT(rc == 0);
    }
#endif
}

void Console::write(const String& str)
{
    write(str.chars(), str.length());
}

void Console::write(const char_t* chars, int len)
{
    ASSERT(chars);
    int l = len < 0 ? strLen(chars) : len;

#ifdef PLATFORM_WINDOWS
    DWORD written;

    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    ASSERT(handle);

    BOOL rc = WriteConsole(handle, chars, l, &written, NULL);
    ASSERT(rc);
#else
    int written = ::write(STDOUT_FILENO, chars, l);
    ASSERT(written >= 0);
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
    write(line, column, str.chars(), str.length());
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

    BOOL rc = WriteConsoleOutputCharacter(handle,
        reinterpret_cast<LPCTSTR>(chars), l, pos, &written);
    ASSERT(rc);
#else
    setCursorPosition(line, column);

    int written = ::write(STDOUT_FILENO, chars, l);
    ASSERT(written >= 0);
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
    BOOL rc = ReadConsole(handle, &ch, 1, &written, NULL);
    ASSERT(rc);

    if (ch == '\r')
    {
        rc = ReadConsole(handle, &ch, 1, &written, NULL);
        ASSERT(rc);
    }

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

    BOOL rc = GetConsoleScreenBufferInfo(handle, &csbi);
    ASSERT(rc);

    width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
}

void Console::clear()
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    ASSERT(handle);

    BOOL rc = GetConsoleScreenBufferInfo(handle, &csbi);
    ASSERT(rc);

    COORD pos = { 0, 0 };
    DWORD size = csbi.dwSize.X * csbi.dwSize.Y, written;

    rc = FillConsoleOutputCharacter(handle, ' ', size, pos, &written);
    ASSERT(rc);

    rc = FillConsoleOutputAttribute(handle, csbi.wAttributes, size, pos, &written);
    ASSERT(rc);

    rc = SetConsoleCursorPosition(handle, pos);
    ASSERT(rc);
}

void Console::showCursor(bool show)
{
    CONSOLE_CURSOR_INFO cci;

    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    ASSERT(handle);

    BOOL rc = GetConsoleCursorInfo(handle, &cci);
    ASSERT(rc);

    cci.bVisible = show;
    rc = SetConsoleCursorInfo(handle, &cci);
    ASSERT(rc);
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

    BOOL rc = SetConsoleCursorPosition(handle, pos);
    ASSERT(rc);
}

#else

void Console::getSize(int& width, int& height)
{
    struct winsize ws;
    int rc = ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
    ASSERT(rc >= 0);

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
        BOOL rc = GetNumberOfConsoleInputEvents(handle, &numInputRec);
        ASSERT(rc);

        _input.resize(numInputRec);
        rc = ReadConsoleInput(handle, _input.values(), numInputRec, &numInputRec);
        ASSERT(rc);

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

#else

    bool gotChars = false;
    _input.clear();

    while (true)
    {
        char chars[16];

        int len = read(STDIN_FILENO, chars, sizeof(chars));

        if (len > 0)
        {
            gotChars = true;
            for (int i = 0; i < len; ++i)
                _input.pushBack(chars[i]);
        }
        else
        {
            if (gotChars)
                break;
            else
                usleep(10000);
        }
    }

    const char* p = _input.values();
    const char* e = p + _input.size();
    _input.pushBack(0);

    while (p < e)
    {
        int n = sizeof(keyMapping) / sizeof(KeyMapping);
        bool found = false;

        for (int i = 0; i < n; ++i)
        {
            if (!strncmp(p, keyMapping[i].chars, keyMapping[i].len))
            {
                _keys.pushBack(keyMapping[i].key);
                p += keyMapping[i].len;
                found = true;
                break;
            }
        }

        if (!found)
        {
            Key key;

            if (*p == 0x1b)
            {
                ++p;

                if (*p)
                    key.alt = true;
                else
                {
                    _keys.pushBack(Key(KEY_ESC));
                    continue;
                }
            }

            unichar_t ch;
            p += UTF_CHAR_TO_UNICODE(p, ch);
            key.ch = ch;
            key.shift = charIsUpper(ch);

            if (ch == '\t')
                key.code = KEY_TAB;
            else if (ch == '\n')
                key.code = KEY_ENTER;
            else if (ch == 0x7f)
                key.code = KEY_BACKSPACE;
            else if (iswcntrl(ch))
                key.ctrl = true;

            _keys.pushBack(key);
        }
    }

#endif

    return _keys;
}
