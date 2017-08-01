#include <eve.h>

const int TAB_SIZE = 4;
const char_t* tab = STR("    ");

// Text

bool Text::moveForward()
{
    if (_position < _chars + _length)
    {
        _position = charForward(_position);
        return true;
    }

    return false;
}

bool Text::moveBack()
{
    if (_position > _chars)
    {
        _position = charBack(_position);
        return true;
    }

    return false;
}

bool Text::moveWordForward()
{
    char_t* start = _position;

    if (*_position)
    {
        unichar_t prevChar = charAt(_position);
        _position = charForward(_position);

        while (*_position)
        {
            unichar_t ch = charAt(_position);
            if (charIsSpace(prevChar) && !charIsSpace(ch))
                break;

            prevChar = ch;
            _position = charForward(_position);
        }
    }

    return _position > start;
}

bool Text::moveWordBack()
{
    char_t* start = _position;

    if (_position > _chars)
    {
        _position = charBack(_position);

        if (_position > _chars)
        {
            unichar_t prevChar = charAt(_position);
            char_t* prevPos = _position;

            do
            {
                _position = charBack(_position);

                unichar_t ch = charAt(_position);
                if (charIsSpace(ch) && !charIsSpace(prevChar))
                {
                    _position = prevPos;
                    break;
                }

                prevChar = ch;
                prevPos = _position;
            }
            while (_position > _chars);
        }
    }

    return _position < start;
}

bool Text::moveToStart()
{
    if (_position > _chars)
    {
        _position = _chars;
        return true;
    }

    return false;
}

bool Text::moveToEnd()
{
    if (_position < _chars + _length)
    {
        _position = _chars + _length;
        return true;
    }

    return false;
}

bool Text::moveToLineStart()
{
    char_t* p = findCurrentLineStart();

    if (p < _position)
    {
        _position = p;
        return true;
    }
    else
        return false;
}

bool Text::moveToLineEnd()
{
    char_t* p = findCurrentLineEnd();

    if (p > _position)
    {
        _position = p;
        return true;
    }
    else
        return false;
}

void Text::insertChar(unichar_t ch)
{
    ASSERT(ch != 0);

    if (ch == '\n') // new line
    {
        char_t* p = findCurrentLineStart();
        _indent.assign('\n');
        unichar_t ch;

        ch = charAt(p);
        while (charIsSpace(ch))
        {
            _indent += ch;
            p = charForward(p);
            ch = charAt(p);
        }

        _position = insert(_position, _indent) + _indent.length();
    }
    else if (ch == '\t') // tab
    {
        _position = charForward(insert(_position, tab), TAB_SIZE);
    }
    else if (ch == 0x14) // real tab
    {
        _position = charForward(insert(_position, '\t'));
    }
    else
    {
        _position = charForward(insert(_position, ch));
    }
}

bool Text::deleteCharForward()
{
    if (_position < _position + _length)
    {
        erase(_position, charForward(_position) - _position);
        return true;
    }

    return false;
}

bool Text::deleteCharBack()
{
    if (_position > _chars)
    {
        char_t* prev = _position;
        _position = charBack(_position);
        erase(_position, prev - _position);
        return true;
    }

    return false;
}

bool Text::deleteWordForward()
{
    char_t* prev = _position;

    if (moveWordForward())
    {
        erase(prev, _position - prev);
        _position = prev;
        return true;
    }

    return false;
}

bool Text::deleteWordBack()
{
    char_t* prev = _position;

    if (moveWordBack())
    {
        erase(_position, prev - _position);
        return true;
    }

    return false;
}

String Text::copyDeleteText(int pos, bool copy)
{
    ASSERT(pos < 0 || (pos >= 0 && pos <= _length));

    char_t* start;
    char_t* end;

    if (pos < 0)
    {
        start = findCurrentLineStart();
        end = findCurrentLineEnd();
        if (charAt(end) == '\n')
            end = charForward(end);
    }
    else
    {
        char_t* p = _chars + pos;

        if (p < _position)
        {
            start = p;
            end = _position;
        }
        else
        {
            start = _position;
            end = p;
        }
    }
    
    if (start < end)
    {
        String text = substr(start, end - start);

        if (!copy)
        {
            _position = start;
            erase(start, end - start);
        }

        return text;
    }

    return String();
}

void Text::pasteText(const String& text, bool lineSelection)
{
    if (lineSelection)
    {
        char_t* p = findCurrentLineStart();
        _position = insert(p, text) + text.length();
    }
    else
        _position = insert(_position, text) + text.length();
}

char_t* Text::findCurrentLineStart()
{
    char_t* p = _position;

    while (p > _chars)
    {
        char* q = charBack(p);
        if (charAt(q) == '\n')
            break;
        p = q;
    }

    return p;
}

