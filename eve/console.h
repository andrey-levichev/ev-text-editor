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

    KeyEvent() :
        key(KEY_NONE), ch(0), keyDown(true),
        ctrl(false), alt(false), shift(false)
    {
    }

    KeyEvent(Key key, bool ctrl = false, bool alt = false, bool shift = false) :
        key(key), ch(0), keyDown(true),
        ctrl(ctrl), alt(alt), shift(shift)
    {
    }

    KeyEvent(unichar_t ch, bool ctrl = false, bool alt = false, bool shift = false) :
        key(KEY_NONE), ch(ch), keyDown(true),
        ctrl(ctrl), alt(alt), shift(shift)
    {
    }
};

// MouseButton

enum MouseButton
{
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

    MouseEvent(MouseButton button, bool buttonDown, int x, int y) :
        button(button), buttonDown(buttonDown), x(x), y(y)
    {
    }
};

// WindowEvent

struct WindowEvent
{
    int width, height;

    WindowEvent(int width, int height) :
        width(width), height(height)
    {
    }
};

// InputEvent

struct InputEvent
{
    InputEventType eventType;

    union Event
    {
        KeyEvent keyEvent;
        MouseEvent mouseEvent;
        WindowEvent windowEvent;

        Event(const KeyEvent& keyEvent) :
            keyEvent(keyEvent)
        {
        }

        Event(const MouseEvent& mouseEvent) :
            mouseEvent(mouseEvent)
        {
        }

        Event(const WindowEvent& windowEvent) :
            windowEvent(windowEvent)
        {
        }
    } event;

    InputEvent(KeyEvent keyEvent) :
        eventType(INPUT_EVENT_TYPE_KEY), event(keyEvent)
    {
    }

    InputEvent(MouseEvent mouseEvent) :
        eventType(INPUT_EVENT_TYPE_MOUSE), event(mouseEvent)
    {
    }

    InputEvent(WindowEvent windowEvent) :
        eventType(INPUT_EVENT_TYPE_WINDOW), event(windowEvent)
    {
    }
};

// Console

class Console
{
public:
    static void initialize();
    static void setLineMode(bool lineMode);

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
    static void writeFormatted(const char_t* format, va_list args);

    static void writeLineFormatted(const char_t* format, ...);
    static void writeLineFormatted(const char_t* format, va_list args);

    static unichar_t readChar();
    static String readLine();

    static void getSize(int& width, int& height);
    static void clear();

    static void showCursor(bool show);
    static void setCursorPosition(int line, int column);

    static const Array<InputEvent>& readInput();

protected:
#ifdef PLATFORM_WINDOWS
    static Array<INPUT_RECORD> _inputRecords;
#else
    static Array<char> _inputChars;
#endif

    static Array<InputEvent> _inputEvents;
};

#endif
