#ifndef APPLICATION_INCLUDED
#define APPLICATION_INCLUDED

#include <window.h>

// Application

class Application
{
public:
    Application(const char_t* commandLine) :
        _commandLine(commandLine),
        _window(STR("DrawingTestMainWindow"), &_windowEventHandler)
    {
    }

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    const char_t* commandLine() const
    {
        return _commandLine;
    }

    WindowEventHandler& windowEventHandler()
    {
        return _windowEventHandler;
    }

    Window& window()
    {
        return _window;
    }

    void run();

private:
    const char_t* _commandLine;
    MainWindowEventHandler _windowEventHandler;
    Window _window;
};

#endif
