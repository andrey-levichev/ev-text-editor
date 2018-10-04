#ifndef WINDOW_INCLUDED
#define WINDOW_INCLUDED

#include <foundation.h>

#ifdef EDITOR_GUI_MODE
#include <graphics.h>
#else
#include <console.h>
#endif

// Window

class Window
{
public:
    Window();
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    void create(const char_t* title, int width = 0, int height = 0);
    void show();

private:
    void onCreate();
    void onDestroy();
    void onPaint();
    void onResize(int width, int height);

private:
    uintptr_t _handle;
    String _text;

#ifdef EDITOR_GUI_MODE
private:
    Unique<Graphics> _graphics;

private:
    static Map<HWND, Window*> _windows;
    static LRESULT CALLBACK windowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam);
#endif
};

#endif
