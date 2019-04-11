#ifndef APPLICATION_INCLUDED
#define APPLICATION_INCLUDED

#include <foundation.h>
#include <input.h>

// Application

class Application
{
private:
    static const char_t* APPLICATION_NAME;
    static const char_t* WINDOW_CLASS;

public:
    Application() :
        _argc(0), _argv(NULL),
        _handle(0)
    {
        _application = this;
    }

    Application(int argc, const char_t** argv) :
        _argc(argc), _argv(argv),
        _handle(0)
    {
        _application = this;
    }

    virtual ~Application();

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    void create(const char_t* className,
                const char_t* title, int width = 0, int height = 0);

    void show();
    void destroy();
    void run();

    virtual void onCreate()
    {
    }

    virtual void onDestroy()
    {
    }

    virtual void onPaint()
    {
    }

    virtual void onInput(const Array<InputEvent>& inputEvents)
    {
    }

    static void registerClass(const char_t* className);
    static void showMessage(const String& message);
    static void showMessage(const char_t* message);
    static void showErrorMessage(const String& message);
    static void showErrorMessage(const char_t* message);

private:
    int _argc;
    const char_t** _argv;
    uintptr_t _handle;
    Array<InputEvent> _inputEvents;

#ifdef EDITOR_GUI_MODE
    static Application* _application;
    static LRESULT CALLBACK windowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam);
#endif
};

#endif
