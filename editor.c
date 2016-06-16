#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
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

bool writeFile(const char* fileName)
{
    int file = open(fileName, O_WRONLY | O_CREAT,
        S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);

    if (file < 0)
        return false;

    if (write(file, text, size) != size)
        return false;

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

void setCursorPosition(int column, int line)
{
    char cmd[30];

    sprintf(cmd, "\x1b[%d;%dH", line, column);
    write(STDOUT_FILENO, cmd, strlen(cmd));
}

char* findChar(char* str, char c)
{
    while (*str && *str != c)
        ++str;

    return str;
}

char* findCharBackwards(char* str, char* pos, char c)
{
    while (pos > str)
        if (*--pos == c)
            return pos;

    return str;
}

char* findLine(char* str, int line)
{
    while (*str && line > 1)
    {
        if (*str++ == '\n')
            --line;
    }

    return str;
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

void positionToLineColumn(int position, int* line, int* column)
{
    char* p = text;
    char* e = text + position;
    int ln = 1, col = 1;

    while (*p && p < e)
    {
        if (*p++ == '\n')
        {
            ++ln;
            col = 1;
        }
        else
            ++col;
    }

    *line = ln;
    *column = col;
}

int lineColumnToPosition(int line, int column)
{
    char* p = text;
    int ln = 1, col = 1;

    while (*p && (ln < line || (ln == line && col < column)))
    {
        if (*p++ == '\n')
        {
            ++ln;
            col = 1;
        }
        else
            ++col;
    }

    if (ln == line && col == column)
        return p - text;
    else
        return -1;
}

void redrawScreen()
{
    if (line < top)
        top = line;
    else if (line >= top + height)
        top = line - height + 1;

    if (column < left)
        left = column;
    else if (column >= left + width)
        left = column - width + 1;

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

    setCursorPosition(column - left + 1, line - top + 1);
    showCursor();
}

bool processKeys()
{
    char keys[10];
    int len = read(STDIN_FILENO, keys, sizeof(keys) - 1);

    if (len > 0)
        keys[len] = 0;
    else
        return true;

    for (char* key = keys; *key;)
    {
        if (*key == 0x18) // ^X
            return false;
        else if (*key == 0x02) // ^B
        {
            if (line > 1 || column > 1)
            {
                line = column = 1;
                redrawScreen();
            }

            ++key;
            continue;
        }
        else if (*key == 0x05) // ^E
        {
            positionToLineColumn(size, &line, &column);
            redrawScreen();

            ++key;
            continue;
        }
        else if (*key == 0x04) // ^D
        {
            int pos = lineColumnToPosition(line , column);
            if (pos > 0)
            {
                char* start = findCharBackwards(text, text + pos, '\n');
                char* end = findChar(text + pos, '\n');
                deleteChars(start - text, end - start);
                redrawScreen();
            }

            ++key;
            continue;
        }
        else if (*key == 0x7f) // Backspace
        {
            int pos = lineColumnToPosition(line , column);
            if (pos > 0)
            {
                --pos;
                deleteChars(pos, 1);
                positionToLineColumn(pos, &line, &column);
                redrawScreen();
            }

            ++key;
            continue;
        }
        else if (memcmp(key, "\x1b\x5b\x41", 3) == 0) // up
        {
            if (line > 1)
            {
                --line;
                redrawScreen();
            }

            key += 3;
            continue;
        }
        else if (memcmp(key, "\x1b\x5b\x42", 3) == 0) // down
        {
            ++line;
            redrawScreen();

            key += 3;
            continue;
        }
        else if (memcmp(key, "\x1b\x5b\x43", 3) == 0) // right
        {
            ++column;
            redrawScreen();

            key += 3;
            continue;
        }
        else if (memcmp(key, "\x1b\x5b\x44", 3) == 0) // left
        {
            if (column > 1)
            {
                --column;
                redrawScreen();
            }

            key += 3;
            continue;
        }
        else if (memcmp(key, "\x1b\x5b\x36\x7e", 4) == 0) // PgDn
        {
            line += height;
            redrawScreen();

            key += 4;
            continue;
        }
        else if (memcmp(key, "\x1b\x5b\x35\x7e", 4) == 0) // PgUp
        {
            if (line > 1)
            {
                line -= height;
                if (line < 1)
                    line = 1;

                redrawScreen();
            }

            key += 4;
            continue;
        }
        else if (memcmp(key, "\x1b\x5b\x31\x7e", 4) == 0) // Home
        {
            if (column > 1)
            {
                column = 1;
                redrawScreen();
            }

            key += 4;
            continue;
        }
        else if (memcmp(key, "\x1b\x5b\x34\x7e", 4) == 0) // End
        {
            int pos = lineColumnToPosition(line, 1);
            if (pos >= 0)
            {
                char* p = findChar(text + pos, '\n');
                positionToLineColumn(p - text, &line, &column);
                redrawScreen();
            }

            key += 4;
            continue;
        }
        else if (memcmp(key, "\x1b\x5b\x33\x7e", 4) == 0) // Delete
        {
            int pos = lineColumnToPosition(line , column);
            if (pos >= 0)
            {
                deleteChars(pos, 1);
                redrawScreen();
            }

            key += 4;
            continue;
        }

        if (*key == '\t' || *key == '\n' || isprint(*key))
        {
            int pos = lineColumnToPosition(line, column);
            if (pos >= 0)
            {
                if (*key == '\t') // Tab
                {
                    insertChars("    ", pos, 4);
                    column += 4;
                }
                else
                {
                    insertChars(key, pos, 1);

                    if (*key == '\n')
                    {
                        ++line; column = 1;
                    }
                    else
                        ++column;
                }

                redrawScreen();
            }
        }

        ++key;
    }

    return true;
}

void editor()
{
    getTerminalSize();
    screen = alloc(width * height);
    setCharInputMode();

    top = 1; left = 1;
    line = 1; column = 1;

    redrawScreen();

    while (processKeys());

    restoreInputMode();
    free(screen);

    clearScreen();
    setCursorPosition(1, 1);
    showCursor();
}

int main(int argc, const char** argv)
{
    if (argc != 2)
    {
        puts("usage: editor filename");
        return 1;
    }

    const char* filename = argv[1];
    if (!readFile(filename))
    {
        size = 0;
        capacity = 1;
        text = alloc(capacity);
        *text = 0;
    }

    editor();

    if (!writeFile(filename))
    {
        free(text);
        fprintf(stderr, "failed to save text to %s\n", filename);
        return 1;
    }

    free(text);
    return 0;
}
