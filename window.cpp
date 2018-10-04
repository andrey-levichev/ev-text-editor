#include <window.h>
#include <file.h>

// Window

Window::Window() : _handle(NULL)
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
    wc.lpszClassName = L"EvEditorWindow";
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    RegisterClassEx(&wc);
#endif
}

Window::~Window()
{
#ifdef EDITOR_GUI_MODE
    if (_handle)
        DestroyWindow(reinterpret_cast<HWND>(_handle));
#endif
}

void Window::create(const char_t* title, int width, int height)
{
#ifdef EDITOR_GUI_MODE
    if (_handle)
        throw Exception(STR("window already created"));

    if (!CreateWindow(L"EvEditorWindow", reinterpret_cast<LPCWSTR>(title), WS_OVERLAPPEDWINDOW,
                      CW_USEDEFAULT, CW_USEDEFAULT, width > 0 ? width : CW_USEDEFAULT, height > 0 ? height : CW_USEDEFAULT,
                      NULL, NULL, GetModuleHandle(NULL), this))
    {
        throw Exception(STR("failed to create window"));
    }
#endif
}

void Window::show()
{
#ifdef EDITOR_GUI_MODE
    if (_handle)
    {
        ShowWindow(reinterpret_cast<HWND>(_handle), SW_SHOWDEFAULT);
        UpdateWindow(reinterpret_cast<HWND>(_handle));
    }
    else
        throw Exception(STR("window not created"));
#endif
}

void Window::onCreate()
{
    File file(STR("t.cpp"));
    ByteBuffer bytes = file.read();

    TextEncoding encoding;
    bool bom, crLf;
    _text = Unicode::bytesToString(bytes, encoding, bom, crLf);

#ifdef EDITOR_GUI_MODE
    _graphics.create(reinterpret_cast<HWND>(_handle));
#else
    Console::clear();
#endif
}

void Window::onDestroy()
{
#ifdef EDITOR_GUI_MODE
    PostQuitMessage(0);
#endif
}

void Window::onPaint()
{
#ifdef EDITOR_GUI_MODE
    _graphics->beginDraw();

    Size size = _graphics->getSize();
    Rect rect = { 0, 0, size.width, size.height };

    _graphics->drawText(STR("Lucida Console"), 13, _text, rect);

    _graphics->endDraw();
#else
    Console::write(1, 1, _text);
#endif
}

void Window::onResize(int width, int height)
{
#ifdef EDITOR_GUI_MODE
    _graphics->resize(width, height);
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

#endif
