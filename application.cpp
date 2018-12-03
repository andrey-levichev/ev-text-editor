#include <application.h>

// Application

const char_t* Application::APPLICATION_NAME = STR("ev");
const char_t* Application::WINDOW_CLASS = STR("evWindow");

void Application::run()
{
    for (int i = 1; i < _argc; ++i)
    {
        if (strCompare(_argv[i], STR("--bright")) == 0)
            _editor.brightBackground() = true;
        else if (strCompare(_argv[i], STR("--dark")) == 0)
            _editor.brightBackground() = false;
        else
            _editor.openDocument(_argv[i]);
    }

    Window::registerClass(WINDOW_CLASS);

    _editor.create(WINDOW_CLASS, APPLICATION_NAME);
    _editor.show();

#ifdef EDITOR_GUI_MODE
    MSG msg;

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
#else
    while (_editor.handle())
    {
        _inputEvents = Console::readInput();
        _editor.onInput(_inputEvents);
    }
#endif
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

#endif

