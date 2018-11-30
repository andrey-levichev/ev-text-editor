#ifndef APPLICATION_INCLUDED
#define APPLICATION_INCLUDED

#include <foundation.h>
#include <window.h>
#include <editor.h>

#ifndef EDITOR_GUI_MODE
#include <console.h>
#endif

// Application

class Application
{
private:
    static const char_t* APPLICATION_NAME;
    static const char_t* WINDOW_CLASS;

public:
    Application(const String& commandLine) :
        _commandLine(commandLine)
    {
    }

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    const String& commandLine() const
    {
        return _commandLine;
    }

    Window& window()
    {
        return _window;
    }

    void run();

    static void showMessage(const String& message);
    static void showMessage(const char_t* message);
    static void showErrorMessage(const String& message);
    static void showErrorMessage(const char_t* message);

private:
    const String _commandLine;
    Editor _window;
    Array<InputEvent> _inputEvents;
};

#endif