char_t* Text::findCurrentLineEnd()
{
    char_t* p = _position;

    while (*p)
    {
        if (charAt(p) == '\n')
            break;
        p = charForward(p);
    }

    return p;
}

char_t* Text::findLine(int line)
{
    ASSERT(line > 0);
    char_t* p = _chars;

    while (*p && line > 1)
    {
        if (charAt(p) == '\n')
            --line;
        p = charForward(p);
    }

    return p;
}

bool Text::findNext(const String& pattern)
{
    if (!pattern.empty())
    {
        char_t* pos;

        if (_position < _chars + _length)
        {
            pos = find(pattern, charForward(_position));
            if (!pos)
                pos = find(pattern);
        }
        else
			pos = find(pattern);

		if (pos && pos != _position)
		{
			_position = pos;
            return true;
        }
    }

    return false;
}

String Text::currentWord()
{
    char_t* start = _position;

    if (isIdent(charAt(start)))
    {
        while (start > _chars)
        {
            char* p = charBack(start);
            if (!isIdent(charAt(p)))
                break;
            start = p;
        }

        char_t* end = charForward(_position);
        while (*end)
        {
            if (!isIdent(charAt(end)))
                break;
            end = charForward(end);
        }

        return substr(start, end - start);
    }

    return String();
}

void Text::trimTrailingWhitespace()
{
    String trimmed;

    char_t* p = _chars;
    char_t* start = p;
    char_t* whitespace = NULL;

    while (true)
    {
        unichar_t ch = charAt(p);

        if (ch == '\n' || ch == 0)
        {
            if (whitespace)
            {
                trimmed.append(start, whitespace - start);
                start = p;
                whitespace = NULL;
            }
        }
        else if (charIsSpace(ch))
        {
            if (!whitespace)
                whitespace = p;
        }
        else
            whitespace = NULL;

        if (*p)
            p = charForward(p);
        else
            break;
    }

    swap(*this, trimmed);
    _position = _chars;
}

bool Text::isIdent(unichar_t ch)
{
    return charIsAlphaNum(ch) || ch == '_';
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
    Console::setLineMode(false);

    Console::getSize(_width, _screenHeight);
    _height = _screenHeight - 1;

    _screen.resize(_width * _height);
    _window.resize(_width * _height);
}

Editor::~Editor()
{
    Console::setLineMode(true);
    Console::clear();
}

void Editor::run()
{
    openFile();
    updateScreen(true);

    while (processKey());
}

void Editor::positionToLineColumn()
{
    char_t* p = _text.chars();
    char_t* q = _text.position();
    _line = 1, _column = 1;

    while (*p && p < q)
    {
        unichar_t ch = _text.charAt(p);

        if (ch == '\n')
        {
            ++_line;
            _column = 1;
        }
        else if (ch == '\t')
            _column = ((_column - 1) / TAB_SIZE + 1) * TAB_SIZE + 1;
        else
            ++_column;

        p = _text.charForward(p);
    }

    _preferredColumn = _column;
}

void Editor::lineColumnToPosition()
{
    char_t* p = _text.chars();
    int preferredLine = _line;
    _line = 1; _column = 1;
    unichar_t ch;

    while (*p && _line < preferredLine)
    {
        ch = _text.charAt(p);
        if (ch == '\n')
            ++_line;

        p = _text.charForward(p);
    }

    while (*p && ch != '\n' && _column < _preferredColumn)
    {
        if (ch == '\t')
            _column = ((_column - 1) / TAB_SIZE + 1) * TAB_SIZE + 1;
        else
            ++_column;

        p = _text.charForward(p);
    }

    _text.position(p);
}

void Editor::updateScreen(bool redrawAll)
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
    int len = _left + _width - 1;
    unichar_t ch;

    _window.clear();

    for (int j = 1; j <= _height; ++j)
    {
        for (int i = 1; i <= len; ++i)
        {
            ch = _text.charAt(p);

            if (ch == '\t')
            {
                ch = ' ';
                if (i == ((i - 1) / TAB_SIZE + 1) * TAB_SIZE)
                    p = _text.charForward(p);
            }
            else if (ch && ch != '\n')
                p = _text.charForward(p);
            else
                ch = ' ';

            if (i >= _left)
                _window.pushBack(ch);
        }

        ch = _text.charAt(p);

        if (ch == '\n')
            p = _text.charForward(p);
        else
        {
            while (ch && ch != '\n')
            {
                p = _text.charForward(p);
                ch = _text.charAt(p);
            }

            if (ch == '\n')
                p = _text.charForward(p);
        }
    }

