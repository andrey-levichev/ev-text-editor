#ifndef CONSOLE_INCLUDED
#define CONSOLE_INCLUDED

#include <foundation.h>

// KeyCode

enum KeyCode
{
    KEY_NONE,
    KEY_ESC,
    KEY_TAB,
    KEY_BACKSPACE,
    KEY_ENTER,
    KEY_UP,
    KEY_DOWN,
    KEY_LEFT,
    KEY_RIGHT,
    KEY_INSERT,
    KEY_DELETE,
    KEY_HOME,
    KEY_END,
    KEY_PGUP,
    KEY_PGDN,
    KEY_F1,
    KEY_F2,
    KEY_F3,
    KEY_F4,
    KEY_F5,
    KEY_F6,
    KEY_F7,
    KEY_F8,
    KEY_F9,
    KEY_F10,
    KEY_F11,
    KEY_F12
};

// Key

struct Key
{
    KeyCode code = KEY_NONE;
    char_t ch = 0;
    bool ctrl = false;
    bool alt = false;
    bool shift = false;
};

// ConsoleMode

enum ConsoleMode
{
    CONSOLEMODE_UNICODE = 1,
    CONSOLEMODE_CANONICAL = 2
};

// Console

class Console
{
public:
    static void create();
    static void destroy();

    static void setMode(int mode);

    static void write(char_t ch);
    static void write(const String& str);
    static void write(const char_t* format, ...);

    static void writeLine();
    static void writeLine(const String& str);
    static void writeLine(const char_t* format, ...);

    static void write(int line, int column, const char_t* chars, int len);

    static String readLine();

    static void getSize(int& width, int& height);
    static void clear();

    static void showCursor(bool show);
    static void setCursorPosition(int line, int column);

    static int readKeys();
	
    static Key key(int index)
    {
        return _keys[index];
    }

private:
#ifdef PLATFORM_UNIX
    static void readRegularKey(const char_t ch, Key& key);
    static const char_t* readSpecialKey(const char_t* p, Key& key);
#endif

private:
    static int _inputSize;
#ifdef PLATFORM_WINDOWS
    static INPUT_RECORD* _input;
#else
    static char_t* _input;
#endif

    static int _keysSize;
    static Key* _keys;
};

#endif
