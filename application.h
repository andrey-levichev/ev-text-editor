#ifndef APPLICATION_INCLUDED
#define APPLICATION_INCLUDED

#include <foundation.h>
#include <window.h>

// Application

class Application
{
public:
    Application(const char_t* commandLine) : _commandLine(commandLine), _window(STR("EditorMainWindow"))
    {
    }

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    const char_t* commandLine() const
    {
        return _commandLine;
    }

    Window& window()
    {
        return _window;
    }

    void run();

private:
    const char_t* _commandLine;
    Window _window;
};

#endif
