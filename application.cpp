#include <application.h>

#ifndef EDITOR_GUI_MODE
#include <console.h>
#endif

// Application

const char_t* Application::APPLICATION_NAME = STR("ev");
const char_t* Application::WINDOW_CLASS = STR("evWindow");

Application* Application::_application = NULL;

Application::~Application()
{
#ifdef EDITOR_GUI_MODE

    if (_handle)
        DestroyWindow(reinterpret_cast<HWND>(_handle));

#else
    _handle = 0;
    onDestroy();
#endif
}

void Application::create(const char_t* className, const char_t* title, int width, int height)
{
    if (_handle)
        throw Exception(STR("window already created"));

#ifdef EDITOR_GUI_MODE

    _handle = reinterpret_cast<uintptr_t>(CreateWindow(reinterpret_cast<LPCWSTR>(className),
        reinterpret_cast<LPCWSTR>(title), WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, width > 0 ? width : CW_USEDEFAULT, height > 0 ? height : CW_USEDEFAULT,
        NULL, NULL, GetModuleHandle(NULL), NULL));

    if (!_handle)
        throw Exception(STR("failed to create window"));

#else
    _handle = 1;
    onCreate();
#endif
}

void Application::show()
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

void Application::destroy()
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

void Application::registerClass(const char_t* className)
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

void Application::run()
{
//    for (int i = 1; i < _argc; ++i)
//    {
//        if (strCompare(_argv[i], STR("--bright")) == 0)
//            _window.brightBackground() = true;
//        else if (strCompare(_argv[i], STR("--dark")) == 0)
//            _window.brightBackground() = false;
//        else
//            _window.openDocument(_argv[i]);
//    }

    registerClass(WINDOW_CLASS);
    create(WINDOW_CLASS, APPLICATION_NAME);
    show();

#ifdef EDITOR_GUI_MODE

    MSG msg;

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

#else

    while (_handle)
    {
        _inputEvents = Console::readInput();
        onInput(_inputEvents);
    }

#endif
}

int run(int argc, const char_t** argv)
{
    try
    {
        for (int i = 1; i < argc; ++i)
        {
            if (strCompare(argv[i], STR("--version")) == 0)
            {
                Application::showMessage(STR("ev text editor version 2.0\n"
                                             "web: github.com/andrey-levichev/ev-text-editor\n"
                                             "Copyright (C) Andrey Levichev, 2019\n\n"
                                             "usage: ev [OPTIONS] [FILE]...\n\n"
                                             "OPTIONS:\n\n"
                                             "--version - print version information and exit\n"
                                             "--dark - assume dark screen background\n"
                                             "--bright - assume bright screen background\n"));

                return 0;
            }
        }

        Application app(argc, argv);
        app.run();
    }
    catch (Exception& ex)
    {
        Application::showErrorMessage(ex.message());
        return 1;
    }
    catch (...)
    {
        Application::showErrorMessage(STR("unknown error"));
        return 1;
    }

    return 0;
}

#ifdef EDITOR_GUI_MODE

void Application::showMessage(const String& message)
{
    MessageBox(NULL, reinterpret_cast<LPCWSTR>(message.chars()),
               reinterpret_cast<LPCWSTR>(APPLICATION_NAME), MB_OK);
}

void Application::showMessage(const char_t* message)
{
    MessageBox(NULL, reinterpret_cast<LPCWSTR>(message),
               reinterpret_cast<LPCWSTR>(APPLICATION_NAME), MB_OK);
}

void Application::showErrorMessage(const String& message)
{
    MessageBox(NULL, reinterpret_cast<LPCWSTR>(message.chars()),
               reinterpret_cast<LPCWSTR>(APPLICATION_NAME),
               MB_OK | MB_ICONERROR);
}

void Application::showErrorMessage(const char_t* message)
{
    MessageBox(NULL, reinterpret_cast<LPCWSTR>(message),
               reinterpret_cast<LPCWSTR>(APPLICATION_NAME),
               MB_OK | MB_ICONERROR);
}

LRESULT CALLBACK Application::windowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam)
{
    try
    {
        switch (message)
        {
        case WM_CREATE:
            _application->onCreate();
            return 0;

        case WM_DESTROY:
            _application->_handle = 0;
            _application->onDestroy();
            PostQuitMessage(0);
            return 0;

        case WM_PAINT:
            _application->onPaint();
            return 0;

        case WM_ERASEBKGND:
            return 1;

        case WM_KEYDOWN:
            {
                _application->_inputEvents.clear();

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

                    _application->_inputEvents.addLast(keyEvent);
                    _application->onInput(_application->_inputEvents);

                    return 0;
                }
            }
            break;

        case WM_CHAR:
            {
                _application->_inputEvents.clear();

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

                    _application->_inputEvents.addLast(keyEvent);
                    _application->onInput(_application->_inputEvents);

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

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE prevInstance, wchar_t* commandLine, int cmdShow)
{
    CoInitialize(NULL);
    SetProcessDPIAware();

    int rc = run(__argc, reinterpret_cast<const char_t**>(const_cast<const wchar_t**>(__wargv)));

    CoUninitialize();

    return rc;
}

#else

void Application::showMessage(const String& message)
{
    Console::writeLine(message);
}

void Application::showMessage(const char_t* message)
{
    Console::writeLine(message);
}

void Application::showErrorMessage(const String& message)
{
    Console::writeLine(message);
}

void Application::showErrorMessage(const char_t* message)
{
    Console::writeLine(message);
}

int MAIN(int argc, const char_t** argv)
{
    return run(argc, argv);
}

#endif
