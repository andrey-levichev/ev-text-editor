#include <application.h>
#include <console.h>

const char_t* APPLICATION_NAME = STR("example");

void run(const Array<String>& args)
{
    // print command line arguments

    for (int i = 0; i < args.size(); ++i)
        Console::writeLine(args[i]);
}
