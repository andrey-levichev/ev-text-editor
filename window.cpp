#include <window.h>

// Window

Map<HWND, Window*> Window::_windows;

Window::Window(const char_t* category) :
    _category(category),
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
    wc.lpszClassName = reinterpret_cast<LPCWSTR>(category);
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

    if (!CreateWindow(reinterpret_cast<LPCWSTR>(_category), reinterpret_cast<LPCWSTR>(title),
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
            window->onCreate();
            break;

        case WM_DESTROY:
            window->onDestroy();
            window->_handle = NULL;
            _windows.remove(handle);
            break;

        case WM_PAINT:
            window->onPaint();
            break;

        case WM_SIZE:
            window->onResize(LOWORD(lParam), HIWORD(lParam));
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

void Window::onCreate()
{
    _graphics.create(_handle);
}

void Window::onDestroy()
{
    _graphics.reset();
    PostQuitMessage(0);
}

void Window::onPaint()
{
    _graphics->beginDraw();

    _graphics->drawText(STR("Segoe UI"), 20, false, { 0, 0, 200, 50 },
        TEXT_ALIGNMENT_LEFT, PARAGRAPH_ALIGNMENT_TOP, 0x000000,
        String::format(STR("%p"), this));

    _graphics->endDraw();
}

void Window::onResize(int width, int height)
{
    _graphics->resize(width, height);
}
