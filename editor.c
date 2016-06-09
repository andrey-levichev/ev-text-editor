#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>

int top, left;
int width, height;
int line, column;

char* text = NULL;
char* screen = NULL;
int size = 0;

struct termios termAttr;

bool readFile(const char* fileName, char** text, int* size)
{
    struct stat st;

    if (stat(fileName, &st) < 0)
        return false;

    int sz = st.st_size;

    int file = open(fileName, O_RDONLY);

    if (file < 0)
        return false;

    char* txt = malloc(sz + 1);
    if (read(file, txt, sz) != sz)
    {
        free(txt);
        close(file);
        return false;
    }

    txt[sz] = 0;
    close(file);
    *text = txt;
    *size = sz;

    return true;
}

void setCharInputMode()
{
    struct termios newTermAttr;

    tcgetattr(STDIN_FILENO, &termAttr);
    memcpy(&newTermAttr, &termAttr, sizeof(termAttr));

    newTermAttr.c_lflag &= ~(ECHO|ICANON);
    newTermAttr.c_cc[VTIME] = 0;
    newTermAttr.c_cc[VMIN] = 1;

    tcsetattr(STDIN_FILENO, TCSANOW, &newTermAttr);
}

void restoreInputMode()
{
    tcsetattr(STDIN_FILENO, TCSANOW, &termAttr);
}

void getTerminalSize(int* width, int* height)
{
    struct winsize ws;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
    *width = ws.ws_col;
    *height = ws.ws_row;
}

void clearScreen()
{
    write(STDOUT_FILENO, "\x1b[2J", 4);
}

void showCursor()
{
    write(STDOUT_FILENO, "\x1b[?25h", 6);
}

void hideCursor()
{
    write(STDOUT_FILENO, "\x1b[?25l", 6);
}

void setCursorPosition(int col, int line)
{
    char cmd[30];

    sprintf(cmd, "\x1b[%d;%dH", line, col);
    write(STDOUT_FILENO, cmd, strlen(cmd));
}

char* findChar(char* text, char c)
{
    while (*text && *text != c)
        ++text;
    return text;
}

char* findLine(char* text, int line)
{
    char* p = text;

    while (line > 1)
    {
        p = findChar(p, '\n');
        if (*p == '\n')
        {
            ++p;
            --line;
        }
        else
            return p;
    }

    return p;
}

int getLineCount(char* text)
{
    int cnt = 1;

    char* p = text;
    while (*p)
    {
        p = findChar(p, '\n');
        if (*p == '\n')
        {
            ++p;
            ++cnt;
        }
    }

    return cnt;
}

void redrawScreen()
{
    char* p = findLine(text, top);
    char* q = screen;

    for (int j = 1; j <= height; ++j)
    {
        for (int i = left; i > 1; --i)
        {
            if (*p && *p != '\n')
                ++p;
            else
                break;
        }

        for (int i = 1; i <= width; ++i)
        {
            if (*p && *p != '\n')
                *q++ = *p++;
            else
                *q++ = ' ';
        }

        if (*p == '\n')
            ++p;
        else
        {
            p = findChar(p, '\n');
            if (*p == '\n')
                ++p;
        }
    }

    hideCursor();
    setCursorPosition(1, 1);
    write(STDOUT_FILENO, screen, width * height);

    int col = column - left + 1;
    int ln = line - top + 1;

    if (1 <= col && col <= width && 1 <= ln && ln <= height)
    {
        setCursorPosition(col, ln);
        showCursor();
    }
}

int main(int argc, const char** argv)
{
    if (argc == 2)
    {
        if (!readFile(argv[1], &text, &size))
        {
            fprintf(stderr, "failed to open %s\n", argv[1]);
            return 1;
        }
    }

    getTerminalSize(&width, &height);
    screen = malloc(width * height);
    setCharInputMode();

    top = 10; left = 1;
    line = 15; column = 1;

    redrawScreen();

    while (true)
    {
        char cmd[10];

        if (read(STDIN_FILENO, cmd, 1) > 0)
        {
            int n = 0;
            ioctl(STDIN_FILENO, FIONREAD, &n);

            if (read(STDIN_FILENO, cmd + 1, n) == n)
            {
                ++n;

                if (n == 1)
                {
                    if (*cmd == 0x18) // ^X
                        break;
                    else if (*cmd == 0x02) // ^B
                    {
                        if (top > 1)
                        {
                            top = 1;
                            redrawScreen();
                        }

                        continue;
                    }
                    else if (*cmd == 0x05) // ^E
                    {
                        int lastLine = getLineCount(text) - height + 1;
                        if (top < lastLine)
                        {
                            top = lastLine;
                            redrawScreen();
                        }

                        continue;
                    }
                    else if (*cmd == 0x0c) // ^L
                    {
                        if (left > 1)
                        {
                            --left;
                            redrawScreen();
                        }

                        continue;
                    }
                    else if (*cmd == 0x12) // ^R
                    {
                        ++left;
                        redrawScreen();
                        continue;
                    }
                }
                else if (n == 4)
                {
                    if (memcmp(cmd, "\x1b\x5b\x36\x7e", n) == 0) // PgDn
                    {
                        int lastLine = getLineCount(text) - height + 1;
                        if (top < lastLine)
                        {
                            if (top < lastLine - height)
                                top += height;
                            else
                                top = lastLine;

                            redrawScreen();
                        }

                        continue;
                    }
                    else if (memcmp(cmd, "\x1b\x5b\x35\x7e", n) == 0) // PgUp
                    {
                        if (top > 1)
                        {
                            if (top > height)
                                top -= height;
                            else
                                top = 1;

                            redrawScreen();
                        }

                        continue;
                    }
                }

                write(STDOUT_FILENO, cmd, n);
            }
        }
    }

    restoreInputMode();
    free(text);
    free(screen);

    clearScreen();
    setCursorPosition(1, 1);

    return 0;
}
