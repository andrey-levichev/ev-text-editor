#include <eve.h>

const int TAB_SIZE = 4;
const char_t* tab = STR("    ");

// Document

Document::Document() :
    _position(0),
    _modified(false),
#ifdef PLATFORM_WINDOWS
    _encoding(TEXT_ENCODING_UTF16_LE),
    _bom(true),
    _crLf(true),
#else
    _encoding(TEXT_ENCODING_UTF8),
    _bom(false),
    _crLf(false),
#endif
    _line(1), _column(1),
    _preferredColumn(1),
    _selection(-1)
{
    _text.ensureCapacity(1);
}

bool Document::moveUp()
{
    if (_line > 1)
        --_line;

    int prev = _position;
    lineColumnToPosition();

    return _position != prev;
}

bool Document::moveDown()
{
    ++_line;

    int prev = _position;
    lineColumnToPosition();

    return _position != prev;
}

bool Document::moveForward()
{
    if (_position < _text.length())
    {
        _position = charForward(_position);
        positionToLineColumn();
        return true;
    }

    return false;
}

bool Document::moveBack()
{
    if (_position > 0)
    {
        _position = charBack(_position);
        positionToLineColumn();
        return true;
    }

    return false;
}

bool Document::moveWordForward()
{
    int start = _position;

    if (_position < _text.length())
    {
        unichar_t prevChar = charAt(_position);
        _position = charForward(_position);

        while (_position < _text.length())
        {
            unichar_t ch = charAt(_position);
            if (charIsSpace(prevChar) && !charIsSpace(ch))
                break;

            prevChar = ch;
            _position = charForward(_position);
        }
    }

    if (_position > start)
    {
        positionToLineColumn();
        return true;
    }
    else
        return false;
}

bool Document::moveWordBack()
{
    int start = _position;

    if (_position > 0)
    {
        _position = charBack(_position);

        if (_position > 0)
        {
            unichar_t prevChar = charAt(_position);
            int prevPos = _position;

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
            while (_position > 0);
        }
    }

    if (_position < start)
    {
        positionToLineColumn();
        return true;
    }
    else
        return false;
}

bool Document::moveToStart()
{
    if (_position > 0)
    {
        _position = 0;
        positionToLineColumn();
        return true;
    }

    return false;
}

bool Document::moveToEnd()
{
    if (_position < _text.length())
    {
        _position = _text.length();
        positionToLineColumn();
        return true;
    }

    return false;
}

bool Document::moveToLineStart()
{
    int p = findCurrentLineStart();

    if (p < _position)
    {
        _position = p;
        positionToLineColumn();
        return true;
    }
    else
        return false;
}

bool Document::moveToLineEnd()
{
    int p = findCurrentLineEnd();

    if (p > _position)
    {
        _position = p;
        positionToLineColumn();
        return true;
    }
    else
        return false;
}

bool Document::moveLinesUp(int lines)
{
    ASSERT(lines >= 0);

    _line -= lines;
    if (_line < 1)
        _line = 1;

    int prev = _position;
    lineColumnToPosition();

    return _position != prev;
}

bool Document::moveLinesDown(int lines)
{
    ASSERT(lines >= 0);

    _line += lines;

    int prev = _position;
    lineColumnToPosition();

    return _position != prev;
}

void Document::insertChar(unichar_t ch)
{
    ASSERT(ch != 0);

    if (ch == '\n') // new line
    {
        int p = findCurrentLineStart();
        _indent.assign('\n');
        unichar_t ch;

        ch = charAt(p);
        while (charIsSpace(ch))
        {
            _indent += ch;
            p = charForward(p);
            ch = charAt(p);
        }

        _text.insert(_position, _indent);
        _position += _indent.length();
    }
    else if (ch == '\t') // tab
    {
        _text.insert(_position, tab);
        _position = charForward(_position, TAB_SIZE);
    }
    else if (ch == 0x14) // real tab
    {
        _text.insert(_position, '\t');
        _position = charForward(_position);
    }
    else
    {
        _text.insert(_position, ch);
        _position = charForward(_position);
    }

    positionToLineColumn();
    _modified = true;
    _selection = -1;
}

bool Document::deleteCharForward()
{
    if (_position < _text.length())
    {
        _text.erase(_position, charForward(_position) - _position);

        positionToLineColumn();
        _modified = true;
        _selection = -1;

        return true;
    }

    return false;
}

bool Document::deleteCharBack()
{
    if (_position > 0)
    {
        int prev = _position;
        _position = charBack(_position);
        _text.erase(_position, prev - _position);

        positionToLineColumn();
        _modified = true;
        _selection = -1;

        return true;
    }

    return false;
}

