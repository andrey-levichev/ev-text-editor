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

const int TAB_SIZE = 8;

const char* filename;

char* screen;
int top, left;
int width, height;

int position;
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
    int file = open(fileName, O_WRONLY | O_CREAT | O_TRUNC,
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

char* findCharBack(char* start, char* str, char c)
{
    while (str > start)
        if (*--str == c)
            return str;

    return start;
}

char* wordForward(char* str)
{
    while (*str)
    {
        if (isspace(*str) && !isspace(*(str + 1)))
            return str + 1;
        ++str;
    }

    return str;
}

char* wordBack(char* start, char* str)
{
    if (str > start)
    {
        --str;
        while (str > start)
        {
            if (isspace(*(str - 1)) && !isspace(*str))
                return str;
            --str;
        }
    }

    return start;
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

void positionToLineColumn()
{
    char* p = text;
    char* q = text + position;
    line = 1, column = 1;

    while (*p && p < q)
    {
        if (*p == '\n')
        {
            ++line;
            column = 1;
        }
        else if (*p == '\t')
            column = ((column - 1) / TAB_SIZE + 1) * TAB_SIZE + 1;
        else
            ++column;

        ++p;
    }
}

void lineColumnToPosition()
{
    char* p = text;
    int ln = 1, col = 1;

    while (*p && ln < line)
    {
        if (*p++ == '\n')
            ++ln;
    }

    while (*p && *p != '\n' && col < column)
    {
        if (*p == '\t')
            col = ((col - 1) / TAB_SIZE + 1) * TAB_SIZE + 1;
        else
            ++col;

        ++p;
    }
    
    position = p - text;    
}

void redrawScreen()
{
    positionToLineColumn();

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
    int len = left + width - 1;

    for (int j = 1; j <= height; ++j)
    {
        for (int i = 1; i <= len; ++i)
        {
            char c;

            if (*p == '\t')
            {
                c = ' ';
                if (i == ((i - 1) / TAB_SIZE + 1) * TAB_SIZE)
                    ++p;
            }
            else if (*p && *p != '\n')
                c = *p++;
            else
                c = ' ';

            if (i >= left)
                *q++ = c;
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

bool processKey()
{
    char key[4];
    int len = read(STDIN_FILENO, key, sizeof(key));

    if (len == 1)
    {
        if (*key == 0x18) // ^X
            return false;
        else if (*key == 0x17) // ^W
        {
            if (!writeFile(filename))
            {
                fprintf(stderr, "failed to save text to %s\n", filename);
                abort();
            }
        }
        else if (*key == 0x01) // ^A
        {
            position = 0;
            redrawScreen();
        }
        else if (*key == 0x05) // ^E
        {
            position = size;
            redrawScreen();
        }
        else if (*key == 0x04) // ^D
        {
            char* p = findCharBack(text, text + position, '\n');
            if (*p == '\n')
                ++p;

            char* q = findChar(text + position, '\n');
            if (*q == '\n')
                ++q;

            deleteChars(p - text, q - p);
            redrawScreen();
        }
        else if (*key == 0x1b) // ^[
        {
            position = wordBack(text, text + position) - text;
            redrawScreen();
        }
        else if (*key == 0x1d) // ^]
        {
            position = wordForward(text + position) - text;
            redrawScreen();
        }
        else if (*key == 0x7f) // Backspace
        {
            if (position > 0)
            {
                deleteChars(--position, 1);
                redrawScreen();
            }
        }
        else if (*key == '\t' || *key == '\n' || isprint(*key))
        {
            if (*key == '\n') // Enter
            {
                char* p = findCharBack(text, text + position, '\n');
                if (*p == '\n')
                    ++p;

                insertChars(key, position++, 1);

                int len = strspn(p, " \t");
                if (len > 0)
                {
                    insertChars(p, position, len);
                    position += len;
                }
            }
            else
                insertChars(key, position++, 1);

            redrawScreen();
        }
    }
    else if (len == 3)
    {
        if (memcmp(key, "\x1b\x5b\x41", 3) == 0) // up
        {
            if (line > 1)
            {
                --line;
                lineColumnToPosition();
                redrawScreen();
            }
        }
        else if (memcmp(key, "\x1b\x5b\x42", 3) == 0) // down
        {
            ++line;
            lineColumnToPosition();
            redrawScreen();
        }
        else if (memcmp(key, "\x1b\x5b\x43", 3) == 0) // right
        {
            if (position < size)
            {
                ++position;
                redrawScreen();
            }
        }
        else if (memcmp(key, "\x1b\x5b\x44", 3) == 0) // left
        {
            if (position > 0)
            {
                --position;
                redrawScreen();
            }
        }
    }
    else if (len == 4)
    {
        if (memcmp(key, "\x1b\x5b\x35\x7e", 4) == 0) // PgUp
        {
            line -= height - 1;
            if (line < 1)
                line = 1;

            lineColumnToPosition();
            redrawScreen();
        }
        else if (memcmp(key, "\x1b\x5b\x36\x7e", 4) == 0) // PgDn
        {
            line += height - 1;
            lineColumnToPosition();
            redrawScreen();
        }
        else if (memcmp(key, "\x1b\x5b\x31\x7e", 4) == 0) // Home
        {
            char* p = findCharBack(text, text + position, '\n');
            if (*p == '\n')
                ++p;

            position = p - text;
            redrawScreen();
        }
        else if (memcmp(key, "\x1b\x5b\x34\x7e", 4) == 0) // End
        {
            position = findChar(text + position, '\n') - text;
            redrawScreen();
        }
        else if (memcmp(key, "\x1b\x5b\x33\x7e", 4) == 0) // Delete
        {
            if (position < size)
            {
                deleteChars(position, 1);
                redrawScreen();
            }
        }
    }

    return true;
}

void editor()
{
    getTerminalSize();
    screen = alloc(width * height);
    setCharInputMode();

    top = 1; left = 1;
    position = 0;

    redrawScreen();

    while (processKey());

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
        puts("usage: editor filename\n");
        puts("arrows, Home, End, PgUp, PgDn - move around");
        puts("^[ - previous word\n");
        puts("^] - next word\n");
        puts("^A - start of file");
        puts("^E - end of file\n");
        puts("Delete, Backspace - delete characters");
        puts("^D - delete line\n");
        puts("^W - save");
        puts("^X - exit");

        return 1;
    }

    filename = argv[1];

    if (!readFile(filename))
    {
        size = 0;
        capacity = 1;
        text = alloc(capacity);
        *text = 0;
    }

    editor();

    free(text);
    return 0;
}
