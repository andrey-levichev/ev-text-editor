#ifndef WINDOW_INCLUDED
#define WINDOW_INCLUDED

#include <foundation.h>
#include <graphics.h>
#include <windows.h>

class Window;

// WindowEventHandler

class WindowEventHandler
{
public:
    virtual ~WindowEventHandler()
    {
    }

    virtual void onCreate(Window& window)
    {
    }

    virtual void onDestroy(Window& window)
    {
    }

    virtual void onPaint(Window& window)
    {
    }

    virtual void onResize(Window& window, int width, int height)
    {
    }
};

// Window

class Window
{
public:
    Window(const char_t* windowClass, WindowEventHandler* windowEventHandler);
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    WindowEventHandler* windowEventHandler()
    {
        return _windowEventHandler;
    }

    HWND handle() const
    {
        return _handle;
    }

    void create(const char_t* title, int width, int height);
    void show();

private:
    static LRESULT CALLBACK windowProc(
        HWND window, UINT message, WPARAM wParam, LPARAM lParam);

private:
    const char_t* _windowClass;
    WindowEventHandler* _windowEventHandler;
    HWND _handle;

    static Map<HWND, Window*> _windows;
};

// MainWindowEventHandler

class MainWindowEventHandler : public WindowEventHandler
{
public:
    MainWindowEventHandler()
    {
    }

    MainWindowEventHandler(const MainWindowEventHandler&) = delete;
    MainWindowEventHandler& operator=(const MainWindowEventHandler&) = delete;

    Graphics& renderer()
    {
        return *_graphics;
    }

private:
    void onCreate(Window& window) override;
    void onDestroy(Window& window) override;
    void onPaint(Window& window) override;
    void onResize(Window& window, int width, int height) override;

private:
    Unique<Graphics> _graphics;
};

#endif
