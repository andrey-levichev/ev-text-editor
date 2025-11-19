#include <application.h>
#include <console.h>

// Application

Application* Application::_application = nullptr;

Application::Application(const Array<String>& args, const char_t* title) :
    _args(args), _title(title)
{
    if (_application)
        terminate(STR("multiple instances for Application are not supported"));
    _application = this;

#ifdef GUI_MODE

#ifdef PLATFORM_WINDOWS
    _dpi = GetDpiForSystem();
#else
    _dpi = 96;
#endif

#else
    _dpi = 96;
#endif
}

Application::~Application()
{
    try
    {
        if (_window)
        {
#ifdef GUI_MODE
#ifdef PLATFORM_WINDOWS
            DestroyWindow(reinterpret_cast<HWND>(_window));
#endif
#else
            onDestroy();
            _window = 0;
#endif
        }
    }
    catch (Exception& ex)
    {
        reportError(ex.message());
    }
    catch (...)
    {
        reportError(STR("unknown error"));
    }
}

void Application::run()
{
    createWindow(800, 800);
    showWindow();

#ifdef GUI_MODE

#if defined(PLATFORM_WINDOWS)
    while (_window)
    {
        MSG msg;

        if (GetMessage(&msg, nullptr, 0, 0) > 0)
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
            break;
    }
#elif defined(PLATFORM_LINUX)
    gtk_main();
#endif

#else
    onPaint();

    while (_window)
    {
        const Array<InputEvent>& inputEvents = Console::readInput();

        for (int i = 0; i < inputEvents.size(); ++i)
            if (inputEvents[i].eventType == INPUT_EVENT_TYPE_WINDOW)
            {
                WindowEvent windowEvent = inputEvents[i].event.windowEvent;
                onResize(windowEvent.width, windowEvent.height);
                onPaint();
            }

        onInput(inputEvents);
    }
#endif
}

void Application::createWindow(int width, int height)
{
    if (_window)
        throw Exception(STR("window already created"));

#ifdef GUI_MODE

#if defined(PLATFORM_WINDOWS)
    WNDCLASSEX wc;
    HINSTANCE instance = GetModuleHandle(nullptr);

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = windowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = instance;
    wc.hIcon = LoadIcon(instance, L"AppIcon");
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = nullptr;
    wc.lpszMenuName = nullptr;
    wc.lpszClassName = reinterpret_cast<LPCWSTR>(WINDOW_CLASS);
    wc.hIconSm = LoadIcon(instance, L"AppIconSmall");

    ATOM rc = RegisterClassEx(&wc);
    ASSERT(rc != 0);

    CreateWindow(reinterpret_cast<LPCWSTR>(WINDOW_CLASS), reinterpret_cast<LPCWSTR>(_title), WS_OVERLAPPEDWINDOW,
                 CW_USEDEFAULT, CW_USEDEFAULT, width > 0 ? width : CW_USEDEFAULT, height > 0 ? height : CW_USEDEFAULT,
                 nullptr, nullptr, GetModuleHandle(nullptr), nullptr);

    if (!_window)
        throw Exception(STR("failed to create window"));
#elif defined(PLATFORM_LINUX)
    _window = reinterpret_cast<uintptr_t>(gtk_window_new(GTK_WINDOW_TOPLEVEL));
    gtk_window_set_title(GTK_WINDOW(_window), _title);

    g_signal_connect(reinterpret_cast<GtkWidget*>(_window), "realize", G_CALLBACK(realizeEventHandler), nullptr);
    g_signal_connect(reinterpret_cast<GtkWidget*>(_window), "destroy", G_CALLBACK(destroyEventHandler), nullptr);

    _drawingArea = gtk_drawing_area_new();
    gtk_container_add(GTK_CONTAINER(_window), _drawingArea);

    g_signal_connect(_drawingArea, "draw", G_CALLBACK(drawEventHandler), nullptr);
    g_signal_connect(_drawingArea, "configure-event", G_CALLBACK(configureEventHandler), nullptr);
    g_signal_connect(_drawingArea, "button-press-event", G_CALLBACK(buttonPressEventHandler), nullptr);
    g_signal_connect(_drawingArea, "key-press-event", G_CALLBACK(keyPressEventHandler), nullptr);

    gtk_widget_set_events(_drawingArea,
        gtk_widget_get_events(_drawingArea) | GDK_BUTTON_PRESS_MASK | GDK_KEY_PRESS_MASK);

    // can-focus

    if (width > 0 && height > 0)
        gtk_widget_set_size_request(reinterpret_cast<GtkWidget*>(_window), width, height);
#endif

#else
    _window = 1;
    onCreate();
#endif
}

void Application::resizeWindow(int width, int height)
{
    ASSERT(width > 0 && height > 0);

    if (_window)
    {
#ifdef GUI_MODE

#if defined(PLATFORM_WINDOWS)
        SetWindowPos(reinterpret_cast<HWND>(_window), nullptr, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER);
#elif defined(PLATFORM_LINUX)
        gtk_widget_set_size_request(reinterpret_cast<GtkWidget*>(_window), width, height);
#endif

#endif
    }
    else
        throw Exception(STR("window not created"));
}

