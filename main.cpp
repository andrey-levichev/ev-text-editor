#include <application.h>

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

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE prevInstance, wchar_t* commandLine, int cmdShow)
{
    CoInitialize(NULL);
    SetProcessDPIAware();

    int rc = run(__argc, reinterpret_cast<const char_t**>(const_cast<const wchar_t**>(__wargv)));

    CoUninitialize();

    return rc;
}

#else

int MAIN(int argc, const char_t** argv)
{
    return run(argc, argv);
}

#endif
