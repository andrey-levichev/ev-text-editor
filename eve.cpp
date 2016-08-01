#include <foundation.h>
#include <alloca.h>

const int TAB_SIZE = 4;

String fileName;

int top, left;
int width, height;

int position;
int line, column, preferredColumn;
int selection;

char_t* text;
int size, capacity;

char_t* buffer;
char_t* pattern;

bool isIdent(char_t c)
{
    return isalnum(c) || c == '_';
}

char_t* findChar(char_t* str, char_t c)
{
    while (*str && *str != c)
        ++str;

    return str;
}

char_t* findCharBack(char_t* start, char_t* str, char_t c)
{
    while (str > start)
        if (*--str == c)
            return str;

    return start;
}

char_t* wordForward(char_t* str)
{
    while (*str)
    {
        if (isspace(*str) && !isspace(*(str + 1)))
            return str + 1;
        ++str;
    }

    return str;
}

char_t* wordBack(char_t* start, char_t* str)
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

char_t* findLine(char_t* str, int line)
{
    while (*str && line > 1)
    {
        if (*str++ == '\n')
            --line;
    }

    return str;
}

void insertChars(const char_t* chars, int pos, int len)
{
    int cap = size + len + 1;
    if (cap > capacity)
    {
        capacity = cap * 2;
        char_t* txt = Memory::allocateArray<char_t>(capacity);
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
    char_t* p = text;
    char_t* q = p;
    char_t* r = NULL;

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
                r = NULL;
            }
        }
        else
            r = NULL;

        *q = *p;

        if (!*p)
            break;

        ++p; ++q;
    }

    size = strlen(text);
    selection = -1;
}

char_t* copyChars(const char_t* start, const char_t* end)
{
    int len = end - start;
    char_t* str = Memory::allocateArray<char_t>(len + 1);
    memcpy(str, start, len);
    str[len] = 0;

    return str;
}