#ifndef PLATFORM_WINDOWS
    Console::showCursor(false);
#endif

    if (redrawAll)
    {
        Console::write(1, 1, _window.values(), _window.size());
    }
    else
    {
        int i = 0, j = 0;
        int len = min(_window.size(), _screen.size());
        bool matching = true;

        for (; j < len; ++j)
        {
            if (_window[j] == _screen[j])
            {
                if (!matching)
                {
                    Console::write(i / _width + 1, i % _width + 1,
                                   _window.values() + i, j - i);
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
                           _window.values() + i, j - i);
    }

    swap(_window, _screen);

    _status = _filename;
    _status += _encoding == TEXT_ENCODING_UTF8 ? STR("  UTF-8") : STR("  UTF-16");
    _status += _crLf ? STR("  CRLF") : STR("  LF");
    _status.appendFormat(STR("  %d, %d"), _line, _column);

    len = _status.charLength();
    
    if (len <= _width)
    {
        Console::write(_screenHeight, 1, ' ', _width - len);
        Console::write(_screenHeight, _width - len + 1, _status);
    }
    else
    {
        Console::write(_screenHeight, 1, STR("..."), 3);
        Console::write(_screenHeight, 4, _status.charBack(
                _status.chars() + _status.length(), _width - 3));
    }

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
                if (_text.moveWordForward())
                {
                    positionToLineColumn();
                    update = true;
                }
            }
            else if (key.code == KEY_LEFT)
            {
                if (_text.moveWordBack())
                {
                    positionToLineColumn();
                    update = true;
                }
            }
            else if (key.code == KEY_TAB || key.ch == 0x14) // ^Tab or ^T
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
                if (_text.moveToStart())
                {
                    positionToLineColumn();
                    update = true;
                }
            }
            else if (key.code == KEY_END)
            {
                if (_text.moveToEnd())
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
                _selection = _text.positionIndex();
            }
            else if (key.ch == 'f')
            {
                _pattern = getCommand(STR("find: "));

                if (_text.findNext(_pattern))
                    positionToLineColumn();
                update = true;
            }
            else if (key.ch == 'w')
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
            if (_text.moveForward())
            {
                positionToLineColumn();
                update = true;
            }
        }
        else if (key.code == KEY_LEFT)
        {
            if (_text.moveBack())
            {
                positionToLineColumn();
                update = true;
            }
        }
        else if (key.code == KEY_HOME)
        {
            if (_text.moveToLineStart())
            {
                positionToLineColumn();
                update = true;
            }
        }
        else if (key.code == KEY_END)
        {
            if (_text.moveToLineEnd())
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
        else if (key.ch == '\n' || key.ch == '\t' || charIsPrint(key.ch))
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

	if (file.open(_filename, FILE_MODE_OPEN_EXISTING))
		_text.assign(file.readString(_encoding, _bom, _crLf));
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

	File file(_filename, FILE_MODE_CREATE);
	file.writeString(_text, _encoding, _bom, _crLf);
}

String Editor::getCommand(const char_t* prompt)
{
    ASSERT(prompt);

    int promptLength = strLen(prompt);
    String cmdLine = prompt;

    while (true)
    {
        int pos, len = cmdLine.charLength();

        if (len < _width)
        {
            pos = len + 1;
            Console::write(_screenHeight, 1, cmdLine);
            Console::write(_screenHeight, len + 1, ' ', _width - len);
        }
        else
        {
            pos = _width;
            Console::write(_screenHeight, 1, 
                cmdLine.charBack(cmdLine.chars() + cmdLine.length(), _width - 1));
        }

        Console::setCursorPosition(_screenHeight, pos);

        const Array<Key>& keys = Console::readKeys();

        for (int i = 0; i < keys.size(); ++i)
        {
            Key key = keys[i];

            if (key.code == KEY_ENTER)
            {
                return cmdLine.substr(cmdLine.chars() + promptLength);
            }
            else if (key.code == KEY_ESC)
            {
                return String();
            }
            else if (key.code == KEY_BACKSPACE)
            {
                if (cmdLine.length() > promptLength)
                    cmdLine.erase(cmdLine.charBack(cmdLine.chars() + cmdLine.length()));
            }
            else if (charIsPrint(key.ch))
            {
                cmdLine += key.ch;
            }
        }
    }
}

void Editor::buildProject()
{
    Console::setLineMode(true);
    Console::clear();

    saveFile();
    system("gmake");

    Console::writeLine(STR("Press any key to continue..."));
    Console::readChar();

    Console::setLineMode(false);

    updateScreen(true);
}

int MAIN(int argc, const char_t** argv)
{
    Console::initialize();
    
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
            "alt+W - find word at cursor\n"
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
