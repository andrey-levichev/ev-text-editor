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
    _top(1), _left(1),
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
        _position = _text.charForward(_position);
        positionToLineColumn();
        return true;
    }

    return false;
}

bool Document::moveBack()
{
    if (_position > 0)
    {
        _position = _text.charBack(_position);
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
        unichar_t prevChar = _text.charAt(_position);
        _position = _text.charForward(_position);

        while (_position < _text.length())
        {
            unichar_t ch = _text.charAt(_position);
            if (charIsSpace(prevChar) && !charIsSpace(ch))
                break;

            prevChar = ch;
            _position = _text.charForward(_position);
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
        _position = _text.charBack(_position);

        if (_position > 0)
        {
            unichar_t prevChar = _text.charAt(_position);
            int prevPos = _position;

            do
            {
                _position = _text.charBack(_position);

                unichar_t ch = _text.charAt(_position);
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

        ch = _text.charAt(p);
        while (charIsSpace(ch))
        {
            _indent += ch;
            p = _text.charForward(p);
            ch = _text.charAt(p);
        }

        _text.insert(_position, _indent);
        _position += _indent.length();
    }
    else if (ch == '\t') // tab
    {
        _text.insert(_position, tab);
        _position = _text.charForward(_position, TAB_SIZE);
    }
    else if (ch == 0x14) // real tab
    {
        _text.insert(_position, '\t');
        _position = _text.charForward(_position);
    }
    else
    {
        _text.insert(_position, ch);
        _position = _text.charForward(_position);
    }

    positionToLineColumn();
    _modified = true;
    _selection = -1;
}

bool Document::deleteCharForward()
{
    if (_position < _text.length())
    {
        _text.erase(_position, _text.charForward(_position) - _position);

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
        _position = _text.charBack(_position);
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
        if (_text.charAt(end) == '\n')
            end = _text.charForward(end);
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

int Document::findLine(int line) const
{
    ASSERT(line > 0);
    int p = 0;

    while (p < _text.length() && line > 1)
    {
        if (_text.charAt(p) == '\n')
            --line;
        p = _text.charForward(p);
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
            p = _text.find(pattern, _text.charForward(_position));
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

    if (isIdent(_text.charAt(start)))
    {
        while (start > 0)
        {
            int p = _text.charBack(start);
            if (!isIdent(_text.charAt(p)))
                break;
            start = p;
        }

        int end = _text.charForward(_position);
        while (end < _text.length())
        {
            if (!isIdent(_text.charAt(end)))
                break;
            end = _text.charForward(end);
        }

        return _text.substr(start, end - start);
    }

    return String();
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

    _filename = filename;
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

int Document::findCurrentLineStart() const
{
    int p = _position;

    while (p > 0)
    {
        int q = _text.charBack(p);
        if (_text.charAt(q) == '\n')
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
        if (_text.charAt(p) == '\n')
            break;
        p = _text.charForward(p);
    }

    return p;
}

void Document::positionToLineColumn()
{
    int p = 0;
    _line = 1, _column = 1;

    while (p < _position)
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

void Document::lineColumnToPosition()
{
    int p = 0;
    int preferredLine = _line;
    _line = 1; _column = 1;
    unichar_t ch = _text.charAt(p);

    while (p < _text.length() && _line < preferredLine)
    {
        if (ch == '\n')
            ++_line;

        p = _text.charForward(p);
		ch = _text.charAt(p);
    }

    while (p < _text.length() && ch != '\n' && _column < _preferredColumn)
    {
        if (ch == '\t')
            _column = ((_column - 1) / TAB_SIZE + 1) * TAB_SIZE + 1;
        else
            ++_column;

        p = _text.charForward(p);
		ch = _text.charAt(p);
    }

    _position = p;
}

void Document::lineColumnToTopLeft(int width, int height)
{
    if (_line < _top)
        _top = _line;
    else if (_line >= _top + height)
        _top = _line - height + 1;

    if (_column < _left)
        _left = _column;
    else if (_column >= _left + width)
        _left = _column - width + 1;
}

void Document::trimTrailingWhitespace()
{
    String trimmed;
    int p = 0, start = 0, whitespace = 0;

    while (true)
    {
        unichar_t ch = _text.charAt(p);

        if (ch == '\n' || ch == 0)
        {
            if (whitespace)
            {
                trimmed.append(_text.chars() + start, whitespace - start);
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
            p = _text.charForward(p);
        else
        {
            trimmed.append(_text.chars() + start, p - start);
            break;
        }
    }

    swap(trimmed, _text);
    _position = 0;
}

bool Document::isIdent(unichar_t ch)
{
    return charIsAlphaNum(ch) || ch == '_';
}

// Editor

Editor::Editor() :
    _document(NULL),
    _lineSelection(false)
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

void Editor::openDocument(const char_t* filename)
{
    _documents.pushBack(Document());
    _document = _documents.back();
    _document->value.open(filename);
}

void Editor::saveDocuments()
{
    for (auto node = _documents.front(); node; node = node->next)
        node->value.save();
}

void Editor::run()
{
    _document = _documents.front();
    updateScreen(true);

    while (processKey());
}

void Editor::updateScreen(bool redrawAll)
{
    if (_document)
    {
        _window.clear();
        _document->value.lineColumnToTopLeft(_width, _height);

        int p = _document->value.findLine(_document->value.top());
        int left = _document->value.left();
        int len = left + _width - 1;
        unichar_t ch;

        for (int j = 1; j <= _height; ++j)
        {
            for (int i = 1; i <= len; ++i)
            {
                ch = _document->value.text().charAt(p);

                if (ch == '\t')
                {
                    ch = ' ';
                    if (i == ((i - 1) / TAB_SIZE + 1) * TAB_SIZE)
                        p = _document->value.text().charForward(p);
                }
                else if (ch && ch != '\n')
                    p = _document->value.text().charForward(p);
                else
                    ch = ' ';

                if (i >= left)
                    _window.pushBack(ch);
            }

            ch = _document->value.text().charAt(p);

            if (ch == '\n')
                p = _document->value.text().charForward(p);
            else
            {
                while (ch && ch != '\n')
                {
                    p = _document->value.text().charForward(p);
                    ch = _document->value.text().charAt(p);
                }

                if (ch == '\n')
                    p = _document->value.text().charForward(p);
            }
        }
    }
    else
        _window.assign(_width * _height, ' ');

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

    if (_document)
    {
        _status = _document->value.filename();
        _status += _document->value.encoding() == TEXT_ENCODING_UTF8 ? STR("  UTF-8") : STR("  UTF-16");
        _status += _document->value.crlf() ? STR("  CRLF") : STR("  LF");
        _status.appendFormat(STR("  %d, %d"), _document->value.line(), _document->value.column());

        int len = _status.charLength();

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
            _document->value.line() - _document->value.top() + 1, 
            _document->value.column() - _document->value.left() + 1);
    }
    else
        Console::setCursorPosition(1, 1);

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

        if (_document)
        {
            if (key.ctrl)
            {
                if (key.code == KEY_RIGHT)
                {
                    update = _document->value.moveWordForward();
                }
                else if (key.code == KEY_LEFT)
                {
                    update = _document->value.moveWordBack();
                }
                else if (key.code == KEY_TAB || key.ch == 0x14) // ^Tab or ^T
                {
                    _document->value.insertChar(0x14);
                    update = true;
                }
            }
            else if (key.alt)
            {
                if (key.code == KEY_DELETE)
                {
                    update = _document->value.deleteWordForward();
                }
                else if (key.code == KEY_BACKSPACE)
                {
                    update = _document->value.deleteWordBack();
                }
                else if (key.code == KEY_HOME)
                {
                    update = _document->value.moveToStart();
                }
                else if (key.code == KEY_END)
                {
                    update = _document->value.moveToEnd();
                }
                else if (key.code == KEY_PGUP)
                {
                    update = _document->value.moveLinesUp(_height / 2);
                }
                else if (key.code == KEY_PGDN)
                {
                    update = _document->value.moveLinesDown(_height / 2);
                }
                else if (key.ch == ',')
                {
                    if (_document->prev)
                    {
                        _document = _document->prev;
                        updateScreen(true);
                    }
                }
                else if (key.ch == '.')
                {
                    if (_document->next)
                    {
                        _document = _document->next;
                        updateScreen(true);
                    }
                }
                else if (key.ch == 'q')
                {
                    return false;
                }
                else if (key.ch == 's')
                {
                    _document->value.save();
                    update = true;
                }
                else if (key.ch == 'x')
                {
                    saveDocuments();
                    return false;
                }
                else if (key.ch == 'd')
                {
                    _buffer = _document->value.copyDeleteText(false);
                    _lineSelection = _document->value.selection() < 0;

                    if (!_buffer.empty())
                        update = true;
                }
                else if (key.ch == 'c')
                {
                    _buffer = _document->value.copyDeleteText(true);
                    _lineSelection = _document->value.selection() < 0;
                }
                else if (key.ch == 'p')
                {
                    if (!_buffer.empty())
                    {
                        _document->value.pasteText(_buffer, _lineSelection);
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
                    _document->value.markSelection();
                }
                else if (key.ch == 'f')
                {
                    _pattern = getCommand(STR("find: "));
                    update = _document->value.findNext(_pattern);
                }
                else if (key.ch == 'w')
                {
                    _pattern = _document->value.currentWord();
                    update = _document->value.findNext(_pattern);
                }
                else if (key.ch == 'n')
                {
                    update = _document->value.findNext(_pattern);
                }
            }
            else if (key.code == KEY_BACKSPACE)
            {
                update = _document->value.deleteCharBack();
            }
            else if (key.code == KEY_DELETE)
            {
                update = _document->value.deleteCharForward();
            }
            else if (key.code == KEY_UP)
            {
                update = _document->value.moveUp();
            }
            else if (key.code == KEY_DOWN)
            {
                update = _document->value.moveDown();
            }
            else if (key.code == KEY_RIGHT)
            {
                update = _document->value.moveForward();
            }
            else if (key.code == KEY_LEFT)
            {
                update = _document->value.moveBack();
            }
            else if (key.code == KEY_HOME)
            {
                update = _document->value.moveToLineStart();
            }
            else if (key.code == KEY_END)
            {
                update = _document->value.moveToLineEnd();
            }
            else if (key.code == KEY_PGUP)
            {
                update = _document->value.moveLinesUp(_height - 1);
            }
            else if (key.code == KEY_PGDN)
            {
                update = _document->value.moveLinesDown(_height - 1);
            }
            else if (key.ch == '\n' || key.ch == '\t' || charIsPrint(key.ch))
            {
                _document->value.insertChar(key.ch);
                update = true;
            }
        }
        else
        {
            if (key.alt)
            {
                if (key.ch == 'q')
                {
                    return false;
                }
            }
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

    saveDocuments();
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

        for (int i = 1; i < argc; ++i)
            editor.openDocument(argv[i]);

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
