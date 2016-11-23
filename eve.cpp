#include <foundation.h>
#include <console.h>
#include <file.h>

const int TAB_SIZE = 4;
const char_t* tab = STR("    ");

CharArray screen;
CharArray window;

String filename;

int top, left;
int width, height, screenHeight;

int position;
int line, column, preferredColumn;
int selection;

String text;
String buffer;
String pattern;

bool isIdent(char_t ch)
{
    return ISALNUM(ch) || ch == '_';
}

char_t* findChar(char_t* str, char_t ch)
{
    while (*str && *str != ch)
        ++str;

    return str;
}

char_t* findCharBack(char_t* start, char_t* str, char_t ch)
{
    while (str > start)
        if (*--str == ch)
            return str;

    return start;
}

char_t* wordForward(char_t* str)
{
    while (*str)
    {
        if (ISSPACE(*str) && !ISSPACE(*(str + 1)))
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
            if (ISSPACE(*(str - 1)) && !ISSPACE(*str))
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

void trimTrailingWhitespace()
{
    /*char_t* p = oldtext;
    char_t* q = p;
    char_t* r = nullptr;

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

    size = STRLEN(oldtext);*/
    selection = -1;
}

void positionToLineColumn()
{
    char_t* p = text.chars();
    char_t* q = p + position;
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
    char_t* p = text.chars();
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

    position = p - text.chars();
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

    char_t* p = findLine(text.chars(), top);
    char_t* q = window.elements();
    int len = left + width - 1;

    for (int j = 1; j <= height; ++j)
    {
        for (int i = 1; i <= len; ++i)
        {
            char_t ch;

            if (*p == '\t')
            {
                ch = ' ';
                if (i == ((i - 1) / TAB_SIZE + 1) * TAB_SIZE)
                    ++p;
            }
            else if (*p && *p != '\n')
                ch = *p++;
            else
                ch = ' ';

            if (i >= left)
                *q++ = ch;
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

    STRSET(q, ' ', width);

    char_t lnCol[30];
    len = SPRINTF(lnCol, 30, STR("%d, %d"), line, column);
    if (len > 0 && len <= width)
        STRNCPY(q + width - len, lnCol, len);

    int i = 0, j = 0;
    bool matching = true;
    int nchars = width * screenHeight;
    
    Console::showCursor(false);
    
    for (; j < nchars; ++j)
    {
        if (window[j] == screen[j])
        {
            if (!matching)
            {
                Console::write(i / width + 1, i % width + 1, 
					window.elements() + i, j - i);
                i = j;
                matching = true;
            }
        }
        else
        {
            if (matching)
            {
                i = j;
                matching = false;
            }
        }
    }
    
    if (!matching)
        Console::write(i / width + 1, i % width + 1, 
			window.elements() + i, j - i);
    
    Console::setCursorPosition(line - top + 1, column - left + 1);
    Console::showCursor(true);
    
    swap(window, screen);
}

void openFile()
{
	File file;
	
	if (file.open(filename))
		text = file.readString();
	else
		text.clear();

    position = 0;
    line = 1; column = 1;
    preferredColumn = 1;
    selection = -1;
}

void saveFile()
{
    trimTrailingWhitespace();
    lineColumnToPosition();

	File file(filename, FILE_MODE_CREATE_ALWAYS);
	file.writeString(text);
}

String getCommand(const char_t* prompt)
{
    int start = STRLEN(prompt), end = start, cmdLine = screenHeight;
    char_t* cmd = (char_t*)alloca(sizeof(char_t) * width);
    
    STRSET(cmd, ' ', width);
    STRNCPY(cmd, prompt, start);
    Console::write(cmdLine, 1, cmd, width);
    Console::setCursorPosition(cmdLine, end + 1);

    while (true)
    {
        int numKeys = Console::readKeys();

        for (int i = 0; i < numKeys; ++i)
        {
            Key key = Console::key(i);

            if (key.code == KEY_ENTER)
            {
                return String(cmd, start, end - start);
            }
            else if (key.code == KEY_ESC)
            {
                return String();
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
            
            Console::write(cmdLine, 1, cmd, width);
            Console::setCursorPosition(cmdLine, end + 1);
        }
    }
}

bool deleteWordForward()
{
    int pos = wordForward(text.chars() + position) - text.chars();
    if (pos > position)
    {
        text.erase(position, pos - position);
		selection = -1;
		
        return true;
    }

    return false;
}

bool deleteWordBack()
{
    int pos = wordBack(text.chars(), text.chars() + position) - text.chars();
    if (pos < position)
    {
        text.erase(pos, position - pos);
        position = pos;
        positionToLineColumn();
		selection = -1;
		
        return true;
    }

    return false;
}

bool copyDeleteText(bool copy)
{
    char_t* p;
    char_t* q;

    if (selection < 0)
    {
        p = findCharBack(text.chars(), text.chars() + position, '\n');
        if (*p == '\n')
            ++p;

        q = findChar(text.chars() + position, '\n');
        if (*q == '\n')
            ++q;
    }
    else
    {
        if (selection < position)
        {
            p = text.chars() + selection;
            q = text.chars() + position;
        }
        else
        {
            p = text.chars() + position;
            q = text.chars() + selection;
        }

        selection = -1;
    }

    if (p < q)
    {
        buffer = text.substr(p - text.chars(), q - text.chars());

        if (!copy)
        {
            position = p - text.chars();
            positionToLineColumn();
            text.erase(position, q - p);
			selection = -1;
			
            return true;
        }
    }

    return false;
}

bool pasteText()
{
    if (!buffer.empty())
    {
		text.insert(position, buffer);
        position += buffer.length();
        positionToLineColumn();
		selection = -1;
		
        return true;
    }

    return false;
}

void buildProject()
{
    Console::setMode(CONSOLE_MODE_CANONICAL);
    Console::clear();

    saveFile();
    system("gmake");

    printf("Press ENTER to contiue...");
    getchar();

    Console::setMode(0);
}

bool findNext()
{
    if (!pattern.empty())
    {
		int pos = text.find(pattern, position);
		if (pos == String::INVALID_POSITION)
			pos = text.find(pattern);
		
		if (pos != String::INVALID_POSITION && pos != position)
		{
			position = pos;
            positionToLineColumn();
            return true;
        }
    }

    return false;
}

bool findWordAtCursor()
{
    char_t* p = text.chars() + position;

    if (isIdent(*p))
    {
        while (p > text.chars())
        {
            if (!isIdent(*(p - 1)))
                break;
            --p;
        }

        char_t* q = text.chars() + position + 1;
        while (*q)
        {
            if (!isIdent(*q))
                break;
            ++q;
        }

        pattern = text.substr(p - text.chars(), q - text.chars());
    }
    else
        pattern.clear();

    return findNext();
}

void insertChar(char_t ch)
{
    if (ch == '\n') // new line
    {
        char_t* p = findCharBack(text.chars(), text.chars() + position, '\n');
        if (*p == '\n')
            ++p;

        char_t* q = p;
        while (*q == ' ' || *q == '\t')
            ++q;

        int len = q - p + 1;
        char_t* chars = (char_t*)alloca(sizeof(char_t) * (len + 1));

        chars[0] = '\n';
        *STRNCPY(chars + 1, p, q - p) = 0;

        text.insert(position, chars);
        position += len;
    }
    else if (ch == '\t') // tab
    {
        text.insert(position, tab);
        position += TAB_SIZE;
    }
    else if (ch == 0x14) // real tab
        text.insert(position++, STR("\t"));
    else
	{
		char_t chars[2] = { ch, 0 };
        text.insert(position++, chars);
	}

    positionToLineColumn();
	selection = -1;
}

bool processKey()
{
    bool update = false;
    int numKeys = Console::readKeys();

    for (int i = 0; i < numKeys; ++i)
    {
        Key key = Console::key(i);

        if (key.ctrl)
        {
            if (key.code == KEY_RIGHT)
            {
                position = wordForward(text.chars() + position) - text.chars();
                positionToLineColumn();
                update = true;
            }
            else if (key.code == KEY_LEFT)
            {
                position = wordBack(text.chars(), text.chars() + position) - text.chars();
                positionToLineColumn();
                update = true;
            }
            else if (key.code == KEY_TAB || key.ch == 0x14)
            {
                insertChar(0x14);
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
                position = text.length();
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
                pattern = getCommand(STR("find: "));
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
                text.erase(--position, 1);
                positionToLineColumn();
				
				selection = -1;
                update = true;
            }
        }
        else if (key.code == KEY_DELETE)
        {
            if (position < text.length())
            {
                text.erase(position, 1);
				selection = -1;
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
            if (position < text.length())
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
            char_t* p = findCharBack(text.chars(), text.chars() + position, '\n');
            if (*p == '\n')
                ++p;

            position = p - text.chars();
            positionToLineColumn();
            update = true;
        }
        else if (key.code == KEY_END)
        {
            position = findChar(text.chars() + position, '\n') - text.chars();
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
        else if (key.ch == '\n' || key.ch == '\t' || ISPRINT(key.ch))
        {
            insertChar(key.ch);
            update = true;
        }
    }

    if (update)
        updateScreen();

    return true;
}

void editor()
{
    openFile();

    Console::create();
    Console::getSize(width, screenHeight);
	
    screen.resize(width * screenHeight);
    window.resize(width * screenHeight);

    height = screenHeight - 1;
    top = 1; left = 1;

    updateScreen();
    Console::setMode(0);

    while (processKey());

    Console::setMode(CONSOLE_MODE_CANONICAL);
    Console::clear();
    Console::destroy();
}

int MAIN(int argc, const char_t** argv)
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

    try
    {
        filename = argv[1];
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
