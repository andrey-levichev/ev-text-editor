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

static const char* controlKeys = "@abcdefghijklmnopqrstuvwxyz[\\]^_";

#ifdef PLATFORM_UNIX

static volatile bool screenSizeChanged = false;

extern "C" void onSIGWINCH(int sig)
{
    screenSizeChanged = true;
}

struct KeyMapping
{
    int len;
    const char* chars;
    KeyEvent keyEvent;
};

static KeyMapping keyMapping[] =
    {
        { 7, "\x1b[15;5~", KeyEvent(KEY_F5, true) },
        { 7, "\x1b[17;5~", KeyEvent(KEY_F6, true) },
        { 7, "\x1b[18;5~", KeyEvent(KEY_F7, true) },
        { 7, "\x1b[19;5~", KeyEvent(KEY_F8, true) },
        { 7, "\x1b[20;5~", KeyEvent(KEY_F9, true) },
        { 7, "\x1b[21;5~", KeyEvent(KEY_F10, true) },
        { 7, "\x1b[23;5~", KeyEvent(KEY_F11, true) },
        { 7, "\x1b[24;5~", KeyEvent(KEY_F12, true) },
        { 7, "\x1b[15;3~", KeyEvent(KEY_F5, false, true) },
        { 7, "\x1b[17;3~", KeyEvent(KEY_F6, false, true) },
        { 7, "\x1b[18;3~", KeyEvent(KEY_F7, false, true) },
        { 7, "\x1b[19;3~", KeyEvent(KEY_F8, false, true) },
        { 7, "\x1b[20;3~", KeyEvent(KEY_F9, false, true) },
        { 7, "\x1b[21;3~", KeyEvent(KEY_F10, false, true) },
        { 7, "\x1b[23;3~", KeyEvent(KEY_F11, false, true) },
        { 7, "\x1b[24;3~", KeyEvent(KEY_F12, false, true) },
        { 7, "\x1b[15;7~", KeyEvent(KEY_F5, true, true) },
        { 7, "\x1b[17;7~", KeyEvent(KEY_F6, true, true) },
        { 7, "\x1b[18;7~", KeyEvent(KEY_F7, true, true) },
        { 7, "\x1b[19;7~", KeyEvent(KEY_F8, true, true) },
        { 7, "\x1b[20;7~", KeyEvent(KEY_F9, true, true) },
        { 7, "\x1b[21;7~", KeyEvent(KEY_F10, true, true) },
        { 7, "\x1b[23;7~", KeyEvent(KEY_F11, true, true) },
        { 7, "\x1b[24;7~", KeyEvent(KEY_F12, true, true) },
        { 7, "\x1b[15;2~", KeyEvent(KEY_F5, false, false, true) },
        { 7, "\x1b[17;2~", KeyEvent(KEY_F6, false, false, true) },
        { 7, "\x1b[18;2~", KeyEvent(KEY_F7, false, false, true) },
        { 7, "\x1b[19;2~", KeyEvent(KEY_F8, false, false, true) },
        { 7, "\x1b[20;2~", KeyEvent(KEY_F9, false, false, true) },
        { 7, "\x1b[21;2~", KeyEvent(KEY_F10, false, false, true) },
        { 7, "\x1b[23;2~", KeyEvent(KEY_F11, false, false, true) },
        { 7, "\x1b[24;2~", KeyEvent(KEY_F12, false, false, true) },
        { 7, "\x1b[15;6~", KeyEvent(KEY_F5, true, false, true) },
        { 7, "\x1b[17;6~", KeyEvent(KEY_F6, true, false, true) },
        { 7, "\x1b[18;6~", KeyEvent(KEY_F7, true, false, true) },
        { 7, "\x1b[19;6~", KeyEvent(KEY_F8, true, false, true) },
        { 7, "\x1b[20;6~", KeyEvent(KEY_F9, true, false, true) },
        { 7, "\x1b[21;6~", KeyEvent(KEY_F10, true, false, true) },
        { 7, "\x1b[23;6~", KeyEvent(KEY_F11, true, false, true) },
        { 7, "\x1b[24;6~", KeyEvent(KEY_F12, true, false, true) },
        { 7, "\x1b[15;4~", KeyEvent(KEY_F5, false, true, true) },
        { 7, "\x1b[17;4~", KeyEvent(KEY_F6, false, true, true) },
        { 7, "\x1b[18;4~", KeyEvent(KEY_F7, false, true, true) },
        { 7, "\x1b[19;4~", KeyEvent(KEY_F8, false, true, true) },
        { 7, "\x1b[20;4~", KeyEvent(KEY_F9, false, true, true) },
        { 7, "\x1b[21;4~", KeyEvent(KEY_F10, false, true, true) },
        { 7, "\x1b[23;4~", KeyEvent(KEY_F11, false, true, true) },
        { 7, "\x1b[24;4~", KeyEvent(KEY_F12, false, true, true) },
        { 7, "\x1b[15;8~", KeyEvent(KEY_F5, true, true, true) },
        { 7, "\x1b[17;8~", KeyEvent(KEY_F6, true, true, true) },
        { 7, "\x1b[18;8~", KeyEvent(KEY_F7, true, true, true) },
        { 7, "\x1b[19;8~", KeyEvent(KEY_F8, true, true, true) },
        { 7, "\x1b[20;8~", KeyEvent(KEY_F9, true, true, true) },
        { 7, "\x1b[21;8~", KeyEvent(KEY_F10, true, true, true) },
        { 7, "\x1b[23;8~", KeyEvent(KEY_F11, true, true, true) },
        { 7, "\x1b[24;8~", KeyEvent(KEY_F12, true, true, true) },
        { 6, "\x1b\x1b[11~", KeyEvent(KEY_F1, false, true) },
        { 6, "\x1b\x1b[12~", KeyEvent(KEY_F2, false, true) },
        { 6, "\x1b\x1b[13~", KeyEvent(KEY_F3, false, true) },
        { 6, "\x1b\x1b[14~", KeyEvent(KEY_F4, false, true) },
        { 6, "\x1b\x1b[15~", KeyEvent(KEY_F5, false, true) },
        { 6, "\x1b\x1b[17~", KeyEvent(KEY_F6, false, true) },
        { 6, "\x1b\x1b[18~", KeyEvent(KEY_F7, false, true) },
        { 6, "\x1b\x1b[19~", KeyEvent(KEY_F8, false, true) },
        { 6, "\x1b\x1b[20~", KeyEvent(KEY_F9, false, true) },
        { 6, "\x1b\x1b[21~", KeyEvent(KEY_F10, false, true) },
        { 6, "\x1b\x1b[23~", KeyEvent(KEY_F11, false, true) },
        { 6, "\x1b\x1b[24~", KeyEvent(KEY_F12, false, true) },
        { 6, "\x1b[3;2~", KeyEvent(KEY_BACKSPACE, false, false, true) },
        { 6, "\x1b[3;6~", KeyEvent(KEY_BACKSPACE, true, false, true) },
        { 6, "\x1b[3;4~", KeyEvent(KEY_BACKSPACE, false, true, true) },
        { 6, "\x1b[3;8~", KeyEvent(KEY_BACKSPACE, true, true, true) },
        { 6, "\x1b[1;5A", KeyEvent(KEY_UP, true) },
        { 6, "\x1b[1;5B", KeyEvent(KEY_DOWN, true) },
        { 6, "\x1b[1;5C", KeyEvent(KEY_RIGHT, true) },
        { 6, "\x1b[1;5D", KeyEvent(KEY_LEFT, true) },
        { 6, "\x1b[1;3A", KeyEvent(KEY_UP, false, true) },
        { 6, "\x1b[1;3B", KeyEvent(KEY_DOWN, false, true) },
        { 6, "\x1b[1;3C", KeyEvent(KEY_RIGHT, false, true) },
        { 6, "\x1b[1;3D", KeyEvent(KEY_LEFT, false, true) },
        { 6, "\x1b[1;7A", KeyEvent(KEY_UP, true, true) },
        { 6, "\x1b[1;7B", KeyEvent(KEY_DOWN, true, true) },
        { 6, "\x1b[1;7C", KeyEvent(KEY_RIGHT, true, true) },
        { 6, "\x1b[1;7D", KeyEvent(KEY_LEFT, true, true) },
        { 6, "\x1b[1;2A", KeyEvent(KEY_UP, false, false, true) },
        { 6, "\x1b[1;2B", KeyEvent(KEY_DOWN, false, false, true) },
        { 6, "\x1b[1;2C", KeyEvent(KEY_RIGHT, false, false, true) },
        { 6, "\x1b[1;2D", KeyEvent(KEY_LEFT, false, false, true) },
        { 6, "\x1b[1;6A", KeyEvent(KEY_UP, true, false, true) },
        { 6, "\x1b[1;6B", KeyEvent(KEY_DOWN, true, false, true) },
        { 6, "\x1b[1;6C", KeyEvent(KEY_RIGHT, true, false, true) },
        { 6, "\x1b[1;6D", KeyEvent(KEY_LEFT, true, false, true) },
        { 6, "\x1b[1;4A", KeyEvent(KEY_UP, false, true, true) },
        { 6, "\x1b[1;4B", KeyEvent(KEY_DOWN, false, true, true) },
        { 6, "\x1b[1;4C", KeyEvent(KEY_RIGHT, false, true, true) },
        { 6, "\x1b[1;4D", KeyEvent(KEY_LEFT, false, true, true) },
        { 6, "\x1b[1;8A", KeyEvent(KEY_UP, true, true, true) },
        { 6, "\x1b[1;8B", KeyEvent(KEY_DOWN, true, true, true) },
        { 6, "\x1b[1;8C", KeyEvent(KEY_RIGHT, true, true, true) },
        { 6, "\x1b[1;8D", KeyEvent(KEY_LEFT, true, true, true) },
        { 6, "\x1b[2;5~", KeyEvent(KEY_INSERT, true) },
        { 6, "\x1b[3;5~", KeyEvent(KEY_DELETE, true) },
        { 6, "\x1b[1;5H", KeyEvent(KEY_HOME, true) },
        { 6, "\x1b[1;5F", KeyEvent(KEY_END, true) },
        { 6, "\x1b[5;5~", KeyEvent(KEY_PGUP, true) },
        { 6, "\x1b[6;5~", KeyEvent(KEY_PGDN, true) },
        { 6, "\x1b[2;3~", KeyEvent(KEY_INSERT, false, true) },
        { 6, "\x1b[3;3~", KeyEvent(KEY_DELETE, false, true) },
        { 6, "\x1b[1;3H", KeyEvent(KEY_HOME, false, true) },
        { 6, "\x1b[1;3F", KeyEvent(KEY_END, false, true) },
        { 6, "\x1b[5;3~", KeyEvent(KEY_PGUP, false, true) },
        { 6, "\x1b[6;3~", KeyEvent(KEY_PGDN, false, true) },
        { 6, "\x1b[2;7~", KeyEvent(KEY_INSERT, true, true) },
        { 6, "\x1b[3;7~", KeyEvent(KEY_DELETE, true, true) },
        { 6, "\x1b[1;7H", KeyEvent(KEY_HOME, true, true) },
        { 6, "\x1b[1;7F", KeyEvent(KEY_END, true, true) },
        { 6, "\x1b[5;7~", KeyEvent(KEY_PGUP, true, true) },
        { 6, "\x1b[6;7~", KeyEvent(KEY_PGDN, true, true) },
        { 6, "\x1b[2;2~", KeyEvent(KEY_INSERT, false, false, true) },
        { 6, "\x1b[3;2~", KeyEvent(KEY_DELETE, false, false, true) },
        { 6, "\x1b[1;2H", KeyEvent(KEY_HOME, false, false, true) },
        { 6, "\x1b[1;2F", KeyEvent(KEY_END, false, false, true) },
        { 6, "\x1b[5;2~", KeyEvent(KEY_PGUP, false, false, true) },
        { 6, "\x1b[6;2~", KeyEvent(KEY_PGDN, false, false, true) },
        { 6, "\x1b[2;6~", KeyEvent(KEY_INSERT, true, false, true) },
        { 6, "\x1b[3;6~", KeyEvent(KEY_DELETE, true, false, true) },
        { 6, "\x1b[1;6H", KeyEvent(KEY_HOME, true, false, true) },
        { 6, "\x1b[1;6F", KeyEvent(KEY_END, true, false, true) },
        { 6, "\x1b[5;6~", KeyEvent(KEY_PGUP, true, false, true) },
        { 6, "\x1b[6;6~", KeyEvent(KEY_PGDN, true, false, true) },
        { 6, "\x1b[2;4~", KeyEvent(KEY_INSERT, false, true, true) },
        { 6, "\x1b[3;4~", KeyEvent(KEY_DELETE, false, true, true) },
        { 6, "\x1b[1;4H", KeyEvent(KEY_HOME, false, true, true) },
        { 6, "\x1b[1;4F", KeyEvent(KEY_END, false, true, true) },
        { 6, "\x1b[5;4~", KeyEvent(KEY_PGUP, false, true, true) },
        { 6, "\x1b[6;4~", KeyEvent(KEY_PGDN, false, true, true) },
        { 6, "\x1b[2;8~", KeyEvent(KEY_INSERT, true, true, true) },
        { 6, "\x1b[3;8~", KeyEvent(KEY_DELETE, true, true, true) },
        { 6, "\x1b[1;8H", KeyEvent(KEY_HOME, true, true, true) },
        { 6, "\x1b[1;8F", KeyEvent(KEY_END, true, true, true) },
        { 6, "\x1b[5;8~", KeyEvent(KEY_PGUP, true, true, true) },
        { 6, "\x1b[6;8~", KeyEvent(KEY_PGDN, true, true, true) },
        { 5, "\x1b[11~", KeyEvent(KEY_F1) },
        { 5, "\x1b[12~", KeyEvent(KEY_F2) },
        { 5, "\x1b[13~", KeyEvent(KEY_F3) },
        { 5, "\x1b[14~", KeyEvent(KEY_F4) },
        { 5, "\x1b[15~", KeyEvent(KEY_F5) },
        { 5, "\x1b[17~", KeyEvent(KEY_F6) },
        { 5, "\x1b[18~", KeyEvent(KEY_F7) },
        { 5, "\x1b[19~", KeyEvent(KEY_F8) },
        { 5, "\x1b[20~", KeyEvent(KEY_F9) },
        { 5, "\x1b[21~", KeyEvent(KEY_F10) },
        { 5, "\x1b[23~", KeyEvent(KEY_F11) },
        { 5, "\x1b[24~", KeyEvent(KEY_F12) },
        { 5, "\x1b[23~", KeyEvent(KEY_F1, false, false, true) },
        { 5, "\x1b[24~", KeyEvent(KEY_F2, false, false, true) },
        { 5, "\x1b[25~", KeyEvent(KEY_F3, false, false, true) },
        { 5, "\x1b[26~", KeyEvent(KEY_F4, false, false, true) },
        { 5, "\x1b[28~", KeyEvent(KEY_F5, false, false, true) },
        { 5, "\x1b[29~", KeyEvent(KEY_F6, false, false, true) },
        { 5, "\x1b[31~", KeyEvent(KEY_F7, false, false, true) },
        { 5, "\x1b[32~", KeyEvent(KEY_F8, false, false, true) },
        { 5, "\x1b[33~", KeyEvent(KEY_F9, false, false, true) },
        { 5, "\x1b[34~", KeyEvent(KEY_F10, false, false, true) },
        { 5, "\x1b[23~", KeyEvent(KEY_F11, false, false, true) },
        { 5, "\x1b[24~", KeyEvent(KEY_F12, false, false, true) },
        { 5, "\x1b\x1b[2~", KeyEvent(KEY_INSERT, false, true) },
        { 5, "\x1b\x1b[3~", KeyEvent(KEY_DELETE, false, true) },
        { 5, "\x1b\x1b[1~", KeyEvent(KEY_HOME, false, true) },
        { 5, "\x1b\x1b[4~", KeyEvent(KEY_END, false, true) },
        { 5, "\x1b\x1b[5~", KeyEvent(KEY_PGUP, false, true) },
        { 5, "\x1b\x1b[6~", KeyEvent(KEY_PGDN, false, true) },
        { 4, "\x1b[2~", KeyEvent(KEY_INSERT) },
        { 4, "\x1b[3~", KeyEvent(KEY_DELETE) },
        { 4, "\x1b[1~", KeyEvent(KEY_HOME) },
        { 4, "\x1b[4~", KeyEvent(KEY_END) },
        { 4, "\x1b[5~", KeyEvent(KEY_PGUP) },
        { 4, "\x1b[6~", KeyEvent(KEY_PGDN) },
        { 4, "\x1b\x1b[A", KeyEvent(KEY_UP, false, true) },
        { 4, "\x1b\x1b[B", KeyEvent(KEY_DOWN, false, true) },
        { 4, "\x1b\x1b[C", KeyEvent(KEY_RIGHT, false, true) },
        { 4, "\x1b\x1b[D", KeyEvent(KEY_LEFT, false, true) },
        { 4, "\x1b\x1bOA", KeyEvent(KEY_UP, true, true) },
        { 4, "\x1b\x1bOB", KeyEvent(KEY_DOWN, true, true) },
        { 4, "\x1b\x1bOC", KeyEvent(KEY_RIGHT, true, true) },
        { 4, "\x1b\x1bOD", KeyEvent(KEY_LEFT, true, true) },
        { 4, "\x1bO5P", KeyEvent(KEY_F1, true) },
        { 4, "\x1bO5Q", KeyEvent(KEY_F2, true) },
        { 4, "\x1bO5R", KeyEvent(KEY_F3, true) },
        { 4, "\x1bO5S", KeyEvent(KEY_F4, true) },
        { 4, "\x1bO3P", KeyEvent(KEY_F1, false, true) },
        { 4, "\x1bO3Q", KeyEvent(KEY_F2, false, true) },
        { 4, "\x1bO3R", KeyEvent(KEY_F3, false, true) },
        { 4, "\x1bO3S", KeyEvent(KEY_F4, false, true) },
        { 4, "\x1bO7P", KeyEvent(KEY_F1, true, true) },
        { 4, "\x1bO7Q", KeyEvent(KEY_F2, true, true) },
        { 4, "\x1bO7R", KeyEvent(KEY_F3, true, true) },
        { 4, "\x1bO7S", KeyEvent(KEY_F4, true, true) },
        { 4, "\x1bO2P", KeyEvent(KEY_F1, false, false, true) },
        { 4, "\x1bO2Q", KeyEvent(KEY_F2, false, false, true) },
        { 4, "\x1bO2R", KeyEvent(KEY_F3, false, false, true) },
        { 4, "\x1bO2S", KeyEvent(KEY_F4, false, false, true) },
        { 4, "\x1bO6P", KeyEvent(KEY_F1, true, false, true) },
        { 4, "\x1bO6Q", KeyEvent(KEY_F2, true, false, true) },
        { 4, "\x1bO6R", KeyEvent(KEY_F3, true, false, true) },
        { 4, "\x1bO6S", KeyEvent(KEY_F4, true, false, true) },
        { 4, "\x1bO4P", KeyEvent(KEY_F1, false, true, true) },
        { 4, "\x1bO4Q", KeyEvent(KEY_F2, false, true, true) },
        { 4, "\x1bO4R", KeyEvent(KEY_F3, false, true, true) },
        { 4, "\x1bO4S", KeyEvent(KEY_F4, false, true, true) },
        { 4, "\x1bO8P", KeyEvent(KEY_F1, true, true, true) },
        { 4, "\x1bO8Q", KeyEvent(KEY_F2, true, true, true) },
        { 4, "\x1bO8R", KeyEvent(KEY_F3, true, true, true) },
        { 4, "\x1bO8S", KeyEvent(KEY_F4, true, true, true) },
        { 3, "\x1b[A", KeyEvent(KEY_UP) },
        { 3, "\x1b[B", KeyEvent(KEY_DOWN) },
        { 3, "\x1b[C", KeyEvent(KEY_RIGHT) },
        { 3, "\x1b[D", KeyEvent(KEY_LEFT) },
        { 3, "\x1bOA", KeyEvent(KEY_UP, true) },
        { 3, "\x1bOB", KeyEvent(KEY_DOWN, true) },
        { 3, "\x1bOC", KeyEvent(KEY_RIGHT, true) },
        { 3, "\x1bOD", KeyEvent(KEY_LEFT, true) },
        { 3, "\x1b[Z", KeyEvent(KEY_TAB, false, false, true) },
        { 3, "\x1bOP", KeyEvent(KEY_F1) },
        { 3, "\x1bOQ", KeyEvent(KEY_F2) },
        { 3, "\x1bOR", KeyEvent(KEY_F3) },
        { 3, "\x1bOS", KeyEvent(KEY_F4) },
        { 3, "\x1b[H", KeyEvent(KEY_HOME) },
        { 3, "\x1b[F", KeyEvent(KEY_END) }
    };

