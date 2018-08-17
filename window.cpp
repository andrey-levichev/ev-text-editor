#include <window.h>
#include <application.h>

Map<HWND, Window*> Window::_windows;

Window::Window(const char_t* windowClass, WindowEventHandler* windowEventHandler) :
    _windowClass(windowClass),
    _windowEventHandler(windowEventHandler),
    _handle(NULL)
{
    WNDCLASSEX wc;

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = windowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = GetModuleHandle(NULL);
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = NULL;
    wc.lpszMenuName = NULL;
    wc.lpszClassName = reinterpret_cast<LPCWSTR>(windowClass);
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    RegisterClassEx(&wc);
}

Window::~Window()
{
    if (_handle)
        DestroyWindow(_handle);
}

void Window::create(const char_t* title, int width, int height)
{
    if (_handle)
        throw Exception(STR("window already created"));

    if (!CreateWindow(reinterpret_cast<LPCWSTR>(_windowClass), reinterpret_cast<LPCWSTR>(title),
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
        width, height, NULL, NULL, GetModuleHandle(NULL), this))
    {
        throw Exception(STR("failed to create window"));
    }
}

void Window::show()
{
    if (_handle)
    {
        ShowWindow(_handle, SW_SHOWDEFAULT);
        UpdateWindow(_handle);
    }
    else
        throw Exception(STR("window not created"));
}

LRESULT CALLBACK Window::windowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam)
{
    try
    {
        Window* window;

        if (message == WM_CREATE)
        {
            window = reinterpret_cast<Window*>(reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams);
        }
        else
        {
            Window** ptr = _windows.find(handle);
            if (ptr)
                window = *ptr;
            else
                return DefWindowProc(handle, message, wParam, lParam);
        }

        switch (message)
        {
        case WM_CREATE:
            window->_handle = handle;
            _windows.add(handle, window);
            window->_windowEventHandler->onCreate(*window);
            break;

        case WM_DESTROY:
            window->_windowEventHandler->onDestroy(*window);
            window->_handle = NULL;
            _windows.remove(handle);
            break;

        case WM_PAINT:
            window->_windowEventHandler->onPaint(*window);
            break;

        case WM_SIZE:
            window->_windowEventHandler->onResize(*window, LOWORD(lParam), HIWORD(lParam));
            break;
        }
    }
    catch (Exception& ex)
    {
        MessageBox(NULL, reinterpret_cast<LPCWSTR>(ex.message()), L"Error", MB_OK | MB_ICONERROR);
    }
    catch (...)
    {
        MessageBox(NULL, L"unknown error", L"Error", MB_OK | MB_ICONERROR);
    }

    return DefWindowProc(handle, message, wParam, lParam);
}

void MainWindowEventHandler::onCreate(Window& window)
{
    _graphics.create(window.handle());
}

void MainWindowEventHandler::onDestroy(Window& window)
{
    _graphics.reset();
    PostQuitMessage(0);
}

void MainWindowEventHandler::onPaint(Window& window)
{
    _graphics->beginDraw();

    _graphics->drawText(STR("Segoe UI"), 20, false, { 0, 0, 200, 50 },
        TEXT_ALIGNMENT_LEFT, PARAGRAPH_ALIGNMENT_TOP, 0x000000,
        String::format(STR("%p"), this));

    _graphics->endDraw();
}

void MainWindowEventHandler::onResize(Window& window, int width, int height)
{
    _graphics->resize(width, height);
}
