#ifndef INPUT_INCLUDED
#define INPUT_INCLUDED

#include <foundation.h>

extern const char* CONTROL_KEYS;

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

#endif
