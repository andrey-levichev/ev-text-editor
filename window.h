#ifndef WINDOW_INCLUDED
#define WINDOW_INCLUDED

#include <foundation.h>
#include <graphics.h>

// Window

class Window
{
public:
    Window(const char_t* category);
    virtual ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    void create(const char_t* title, int width, int height);
    void show();

protected:
    virtual void onCreate();
    virtual void onDestroy();
    virtual void onPaint();
    virtual void onResize(int width, int height);

private:
    static LRESULT CALLBACK windowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam);

private:
    const char_t* _category;
    HWND _handle;

    static Map<HWND, Window*> _windows;

    Unique<Graphics> _graphics;
    String _text;
};

#endif
