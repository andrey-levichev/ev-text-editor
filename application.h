#ifndef APPLICATION_INCLUDED
#define APPLICATION_INCLUDED

#include <foundation.h>
#include <input.h>

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

    virtual void onCreate()
    {
    }

    virtual void onDestroy()
    {
    }

    virtual void onPaint()
    {
    }

    virtual void onResize(int width, int height)
    {
    }

    virtual void onInput(const Array<InputEvent>& inputEvents)
    {
    }

protected:
    Array<String> _args;
    uintptr_t _window = 0;
    const char_t* _title;
    int _dpi;

    static Application* _application;

#ifdef GUI_MODE

#if defined(PLATFORM_WINDOWS)
    static const char_t* WINDOW_CLASS;

    static LRESULT CALLBACK windowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam);
#elif defined(PLATFORM_LINUX)
    GtkWidget* _drawingArea = nullptr;
    cairo_surface_t* _surface = nullptr;

    static gboolean drawEventHandler(GtkWidget* widget, cairo_t* cr, gpointer data);
    static gboolean configureEventHandler(GtkWidget* widget, GdkEventConfigure* event, gpointer data);
    static gboolean buttonPressEventHandler(GtkWidget* widget, GdkEventButton* event, gpointer data);
#endif

#endif
};

#endif
