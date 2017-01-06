#include <eve.h>

const int TAB_SIZE = 4;
const char_t* tab = STR("    ");

// Text

bool Text::charForward()
{
    if (_position < _length)
    {
        ++_position;
        return true;
    }

    return false;
}

bool Text::charBack()
{
    if (_position > 0)
    {
        --_position;
        return true;
    }

    return false;
}

bool Text::wordForward()
{
    int prev = _position;
    char_t* p = _chars + _position;

    while (*p)
    {
        ++p;

        if (ISSPACE(*(p - 1)) && !ISSPACE(*p))
            break;
    }

    _position = p - _chars;
    return _position > prev;
}

bool Text::wordBack()
{
    int prev = _position;
    char_t* p = _chars + _position;

    if (p > _chars)
    {
        --p;
        while (p > _chars)
        {
            if (ISSPACE(*(p - 1)) && !ISSPACE(*p))
                break;

            --p;
        }
    }

    _position = p - _chars;
    return _position < prev;
}

bool Text::toStart()
{
    if (_position > 0)
    {
        _position = 0;
        return true;
    }

    return false;
}

bool Text::toEnd()
{
    if (_position < _length)
    {
        _position = _length;
        return true;
    }

    return false;
}

bool Text::toLineStart()
{
    if (_position > 0)
    {
        char_t* p = Text::findCharBack('\n');
        if (*p == '\n')
            ++p;

        _position = p - _chars;
        return true;
    }

    return false;
}

bool Text::toLineEnd()
{
    if (_position < _length)
    {
        _position = findChar('\n') - _chars;
        return true;
    }

    return false;
}

void Text::insertChar(char_t ch)
{
    if (ch == '\n') // new line
    {
        char_t* p = findCharBack('\n');
        if (*p == '\n' && _position > 0)
            ++p;

        char_t* q = p;
        while (*q == ' ' || *q == '\t')
            ++q;

        int len = q - p + 1;
        char_t* chars = ALLOCATE_STACK(char_t, len + 1);

        chars[0] = '\n';
        *STRNCPY(chars + 1, p, q - p) = 0;

        insert(_position, chars);
        _position += len;
    }
    else if (ch == '\t') // tab
    {
        insert(_position, tab);
        _position += TAB_SIZE;
    }
    else if (ch == 0x14) // real tab
        insert(_position++, '\t');
    else
        insert(_position++, ch);
}

bool Text::deleteCharForward()
{
    if (_position < _length)
    {
        erase(_position, 1);
        return true;
    }

    return false;
}

bool Text::deleteCharBack()
{
    if (_position > 0)
    {
        erase(--_position, 1);
        return true;
    }

    return false;
}

bool Text::deleteWordForward()
{
    int prev = _position;

    if (wordForward())
    {
        erase(prev, _position - prev);
        _position = prev;
        return true;
    }

    return false;
}

bool Text::deleteWordBack()
{
    int prev = _position;

    if (wordBack())
    {
        erase(_position, prev - _position);
        return true;
    }

    return false;
}

String Text::copyDeleteText(int pos, bool copy)
{
    ASSERT(pos > 0 && pos <= _length);

    char_t* p;
    char_t* q;

    if (pos < 0)
    {
        p = findCharBack('\n');
        if (*p == '\n' && _position > 0)
            ++p;

        q = findChar('\n');
        if (*q == '\n')
            ++q;
    }
    else
    {
        if (pos < _position)
        {
            p = _chars + pos;
            q = _chars + _position;
        }
        else
        {
            p = _chars + _position;
            q = _chars + pos;
        }
    }

    if (p < q)
    {
        String text = substr(p - _chars, q - p);

        if (!copy)
        {
            _position = p - _chars;
            erase(_position, q - p);
        }

        return text;
    }

    return String();
}

void Text::pasteText(const String& text, bool lineSelection)
{
    if (lineSelection)
    {
        char_t* p = findCharBack('\n');
        if (*p == '\n' && _position > 0)
            ++p;

        insert(p - _chars, text);
    }
    else
        insert(_position, text);

    _position += text.length();
}

char_t* Text::findChar(char_t ch) const
{
    char_t* p = _chars + _position;

    while (*p && *p != ch)
        ++p;

    return p;
}

char_t* Text::findCharBack(char_t ch) const
{
    char_t* p = _chars + _position;

    while (p > _chars)
        if (*--p == ch)
            return p;

    return p;
}

char_t* Text::findLine(int line) const
{
    ASSERT(line > 0);
    char_t* p = _chars;

    while (*p && line > 1)
    {
        if (*p++ == '\n')
            --line;
    }

    return p;
}

