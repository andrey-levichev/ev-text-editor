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

char* screen;
int top, left;
int width, height;

int line, column;
int position;

char* text;
int size, capacity;

struct termios termAttr;

void* alloc(int size)
{
    void* p = malloc(size);
    if (p)
        return p;

    fprintf(stderr, "out of memory");
    abort();
}

bool readFile(const char* fileName)
{
    struct stat st;

    if (stat(fileName, &st) < 0)
        return false;

    int file = open(fileName, O_RDONLY);

    if (file < 0)
        return false;

    size = st.st_size;
    capacity = size + 1;
    text = alloc(capacity);

    if (read(file, text, size) != size)
    {
        free(text);
        close(file);
        return false;
    }

    text[size] = 0;
    close(file);

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

void getTerminalSize()
{
    struct winsize ws;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
    width = ws.ws_col;
    height = ws.ws_row;
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

bool processKey()
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
                    return false;
                else if (*cmd == 0x02) // ^B
                {
                    if (top > 1)
                    {
                        top = 1;
                        redrawScreen();
                    }

                    return true;
                }
                else if (*cmd == 0x05) // ^E
                {
                    int lastLine = getLineCount(text) - height + 1;
                    if (top < lastLine)
                    {
                        top = lastLine;
                        redrawScreen();
                    }

                    return true;
                }
                else if (*cmd == 0x0c) // ^L
                {
                    if (left > 1)
                    {
                        --left;
                        redrawScreen();
                    }

                    return true;
                }
                else if (*cmd == 0x12) // ^R
                {
                    ++left;
                    redrawScreen();
                    return true;
                }
            }
            else if (n == 3)
            {
                if (memcmp(cmd, "\x1b\x5b\x41", n) == 0) // up
                {
                    return true;
                }
                else if (memcmp(cmd, "\x1b\x5b\x42", n) == 0) // down
                {
                    return true;
                }
                else if (memcmp(cmd, "\x1b\x5b\x43", n) == 0) // right
                {
                    return true;
                }
                else if (memcmp(cmd, "\x1b\x5b\x44", n) == 0) // left
                {
                    return true;
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

                    return true;
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

                    return true;
                }
                else if (memcmp(cmd, "\x1b\x5b\x31\x7e", n) == 0) // Home
                {
                    return true;
                }
                else if (memcmp(cmd, "\x1b\x5b\x34\x7e", n) == 0) // End
                {
                    return true;
                }
            }

            write(STDOUT_FILENO, cmd, n);
        }
    }

    return true;
}

char* strCopy(char* destStr, const char* srcStr, int len)
{
    memcpy(destStr, srcStr, len);
    return destStr + len;
}

void insertChars(const char* chars, int pos, int len)
{
    int cap = size + len + 1;
    if (cap > capacity)
    {
        capacity = cap * 2;
        char* tx = alloc(capacity);
        strcpy(tx, text);
        free(text);
        text = tx;
    }

    memmove(text + pos + len, text + pos, size - pos + 1);
    memcpy(text + pos, chars, len);
    size += len; 
}

void deleteChars(int pos, int len)
{
    memmove(text + pos, text + pos + len, size - pos - len + 1);
    size -= len;
}

int main(int argc, const char** argv)
{
    if (argc == 2)
    {
        if (!readFile(argv[1]))
        {
            fprintf(stderr, "failed to open %s\n", argv[1]);
            return 1;
        }
    }
    else
    {
        size = 0;
        capacity = 1;
        text = alloc(capacity);
        *text = 0;
    }

    getTerminalSize();
    screen = alloc(width * height);
    setCharInputMode();

    top = 1; left = 1;
    line = 1; column = 1;
    position = 0;

    redrawScreen();

    while (processKey());

    restoreInputMode();
    free(screen);
    free(text);

    clearScreen();
    setCursorPosition(1, 1);
    showCursor();

    return 0;
}
