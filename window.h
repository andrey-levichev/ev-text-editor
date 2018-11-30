#ifndef WINDOW_INCLUDED
#define WINDOW_INCLUDED

#include <foundation.h>
#include <console.h>

#ifdef EDITOR_GUI_MODE
#include <graphics.h>
#endif

// Window

class Window
{
public:
    Window() :
        _handle(0)
    {
    }

    virtual ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    uintptr_t handle()
    {
        return _handle;
    }

    void create(const char_t* className,
        const char_t* title, int width = 0, int height = 0);

    void show();
    void destroy();

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

private:
    uintptr_t _handle;

#ifdef EDITOR_GUI_MODE
    static Map<HWND, Window*> _windows;
    static LRESULT CALLBACK windowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam);
#endif
};

#endif
