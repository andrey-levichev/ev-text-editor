#include <foundation.h>
#include <console.h>

void run(const Array<String>& args);

void __run(int argc, const char_t** argv)
{
    try
    {
        Array<String> args;

        for (int i = 0; i < argc; ++i)
            args.addLast(String(argv[i]));

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

#if defined(GUI_MODE) && defined(PLATFORM_WINDOWS)
int WINAPI wWinMain(HINSTANCE instance, HINSTANCE prevInstance, wchar_t* commandLine, int cmdShow)
#else
int MAIN(int argc, const char_t** argv)
#endif
{
    try
    {
#ifdef GUI_MODE

#if defined(PLATFORM_WINDOWS)
        CoInitialize(nullptr);
        SetProcessDPIAware();

        __run(__argc, reinterpret_cast<const char_t**>(const_cast<const wchar_t**>(__wargv)));

        CoUninitialize();
#elif defined(PLATFORM_LINUX)
        gtk_init(nullptr, nullptr);
        __run(argc, argv);
#else
#error Unsupported GUI platform
#endif

#else
        Console::initialize();

        __run(argc, argv);

        Console::shutdown();
#endif
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