bool Text::findNext(const String& pattern)
{
    if (!pattern.empty())
    {
        int pos;

        if (_position < _length)
        {
            pos = find(pattern, _position + 1);
            if (pos == String::INVALID_POSITION)
                pos = find(pattern);
        }
        else
			pos = find(pattern);

		if (pos != String::INVALID_POSITION && pos != _position)
		{
			_position = pos;
            return true;
        }
    }

    return false;
}

String Text::currentWord() const
{
    char_t* p = _chars + _position;

    if (isIdent(*p))
    {
        while (p > _chars)
        {
            if (!isIdent(*(p - 1)))
                break;
            --p;
        }

        char_t* q = _chars + _position + 1;
        while (*q)
        {
            if (!isIdent(*q))
                break;
            ++q;
        }

        return substr(p - _chars, q - p);
    }

    return String();
}

void Text::trimTrailingWhitespace()
{
    char_t* p = _chars;
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

    _length = STRLEN(_chars);
}

bool Text::isIdent(char_t ch)
{
    return ISALNUM(ch) || ch == '_';
}

// Editor

Editor::Editor(const char_t* filename) :
    _filename(filename),
    _top(1), _left(1),
    _line(1), _column(1),
    _preferredColumn(1),
    _selection(-1),
    _lineSelection(false)

{
    Console::setMode(CONSOLE_MODE_NONCANONICAL | CONSOLE_MODE_NOTBUFFERED);
    Console::getSize(_width, _screenHeight);
    _height = _screenHeight - 1;
    _text.ensureCapacity(1);
}

Editor::~Editor()
{
    Console::setMode(CONSOLE_MODE_DEFAULT);
    Console::clear();
}

void Editor::run()
{
    openFile();
    updateScreen();

    while (processKey());
}

void Editor::positionToLineColumn()
{
    char_t* p = _text.chars();
    char_t* q = p + _text.position();
    _line = 1, _column = 1;

    while (*p && p < q)
    {
        if (*p == '\n')
        {
            ++_line;
            _column = 1;
        }
        else if (*p == '\t')
            _column = ((_column - 1) / TAB_SIZE + 1) * TAB_SIZE + 1;
        else
            ++_column;

        ++p;
    }

    _preferredColumn = _column;
}

void Editor::lineColumnToPosition()
{
    char_t* p = _text.chars();
    int preferredLine = _line;
    _line = 1; _column = 1;

    while (*p && _line < preferredLine)
    {
        if (*p++ == '\n')
            ++_line;
    }

    while (*p && *p != '\n' && _column < _preferredColumn)
    {
        if (*p == '\t')
            _column = ((_column - 1) / TAB_SIZE + 1) * TAB_SIZE + 1;
        else
            ++_column;

        ++p;
    }

    _text.position() = p - _text.chars();
}

void Editor::updateScreen()
{
    _screen.clear();

    if (_line < _top)
        _top = _line;
    else if (_line >= _top + _height)
        _top = _line - _height + 1;

    if (_column < _left)
        _left = _column;
    else if (_column >= _left + _width)
        _left = _column - _width + 1;

    char_t* p = _text.findLine(_top);
    int len = _left + _width - 1;

    for (int j = 1; j <= _height; ++j)
    {
#ifndef PLATFORM_WINDOWS
        bool clearedLine = false;
#endif
        for (int i = 1; i <= len; ++i)
        {
            char_t ch;

            if (*p == '\t')
            {
                ch = ' ';
                if (i == ((i - 1) / TAB_SIZE + 1) * TAB_SIZE)
                    ++p;
            }
            else if (*p == '\n')
                ch = *p;
            else if (*p)
                ch = *p++;
            else
                ch = 0;

            if (i >= _left)
            {
                if (ch == '\n')
                {
#ifdef PLATFORM_WINDOWS
                    _screen += ' ';
#else
                    if (!clearedLine)
                    {
                        _screen += '\x1b';
                        _screen += '[';
                        _screen += 'K';
                        _screen += '\n';
                        clearedLine = true;
                    }
#endif
                }
                else if (ch == 0)
                {
#ifdef PLATFORM_WINDOWS
                    _screen += ' ';
#endif
                }
                else
                    _screen += ch;
            }
        }

        if (*p == '\n')
            ++p;
        else
        {
            while (*p && *p != '\n')
                ++p;

            if (*p == '\n')
                ++p;
        }
    }

#ifdef PLATFORM_WINDOWS
    _screen.append(' ', _width);
#else
    _screen += '\x1b';
    _screen += '[';
    _screen += 'J';

    Console::showCursor(false);
#endif

    Console::write(1, 1, _screen);

    char_t lineCol[30];
    int lineColLen = SPRINTF(lineCol, STR("%d, %d"), _line, _column);

    len = _width;

    if (lineColLen <= len)
    {
        Console::write(_screenHeight, _width - lineColLen + 1, lineCol, lineColLen);
        len -= lineColLen;
        if (len > 0)
            --len;
    }

    if (_filename.length() <= len)
        Console::write(_screenHeight, 1, _filename);

    Console::setCursorPosition(_line - _top + 1, _column - _left + 1);

#ifndef PLATFORM_WINDOWS
    Console::showCursor(true);
#endif
}

