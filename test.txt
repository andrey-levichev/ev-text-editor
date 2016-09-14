#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef _WIN32

#include <malloc.h>
#include <io.h>
#include <windows.h>

#define STDIN_FILENO 0
#define STDOUT_FILENO 1

#define open _open
#define close _close
#define read _read
#define write _write

#else
    
#ifdef __sun
#include <alloca.h>
#endif

#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>

#endif

const int TAB_SIZE = 4;

enum KeyCode
{
    KEY_NONE,
    KEY_ESC,
    KEY_TAB,
    KEY_BACKSPACE,
    KEY_ENTER,
    KEY_UP,
    KEY_DOWN,
    KEY_LEFT,
    KEY_RIGHT,
    KEY_INSERT,
    KEY_DELETE,
    KEY_HOME,
    KEY_END,
    KEY_PGUP,
    KEY_PGDN
};

struct KeyEvent
{
    KeyCode code;
    char ch;
    bool ctrl;
    bool alt;
    bool shift;
};

char* screen;
const char* filename;

int top, left;
int width, height;

int position;
int line, column, preferredColumn;
int selection;

char* text;
int size, capacity;

char* buffer;
char* pattern;

char* alloc(int size)
{
    char* p = (char*)malloc(size);
    if (p)
        return p;

    fprintf(stderr, "out of memory\n");
    abort();

    return nullptr;
}

void setCharInputMode(bool charMode)
{
#ifndef _WIN32
    struct termios ta;
    tcgetattr(STDIN_FILENO, &ta);

    if (charMode)
    {
        ta.c_lflag &= ~(ECHO | ICANON);
        ta.c_cc[VTIME] = 0;
        ta.c_cc[VMIN] = 1;
    }
    else
        ta.c_lflag |= ECHO | ICANON;

    tcsetattr(STDIN_FILENO, TCSANOW, &ta);
#endif
}

void getConsoleSize()
{
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
#else
    struct winsize ws;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
    width = ws.ws_col;
    height = ws.ws_row;
#endif
}

void showCursor(bool show)
{
#ifdef _WIN32
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cci;
    
    GetConsoleCursorInfo(handle, &cci);
    cci.bVisible = show;
    SetConsoleCursorInfo(handle, &cci);
#else
    write(STDOUT_FILENO, show ? "\x1b[?25h" : "\x1b[?25l", 6);
#endif
}

void setCursorPosition(int line, int column)
{
#ifdef _WIN32
    COORD pos;
    pos.X = column - 1;
    pos.Y = line - 1;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
#else
    char cmd[30];
    sprintf(cmd, "\x1b[%d;%dH", line, column);
    write(STDOUT_FILENO, cmd, strlen(cmd));
#endif
}

void writeToConsole(int line, int column, const char* chars, int len)
{
#ifdef _WIN32
    DWORD written;
    COORD pos;
    pos.X = column - 1;
    pos.Y = line - 1;
    
    WriteConsoleOutputCharacter(GetStdHandle(STD_OUTPUT_HANDLE),  
        chars, len, pos, &written);
#else
    setCursorPosition(line, column);
    write(STDOUT_FILENO, chars, len);
#endif
}

void clearConsole()
{
#ifdef _WIN32
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    
    COORD pos = { 0, 0 };
    DWORD size = csbi.dwSize.X * csbi.dwSize.Y, written;
    
    FillConsoleOutputCharacter(handle, ' ', size, pos, &written);
    FillConsoleOutputAttribute(handle, csbi.wAttributes, size, pos, &written);
    SetConsoleCursorPosition(handle, pos);
#else
    write(STDOUT_FILENO, "\x1b[2J", 4);
    setCursorPosition(1, 1);
#endif
}