#endif

// Console

#ifdef PLATFORM_WINDOWS
Array<INPUT_RECORD> Console::_inputRecords(16);
#else
Array<char> Console::_inputChars(16);
#endif

Array<InputEvent> Console::_inputEvents;
Console::Constructor Console::constructor;

Console::Constructor::Constructor()
{
    try
    {
#ifdef PLATFORM_WINDOWS
        int rc = _setmode(_fileno(stdin), _O_U16TEXT);
        ASSERT(rc >= 0);

        rc = _setmode(_fileno(stdout), _O_U16TEXT);
        ASSERT(rc >= 0);
#else
        int rc = setvbuf(stdout, NULL, _IONBF, 0);
        ASSERT(rc == 0);

        signal(SIGWINCH, onSIGWINCH);
#endif

        setlocale(LC_ALL, "");
        setLineMode(true);
    }
    catch (Exception& ex)
    {
        Console::writeLine(ex.message());
        abort();
    }
    catch (...)
    {
        Console::writeLine(STR("unknown error"));
        abort();
    }
}

void Console::setLineMode(bool lineMode)
{
#ifdef PLATFORM_WINDOWS
    HANDLE handle = GetStdHandle(STD_INPUT_HANDLE);
    ASSERT(handle);

    BOOL rc = SetConsoleMode(handle, lineMode ?
        ENABLE_ECHO_INPUT | ENABLE_PROCESSED_INPUT | ENABLE_LINE_INPUT |
            ENABLE_EXTENDED_FLAGS | ENABLE_INSERT_MODE | ENABLE_QUICK_EDIT_MODE :
        ENABLE_EXTENDED_FLAGS | ENABLE_INSERT_MODE | ENABLE_MOUSE_INPUT |
            ENABLE_WINDOW_INPUT);
    ASSERT(rc);

    handle = GetStdHandle(STD_OUTPUT_HANDLE);
    ASSERT(handle);

    rc = SetConsoleMode(handle, ENABLE_PROCESSED_OUTPUT | ENABLE_WRAP_AT_EOL_OUTPUT);
    ASSERT(rc);
#else
    if (lineMode)
    {
        termios ta;

        int rc = tcgetattr(STDIN_FILENO, &ta);
        ASSERT(rc == 0);

        ta.c_lflag |= ECHO | ICANON;
        rc = tcsetattr(STDIN_FILENO, TCSANOW, &ta);
        ASSERT(rc == 0);

        printf("\x1b[?1000l");
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

        printf("\x1b[?1000h");
        printf("\x1b[?1006h");
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
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    ASSERT(handle);

    DWORD written;
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
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    ASSERT(handle);

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    BOOL rc = GetConsoleScreenBufferInfo(handle, &csbi);
    ASSERT(rc);

    COORD pos;
    pos.X = csbi.srWindow.Left + column - 1;
    pos.Y = csbi.srWindow.Top + line - 1;

    DWORD written;
    rc = WriteConsoleOutputCharacter(handle,
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

unichar_t Console::readChar()
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
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    ASSERT(handle);

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    BOOL rc = GetConsoleScreenBufferInfo(handle, &csbi);
    ASSERT(rc);

    width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
}

void Console::clear()
{
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    ASSERT(handle);

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    BOOL rc = GetConsoleScreenBufferInfo(handle, &csbi);
    ASSERT(rc);

    DWORD size = csbi.dwSize.X * csbi.dwSize.Y, written;
    COORD pos = { 0, 0 };

    rc = FillConsoleOutputCharacter(handle, ' ', size, pos, &written);
    ASSERT(rc);

    rc = FillConsoleOutputAttribute(handle, csbi.wAttributes, size, pos, &written);
    ASSERT(rc);

    pos = { csbi.srWindow.Left, csbi.srWindow.Top };
    rc = SetConsoleCursorPosition(handle, pos);
    ASSERT(rc);
}

void Console::showCursor(bool show)
{
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    ASSERT(handle);

    CONSOLE_CURSOR_INFO cci;
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

    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    ASSERT(handle);

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    BOOL rc = GetConsoleScreenBufferInfo(handle, &csbi);
    ASSERT(rc);

    COORD pos;
    pos.X = csbi.srWindow.Left + column - 1;
    pos.Y = csbi.srWindow.Top + line - 1;

    rc = SetConsoleCursorPosition(handle, pos);
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

const Array<InputEvent>& Console::readInput()
{
    _inputEvents.clear();

#ifdef PLATFORM_WINDOWS

    HANDLE handle = GetStdHandle(STD_INPUT_HANDLE);
    ASSERT(handle);

    if (WaitForSingleObject(handle, INFINITE) == WAIT_OBJECT_0)
    {
        DWORD numInputRec = 0;
        BOOL rc = GetNumberOfConsoleInputEvents(handle, &numInputRec);
        ASSERT(rc);

        _inputRecords.resize(numInputRec);
        rc = ReadConsoleInput(handle, _inputRecords.values(), numInputRec, &numInputRec);
        ASSERT(rc);

        for (DWORD i = 0; i < numInputRec; ++i)
        {
            INPUT_RECORD& inputRec = _inputRecords[i];

            if (inputRec.EventType == KEY_EVENT)
            {
                KeyEvent keyEvent(KEY_NONE);
                keyEvent.keyDown = inputRec.Event.KeyEvent.bKeyDown;

                switch (inputRec.Event.KeyEvent.wVirtualKeyCode)
                {
                case VK_ESCAPE:
                    keyEvent.key = KEY_ESC;
                    break;
                case VK_TAB:
                    keyEvent.key = KEY_TAB;
                    keyEvent.ch = '\t';
                    break;
                case VK_BACK:
                    keyEvent.key = KEY_BACKSPACE;
                    break;
                case VK_RETURN:
                    keyEvent.key = KEY_ENTER;
                    keyEvent.ch = '\n';
                    break;
                case VK_UP:
                    keyEvent.key = KEY_UP;
                    break;
                case VK_DOWN:
                    keyEvent.key = KEY_DOWN;
                    break;
                case VK_LEFT:
                    keyEvent.key = KEY_LEFT;
                    break;
                case VK_RIGHT:
                    keyEvent.key = KEY_RIGHT;
                    break;
                case VK_INSERT:
                    keyEvent.key = KEY_INSERT;
                    break;
                case VK_DELETE:
                    keyEvent.key = KEY_DELETE;
                    break;
                case VK_HOME:
                    keyEvent.key = KEY_HOME;
                    break;
                case VK_END:
                    keyEvent.key = KEY_END;
                    break;
                case VK_PRIOR:
                    keyEvent.key = KEY_PGUP;
                    break;
                case VK_NEXT:
                    keyEvent.key = KEY_PGDN;
                    break;
                case VK_F1:
                    keyEvent.key = KEY_F1;
                    break;
                case VK_F2:
                    keyEvent.key = KEY_F2;
                    break;
                case VK_F3:
                    keyEvent.key = KEY_F3;
                    break;
                case VK_F4:
                    keyEvent.key = KEY_F4;
                    break;
                case VK_F5:
                    keyEvent.key = KEY_F5;
                    break;
                case VK_F6:
                    keyEvent.key = KEY_F6;
                    break;
                case VK_F7:
                    keyEvent.key = KEY_F7;
                    break;
                case VK_F8:
                    keyEvent.key = KEY_F8;
                    break;
                case VK_F9:
                    keyEvent.key = KEY_F9;
                    break;
                case VK_F10:
                    keyEvent.key = KEY_F10;
                    break;
                case VK_F11:
                    keyEvent.key = KEY_F11;
                    break;
                case VK_F12:
                    keyEvent.key = KEY_F12;
                    break;
                default:
                    if (inputRec.Event.KeyEvent.uChar.UnicodeChar == 0)
                        continue;
                    if (inputRec.Event.KeyEvent.uChar.UnicodeChar < 0x20)
                        keyEvent.ch = controlKeys[inputRec.Event.KeyEvent.uChar.UnicodeChar];
                    else
                        keyEvent.ch = inputRec.Event.KeyEvent.uChar.UnicodeChar;
                    break;
                }

                DWORD modifierKeys = inputRec.Event.KeyEvent.dwControlKeyState;
                keyEvent.ctrl = (modifierKeys & (LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED)) != 0;
                keyEvent.alt = (modifierKeys & (LEFT_ALT_PRESSED | RIGHT_ALT_PRESSED)) != 0;
                keyEvent.shift = (modifierKeys & SHIFT_PRESSED) != 0;

                _inputEvents.addLast(keyEvent);
            }
            else if (inputRec.EventType == MOUSE_EVENT)
            {
                HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
                ASSERT(handle);

                CONSOLE_SCREEN_BUFFER_INFO csbi;
                BOOL rc = GetConsoleScreenBufferInfo(handle, &csbi);
                ASSERT(rc);

                MouseEvent mouseEvent(
                    inputRec.Event.MouseEvent.dwMousePosition.X - csbi.srWindow.Left + 1,
                    inputRec.Event.MouseEvent.dwMousePosition.Y - csbi.srWindow.Top + 1);

                if (inputRec.Event.MouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED)
                    mouseEvent.button = MOUSE_BUTTON_PRIMARY;
                else if (inputRec.Event.MouseEvent.dwButtonState & FROM_LEFT_2ND_BUTTON_PRESSED)
                    mouseEvent.button = MOUSE_BUTTON_WHEEL;
                else if (inputRec.Event.MouseEvent.dwButtonState & RIGHTMOST_BUTTON_PRESSED)
                    mouseEvent.button = MOUSE_BUTTON_SECONDARY;
                else if (inputRec.Event.MouseEvent.dwEventFlags & MOUSE_WHEELED)
                {
                    mouseEvent.button = *reinterpret_cast<int*>(
                        &inputRec.Event.MouseEvent.dwButtonState) > 0 ?
                        MOUSE_BUTTON_WHEEL_UP : MOUSE_BUTTON_WHEEL_DOWN;
                }

                if (mouseEvent.button != MOUSE_BUTTON_NONE)
                {
                    if (inputRec.Event.MouseEvent.dwControlKeyState & LEFT_ALT_PRESSED)
                        mouseEvent.alt = true;
                    else if (inputRec.Event.MouseEvent.dwControlKeyState & RIGHT_ALT_PRESSED)
                        mouseEvent.alt = true;
                    else if (inputRec.Event.MouseEvent.dwControlKeyState & LEFT_CTRL_PRESSED)
                        mouseEvent.ctrl = true;
                    else if (inputRec.Event.MouseEvent.dwControlKeyState & RIGHT_CTRL_PRESSED)
                        mouseEvent.ctrl = true;
                    else if (inputRec.Event.MouseEvent.dwControlKeyState & SHIFT_PRESSED)
                        mouseEvent.shift = true;

                    _inputEvents.addLast(mouseEvent);
                }
            }
            else if (inputRec.EventType == WINDOW_BUFFER_SIZE_EVENT)
            {
                _inputEvents.addLast(WindowEvent(
                    inputRec.Event.WindowBufferSizeEvent.dwSize.X,
                    inputRec.Event.WindowBufferSizeEvent.dwSize.Y));
            }
        }
    }

#else

    bool gotChars = false;
    _inputChars.clear();

    while (true)
    {
        char chars[16];

        int len = read(STDIN_FILENO, chars, sizeof(chars));

        if (len > 0)
        {
            gotChars = true;
            for (int i = 0; i < len; ++i)
                _inputChars.addLast(chars[i]);
        }
        else
        {
            if (gotChars)
                break;
            else if (screenSizeChanged)
            {
                screenSizeChanged = false;

                int width, height;
                getSize(width, height);

                _inputEvents.addLast(WindowEvent(width, height));
                return _inputEvents;
            }
            else
                usleep(10000);
        }
    }

    const char* p = _inputChars.values();
    const char* e = p + _inputChars.size();
    _inputChars.addLast(0);

    while (p < e)
    {
        int n = sizeof(keyMapping) / sizeof(KeyMapping);
        bool found = false;

        for (int i = 0; i < n; ++i)
        {
            if (!strncmp(p, keyMapping[i].chars, keyMapping[i].len))
            {
                _inputEvents.addLast(keyMapping[i].keyEvent);
                p += keyMapping[i].len;
                found = true;
                break;
            }
        }

        if (!found)
        {
            bool alt = false;

            if (*p == 0x1b)
            {
                ++p;

                if (*p)
                {
                    if (*p == '[' && *(p + 1) == '<')
                    {
                        int button, x, y;
                        char down;

                        if (sscanf(p + 2, "%d;%d;%d%c%n", &button, &x, &y, &down, &n) == 4)
                        {
                            MouseEvent mouseEvent(x, y);
                            mouseEvent.buttonDown = down == 'M';

                            switch (button)
                            {
                                case 0:
                                    mouseEvent.button = MOUSE_BUTTON_PRIMARY;
                                    break;
                                case 1:
                                    mouseEvent.button = MOUSE_BUTTON_WHEEL;
                                    break;
                                case 2:
                                    mouseEvent.button = MOUSE_BUTTON_SECONDARY;
                                    break;
                                case 64:
                                    mouseEvent.button = MOUSE_BUTTON_WHEEL_UP;
                                    break;
                                case 65:
                                    mouseEvent.button = MOUSE_BUTTON_WHEEL_DOWN;
                                    break;
                                case 16:
                                    mouseEvent.button = MOUSE_BUTTON_PRIMARY;
                                    mouseEvent.ctrl = true;
                                    break;
                                case 17:
                                    mouseEvent.button = MOUSE_BUTTON_WHEEL;
                                    mouseEvent.ctrl = true;
                                    break;
                                case 18:
                                    mouseEvent.button = MOUSE_BUTTON_SECONDARY;
                                    mouseEvent.ctrl = true;
                                    break;
                                case 80:
                                    mouseEvent.button = MOUSE_BUTTON_WHEEL_UP;
                                    mouseEvent.ctrl = true;
                                    break;
                                case 81:
                                    mouseEvent.button = MOUSE_BUTTON_WHEEL_DOWN;
                                    mouseEvent.ctrl = true;
                                    break;
                            }

                             _inputEvents.addLast(mouseEvent);
                             p += 2 + n;

                             continue;
                        }
                    }

                    alt = true;
                }
                else
                {
                    _inputEvents.addLast(KeyEvent(KEY_ESC));
                    continue;
                }
            }

            unichar_t ch;
            p += UTF_CHAR_TO_UNICODE(p, ch);

            KeyEvent keyEvent(ch, false, alt, charIsUpper(ch));

            if (ch == '\t')
                keyEvent.key = KEY_TAB;
            else if (ch == '\n')
                keyEvent.key = KEY_ENTER;
            else if (ch == 0x7f)
                keyEvent.key = KEY_BACKSPACE;
            else if (ch < 0x20)
            {
                keyEvent.ctrl = true;
                keyEvent.ch = controlKeys[ch];
            }
            else
                keyEvent.ch = ch;

            _inputEvents.addLast(keyEvent);
        }
    }

#endif

    return _inputEvents;
}
