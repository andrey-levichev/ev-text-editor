#ifndef CONSOLE_INCLUDED
#define CONSOLE_INCLUDED

#include <foundation.h>

// console I/O support

void printLine(const char_t* str);
void print(const char_t* format, ...);
void printArgs(const char_t* format, va_list args);

// InputEventType

enum InputEventType
{
    INPUT_EVENT_TYPE_KEY,
    INPUT_EVENT_TYPE_MOUSE,
    INPUT_EVENT_TYPE_WINDOW
};

// Key

enum Key
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

// KeyEvent

struct KeyEvent
{
    Key key;
    unichar_t ch;
    bool keyDown;
    bool ctrl, alt, shift;
};

// MouseButton

enum MouseButton
{
    MOUSE_BUTTON_NONE,
    MOUSE_BUTTON_PRIMARY,
    MOUSE_BUTTON_SECONDARY,
    MOUSE_BUTTON_WHEEL,
    MOUSE_BUTTON_WHEEL_UP,
    MOUSE_BUTTON_WHEEL_DOWN
};

// MouseEvent

struct MouseEvent
{
    MouseButton button;
    bool buttonDown;
    int x, y;
    bool ctrl, alt, shift;
};

// WindowEvent

struct WindowEvent
{
    int width, height;
};

// InputEvent

struct InputEvent
{
    InputEventType eventType;

    union
    {
        KeyEvent keyEvent;
        MouseEvent mouseEvent;
        WindowEvent windowEvent;
    } event;

    InputEvent(KeyEvent keyEvent) : eventType(INPUT_EVENT_TYPE_KEY)
    {
        event.keyEvent = keyEvent;
    }

    InputEvent(MouseEvent mouseEvent) : eventType(INPUT_EVENT_TYPE_MOUSE)
    {
        event.mouseEvent = mouseEvent;
    }

    InputEvent(WindowEvent windowEvent) : eventType(INPUT_EVENT_TYPE_WINDOW)
    {
        event.windowEvent = windowEvent;
    }
};

// ForegroundColor

enum ForegroundColor
{
#ifdef PLATFORM_WINDOWS
    FOREGROUND_COLOR_BLACK = 0,
    FOREGROUND_COLOR_RED = FOREGROUND_RED,
    FOREGROUND_COLOR_GREEN = FOREGROUND_GREEN,
    FOREGROUND_COLOR_YELLOW = FOREGROUND_GREEN | FOREGROUND_RED,
    FOREGROUND_COLOR_BLUE = FOREGROUND_BLUE,
    FOREGROUND_COLOR_MAGENTA = FOREGROUND_RED | FOREGROUND_BLUE,
    FOREGROUND_COLOR_CYAN = FOREGROUND_GREEN | FOREGROUND_BLUE,
    FOREGROUND_COLOR_WHITE = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
    FOREGROUND_COLOR_BRIGHT_BLACK = FOREGROUND_INTENSITY,
    FOREGROUND_COLOR_BRIGHT_RED = FOREGROUND_RED | FOREGROUND_INTENSITY,
    FOREGROUND_COLOR_BRIGHT_GREEN = FOREGROUND_GREEN | FOREGROUND_INTENSITY,
    FOREGROUND_COLOR_BRIGHT_YELLOW = FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY,
    FOREGROUND_COLOR_BRIGHT_BLUE = FOREGROUND_BLUE | FOREGROUND_INTENSITY,
    FOREGROUND_COLOR_BRIGHT_MAGENTA = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
    FOREGROUND_COLOR_BRIGHT_CYAN = FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
    FOREGROUND_COLOR_BRIGHT_WHITE = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY
#else
    FOREGROUND_COLOR_DEFAULT = 49,
    FOREGROUND_COLOR_BLACK = 30,
    FOREGROUND_COLOR_RED = 31,
    FOREGROUND_COLOR_GREEN = 32,
    FOREGROUND_COLOR_YELLOW = 33,
    FOREGROUND_COLOR_BLUE = 34,
    FOREGROUND_COLOR_MAGENTA = 35,
    FOREGROUND_COLOR_CYAN = 36,
    FOREGROUND_COLOR_WHITE = 37,
    FOREGROUND_COLOR_BRIGHT_BLACK = 90,
    FOREGROUND_COLOR_BRIGHT_RED = 91,
    FOREGROUND_COLOR_BRIGHT_GREEN = 92,
    FOREGROUND_COLOR_BRIGHT_YELLOW = 93,
    FOREGROUND_COLOR_BRIGHT_BLUE = 94,
    FOREGROUND_COLOR_BRIGHT_MAGENTA = 95,
    FOREGROUND_COLOR_BRIGHT_CYAN = 96,
    FOREGROUND_COLOR_BRIGHT_WHITE = 97
#endif
};

