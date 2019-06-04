#include <foundation.h>

void run(const Array<String>& args);

int __run(int argc, const char_t** argv)
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
        return 1;
    }
    catch (...)
    {
        reportError(STR("unknown error"));
        return 1;
    }

    return 0;
}

#ifdef GUI_MODE

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE prevInstance, wchar_t* commandLine, int cmdShow)
{
    CoInitialize(NULL);
    SetProcessDPIAware();

    int rc = __run(__argc, reinterpret_cast<const char_t**>(const_cast<const wchar_t**>(__wargv)));

    CoUninitialize();

    return rc;
}

#else

int MAIN(int argc, const char_t** argv)
{
    return __run(argc, argv);
}

#endif
