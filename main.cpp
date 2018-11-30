#include <application.h>

#ifdef EDITOR_GUI_MODE

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE prevInstance, wchar_t* commandLine, int cmdShow)
{
    CoInitialize(NULL);
    SetProcessDPIAware();

    try
    {
        Application app(reinterpret_cast<const char_t*>(commandLine));
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
        return 2;
    }

    CoUninitialize();

    return 0;
}

#else

int MAIN(int argc, const char_t** argv)
{
    try
    {
        String commandLine;

        for (int i = 1; i < argc; ++i)
        {
            if (i > 1)
                commandLine += ' ';

            commandLine += argv[i];
        }

        Application app(commandLine);
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

#endif
