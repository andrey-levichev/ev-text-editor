#ifndef APPLICATION_INCLUDED
#define APPLICATION_INCLUDED

#include <foundation.h>
#include <input.h>

// Application

class Application
{
public:
    Application(const Array<String>& args, const char_t* title = STR("Application")) :
        _args(args), _title(title), _window(0)
    {
        _application = this;
    }

    virtual ~Application();

    virtual bool start()
    {
        return true;
    }

    virtual void run();

private:
    Application(const Application&);
    Application& operator=(const Application&);

protected:
    void createWindow(const char_t* title, int width = 0, int height = 0);
    void showWindow();
    void destroyWindow();

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

protected:
    Array<String> _args;
    const char_t* _title;
    uintptr_t _window;

    static const char_t* WINDOW_CLASS;
    static Application* _application;

#ifdef GUI_MODE
    static LRESULT CALLBACK windowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam);
#endif
};

#endif