void positionToLineColumn()
{
    char_t* p = text;
    char_t* q = text + position;
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
    char_t* p = text;
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

void redrawScreen()
{
}

void updateScreen()
{
    char* screen = (char*)alloca(width * height);

    if (line < top)
        top = line;
    else if (line >= top + height)
        top = line - height + 1;

    if (column < left)
        left = column;
    else if (column >= left + width)
        left = column - width + 1;

    char_t* p = findLine(text, top);
    char_t* q = screen;
    int len = left + width - 1;

    for (int j = 1; j <= height; ++j)
    {
        for (int i = 1; i <= len; ++i)
        {
            char_t c;

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

    char_t lnCol[30];
    len = sprintf(lnCol, "%d, %d", line, column);
    if (len > 0 && len <= width)
        memcpy(q + width - len, lnCol, len);

    Console::hideCursor();
    Console::setCursorPosition(1, 1);
    write(STDOUT_FILENO, screen, width * (height + 1));

    Console::setCursorPosition(line - top + 1, column - left + 1);
    Console::showCursor();
}

void openFile()
{
    File file;

    if (file.open(fileName))
    {
        text = file.readString().release();
        capacity = strlen(text);
        size = 0;
    }
    else
    {
        size = 0;
        capacity = 1;
        text = Memory::allocateArray<char_t>(capacity);
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

    File file(fileName);
    file.writeString(text);
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

bool copyDeleteText(bool deleteFlag)
{
    char_t* p;
    char_t* q;

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

        if (deleteFlag)
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
    Console::setLineInputMode();
    Console::clearScreen();

    saveFile();
    system("make");

    printf("Press ENTER to contiue...");
    getchar();

    Console::setCharInputMode();
}

bool findNext()
{
    if (pattern)
    {
        char_t* p = text + position;
        char_t* q = strstr(p + 1, pattern);
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

void findWord()
{
    free(pattern);

    int pos = 0, h = height + 1;
    char_t* cmd = (char_t*)alloca(width);

    memset(cmd, ' ', width);
    memcpy(cmd, "find:", 5);
    Console::setCursorPosition(h, 1);
    write(STDOUT_FILENO, cmd, width);
    Console::setCursorPosition(h, 7);

    while (true)
    {
        char_t key;
        read(STDIN_FILENO, &key, 1);

        if (key == 0x7f) // Backspace
        {
            if (pos > 0)
            {
                --pos;
                write(STDOUT_FILENO, &key, 1);
            }
        }
        else if (key == '\n') // Enter
        {
            pattern = copyChars(cmd, cmd + pos);
            break;
        }
        else if (key == 0x1b) // Esc
        {
            break;
        }
        else
        {
            if (pos < width)
            {
                Console::setCursorPosition(h, pos + 7);
                write(STDOUT_FILENO, &key, 1);
                cmd[pos++] = key;
            }
        }
    }

    findNext();
}

bool findWordAtCursor()
{
    char_t* p = text + position;
    free(pattern);

    if (isIdent(*p))
    {
        while (p > text)
        {
            if (!isIdent(*(p - 1)))
                break;
            --p;
        }

        char_t* q = text + position + 1;
        while (*q)
        {
            if (!isIdent(*q))
                break;
            ++q;
        }

        pattern = copyChars(p, q);
    }
    else
        pattern = NULL;

    return findNext();
}

void insertChar(char_t c)
{
    if (c == '\n') // Enter
    {
        char_t* p = findCharBack(text, text + position, '\n');
        if (*p == '\n')
            ++p;

        char_t* q = p;
        while (*q == ' ' || *q == '\t')
            ++q;

        int len = q - p + 1;
        char_t* chars = (char_t*)alloca(len);

        chars[0] = '\n';
        memcpy(chars + 1, p, q - p);

        insertChars(chars, position, len);
        position += len;
    }
    else if (c == '\t') // Tab
    {
        char_t chars[16];
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
    char_t keys[1024];
    char_t* key = keys;
    bool update = false;

    int len = read(STDIN_FILENO, keys, sizeof(keys) - 1);
    keys[len] = 0;

    while (*key)
    {
        /*printf("%x\n", ((unsigned char_t)*key++));
        continue;*/

        if (*key == 0x7f) // Backspace
        {
            if (position > 0)
            {
                deleteChars(--position, 1);
                positionToLineColumn();
                update = true;
            }

            ++key;
        }
        else if (*key == 0x1b)
        {
            if (!strcmp(key, "\x1b\x5b\x41")) // up
            {
                if (line > 1)
                {
                    --line;
                    lineColumnToPosition();
                    update = true;
                }

                key += 3;
            }
            else if (!strcmp(key, "\x1b\x5b\x42")) // down
            {
                ++line;
                lineColumnToPosition();
                update = true;
                key += 3;
            }
            else if (!strcmp(key, "\x1b\x5b\x43")) // right
            {
                if (position < size)
                {
                    ++position;
                    positionToLineColumn();
                    update = true;
                }

                key += 3;
            }
            else if (!strcmp(key, "\x1b\x5b\x44")) // left
            {
                if (position > 0)
                {
                    --position;
                    positionToLineColumn();
                    update = true;
                }

                key += 3;
            }
            else if (!strcmp(key, "\x1b\x5b\x35\x7e")) // PgUp
            {
                line -= height - 1;
                if (line < 1)
                    line = 1;

                lineColumnToPosition();
                update = true;
                key += 4;
            }
            else if (!strcmp(key, "\x1b\x5b\x36\x7e")) // PgDn
            {
                line += height - 1;
                lineColumnToPosition();
                update = true;
                key += 4;
            }
            else if (!strcmp(key, "\x1b\x5b\x31\x7e")) // Home
            {
                char_t* p = findCharBack(text, text + position, '\n');
                if (*p == '\n')
                    ++p;

                position = p - text;
                positionToLineColumn();
                update = true;
                key += 4;
            }
            else if (!strcmp(key, "\x1b\x5b\x34\x7e")) // End
            {
                position = findChar(text + position, '\n') - text;
                positionToLineColumn();
                update = true;
                key += 4;
            }
            else if (!strcmp(key, "\x1b\x5b\x32\x7e")) // Insert
            {
                key += 4;
            }
            else if (!strcmp(key, "\x1b\x5b\x33\x7e")) // Delete
            {
                if (position < size)
                {
                    deleteChars(position, 1);
                    update = true;
                }

                key += 4;
            }
            else if (!strcmp(key, "\x1b\x4f\x41")) // ^up
            {
                key += 3;
            }
            else if (!strcmp(key, "\x1b\x4f\x42")) // ^down
            {
                key += 3;
            }
            else if (!strcmp(key, "\x1b\x4f\x43")) // ^right
            {
                position = wordForward(text + position) - text;
                positionToLineColumn();
                update = true;
                key += 3;
            }
            else if (!strcmp(key, "\x1b\x4f\x44")) // ^left
            {
                position = wordBack(text, text + position) - text;
                positionToLineColumn();
                update = true;
                key += 3;
            }
            else if (!strcmp(key, "\x1b\x1b\x5b\x41")) // alt+up
            {
                key += 4;
            }
            else if (!strcmp(key, "\x1b\x1b\x5b\x42")) // alt+down
            {
                key += 4;
            }
            else if (!strcmp(key, "\x1b\x1b\x5b\x43")) // alt+right
            {
                key += 4;
            }
            else if (!strcmp(key, "\x1b\x1b\x5b\x44")) // alt+left
            {
                key += 4;
            }
            else if (!strcmp(key, "\x1b\x1b\x5b\x35\x7e")) // alt+PgUp
            {
                line -= height / 2;
                if (line < 1)
                    line = 1;

                lineColumnToPosition();
                update = true;
                key += 5;
            }
            else if (!strcmp(key, "\x1b\x1b\x5b\x36\x7e")) // alt+PgDn
            {
                line += height / 2;
                lineColumnToPosition();
                update = true;
                key += 5;
            }
            else if (!strcmp(key, "\x1b\x1b\x5b\x31\x7e")) // alt+Home
            {
                position = 0;
                positionToLineColumn();
                update = true;
                key += 5;
            }
            else if (!strcmp(key, "\x1b\x1b\x5b\x34\x7e")) // alt+End
            {
                position = size;
                positionToLineColumn();
                update = true;
                key += 5;
            }
            else if (!strcmp(key, "\x1b\x1b\x5b\x33\x7e")) // alt+Delete
            {
                update = deleteWordForward();
                key += 5;
            }
            else if (!strcmp(key, "\x1b\x7f")) // alt+Backspace
            {
                update = deleteWordBack();
                key += 2;
            }
            else if (!strcmp(key, "\x1b\x71")) // alt+q
            {
                return false;
            }
            else if (!strcmp(key, "\x1b\x73")) // alt+s
            {
                saveFile();
                update = true;
                key += 2;
            }
            else if (!strcmp(key, "\x1b\x78")) // alt+x
            {
                saveFile();
                return false;
            }
            else if (!strcmp(key, "\x1b\x64") || !strcmp(key, "\x1b\x63")) // alt+d or alt+c
            {
                update = copyDeleteText(*(key + 1) == 0x64);
                key += 2;
            }
            else if (!strcmp(key, "\x1b\x70")) // alt+p
            {
                update = pasteText();
                key += 2;
            }
            else if (!strcmp(key, "\x1b\x62")) // alt+b
            {
                buildProject();
                update = true;
                key += 2;
            }
            else if (!strcmp(key, "\x1b\x6d")) // alt+m
            {
                selection = position;
                key += 2;
            }
            else if (!strcmp(key, "\x1b\x66")) // alt+f
            {
                findWord();
                update = true;
                key += 2;
            }
            else if (!strcmp(key, "\x1b\x6f")) // alt+o
            {
                update = findWordAtCursor();
                key += 2;
            }
            else if (!strcmp(key, "\x1b\x6e")) // alt+n
            {
                update = findNext();
                key += 2;
            }
            else
            {
                ++key;
                if (*key == 0x5b)
                {
                    ++key;
                    while (*key && *key != 0x7e)
                        ++key;

                    if (*key == 0x7e)
                        ++key;
                }
            }
        }
        else if (*key == '\n' || *key == '\t' || *key == 0x14 || isprint(*key))
        {
            insertChar(*key);
            update = true;
            ++key;
        }
        else
            ++key;
    }

    if (update)
        updateScreen();

    return true;
}

void editor()
{
    openFile();

    Console::getSize(width, height);
    Console::setCharInputMode();

    --height;
    top = 1; left = 1;

    updateScreen();

    while (processKey());

    Console::setLineInputMode();
    Console::clearScreen();

    free(text);
    free(buffer);
    free(pattern);
}

extern void testFoundation();

int MAIN(int argc, const char_t** argv)
{
    try
    {
        testFoundation();
        return 0;

        if (argc != 2)
        {
            Console::writeLine(STR("usage: eve filename\n\n"
                "keys:\n"
                "arrows - move cursor\n"
                "ctrl+left/right - word back/forward\n"
                "ctrl+up/down - paragraph back/forward\n"
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
                "alt+Q - quit without saving\n\n"));

            return 1;
        }

        fileName = argv[1];
        editor();
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
