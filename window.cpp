#include <window.h>
#include <application.h>

#ifndef EDITOR_GUI_MODE
#include <console.h>
#endif

// Window

Array<InputEvent> Window::_inputEvents;

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
        throw Exception(STR("failed to create window"));

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

void Window::processInput()
{
#ifndef EDITOR_GUI_MODE
    _inputEvents = Console::readInput();
    onInput(_inputEvents);
#endif
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
            window = reinterpret_cast<Window*>(reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams);
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
            return 0;

        case WM_DESTROY:
            window->_handle = 0;
            _windows.remove(handle);
            window->onDestroy();
            PostQuitMessage(0);
            return 0;

        case WM_PAINT:
            window->onPaint();
            return 0;

        case WM_ERASEBKGND:
            return 1;

        case WM_KEYDOWN:
            {
                LOG_MSG("WM_KEYDOWN");
                _inputEvents.clear();

                KeyEvent keyEvent = { KEY_NONE };
                keyEvent.keyDown = true;

                switch (wParam)
                {
                case VK_ESCAPE:
                    keyEvent.key = KEY_ESC;
                    break;

                case VK_TAB:
                    keyEvent.key = KEY_TAB;
                    keyEvent.ch = '\t';
                    break;

                case VK_BACK:
                    keyEvent.key = KEY_BACKSPACE;
                    break;

                case VK_RETURN:
                    keyEvent.key = KEY_ENTER;
                    keyEvent.ch = '\n';
                    break;

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
                    keyEvent.ctrl = GetKeyState(VK_CONTROL);
                    keyEvent.alt = GetKeyState(VK_MENU);
                    keyEvent.shift = GetKeyState(VK_SHIFT);

                    _inputEvents.addLast(keyEvent);
                    window->onInput(_inputEvents);

                    LOG_MSG("WM_KEYDOWN processed");
                    return 0;
                }
            }
            break;

        case WM_CHAR:
            {
                LOG_MSG("WM_CHAR");
                _inputEvents.clear();

                KeyEvent keyEvent = { KEY_NONE };
                keyEvent.keyDown = true;

                if (wParam > 0)
                {
                    if (wParam < 0x20)
                        keyEvent.ch = CONTROL_KEYS[wParam];
                    else
                        keyEvent.ch = wParam;

                    keyEvent.ctrl = GetKeyState(VK_CONTROL);
                    keyEvent.alt = GetKeyState(VK_MENU);
                    keyEvent.shift = GetKeyState(VK_SHIFT);

                    _inputEvents.addLast(keyEvent);
                    window->onInput(_inputEvents);

                    LOG_MSG("WM_CHAR processed");
                    return 0;
                }
            }
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