void Application::showWindow()
{
    if (_window)
    {
#ifdef GUI_MODE

#if defined(PLATFORM_WINDOWS)
        ShowWindow(reinterpret_cast<HWND>(_window), _maximized ? SW_MAXIMIZE : SW_SHOWDEFAULT);
        UpdateWindow(reinterpret_cast<HWND>(_window));
#elif defined(PLATFORM_LINUX)
        gtk_widget_show_all(reinterpret_cast<GtkWidget*>(_window));
#endif

#endif
    }
    else
        throw Exception(STR("window not created"));
}

void Application::destroyWindow()
{
    if (_window)
    {
#ifdef GUI_MODE
#ifdef PLATFORM_WINDOWS
        DestroyWindow(reinterpret_cast<HWND>(_window));
#endif
#else
        onDestroy();
        _window = 0;
#endif
    }
    else
        throw Exception(STR("window not created"));
}

void Application::onCreate()
{
#ifdef GUI_MODE
    _graphics.create(_window);
#endif
}

void Application::onDestroy()
{
#ifdef GUI_MODE
    _graphics.reset();
#endif
}

void Application::onPaint(uintptr_t context)
{
#ifdef GUI_MODE
    _graphics->beginDraw(context);
    _graphics->clear();

    auto tb = _graphics->createTextBlock(STR("Source Code Pro"), 20,
        STR("Cairo sucks. People who use it are idiots. "
            "Pango is poorly documented buggy piece of crap."), { 300, 60 });

    Rect rect = { 100, 100, 400, 400 };
    _graphics->fillRectangle(rect, 0xffeeff);
    _graphics->drawText(tb, { 100, 100 });

    _graphics->endDraw();
#endif
}

void Application::onResize(int width, int height)
{
#ifdef GUI_MODE
    _graphics->resize(width, height);
#endif
}

void Application::onInput(const Array<InputEvent>& inputEvents)
{
}

#ifdef GUI_MODE

#if defined(PLATFORM_WINDOWS)

const char_t* Application::WINDOW_CLASS = STR("WINDOW_CLASS");

static bool isKeyPressed(int key)
{
    return (GetKeyState(key) & 0x8000) != 0;
}

