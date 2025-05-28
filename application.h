#ifndef APPLICATION_INCLUDED
#define APPLICATION_INCLUDED

#include <foundation.h>
#include <input.h>

#ifdef GUI_MODE
#include <graphics.h>
#endif

// Application

class Application
{
public:
    Application(const Array<String>& args, const char_t* title = STR("Application"));

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    virtual ~Application();

    virtual bool start()
    {
        return true;
    }

    virtual void run();

    float fromDevice(int coord)
    {
        return coord * 96.0f / _dpi;
    }

    int toDevice(float coord)
    {
        return coord * _dpi / 96;
    }

protected:
    void createWindow(int width = 0, int height = 0);
    void showWindow();
    void resizeWindow(int width, int height);
    void destroyWindow();

    virtual void onCreate();
    virtual void onDestroy();
    virtual void onPaint(uintptr_t context = 0);
    virtual void onResize(int width, int height);
    virtual void onInput(const Array<InputEvent>& inputEvents);

protected:
    Array<String> _args;
    uintptr_t _window = 0;
    const char_t* _title;
    bool _maximized = false;
    int _dpi;

    static Application* _application;

#ifdef GUI_MODE

    Unique<Graphics> _graphics;

#if defined(PLATFORM_WINDOWS)
    static const char_t* WINDOW_CLASS;

    static LRESULT CALLBACK windowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam);
#elif defined(PLATFORM_LINUX)
    GtkWidget* _drawingArea = nullptr;

    static void realizeEventHandler(GtkWidget* widget, gpointer data);
    static void destroyEventHandler(GtkWidget* widget, GdkEvent* event, gpointer data);

    static gboolean drawEventHandler(GtkWidget* widget, cairo_t* cr, gpointer data);
    static gboolean configureEventHandler(GtkWidget* widget, GdkEvent* event, gpointer data);
    static gboolean buttonPressEventHandler(GtkWidget* widget, GdkEventButton* event, gpointer data);
    static gboolean keyPressEventHandler(GtkWidget* widget, GdkEventKey* event, gpointer data);
#endif

#endif
};

#endif
