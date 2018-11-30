#include <window.h>
#include <file.h>
#include <application.h>

// Window

Window::~Window()
{
#ifdef EDITOR_GUI_MODE
    if (_handle)
        DestroyWindow(reinterpret_cast<HWND>(_handle));
#else
    _handle = 0;
    onDestroy();
#endif
}

void Window::create(const char_t* className, const char_t* title, int width, int height)
{
    if (_handle)
        throw Exception(STR("window already created"));

#ifdef EDITOR_GUI_MODE
    if (!CreateWindow(reinterpret_cast<LPCWSTR>(className), reinterpret_cast<LPCWSTR>(title), WS_OVERLAPPEDWINDOW,
                      CW_USEDEFAULT, CW_USEDEFAULT, width > 0 ? width : CW_USEDEFAULT, height > 0 ? height : CW_USEDEFAULT,
                      NULL, NULL, GetModuleHandle(NULL), this))
    {
        throw Exception(STR("failed to create window"));
    }
#else
    _handle = 1;
    onCreate();
#endif
}

void Window::show()
{
    if (_handle)
    {
#ifdef EDITOR_GUI_MODE
        ShowWindow(reinterpret_cast<HWND>(_handle), SW_SHOWDEFAULT);
        UpdateWindow(reinterpret_cast<HWND>(_handle));
#endif
    }
    else
        throw Exception(STR("window not created"));
}

void Window::destroy()
{
    if (_handle)
    {
#ifdef EDITOR_GUI_MODE
        DestroyWindow(reinterpret_cast<HWND>(_handle));
#else
        _handle = 0;
        onDestroy();
#endif
    }
    else
        throw Exception(STR("window not created"));
}

void Window::registerClass(const char_t* className)
{
#ifdef EDITOR_GUI_MODE
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
    wc.lpszClassName = reinterpret_cast<LPCWSTR>(className);
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    RegisterClassEx(&wc);
#endif
}

#ifdef EDITOR_GUI_MODE

Map<HWND, Window*> Window::_windows;

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
            window->_handle = reinterpret_cast<uintptr_t>(handle);
            _windows.add(handle, window);
            window->onCreate();
            break;

        case WM_DESTROY:
            window->_handle = 0;
            _windows.remove(handle);
            window->onDestroy();
            PostQuitMessage(0);
            break;

        case WM_PAINT:
            window->onPaint();
            break;
        }
    }
    catch (Exception& ex)
    {
        Application::showErrorMessage(ex.message());
    }
    catch (...)
    {
        Application::showErrorMessage(STR("unknown error"));
    }

    return DefWindowProc(handle, message, wParam, lParam);
}

#endif
