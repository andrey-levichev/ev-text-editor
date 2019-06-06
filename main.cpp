#include <foundation.h>
#include <console.h>

void run(const Array<String>& args);

void __run(int argc, const char_t** argv)
{
    try
    {
        Array<String> args;

        for (int i = 0; i < argc; ++i)
            args.addLast(argv[i]);

        run(args);
    }
    catch (Exception& ex)
    {
        reportError(ex.message());
    }
    catch (...)
    {
        reportError(STR("unknown error"));
    }
}

#ifdef GUI_MODE

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE prevInstance, wchar_t* commandLine, int cmdShow)
{
    try
    {
        CoInitialize(NULL);
        SetProcessDPIAware();

        __run(__argc, reinterpret_cast<const char_t**>(const_cast<const wchar_t**>(__wargv)));

        CoUninitialize();
    }
    catch (Exception& ex)
    {
        reportError(ex.message());
    }
    catch (...)
    {
        reportError(STR("unknown error"));
    }

    return 0;
}

#else

int MAIN(int argc, const char_t** argv)
{
    try
    {
        Console::initialize();

        __run(argc, argv);

        Console::shutdown();
    }
    catch (Exception& ex)
    {
        reportError(ex.message());
    }
    catch (...)
    {
        reportError(STR("unknown error"));
    }

    return 0;
}

#endif
