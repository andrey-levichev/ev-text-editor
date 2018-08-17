#include <application.h>

// Application

void Application::run()
{
    _window.create(STR("DrawingTest"), 800, 800);
    _window.show();

    MSG msg;

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}
