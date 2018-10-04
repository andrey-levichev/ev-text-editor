#include <editor.h>

#ifdef EDITOR_GUI_MODE

#include <application.h>

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
        MessageBox(NULL, reinterpret_cast<LPCWSTR>(ex.message()), L"Error", MB_OK | MB_ICONERROR);
        return 1;
    }
    catch (...)
    {
        MessageBox(NULL, L"unknown error", L"Error", MB_OK | MB_ICONERROR);
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
        for (int i = 1; i < argc; ++i)
        {
            if (strCompare(argv[i], STR("--version")) == 0)
            {
                Console::writeLine(STR("ev text editor version 1.7\n"
                                       "web: https://andreylevichev.github.io/ev-text-editor\n"
                                       "Copyright (C) Andrey Levichev, 2018\n\n"
                                       "usage: ev [OPTIONS] [FILE]...\n\n"
                                       "OPTIONS:\n"
                                       "\t--version\tprint version information and exit\n"
                                       "\t--dark\t\tassume dark screen background\n"
                                       "\t--bright\tassume bright screen background\n\n"));

                return 0;
            }
        }

        Editor editor;

        for (int i = 1; i < argc; ++i)
        {
            if (strCompare(argv[i], STR("--bright")) == 0)
                editor.brightBackground() = true;
            else if (strCompare(argv[i], STR("--dark")) == 0)
                editor.brightBackground() = false;
            else
                editor.openDocument(argv[i]);
        }

        editor.run();
    }
    catch (Exception& ex)
    {
        Console::writeLine(ex.message());
        return 1;
    }
    catch (...)
    {
        Console::writeLine(STR("unknown error"));
        return 1;
    }

    return 0;
}

#endif