bool Editor::processKey()
{
    bool update = false, modified = false;
    const Array<Key>& keys = Console::readKeys();

    for (int i = 0; i < keys.size(); ++i)
    {
        Key key = keys[i];

        if (key.ctrl)
        {
            if (key.code == KEY_RIGHT)
            {
                if (_text.wordForward())
                {
                    positionToLineColumn();
                    update = true;
                }
            }
            else if (key.code == KEY_LEFT)
            {
                if (_text.wordBack())
                {
                    positionToLineColumn();
                    update = true;
                }
            }
            else if (key.code == KEY_TAB || key.ch == 0x14)
            {
                _text.insertChar(0x14);
                positionToLineColumn();
                modified = true;
                update = true;
            }
        }
        else if (key.alt)
        {
            if (key.code == KEY_DELETE)
            {
                if (_text.deleteWordForward())
                {
                    positionToLineColumn();
                    modified = true;
                    update = true;
                }
            }
            else if (key.code == KEY_BACKSPACE)
            {
                if (_text.deleteWordBack())
                {
                    positionToLineColumn();
                    modified = true;
                    update = true;
                }
            }
            else if (key.code == KEY_HOME)
            {
                if (_text.toStart())
                {
                    positionToLineColumn();
                    update = true;
                }
            }
            else if (key.code == KEY_END)
            {
                if (_text.toEnd())
                {
                    positionToLineColumn();
                    update = true;
                }
            }
            else if (key.code == KEY_PGUP)
            {
                _line -= _height / 2;
                if (_line < 1)
                    _line = 1;

                lineColumnToPosition();
                update = true;
            }
            else if (key.code == KEY_PGDN)
            {
                _line += _height / 2;
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
                _buffer = _text.copyDeleteText(_selection, false);
                _lineSelection = _selection < 0;

                if (!_buffer.empty())
                {
                    positionToLineColumn();
                    modified = true;
                    update = true;
                }
            }
            else if (key.ch == 'c')
            {
                _buffer = _text.copyDeleteText(_selection, true);
                _lineSelection = _selection < 0;
            }
            else if (key.ch == 'p')
            {
                if (!_buffer.empty())
                {
                    _text.pasteText(_buffer, _lineSelection);
                    positionToLineColumn();
                    modified = true;
                    update = true;
                }
            }
            else if (key.ch == 'b')
            {
                buildProject();
                return true;
            }
            else if (key.ch == 'm')
            {
                _selection = _text.position();
            }
            else if (key.ch == 'f')
            {
                _pattern = getCommand(STR("find: "));

                if (_text.findNext(_pattern))
                    positionToLineColumn();
                update = true;
            }
            else if (key.ch == 'o')
            {
                _pattern = _text.currentWord();

                if (_text.findNext(_pattern))
                {
                    positionToLineColumn();
                    update = true;
                }
            }
            else if (key.ch == 'n')
            {
                if (_text.findNext(_pattern))
                {
                    positionToLineColumn();
                    update = true;
                }
            }
        }
        else if (key.code == KEY_BACKSPACE)
        {
            if (_text.deleteCharBack())
            {
                positionToLineColumn();
                modified = true;
                update = true;
            }
        }
        else if (key.code == KEY_DELETE)
        {
            if (_text.deleteCharForward())
            {
                positionToLineColumn();
                modified = true;
                update = true;
            }
        }
        else if (key.code == KEY_UP)
        {
            if (_line > 1)
            {
                --_line;
                lineColumnToPosition();
                update = true;
            }
        }
        else if (key.code == KEY_DOWN)
        {
            ++_line;
            lineColumnToPosition();
            update = true;
        }
        else if (key.code == KEY_RIGHT)
        {
            if (_text.charForward())
            {
                positionToLineColumn();
                update = true;
            }
        }
        else if (key.code == KEY_LEFT)
        {
            if (_text.charBack())
            {
                positionToLineColumn();
                update = true;
            }
        }
        else if (key.code == KEY_HOME)
        {
            if (_text.toLineStart())
            {
                positionToLineColumn();
                update = true;
            }
        }
        else if (key.code == KEY_END)
        {
            if (_text.toLineEnd())
            {
                positionToLineColumn();
                update = true;
            }
        }
        else if (key.code == KEY_PGUP)
        {
            _line -= _height - 1;
            if (_line < 1)
                _line = 1;

            lineColumnToPosition();
            update = true;
        }
        else if (key.code == KEY_PGDN)
        {
            _line += _height - 1;
            lineColumnToPosition();
            update = true;
        }
        else if (key.ch == '\n' || key.ch == '\t' || ISPRINT(key.ch))
        {
            _text.insertChar(key.ch);
            positionToLineColumn();
            modified = true;
            update = true;
        }
    }

    if (modified)
        _selection = -1;

    if (update)
        updateScreen();

    return true;
}

