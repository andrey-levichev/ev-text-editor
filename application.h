#ifndef APPLICATION_INCLUDED
#define APPLICATION_INCLUDED

#include <foundation.h>
#include <editor.h>

// Application

class Application
{
private:
    static const char_t* APPLICATION_NAME;
    static const char_t* WINDOW_CLASS;

public:
    Application(int argc, const char_t** argv) :
        _argc(argc), _argv(argv)
    {
    }

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

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
    int _argc;
    const char_t** _argv;
    Editor _window;
};

#endif