void readKey(KeyEvent& key)
{
#ifdef _WIN32
    while (true)
    {
        INPUT_RECORD event;
        DWORD numEvents = 0;
            
        ReadConsoleInput(GetStdHandle(STD_INPUT_HANDLE), &event, 1, &numEvents);
            
        if (numEvents > 0 && event.EventType == KEY_EVENT && event.Event.KeyEvent.bKeyDown)
        {
            key.ch = 0;
            
            switch (event.Event.KeyEvent.wVirtualKeyCode)
            {
                case VK_ESCAPE:
                    key.code = KEY_ESC;
                    break;
                case VK_TAB:
                    key.code = KEY_TAB;
                    key.ch = '\t';
                    break;
                case VK_BACK:
                    key.code = KEY_BACKSPACE;
                    break;
                case VK_RETURN:
                    key.code = KEY_ENTER;
                    key.ch = '\n';
                    break;
                case VK_UP:
                    key.code = KEY_UP;
                    break;
                case VK_DOWN:
                    key.code = KEY_DOWN;
                    break;
                case VK_LEFT:
                    key.code = KEY_LEFT;
                    break;
                case VK_RIGHT:
                    key.code = KEY_RIGHT;
                    break;
                case VK_INSERT:
                    key.code = KEY_INSERT;
                    break;
                case VK_DELETE:
                    key.code = KEY_DELETE;
                    break;
                case VK_HOME:
                    key.code = KEY_HOME;
                    break;
                case VK_END:
                    key.code = KEY_END;
                    break;
                case VK_PRIOR:
                    key.code = KEY_PGUP;
                    break;
                case VK_NEXT:
                    key.code = KEY_PGDN;
                    break;
                default:
                    key.code = KEY_NONE;
                    key.ch = event.Event.KeyEvent.uChar.AsciiChar;
                    break;
            }
            
            DWORD controlKeys = event.Event.KeyEvent.dwControlKeyState;
            key.ctrl = (controlKeys & (LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED)) != 0;
            key.alt = (controlKeys & (LEFT_ALT_PRESSED | RIGHT_ALT_PRESSED)) != 0;
            key.shift = (controlKeys & SHIFT_PRESSED) != 0;
            
            return;
        }
    }
#else
#endif
}

bool isIdent(char c)
{
    return isalnum(c) || c == '_';
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
        char* txt = alloc(capacity);
        memcpy(txt, text, size + 1);
        free(text);
        text = txt;
    }

    memmove(text + pos + len, text + pos, size - pos + 1);
    memcpy(text + pos, chars, len);
    size += len;
    selection = -1;
}

void deleteChars(int pos, int len)
{
    memmove(text + pos, text + pos + len, size - pos - len + 1);
    size -= len;
    selection = -1;
}

void trimTrailingWhitespace()
{
    char* p = text;
    char* q = p;
    char* r = nullptr;

    while (true)
    {
        if (*p == ' ' || *p == '\t')
        {
            if (!r)
                r = q;
        }
        else if (*p == '\n' || !*p)
        {
            if (r)
            {
                q = r;
                r = nullptr;
            }
        }
        else
            r = nullptr;

        *q = *p;

        if (!*p)
            break;

        ++p; ++q;
    }

    size = strlen(text);
    selection = -1;
}

char* copyChars(const char* start, const char* end)
{
    int len = end - start;
    char* str = alloc(len + 1);
    memcpy(str, start, len);
    str[len] = 0;

    return str;
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

    preferredColumn = column;
}

void lineColumnToPosition()
{
    char* p = text;
    int preferredLine = line;
    line = 1; column = 1;

    while (*p && line < preferredLine)
    {
        if (*p++ == '\n')
            ++line;
    }

    while (*p && *p != '\n' && column < preferredColumn)
    {
        if (*p == '\t')
            column = ((column - 1) / TAB_SIZE + 1) * TAB_SIZE + 1;
        else
            ++column;

        ++p;
    }

    position = p - text;
}

void updateScreen()
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

    memset(q, ' ', width);

    char lnCol[30];
    len = sprintf(lnCol, "%d, %d", line, column);
    if (len > 0 && len <= width)
        memcpy(q + width - len, lnCol, len);

    writeToConsole(1, 1, screen, width * (height + 1));
    setCursorPosition(line - top + 1, column - left + 1);
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
    
    size = read(file, text, size);

    if (size >= 0)
    {
        text[size] = 0;
        close(file);
        return true;
    }
    else
    {
        free(text);
        close(file);
        return false;
    }
}

bool writeFile(const char* fileName)
{
    int file = open(fileName, O_WRONLY | O_CREAT | O_TRUNC,
#ifdef _WIN32
        _S_IREAD | _S_IWRITE);
#else
        S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
#endif

    if (file < 0)
        return false;

    if (write(file, text, size) >= 0)
    {
        close(file);
        return true;
    }
    else
    {
        close(file);
        return false;
    }
}

void openFile()
{
    if (!readFile(filename))
    {
        size = 0;
        capacity = 1;
        text = alloc(capacity);
        *text = 0;
    }

    position = 0;
    line = 1; column = 1;
    preferredColumn = 1;
    selection = -1;
}

void saveFile()
{
    trimTrailingWhitespace();
    lineColumnToPosition();

    if (!writeFile(filename))
        fprintf(stderr, "failed to save text to %s\n", filename);
}