bool Document::deleteWordForward()
{
    int prev = _position;

    if (moveWordForward())
    {
        _text.erase(prev, _position - prev);
        _position = prev;

        positionToLineColumn();
        _modified = true;
        _selection = -1;

        return true;
    }

    return false;
}

bool Document::deleteWordBack()
{
    int prev = _position;

    if (moveWordBack())
    {
        _text.erase(_position, prev - _position);

        positionToLineColumn();
        _modified = true;
        _selection = -1;

        return true;
    }

    return false;
}

void Document::markSelection()
{
    _selection = _position;
}

String Document::copyDeleteText(bool copy)
{
    int start;
    int end;

    if (_selection < 0)
    {
        start = findCurrentLineStart();
        end = findCurrentLineEnd();
        if (charAt(end) == '\n')
            end = charForward(end);
    }
    else
    {
        if (_selection < _position)
        {
            start = _selection;
            end = _position;
        }
        else
        {
            start = _position;
            end = _selection;
        }
    }
    
    if (start < end)
    {
        String text = _text.substr(start, end - start);

        if (!copy)
        {
            _position = start;
            _text.erase(start, end - start);

            positionToLineColumn();
            _modified = true;
            _selection = -1;
        }

        return text;
    }

    return String();
}

void Document::pasteText(const String& text, bool lineSelection)
{
    if (lineSelection)
        _text.insert(findCurrentLineStart(), text);
    else
        _text.insert(_position, text);

    _position += text.length();
    positionToLineColumn();
    _modified = true;
    _selection = -1;
}

int Document::findCurrentLineStart() const
{
    int p = _position;

    while (p > 0)
    {
        int q = charBack(p);
        if (charAt(q) == '\n')
            break;
        p = q;
    }

    return p;
}

int Document::findCurrentLineEnd() const
{
    int p = _position;

    while (p < _text.length())
    {
        if (charAt(p) == '\n')
            break;
        p = charForward(p);
    }

    return p;
}

int Document::findLine(int line) const
{
    ASSERT(line > 0);
    int p = 0;

    while (p < _text.length() && line > 1)
    {
        if (charAt(p) == '\n')
            --line;
        p = charForward(p);
    }

    return p;
}

bool Document::findNext(const String& pattern)
{
    if (!pattern.empty())
    {
        int p;

        if (_position < _text.length())
        {
            p = _text.find(pattern, charForward(_position));
            if (p < 0)
                p = _text.find(pattern);
        }
        else
			p = _text.find(pattern);

		if (p >= 0 && p != _position)
		{
			_position = p;
            positionToLineColumn();
            return true;
        }
    }

    return false;
}

String Document::currentWord() const
{
    int start = _position;

    if (isIdent(charAt(start)))
    {
        while (start > 0)
        {
            int p = charBack(start);
            if (!isIdent(charAt(p)))
                break;
            start = p;
        }

        int end = charForward(_position);
        while (end < _text.length())
        {
            if (!isIdent(charAt(end)))
                break;
            end = charForward(end);
        }

        return _text.substr(start, end - start);
    }

    return String();
}

void Document::trimTrailingWhitespace()
{
    String trimmed;
    int p = 0, start = 0, whitespace = 0;

    while (true)
    {
        unichar_t ch = charAt(p);

        if (ch == '\n' || ch == 0)
        {
            if (whitespace)
            {
                trimmed.append(chars() + start, whitespace - start);
                start = p;
                whitespace = 0;
            }
        }
        else if (charIsSpace(ch))
        {
            if (!whitespace)
                whitespace = p;
        }
        else
            whitespace = 0;

        if (p < _text.length())
            p = charForward(p);
        else
        {
            trimmed.append(chars() + start, p - start);
            break;
        }
    }

    swap(trimmed, _text);
    _position = 0;
}

void Document::positionToLineColumn()
{
    int p = 0;
    _line = 1, _column = 1;

    while (p < _position)
    {
        unichar_t ch = charAt(p);

        if (ch == '\n')
        {
            ++_line;
            _column = 1;
        }
        else if (ch == '\t')
            _column = ((_column - 1) / TAB_SIZE + 1) * TAB_SIZE + 1;
        else
            ++_column;

        p = charForward(p);
    }

    _preferredColumn = _column;
}