void Editor::openFile()
{
	File file;

	if (file.open(_filename))
		_text.assign(file.readString());
	else
		_text.clear();

    _line = 1; _column = 1;
    _preferredColumn = 1;
    _selection = -1;
}

void Editor::saveFile()
{
    _text.trimTrailingWhitespace();
    lineColumnToPosition();
    _selection = -1;

	File file(_filename, FILE_MODE_CREATE_ALWAYS);
	file.writeString(_text);
}

String Editor::getCommand(const char_t* prompt)
{
    ASSERT(prompt);

    String command;
    String cmdLine;
    
    int width = _width - STRLEN(prompt);
    if (width < 2)
        throw Exception(STR("window is too narrow"));

    while (true)
    {
        cmdLine.clear();
        cmdLine += prompt;

        int pos;
        if (command.length() < width)
        {
            cmdLine += command;
            pos = cmdLine.length() + 1;

#ifdef PLATFORM_WINDOWS
            cmdLine.append(' ', width - command.length());
#else
            cmdLine += STR("\x1b[K");
#endif
        }
        else
        {
            cmdLine += command.chars() + command.length() - width + 1;
            cmdLine += ' ';
            pos = _width;
        }

        Console::write(_screenHeight, 1, cmdLine);
        Console::setCursorPosition(_screenHeight, pos);

        const Array<Key>& keys = Console::readKeys();

        for (int i = 0; i < keys.size(); ++i)
        {
            Key key = keys[i];

            if (key.code == KEY_ENTER)
            {
                return command;
            }
            else if (key.code == KEY_ESC)
            {
                return String();
            }
            else if (key.code == KEY_BACKSPACE)
            {
                if (command.length() > 0)
                    command.erase(command.length() - 1, 1);
            }
            else if (ISPRINT(key.ch))
            {
                command += key.ch;
            }
        }
    }
}

void Editor::buildProject()
{
    Console::setMode(CONSOLE_MODE_DEFAULT);
    Console::clear();

    saveFile();
    system("gmake");

    Console::writeLine(STR("Press any key to continue..."));
    Console::setMode(CONSOLE_MODE_NONCANONICAL | CONSOLE_MODE_NOTBUFFERED);
    Console::readKeys();

    updateScreen();
}

#ifndef PLATFORM_WINDOWS

void testKeys()
{
    char chars[10];

    pollfd pfd;
    pfd.fd = STDIN_FILENO;
    pfd.events = POLLIN;
    pfd.revents = 0;

    Console::setMode(CONSOLE_MODE_NONCANONICAL | CONSOLE_MODE_NOTBUFFERED);

    while (true)
    {
        if (poll(&pfd, 1, -1) > 0)
        {
            int len;
            ioctl(STDIN_FILENO, FIONREAD, &len);

            read(STDIN_FILENO, chars, len);

            for (int i = 0; i < len; ++i)
                printf("%x ", (unsigned)chars[i]);

            for (int i = 0; i < len; ++i)
                if (isprint(chars[i]))
                    putchar(chars[i]);
                else
                    printf("\\x%x", (unsigned)chars[i]);

            printf("\n");
        }
    }

    Console::setMode(CONSOLE_MODE_DEFAULT);
}

#endif

int MAIN(int argc, const char_t** argv)
{
    if (argc != 2)
    {
        Console::writeLine(STR("usage: eve filename\n\n"
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
            "alt+Q - quit without saving\n\n"));

        return 1;
    }

    try
    {
        Editor editor(argv[1]);
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