char* getCommand(const char* prompt)
{
    int start = strlen(prompt), end = start, cmdLine = height + 1;
    char* cmd = (char*)alloca(width);
    
    memset(cmd, ' ', width);
    memcpy(cmd, prompt, start);
    writeToConsole(cmdLine, 1, cmd, width);
    setCursorPosition(cmdLine, end + 1);

    while (true)
    {
        KeyEvent key;
        readKey(key);

        if (key.code == KEY_ENTER)
        {
            return copyChars(cmd + start, cmd + end);
        }
        else if (key.code == KEY_ESC)
        {
            return nullptr;
        }
        else if (key.code == KEY_BACKSPACE)
        {
            if (end > start)
                cmd[--end] = ' ';
        }
        else
        {
            if (end < width)
                cmd[end++] = key.ch;
        }
        
        writeToConsole(cmdLine, 1, cmd, width);
        setCursorPosition(cmdLine, end + 1);
    }
}

bool deleteWordForward()
{
    int pos = wordForward(text + position) - text;
    if (pos > position)
    {
        deleteChars(position, pos - position);
        return true;
    }

    return false;
}

bool deleteWordBack()
{
    int pos = wordBack(text, text + position) - text;
    if (pos < position)
    {
        deleteChars(pos, position - pos);
        position = pos;
        positionToLineColumn();
        return true;
    }

    return false;
}

bool copyDeleteText(bool copy)
{
    char* p;
    char* q;

    if (selection < 0)
    {
        p = findCharBack(text, text + position, '\n');
        if (*p == '\n')
            ++p;

        q = findChar(text + position, '\n');
        if (*q == '\n')
            ++q;
    }
    else
    {
        if (selection < position)
        {
            p = text + selection;
            q = text + position;
        }
        else
        {
            p = text + position;
            q = text + selection;
        }

        selection = -1;
    }

    if (p < q)
    {
        free(buffer);
        buffer = copyChars(p, q);

        if (!copy)
        {
            position = p - text;
            positionToLineColumn();
            deleteChars(position, q - p);
            return true;
        }
    }

    return false;
}

bool pasteText()
{
    if (buffer)
    {
        int len = strlen(buffer);
        insertChars(buffer, position, len);
        position += len;
        positionToLineColumn();
        return true;
    }

    return false;
}

void buildProject()
{
    setCharInputMode(false);
    clearConsole();

    saveFile();
    system("gmake");

    printf("Press ENTER to contiue...");
    getchar();

    setCharInputMode(true);
}

bool findNext()
{
    if (pattern)
    {
        char* p = text + position;
        char* q = strstr(p + 1, pattern);
        if (!q)
            q = strstr(text, pattern);

        if (q && q != p)
        {
            position = q - text;
            positionToLineColumn();
            return true;
        }
    }

    return false;
}

bool findWordAtCursor()
{
    char* p = text + position;
    free(pattern);

    if (isIdent(*p))
    {
        while (p > text)
        {
            if (!isIdent(*(p - 1)))
                break;
            --p;
        }

        char* q = text + position + 1;
        while (*q)
        {
            if (!isIdent(*q))
                break;
            ++q;
        }

        pattern = copyChars(p, q);
    }
    else
        pattern = nullptr;

    return findNext();
}

void insertChar(char c)
{
    if (c == '\n') // Enter
    {
        char* p = findCharBack(text, text + position, '\n');
        if (*p == '\n')
            ++p;

        char* q = p;
        while (*q == ' ' || *q == '\t')
            ++q;

        int len = q - p + 1;
        char* chars = (char*)alloca(len);

        chars[0] = '\n';
        memcpy(chars + 1, p, q - p);

        insertChars(chars, position, len);
        position += len;
    }
    else if (c == '\t') // Tab
    {
        char chars[16];
        memset(chars, ' ', TAB_SIZE);
        insertChars(chars, position, TAB_SIZE);
        position += TAB_SIZE;
    }
    else if (c == 0x14) // real tab
        insertChars("\t", position++, 1);
    else
        insertChars(&c, position++, 1);

    positionToLineColumn();
}