void Document::lineColumnToPosition()
{
    int p = 0;
    int preferredLine = _line;
    _line = 1; _column = 1;
    unichar_t ch = charAt(p);

    while (p < _text.length() && _line < preferredLine)
    {
        if (ch == '\n')
            ++_line;

        p = charForward(p);
		ch = charAt(p);
    }

    while (p < _text.length() && ch != '\n' && _column < _preferredColumn)
    {
        if (ch == '\t')
            _column = ((_column - 1) / TAB_SIZE + 1) * TAB_SIZE + 1;
        else
            ++_column;

        p = charForward(p);
		ch = charAt(p);
    }

    _position = p;
}

void Document::open(const String& filename)
{
	File file;

	if (file.open(filename, FILE_MODE_OPEN_EXISTING))
		_text.assign(file.readString(_encoding, _bom, _crLf));
	else
    {
#ifdef PLATFORM_WINDOWS
        _encoding = TEXT_ENCODING_UTF16_LE;
        _bom = true;
        _crLf = true;
#else
        _encoding = TEXT_ENCODING_UTF8;
        _bom = false;
        _crLf = false;
#endif
		_text.clear();
    }

    _position = 0;
    _modified = false; 
    _line = 1; _column = 1;
    _preferredColumn = 1;
    _selection = -1;
}

void Document::save()
{
    trimTrailingWhitespace();
	File file(_filename, FILE_MODE_CREATE);
	file.writeString(_text, _encoding, _bom, _crLf);

    lineColumnToPosition();
    _modified = false;
    _selection = -1;
}

bool Document::isIdent(unichar_t ch)
{
    return charIsAlphaNum(ch) || ch == '_';
}

// Editor

Editor::Editor() :
    _lineSelection(false),
    _top(1), _left(1)
{
    Console::setLineMode(false);

    Console::getSize(_width, _screenHeight);
    _height = _screenHeight - 1;

    _screen.ensureCapacity(_width * _height);
    _window.ensureCapacity(_width * _height);
}

Editor::~Editor()
{
    Console::setLineMode(true);
    Console::clear();
}

void Editor::run()
{
    updateScreen(true);

    while (processKey());
}

void Editor::updateScreen(bool redrawAll)
{
    if (_document.line() < _top)
        _top = _document.line();
    else if (_document.line() >= _top + _height)
        _top = _document.line() - _height + 1;

    if (_document.column() < _left)
        _left = _document.column();
    else if (_document.column() >= _left + _width)
        _left = _document.column() - _width + 1;

    int p = _document.findLine(_top);
    int len = _left + _width - 1;
    unichar_t ch;

    _window.clear();

    for (int j = 1; j <= _height; ++j)
    {
        for (int i = 1; i <= len; ++i)
        {
            ch = _document.charAt(p);

            if (ch == '\t')
            {
                ch = ' ';
                if (i == ((i - 1) / TAB_SIZE + 1) * TAB_SIZE)
                    p = _document.charForward(p);
            }
            else if (ch && ch != '\n')
                p = _document.charForward(p);
            else
                ch = ' ';

            if (i >= _left)
                _window.pushBack(ch);
        }

        ch = _document.charAt(p);

        if (ch == '\n')
            p = _document.charForward(p);
        else
        {
            while (ch && ch != '\n')
            {
                p = _document.charForward(p);
                ch = _document.charAt(p);
            }

            if (ch == '\n')
                p = _document.charForward(p);
        }
    }

    ASSERT(_window.size() == _width * _height);

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

    _status = _document.filename();
    _status += _document.encoding() == TEXT_ENCODING_UTF8 ? STR("  UTF-8") : STR("  UTF-16");
    _status += _document.crlf() ? STR("  CRLF") : STR("  LF");
    _status.appendFormat(STR("  %d, %d"), _document.line(), _document.column());

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
                _status.length(), _width - 3));
    }

    Console::setCursorPosition(
        _document.line() - _top + 1, 
        _document.column() - _left + 1);

#ifndef PLATFORM_WINDOWS
    Console::showCursor(true);
#endif
}