// BackgroundColor

enum BackgroundColor
{
#ifdef PLATFORM_WINDOWS
    BACKGROUND_COLOR_BLACK = 0,
    BACKGROUND_COLOR_RED = BACKGROUND_RED,
    BACKGROUND_COLOR_GREEN = BACKGROUND_GREEN,
    BACKGROUND_COLOR_YELLOW = BACKGROUND_GREEN | BACKGROUND_RED,
    BACKGROUND_COLOR_BLUE = BACKGROUND_BLUE,
    BACKGROUND_COLOR_MAGENTA = BACKGROUND_RED | BACKGROUND_BLUE,
    BACKGROUND_COLOR_CYAN = BACKGROUND_GREEN | BACKGROUND_BLUE,
    BACKGROUND_COLOR_WHITE = BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE,
    BACKGROUND_COLOR_BRIGHT_BLACK = BACKGROUND_INTENSITY,
    BACKGROUND_COLOR_BRIGHT_RED = BACKGROUND_RED | BACKGROUND_INTENSITY,
    BACKGROUND_COLOR_BRIGHT_GREEN = BACKGROUND_GREEN | BACKGROUND_INTENSITY,
    BACKGROUND_COLOR_BRIGHT_YELLOW = BACKGROUND_GREEN | BACKGROUND_RED | BACKGROUND_INTENSITY,
    BACKGROUND_COLOR_BRIGHT_BLUE = BACKGROUND_BLUE | BACKGROUND_INTENSITY,
    BACKGROUND_COLOR_BRIGHT_MAGENTA = BACKGROUND_RED | BACKGROUND_BLUE | BACKGROUND_INTENSITY,
    BACKGROUND_COLOR_BRIGHT_CYAN = BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_INTENSITY,
    BACKGROUND_COLOR_BRIGHT_WHITE = BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_INTENSITY
#else
    BACKGROUND_COLOR_DEFAULT = 49,
    BACKGROUND_COLOR_BLACK = 40,
    BACKGROUND_COLOR_RED = 41,
    BACKGROUND_COLOR_GREEN = 42,
    BACKGROUND_COLOR_YELLOW = 43,
    BACKGROUND_COLOR_BLUE = 44,
    BACKGROUND_COLOR_MAGENTA = 45,
    BACKGROUND_COLOR_CYAN = 46,
    BACKGROUND_COLOR_WHITE = 47,
    BACKGROUND_COLOR_BRIGHT_BLACK = 100,
    BACKGROUND_COLOR_BRIGHT_RED = 101,
    BACKGROUND_COLOR_BRIGHT_GREEN = 102,
    BACKGROUND_COLOR_BRIGHT_YELLOW = 103,
    BACKGROUND_COLOR_BRIGHT_BLUE = 104,
    BACKGROUND_COLOR_BRIGHT_MAGENTA = 105,
    BACKGROUND_COLOR_BRIGHT_CYAN = 106,
    BACKGROUND_COLOR_BRIGHT_WHITE = 107
#endif
};

// Console

class Console
{
public:
    static bool brightBackground();

    static ForegroundColor defaultForeground()
    {
        return _defaultForeground;
    }

    static BackgroundColor defaultBackground()
    {
        return _defaultBackground;
    }

    static void setLineMode(bool lineMode);
    static void setColor(ForegroundColor foreground, BackgroundColor background);

    static void write(const String& str);
    static void write(const char_t* chars, int len = -1);
    static void write(unichar_t ch, int n = 1);

    static void writeLine(const String& str);
    static void writeLine(const char_t* chars, int len = -1);
    static void writeLine(unichar_t ch, int n = 1);
    static void writeLine();

    static void write(int line, int column, const String& str);
    static void write(int line, int column, const char_t* chars, int len = -1);
    static void write(int line, int column, unichar_t ch, int n = 1);

    static void writeFormatted(const char_t* format, ...);
    static void writeFormattedArgs(const char_t* format, va_list args);

    static void writeLineFormatted(const char_t* format, ...);
    static void writeLineFormattedArgs(const char_t* format, va_list args);

    static unichar_t readChar();
    static String readLine();

    static void getSize(int& width, int& height);
    static void clear();

    static void showCursor(bool show);
    static void setCursorPosition(int line, int column);

    static const Array<InputEvent>& readInput();

protected:
    static ForegroundColor _defaultForeground;
    static BackgroundColor _defaultBackground;

#ifdef PLATFORM_WINDOWS
    static Buffer<INPUT_RECORD> _inputRecords;
#else
    static Array<char> _inputChars;
#endif

    static Array<InputEvent> _inputEvents;

    struct Constructor
    {
        Constructor();
        ~Constructor();
    };

    static Constructor constructor;
};

#endif
