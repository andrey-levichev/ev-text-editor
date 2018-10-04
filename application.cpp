#include <application.h>

// Application

const char_t* Application::APPLICATION_NAME = STR("ev");

void Application::run()
{
#ifdef EDITOR_GUI_MODE
    _window.create(APPLICATION_NAME);
    _window.show();

    MSG msg;

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
#else
    _inputEvents = Console::readInput();

    for (int i = 0; i < _inputEvents.size(); ++i)
    {
        InputEvent event = _inputEvents[i];

        if (event.eventType == INPUT_EVENT_TYPE_KEY)
        {
            KeyEvent keyEvent = event.event.keyEvent;

        }
        else if (event.eventType == INPUT_EVENT_TYPE_MOUSE)
        {
            MouseEvent mouseEvent = event.event.mouseEvent;

        }
        else if (event.eventType == INPUT_EVENT_TYPE_WINDOW)
        {
            WindowEvent windowEvent = event.event.windowEvent;
        }

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

