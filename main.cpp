#include <editor.h>

int run(int argc, const char_t** argv)
{
    try
    {
        Editor app(argc, argv);

        if (app.start())
            app.run();
    }
    catch (Exception& ex)
    {
        Application::reportError(ex.message());
        return 1;
    }
    catch (...)
    {
        Application::reportError(STR("unknown error"));
        return 1;
    }

    return 0;
}

#ifdef GUI_MODE

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
