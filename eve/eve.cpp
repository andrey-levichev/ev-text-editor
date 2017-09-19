#include <eve.h>

const int TAB_SIZE = 4;

// Document

Document::Document() :
    _position(0),
    _modified(false),
#ifdef CHAR_ENCODING_UTF8
    _encoding(TEXT_ENCODING_UTF8),
    _bom(false),
#else
    _encoding(TEXT_ENCODING_UTF16_LE),
    _bom(true),
#endif
#ifdef PLATFORM_WINDOWS
    _crLf(true),
#else
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
    int p = _position;

    if (p < _text.length())
    {
        unichar_t prevChar = _text.charAt(p);
        p = _text.charForward(p);

        while (p < _text.length())
        {
            unichar_t ch = _text.charAt(p);
            if (isWordBoundary(prevChar, ch))
                break;

            prevChar = ch;
            p = _text.charForward(p);
        }
    }

    if (p > _position)
    {
        _position = p;
        positionToLineColumn();
        return true;
    }
    else
        return false;
}

bool Document::moveWordBack()
{
    int p = _position;

    if (p > 0)
    {
        p = _text.charBack(p);

        if (p > 0)
        {
            unichar_t prevChar = _text.charAt(p);
            int prevPos = p;

            do
            {
                p = _text.charBack(p);

                unichar_t ch = _text.charAt(p);
                if (isWordBoundary(ch, prevChar))
                {
                    p = prevPos;
                    break;
                }

                prevChar = ch;
                prevPos = p;
            }
            while (p > 0);
        }
    }

    if (p < _position)
    {
        _position = p;
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
    int start = findLineStart(_position);

    unichar_t ch;
    int end = start;

    ch = _text.charAt(end);
    while (ch == ' ' || ch == '\t')
    {
        end = _text.charForward(end);
        ch = _text.charAt(end);
    }

    int p = _position == start || _position > end ? end : start;

    if (p != _position)
    {
        _position = p;
        positionToLineColumn();
        return true;
    }

    return false;
}

bool Document::moveToLineEnd()
{
    int p = findLineEnd(_position);

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

bool Document::moveToLine(int line)
{
    ASSERT(line > 0);

    _line = line;

    int prev = _position;
    lineColumnToPosition();

    return _position != prev;
}

void Document::insertNewLine()
{
    int p = _position;
    unichar_t ch = '\n';

    while (p > 0 && (ch == ' ' || ch == '\t' || ch == '\n'))
    {
        p = _text.charBack(p);
        ch = _text.charAt(p);
    }

    p = findLineStart(p);
    _indent.assign('\n');

    ch = _text.charAt(p);
    while (ch == ' ' || ch == '\t')
    {
        _indent += ch;
        p = _text.charForward(p);
        ch = _text.charAt(p);
    }

    if (p <= _position)
    {
        _text.insert(_position, _indent);
        _position += _indent.length();
    }
    else
    {
        _text.insert(_position, '\n');
        _position = _text.charForward(_position);
    }

    positionToLineColumn();
    _modified = true;
    _selection = -1;
}

void Document::insertChar(unichar_t ch)
{
    ASSERT(ch != 0);

    _text.insert(_position, ch);
    _position = _text.charForward(_position);

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

void Document::indentLines()
{
    if (_selection < 0)
    {
        _position = indentLine(_position);
    }
    else
    {
        int start, end;

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

        start = findLineStart(start);
        int pos = end;

        do
        {
            _position = indentLine(pos);
            pos = findPreviousLine(_position);
        }
        while (pos != INVALID_POSITION && pos >= start);

        _selection = -1;
    }

    positionToLineColumn();
    _modified = true;
}

void Document::unindentLines()
{
    if (_selection < 0)
    {
        _position = unindentLine(_position);
    }
    else
    {
        int start, end;

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

        start = findLineStart(start);
        int pos = end;

        do
        {
            _position = unindentLine(pos);
            pos = findPreviousLine(_position);
        }
        while (pos != INVALID_POSITION && pos >= start);

        _selection = -1;
    }

    positionToLineColumn();
    _modified = true;
}

void Document::toggleComment()
{
    if (_selection < 0)
    {
        _position = toggleComment(_position);
    }
    else
    {
        int start, end;

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

        start = findLineStart(start);
        int pos = end;

        do
        {
            _position = toggleComment(pos);
            pos = findPreviousLine(_position);
        }
        while (pos != INVALID_POSITION && pos >= start);

        _selection = -1;
    }

    positionToLineColumn();
    _modified = true;
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
        start = findLineStart(_position);
        end = findLineEnd(_position);
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

        _selection = -1;
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
        }

        return text;
    }

    return String();
}

void Document::pasteText(const String& text, bool lineSelection)
{
    if (lineSelection)
        _text.insert(findLineStart(_position), text);
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

String Document::currentWord() const
{
    int start = _position;

    if (charIsIdent(_text.charAt(start)))
    {
        while (start > 0)
        {
            int p = _text.charBack(start);
            if (!charIsIdent(_text.charAt(p)))
                break;
            start = p;
        }

        int end = _text.charForward(_position);
        while (end < _text.length())
        {
            if (!charIsIdent(_text.charAt(end)))
                break;
            end = _text.charForward(end);
        }

        return _text.substr(start, end - start);
    }

    return String();
}

int Document::findPosition(const String& searchStr, bool next)
{
    int p = INVALID_POSITION;

    if (!searchStr.empty())
    {
        if (_position < _text.length())
        {
            p = next ? _text.charForward(_position) : _position;

            p = _text.find(searchStr, p);
            if (p == INVALID_POSITION)
                p = _text.find(searchStr);
        }
        else
			p = _text.find(searchStr);
    }

    return p;
}

bool Document::find(const String& searchStr, bool next)
{
    int p = findPosition(searchStr, next);

	if (p != INVALID_POSITION && p != _position)
	{
		_position = p;
        positionToLineColumn();
        return true;
    }

    return false;
}

bool Document::replace(const String& searchStr, const String& replaceStr)
{
    int p = findPosition(searchStr, false);

	if (p == _position)
	{
        _text.replace(p, replaceStr, searchStr.length());

        _position = p + replaceStr.length();
        p = findPosition(searchStr, false);
        if (p != INVALID_POSITION)
            _position = p;

        positionToLineColumn();
        _modified = true;
        _selection = -1;

        return true;
    }

    return false;
}

void Document::open(const String& filename)
{
	File file;

	if (file.open(filename, FILE_MODE_OPEN_EXISTING))
		_text.assign(file.readString(_encoding, _bom, _crLf));
	else
    {
#ifdef CHAR_ENCODING_UTF8
        _encoding = TEXT_ENCODING_UTF8;
        _bom = false;
#else
        _encoding = TEXT_ENCODING_UTF16_LE;
        _bom = true;
#endif
#ifdef PLATFORM_WINDOWS
        _crLf = true;
#else
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

int Document::findLineStart(int pos) const
{
    ASSERT(pos >= 0);

    while (pos > 0)
    {
        int p = _text.charBack(pos);
        if (_text.charAt(p) == '\n')
            break;
        pos = p;
    }

    return pos;
}

int Document::findLineEnd(int pos) const
{
    ASSERT(pos >= 0);

    while (pos < _text.length())
    {
        if (_text.charAt(pos) == '\n')
            break;
        pos = _text.charForward(pos);
    }

    return pos;
}

int Document::findNextLine(int pos) const
{
    ASSERT(pos >= 0);

    pos = findLineEnd(pos);

    if (pos < _text.length())
        return _text.charForward(pos);
    else
        return INVALID_POSITION;
}

int Document::findPreviousLine(int pos) const
{
    ASSERT(pos >= 0);

    pos = findLineStart(pos);

    if (pos > 0)
    {
        pos = _text.charBack(pos);
        return findLineStart(pos);
    }
    else
        return INVALID_POSITION;
}

int Document::indentLine(int pos)
{
    ASSERT(pos >= 0);

    int start = findLineStart(pos), p = start;
    unichar_t ch;
    int indent = 0;

    ch = _text.charAt(p);
    while (ch == ' ' || ch == '\t')
    {
        if (ch == '\t')
            indent += TAB_SIZE - (p - start) % TAB_SIZE;
        else
            ++indent;

        p = _text.charForward(p);
        ch = _text.charAt(p);
    }

    indent = (indent / TAB_SIZE + 1) * TAB_SIZE;
    _text.erase(start, p - start);
    _text.insert(start, ' ', indent);

    return _text.charForward(start, indent);
}

int Document::unindentLine(int pos)
{
    ASSERT(pos >= 0);

    int start = findLineStart(pos), p = start;
    unichar_t ch;
    int indent = 0;

    ch = _text.charAt(p);
    while (ch == ' ' || ch == '\t')
    {
        if (ch == '\t')
            indent += TAB_SIZE - (p - start) % TAB_SIZE;
        else
            ++indent;

        p = _text.charForward(p);
        ch = _text.charAt(p);
    }

    if (indent > 0)
    {
        indent = (indent - 1) / TAB_SIZE * TAB_SIZE;
        _text.erase(start, p - start);
        _text.insert(start, ' ', indent);

        p = _text.charForward(start, indent);
    }

    return p;
}

int Document::toggleComment(int pos)
{
    ASSERT(pos >= 0);

    int start = findLineStart(pos), p = start;
    unichar_t ch;

    ch = _text.charAt(p);
    while (ch == ' ' || ch == '\t')
    {
        p = _text.charForward(p);
        ch = _text.charAt(p);
    }

    if (ch == '/')
    {
        int q = _text.charForward(p);
        ch = _text.charAt(q);

        if (ch == '/')
        {
            q = _text.charForward(q);
            _text.erase(p, q - p);
            return p;
        }
    }

    _text.insert(start, STR("//"));
    return start;
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
    ASSERT(width > 0 && height > 0);

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
        else if (ch == ' ' || ch == '\t')
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

bool Document::charIsIdent(unichar_t ch)
{
    return charIsAlphaNum(ch) || ch == '_';
}

bool Document::isWordBoundary(unichar_t prevCh, unichar_t ch)
{
    return (!charIsIdent(prevCh) && charIsIdent(ch)) ||
        ((charIsIdent(prevCh) || charIsSpace(prevCh)) && !(charIsIdent(ch) || charIsSpace(ch))) ||
        (prevCh != '\n' && ch == '\n');
}

// Editor

Editor::Editor() :
    _document(NULL),
    _lineSelection(false)
{
    Console::setLineMode(false);

    Console::getSize(_width, _screenHeight);
    _height = _screenHeight - 1;

    _screen.ensureCapacity(_width * _height + 1);
    _window.ensureCapacity(_width * _height + 1);

#ifdef PLATFORM_WINDOWS
    _unicodeLimit16 = true;
#else
    const char* term = getenv("TERM");
    if (term)
        _unicodeLimit16 = strstr(term, "xterm") != NULL;
    else
        _unicodeLimit16 = false;
#endif
}

Editor::~Editor()
{
    Console::setLineMode(true);
    Console::clear();
}

void Editor::openDocument(const char_t* filename)
{
    ASSERT(filename);

    _documents.pushBack(Document());
    _document = _documents.back();
    _document->value.open(filename);
}

void Editor::saveDocuments()
{
    for (auto node = _documents.front(); node; node = node->next)
    {
        if (node->value.modified())
            node->value.save();
    }
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
                {
                    if (_unicodeLimit16 && ch > 0xffff)
                        _window += '?';
                    else
                        _window += ch;
                }
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

    ASSERT(_window.charLength() == _width * _height);

#ifndef PLATFORM_WINDOWS
    Console::showCursor(false);
#endif

    if (redrawAll)
    {
        Console::write(1, 1, _window);
    }
    else
    {
        int windowIndex = 0, screenIndex = 0, matchingStartIndex = 0;
        int matchingStartCharIndex = 0, charIndex = 0;
        bool matching = true;

        while (windowIndex < _window.length())
        {
            if (_window.charAt(windowIndex) == _screen.charAt(screenIndex))
            {
                if (!matching)
                {
                    Console::write(matchingStartCharIndex / _width + 1, matchingStartCharIndex % _width + 1,
                        _window.chars() + matchingStartIndex, windowIndex - matchingStartIndex);
                    matchingStartIndex = windowIndex;
                    matchingStartCharIndex = charIndex;
                    matching = true;
                }
            }
            else
            {
                if (matching)
                {
                    matchingStartIndex = windowIndex;
                    matchingStartCharIndex = charIndex;
                    matching = false;
                }
            }

            windowIndex = _window.charForward(windowIndex);
            screenIndex = _screen.charForward(screenIndex);
            ++charIndex;
        }

        ASSERT(windowIndex == _window.length());
        ASSERT(screenIndex == _screen.length());
        ASSERT(charIndex == _width * _height);

        if (!matching)
        {
            Console::write(matchingStartCharIndex / _width + 1, matchingStartCharIndex % _width + 1,
                _window.chars() + matchingStartIndex, windowIndex - matchingStartIndex);
        }
    }

    swap(_window, _screen);

    if (_document)
    {
        _status = _document->value.filename();

        if (_document->value.modified())
            _status += '*';

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
    bool autoindent = keys.size() == 1;

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
                else if (key.code == KEY_TAB || key.ch == 0x14)
                {
                    _document->value.insertChar(0x9);
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
                else if (key.ch == 'a')
                {
                    saveDocuments();
                    update = true;
                }
                else if (key.ch == 'x')
                {
                    saveDocuments();
                    return false;
                }
                else if (key.ch == 'd')
                {
                    _lineSelection = _document->value.selection() < 0;
                    _buffer = _document->value.copyDeleteText(false);

                    if (!_buffer.empty())
                        update = true;
                }
                else if (key.ch == 'c')
                {
                    _lineSelection = _document->value.selection() < 0;
                    _buffer = _document->value.copyDeleteText(true);
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
                else if (key.ch == 'w')
                {
                    _searchStr = _document->value.currentWord();
                    update = _document->value.find(_searchStr, true);
                }
                else if (key.ch == 'f')
                {
                    update = _document->value.find(_searchStr, true);
                }
                else if (key.ch == 'r')
                {
                    update = _document->value.replace(_searchStr, _replaceStr);
                }
                else if (key.ch == '/')
                {
                    _document->value.toggleComment();
                    update = true;
                }
            }
            else if (key.shift && key.code == KEY_TAB)
            {
                _document->value.unindentLines();
                update = true;
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
            else if (key.code == KEY_ENTER)
            {
                if (autoindent)
                    _document->value.insertNewLine();
                else
                    _document->value.insertChar('\n');
                update = true;
            }
            else if (key.code == KEY_TAB)
            {
                _document->value.indentLines();
                update = true;
            }
            else if (key.code == KEY_ESC)
            {
                try
                {
                    processCommand();
                    update = true;
                }
                catch (Exception& ex)
                {
                    Console::write(_screenHeight, 1, ex.message());
                }
            }
            else if (charIsPrint(key.ch))
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
                cmdLine.chars() + cmdLine.charBack(cmdLine.length(), _width - 1), _width - 1);
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

void Editor::processCommand()
{
    _command = getCommand(STR(":"));

    if (_command.empty())
        return;
    else
    {
        int p = 0;
        unichar_t ch = _command.charAt(p);

        if (ch == 'f')
        {
            p = _command.charForward(p);
            if (_command.charAt(p) == ' ')
            {
                p = _command.charForward(p);
                _searchStr = _command.substr(p);
                _document->value.find(_searchStr, false);
                return;
            }
        }
        else if (ch == 'r')
        {
            p = _command.charForward(p);
            unichar_t sep = _command.charAt(p);

            if (sep != 0)
            {
                p = _command.charForward(p);

                if (p < _command.length())
                {
                    int q = _command.find(sep, p);
                    if (q == INVALID_POSITION)
                    {
                        _searchStr = _command.substr(p);
                        _replaceStr.clear();
                    }
                    else
                    {
                        _searchStr = _command.substr(p, q - p);
                        _replaceStr = _command.substr(q + 1);
                    }

                    _document->value.find(_searchStr, false);
                    return;
                }
            }
        }
        else if (ch == 'g')
        {
            p = _command.charForward(p);
            if (_command.charAt(p) == ' ')
            {
                p = _command.charForward(p);
                int line = _command.substr(p).toInt();

                if (line > 0)
                    _document->value.moveToLine(line);
                else
                    throw Exception(STR("invalid line number"));

                return;
            }
        }
    }

    throw Exception(STR("invalid command"));
}

void Editor::buildProject()

{
    Console::setLineMode(true);
    Console::clear();

    saveDocuments();

#ifdef PLATFORM_WINDOWS
    system("nmake");
#else
    system("make");
#endif

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
            Console::writeLine(STR("eve text editor version 1.0\n"
                "Copyright (C) Andrey Levichev, 2017. All rights reserved.\n\n"
                "usage: eve filename ...\n\n"
                "keys:\n"
                "arrows - move cursor\n"
                "ctrl+left/right - word back/forward\n"
                "home/end - start/end of line\n"
                "alt+home/end - start/end of file\n"
                "pgup/pgdn - page up/down\n"
                "alt+pgup/pgdn - half page up/down\n"
                "tab - indent line\n"
                "shift+tab - unindent line\n"
                "delete - delete character at cursor position\n"
                "backspace - unindent line or delete character to the left of cursor position\n"
                "alt+del - delete word at cursor position\n"
                "alt+backspace - delete word to the left of cursor position\n"
                "alt+m - mark selection start\n"
                "alt+d - delete line/selection\n"
                "alt+c - copy line/selection\n"
                "alt+p - paste line/selection\n"
                "alt+w - find word at cursor\n"
                "alt+f - find again\n"
                "alt+r - replace and find again\n"
                "alt+/ - toggle comment\n"
                "alt+b - build with make\n"
                "alt+, - go to previous document\n"
                "alt+. - go to next document\n"
                "alt+s - save\n"
                "alt+a - save all\n"
                "alt+x - save all and quit\n"
                "alt+q - quit without saving\n"
                "ESC - enter command\n\n"
                "commands:\n"
                "g number - go to line number\n"
                "f string - find string\n"
                "r searchString replaceString - replace string\n"));

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