bool Editor::processKey()
{
    bool update = false;
    const Array<Key>& keys = Console::readKeys();

    for (int i = 0; i < keys.size(); ++i)
    {
        Key key = keys[i];

        if (key.ctrl)
        {
            if (key.code == KEY_RIGHT)
            {
                update = _document.moveWordForward();
            }
            else if (key.code == KEY_LEFT)
            {
                update = _document.moveWordBack();
            }
            else if (key.code == KEY_TAB || key.ch == 0x14) // ^Tab or ^T
            {
                _document.insertChar(0x14);
                update = true;
            }
        }
        else if (key.alt)
        {
            if (key.code == KEY_DELETE)
            {
                update = _document.deleteWordForward();
            }
            else if (key.code == KEY_BACKSPACE)
            {
                update = _document.deleteWordBack();
            }
            else if (key.code == KEY_HOME)
            {
                update = _document.moveToStart();
            }
            else if (key.code == KEY_END)
            {
                update = _document.moveToEnd();
            }
            else if (key.code == KEY_PGUP)
            {
                update = _document.moveLinesUp(_height / 2);
            }
            else if (key.code == KEY_PGDN)
            {
                update = _document.moveLinesDown(_height / 2);
            }
            else if (key.ch == 'q')
            {
                return false;
            }
            else if (key.ch == 's')
            {
                saveDocument();
                update = true;
            }
            else if (key.ch == 'x')
            {
                saveDocument();
                return false;
            }
            else if (key.ch == 'd')
            {
                _buffer = _document.copyDeleteText(false);
                _lineSelection = _document.selection() < 0;

                if (!_buffer.empty())
                    update = true;
            }
            else if (key.ch == 'c')
            {
                _buffer = _document.copyDeleteText(true);
                _lineSelection = _document.selection() < 0;
            }
            else if (key.ch == 'p')
            {
                if (!_buffer.empty())
                {
                    _document.pasteText(_buffer, _lineSelection);
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
                _document.markSelection();
            }
            else if (key.ch == 'f')
            {
                _pattern = getCommand(STR("find: "));
                update = _document.findNext(_pattern);
            }
            else if (key.ch == 'w')
            {
                _pattern = _document.currentWord();
                update = _document.findNext(_pattern);
            }
            else if (key.ch == 'n')
            {
                update = _document.findNext(_pattern);
            }
        }
        else if (key.code == KEY_BACKSPACE)
        {
            update = _document.deleteCharBack();
        }
        else if (key.code == KEY_DELETE)
        {
            update = _document.deleteCharForward();
        }
        else if (key.code == KEY_UP)
        {
            update = _document.moveUp();
        }
        else if (key.code == KEY_DOWN)
        {
            update = _document.moveDown();
        }
        else if (key.code == KEY_RIGHT)
        {
            update = _document.moveForward();
        }
        else if (key.code == KEY_LEFT)
        {
            update = _document.moveBack();
        }
        else if (key.code == KEY_HOME)
        {
            update = _document.moveToLineStart();
        }
        else if (key.code == KEY_END)
        {
            update = _document.moveToLineEnd();
        }
        else if (key.code == KEY_PGUP)
        {
            update = _document.moveLinesUp(_height - 1);
        }
        else if (key.code == KEY_PGDN)
        {
            update = _document.moveLinesDown(_height - 1);
        }
        else if (key.ch == '\n' || key.ch == '\t' || charIsPrint(key.ch))
        {
            _document.insertChar(key.ch);
            update = true;
        }
    }

    if (update)
        updateScreen();

    return true;
}

String Editor::getCommand(const char_t* prompt)
{
    ASSERT(prompt);

    int promptLength = strLen(prompt);
    String cmdLine = prompt;

    while (true)
    {
        int p, len = cmdLine.charLength();

        if (len < _width)
        {
            p = len + 1;
            Console::write(_screenHeight, 1, cmdLine);
            Console::write(_screenHeight, len + 1, ' ', _width - len);
        }
        else
        {
            p = _width;
            Console::write(_screenHeight, 1, 
                cmdLine.charBack(cmdLine.length(), _width - 1));
        }

        Console::setCursorPosition(_screenHeight, p);

        const Array<Key>& keys = Console::readKeys();

        for (int i = 0; i < keys.size(); ++i)
        {
            Key key = keys[i];

            if (key.code == KEY_ENTER)
            {
                return cmdLine.substr(promptLength);
            }
            else if (key.code == KEY_ESC)
            {
                return String();
            }
            else if (key.code == KEY_BACKSPACE)
            {
                if (cmdLine.length() > promptLength)
                    cmdLine.erase(cmdLine.charBack(cmdLine.length()));
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

    saveDocument();
    system("gmake");

    Console::writeLine(STR("Press any key to continue..."));
    Console::readChar();

    Console::setLineMode(false);

    updateScreen(true);
}

int MAIN(int argc, const char_t** argv)
{
    Console::initialize();
    
    try
    {
        if (argc < 2)
        {
            Console::writeLine(STR("usage: eve filename ...\n\n"
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

        Editor editor;
        editor.openDocument(argv[1]);
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