bool processKey()
{
    bool update = false;
    KeyEvent key;
    
    readKey(key);

    if (key.ctrl)
    {
        if (key.code == KEY_RIGHT)
        {
            position = wordForward(text + position) - text;
            positionToLineColumn();
            update = true;
        }
        else if (key.code == KEY_LEFT)
        {
            position = wordBack(text, text + position) - text;
            positionToLineColumn();
            update = true;
        }
    }
    else if (key.alt)
    {
        if (key.code == KEY_DELETE)
        {
            update = deleteWordForward();
        }
        else if (key.code == KEY_BACKSPACE)
        {
            update = deleteWordBack();
        }
        else if (key.code == KEY_HOME)
        {
            position = 0;
            positionToLineColumn();
            update = true;
        }
        else if (key.code == KEY_END)
        {
            position = size;
            positionToLineColumn();
            update = true;
        }
        else if (key.code == KEY_PGUP)
        {
            line -= height / 2;
            if (line < 1)
                line = 1;

            lineColumnToPosition();
            update = true;
        }
        else if (key.code == KEY_PGDN)
        {
            line += height / 2;
            lineColumnToPosition();
            update = true;
        }
        else if (key.ch == 'q')
        {
            return false;
        }
        else if (key.ch == 's')
        {
            saveFile();
            update = true;
        }
        else if (key.ch == 'x')
        {
            saveFile();
            return false;
        }
        else if (key.ch == 'd')
        {
            update = copyDeleteText(false);
        }
        else if (key.ch == 'c')
        {
            update = copyDeleteText(true);
        }
        else if (key.ch == 'p')
        {
            update = pasteText();
        }
        else if (key.ch == 'b')
        {
            buildProject();
            update = true;
        }
        else if (key.ch == 'm')
        {
            selection = position;
        }
        else if (key.ch == 'f')
        {
            free(pattern);
            pattern = getCommand("find: ");
            findNext();
            update = true;
        }
        else if (key.ch == 'o')
        {
            update = findWordAtCursor();
        }
        else if (key.ch == 'n')
        {
            update = findNext();
        }
    }
    else if (key.code == KEY_BACKSPACE)
    {
        if (position > 0)
        {
            deleteChars(--position, 1);
            positionToLineColumn();
            update = true;
        }
    }
    else if (key.code == KEY_DELETE)
    {
        if (position < size)
        {
            deleteChars(position, 1);
            update = true;
        }
    }
    else if (key.code == KEY_UP)
    {
        if (line > 1)
        {
            --line;
            lineColumnToPosition();
            update = true;
        }
    }
    else if (key.code == KEY_DOWN)
    {
        ++line;
        lineColumnToPosition();
        update = true;
    }
    else if (key.code == KEY_RIGHT)
    {
        if (position < size)
        {
            ++position;
            positionToLineColumn();
            update = true;
        }
    }
    else if (key.code == KEY_LEFT)
    {
        if (position > 0)
        {
            --position;
            positionToLineColumn();
            update = true;
        }
    }
    else if (key.code == KEY_HOME)
    {
        char* p = findCharBack(text, text + position, '\n');
        if (*p == '\n')
            ++p;

        position = p - text;
        positionToLineColumn();
        update = true;
    }
    else if (key.code == KEY_END)
    {
        position = findChar(text + position, '\n') - text;
        positionToLineColumn();
        update = true;
    }
    else if (key.code == KEY_PGUP)
    {
        line -= height - 1;
        if (line < 1)
            line = 1;

        lineColumnToPosition();
        update = true;
    }
    else if (key.code == KEY_PGDN)
    {
        line += height - 1;
        lineColumnToPosition();
        update = true;
    }
    else if (key.ch == '\n' || key.ch == '\t' || 
        key.ch == 0x14 || isprint(key.ch))
    {
        insertChar(key.ch);
        update = true;
    }

    if (update)
        updateScreen();

    return true;
}

void editor()
{
    openFile();

    getConsoleSize();
    screen = alloc(width * height);
    setCharInputMode(true);

    --height;
    top = 1; left = 1;

    updateScreen();

    while (processKey());

    setCharInputMode(false);
    clearConsole();

    free(screen);
    free(text);
    free(buffer);
    free(pattern);
}

int main(int argc, const char** argv)
{
    if (argc != 2)
    {
        printf("usage: eve filename\n\n"
            "keys:\n"
            "arrows - move cursor\n"
            "ctrl+left/right - word back/forward\n"
            "home/end - start/end of line\n"
            "alt+home/end - start/end of file\n"
            "pgup/pgdn - page up/down\n"
            "alt+pgup/pgdn - half page up/down\n"
            "del - delete character at cursor position\n"
            "backspace - delete character to the left of cursor position\n"
            "alt+del - delete word at cursor position\n"
            "alt+backspace - delete word to the left of cursor position\n"
            "alt+M - mark selection start\n"
            "alt+D - delete line/selection\n"
            "alt+C - copy line/selection\n"
            "alt+P - paste line/selection\n"
            "alt+F - find\n"
            "alt+O - find word at cursor\n"
            "alt+N - find again\n"
            "alt+B - build with make\n"
            "alt+S - save\n"
            "alt+X - quit and save\n"
            "alt+Q - quit without saving\n\n");

        return 1;
    }

    filename = argv[1];
    editor();

    return 0;
}
