#include <application.h>
#include <console.h>

const char_t* APPLICATION_NAME = STR("example");

void run(const Array<String>& args)
{
    Console::writeLine(APPLICATION_NAME);
}