LRESULT CALLBACK Application::windowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam)
{
    static Array<InputEvent> inputEvents;

    try
    {
        switch (message)
        {
        case WM_CREATE:
            _application->_window = reinterpret_cast<uintptr_t>(handle);
            _application->onCreate();
            return 0;

        case WM_DESTROY:
            _application->onDestroy();
            _application->_window = 0;
            return 0;

        case WM_PAINT:
        case WM_DISPLAYCHANGE:
            if (GetForegroundWindow() == reinterpret_cast<HWND>(_application->_window))
            {
                _application->onPaint();
                ValidateRect(handle, nullptr);
                return 0;
            }
            break;

        case WM_SIZE:
            _application->onResize(LOWORD(lParam), HIWORD(lParam));
            return 0;

        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
            {
                inputEvents.clear();

                KeyEvent keyEvent = { KEY_NONE };

                switch (wParam)
                {
                case VK_UP:
                    keyEvent.key = KEY_UP;
                    break;

                case VK_DOWN:
                    keyEvent.key = KEY_DOWN;
                    break;

                case VK_LEFT:
                    keyEvent.key = KEY_LEFT;
                    break;

                case VK_RIGHT:
                    keyEvent.key = KEY_RIGHT;
                    break;

                case VK_INSERT:
                    keyEvent.key = KEY_INSERT;
                    break;

                case VK_DELETE:
                    keyEvent.key = KEY_DELETE;
                    break;

                case VK_HOME:
                    keyEvent.key = KEY_HOME;
                    break;

                case VK_END:
                    keyEvent.key = KEY_END;
                    break;

                case VK_PRIOR:
                    keyEvent.key = KEY_PGUP;
                    break;

                case VK_NEXT:
                    keyEvent.key = KEY_PGDN;
                    break;

                case VK_F1:
                    keyEvent.key = KEY_F1;
                    break;

                case VK_F2:
                    keyEvent.key = KEY_F2;
                    break;

                case VK_F3:
                    keyEvent.key = KEY_F3;
                    break;

                case VK_F4:
                    keyEvent.key = KEY_F4;
                    break;

                case VK_F5:
                    keyEvent.key = KEY_F5;
                    break;

                case VK_F6:
                    keyEvent.key = KEY_F6;
                    break;

                case VK_F7:
                    keyEvent.key = KEY_F7;
                    break;

                case VK_F8:
                    keyEvent.key = KEY_F8;
                    break;

                case VK_F9:
                    keyEvent.key = KEY_F9;
                    break;

                case VK_F10:
                    keyEvent.key = KEY_F10;
                    break;

                case VK_F11:
                    keyEvent.key = KEY_F11;
                    break;

                case VK_F12:
                    keyEvent.key = KEY_F12;
                    break;
                }

                if (keyEvent.key != KEY_NONE)
                {
                    keyEvent.ctrl = isKeyPressed(VK_CONTROL);
                    keyEvent.alt = isKeyPressed(VK_MENU);
                    keyEvent.shift = isKeyPressed(VK_SHIFT);

                    int count = lParam & 0x7fff;
                    for (int i = 0; i < count; ++i)
                        inputEvents.addLast(keyEvent);

                    _application->onInput(inputEvents);

                    return 0;
                }
            }
            break;

        case WM_CHAR:
        case WM_SYSCHAR:
            {
                inputEvents.clear();

                KeyEvent keyEvent = { KEY_NONE };

                if (wParam > 0)
                {
                    if (wParam == 0x08)
                        keyEvent.key = KEY_BACKSPACE;
                    else if (wParam == 0x09)
                        keyEvent.key = KEY_TAB;
                    else if (wParam == 0x0d)
                        keyEvent.key = KEY_ENTER;
                    else if (wParam == 0x1b)
                        keyEvent.key = KEY_ESC;
                    else
                        keyEvent.ch = wParam < 0x20 ? CONTROL_KEYS[wParam] : wParam;

                    keyEvent.ctrl = isKeyPressed(VK_CONTROL);
                    keyEvent.alt = isKeyPressed(VK_MENU);
                    keyEvent.shift = isKeyPressed(VK_SHIFT);

                    int count = lParam & 0x7fff;
                    for (int i = 0; i < count; ++i)
                        inputEvents.addLast(keyEvent);

                    _application->onInput(inputEvents);

                    return 0;
                }
            }
            break;

        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN:
            {
                inputEvents.clear();

                int x = GET_X_LPARAM(lParam);
                int y = GET_Y_LPARAM(lParam);
                MouseEvent mouseEvent = { MOUSE_BUTTON_PRIMARY, true,
                    static_cast<int>(_application->fromDevice(x)),
                    static_cast<int>(_application->fromDevice(y)) };

                if (message == WM_RBUTTONDOWN)
                    mouseEvent.button = MOUSE_BUTTON_SECONDARY;
                else if (message == WM_MBUTTONDOWN)
                    mouseEvent.button = MOUSE_BUTTON_WHEEL;

                mouseEvent.ctrl = isKeyPressed(VK_CONTROL);
                mouseEvent.alt = isKeyPressed(VK_MENU);
                mouseEvent.shift = isKeyPressed(VK_SHIFT);

                inputEvents.addLast(mouseEvent);
                _application->onInput(inputEvents);

                return 0;
            }
            break;

        case WM_MOUSEWHEEL:
            {
                inputEvents.clear();

                int delta = GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;

                int x = GET_X_LPARAM(lParam);
                int y = GET_Y_LPARAM(lParam);
                MouseEvent mouseEvent = {
                    delta > 0 ? MOUSE_BUTTON_WHEEL_UP : MOUSE_BUTTON_WHEEL_DOWN, true,
                    static_cast<int>(_application->fromDevice(x)),
                    static_cast<int>(_application->fromDevice(y)) };

                mouseEvent.ctrl = isKeyPressed(VK_CONTROL);
                mouseEvent.alt = isKeyPressed(VK_MENU);
                mouseEvent.shift = isKeyPressed(VK_SHIFT);

                inputEvents.addLast(mouseEvent);
                _application->onInput(inputEvents);

                return 0;
            }
            break;
        }
    }
    catch (Exception& ex)
    {
        reportError(ex.message());
    }
    catch (...)
    {
        reportError(STR("unknown error"));
    }

    return DefWindowProc(handle, message, wParam, lParam);
}

#elif defined(PLATFORM_LINUX)

void Application::realizeEventHandler(GtkWidget* widget, gpointer data)
{
    _application->onCreate();
}

void Application::destroyEventHandler(GtkWidget* widget, GdkEvent* event, gpointer data)
{
    gtk_main_quit();
    _application->onDestroy();
}

gboolean Application::drawEventHandler(GtkWidget* widget, cairo_t* cr, gpointer data)
{
    _application->onPaint(reinterpret_cast<uintptr_t>(cr));
    return TRUE;
}

gboolean Application::configureEventHandler(GtkWidget* widget, GdkEvent* event, gpointer data)
{
    _application->onResize(gtk_widget_get_allocated_width(widget), gtk_widget_get_allocated_height(widget));
    return TRUE;
}

gboolean Application::buttonPressEventHandler(GtkWidget* widget, GdkEventButton* event, gpointer data)
{
    return FALSE;
}

gboolean Application::keyPressEventHandler(GtkWidget* widget, GdkEventKey* event, gpointer data)
{
    return FALSE;
}

#endif

#endif
