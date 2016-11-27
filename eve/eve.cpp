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
    char_t* p = _chars + _position;

    while (*p)
    {
        ++p;

        if (ISSPACE(*p - 1) && !ISSPACE(*(p)))
        {
            _position = p - _chars;
            return true;
        }
    }

    return false;
}

bool Text::wordBack()
{
    char_t* p = _chars + _position;

    if (p > _chars)
    {
        --p;
        while (p > _chars)
        {
            if (ISSPACE(*(p - 1)) && !ISSPACE(*p))
            {
                _position = p - _chars;
                return true;
            }

            --p;
        }
    }

    return false;
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
    char_t* p = Text::findCharBack('\n');
    if (*p == '\n')
        ++p;

    int pos = p - _chars;
    if (pos < _position)
    {
        _position = pos;
        return true;
    }

    return false;
}

bool Text::toLineEnd()
{
    int pos = findChar('\n') - _chars;
    if (pos > _position)
    {
        _position = pos;
        return true;
    }

    return false;
}

void Text::insertChar(char_t ch)
{
    if (ch == '\n') // new line
    {
        char_t* p = findCharBack('\n');
        if (*p == '\n')
            ++p;

        char_t* q = p;
        while (*q == ' ' || *q == '\t')
            ++q;

        int len = q - p + 1;
        char_t* chars = (char_t*)alloca(sizeof(char_t) * (len + 1));

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
        insert(_position++, STR("\t"));
    else
	{
		char_t chars[2] = { ch, 0 };
        insert(_position++, chars);
	}
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
    char_t* p;
    char_t* q;

    if (pos < 0)
    {
        p = findCharBack('\n');
        if (*p == '\n')
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
            return text;
        }
    }

    return String();
}

void Text::pasteText(const String& text)
{
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
		int pos = find(pattern, _position);
        if (pos == _position)
            pos = find(pattern, _position + pattern.length());

		if (pos == String::INVALID_POSITION)
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
    _filename(filename)
{
    Console::create();
    Console::getSize(_width, _screenHeight);
    Console::setMode(0);

    _height = _screenHeight - 1;
    _top = 1; _left = 1;
    
    _line = _column = _preferredColumn = 1;
    _selection = -1;

    _screen.resize(_width * _height);
    _window.resize(_width * _height);
    _commandLine.resize(_width);
}

Editor::~Editor()
{
    Console::setMode(CONSOLE_MODE_CANONICAL);
    Console::clear();
    Console::destroy();
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
    if (_line < _top)
        _top = _line;
    else if (_line >= _top + _height)
        _top = _line - _height + 1;

    if (_column < _left)
        _left = _column;
    else if (_column >= _left + _width)
        _left = _column - _width + 1;

    char_t* p = _text.findLine(_top);
    char_t* q = _window.elements();
    int len = _left + _width - 1;

    for (int j = 1; j <= _height; ++j)
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

            if (i >= _left)
                *q++ = ch;
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

    int i = 0, j = 0;
    bool matching = true;
    int nchars = _width * _height;
    
    Console::showCursor(false);
    
    for (; j < nchars; ++j)
    {
        if (_window[j] == _screen[j])
        {
            if (!matching)
            {
                Console::write(i / _width + 1, i % _width + 1, 
					_window.elements() + i, j - i);
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
        Console::write(i / _width + 1, i % _width + 1, 
			_window.elements() + i, j - i);
    
    STRSET(_commandLine.elements(), ' ', _width);

    char_t lnCol[30];
    len = SPRINTF(lnCol, 30, STR("%d, %d"), _line, _column);
    if (len > 0 && len <= _width)
        STRNCPY(_commandLine.elements() + _width - len, lnCol, len);

    Console::write(_screenHeight, 1, _commandLine.elements(), _width);

    Console::setCursorPosition(_line - _top + 1, _column - _left + 1);
    Console::showCursor(true);
    
    swap(_window, _screen);
}

bool Editor::processKey()
{
    bool update = false, modified = false;
    int numKeys = Console::readKeys();

    for (int i = 0; i < numKeys; ++i)
    {
        Key key = Console::key(i);

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
            }
            else if (key.ch == 'p')
            {
                if (!_buffer.empty())
                {
                    _text.pasteText(_buffer);
                    positionToLineColumn();
                    modified = true;
                    update = true;
                }
            }
            else if (key.ch == 'b')
            {
                buildProject();
                update = true;
            }
            else if (key.ch == 'm')
            {
                _selection = _text.position();
            }
            else if (key.ch == 'f')
            {
                _pattern = getCommand(STR("find: "));

                if (_text.findNext(_pattern))
                {
                    positionToLineColumn();
                    update = true;
                }
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
    int start = STRLEN(prompt), end = start;
    
    STRSET(_commandLine.elements(), ' ', _width);
    STRNCPY(_commandLine.elements(), prompt, start);
    Console::write(_screenHeight, 1, _commandLine.elements(), _width);
    Console::setCursorPosition(_screenHeight, end + 1);

    while (true)
    {
        int numKeys = Console::readKeys();

        for (int i = 0; i < numKeys; ++i)
        {
            Key key = Console::key(i);

            if (key.code == KEY_ENTER)
            {
                return String(_commandLine.elements(), start, end - start);
            }
            else if (key.code == KEY_ESC)
            {
                return String();
            }
            else if (key.code == KEY_BACKSPACE)
            {
                if (end > start)
                    _commandLine[--end] = ' ';
            }
            else
            {
                if (end < _width)
                    _commandLine[end++] = key.ch;
            }
            
            Console::write(_screenHeight, 1, _commandLine.elements(), _width);
            Console::setCursorPosition(_screenHeight, end + 1);
        }
    }
}

void Editor::buildProject()
{
    Console::setMode(CONSOLE_MODE_CANONICAL);
    Console::clear();

    saveFile();
    system("gmake");

    Console::writeLine(STR("Press ENTER to contiue..."));
    getchar();

    Console::setMode(0);
}

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