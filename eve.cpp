#include <eve.h>

const int TAB_SIZE = 4;

bool charIsWord(unichar_t ch)
{
    return charIsAlphaNum(ch) || ch == '_';
}

bool isWordBoundary(unichar_t prevCh, unichar_t ch)
{
    return (!charIsWord(prevCh) && charIsWord(ch)) ||
        ((charIsWord(prevCh) || charIsSpace(prevCh)) && !(charIsWord(ch) || charIsSpace(ch))) ||
        (prevCh != '\n' && ch == '\n');
}

// Document

Document::Document() :
    _top(1), _left(1),
    _x(1), _y(1),
    _width(100), _height(50)
{
    clear();
}

bool Document::moveUp()
{
    if (_line > 1)
        --_line;

    int prev = _position;
    lineColumnToPosition();

    if (_position != prev)
    {
        if (!_selectionMode)
            _selection = -1;

        return true;
    }
    else
        return false;
}

bool Document::moveDown()
{
    ++_line;

    int prev = _position;
    lineColumnToPosition();

    if (_position != prev)
    {
        if (!_selectionMode)
            _selection = -1;

        return true;
    }
    else
        return false;
}

bool Document::moveForward()
{
    if (_position < _text.length())
    {
        _position = _text.charForward(_position);
        positionToLineColumn();

        if (!_selectionMode)
            _selection = -1;

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

        if (!_selectionMode)
            _selection = -1;

        return true;
    }

    return false;
}

bool Document::moveWordForward()
{
    int p = _position;

    if (p < _text.length())
    {
        unichar_t prevCh = _text.charAt(p);
        p = _text.charForward(p);

        while (p < _text.length())
        {
            unichar_t ch = _text.charAt(p);
            if (isWordBoundary(prevCh, ch))
                break;

            prevCh = ch;
            p = _text.charForward(p);
        }
    }

    if (p > _position)
    {
        _position = p;
        positionToLineColumn();

        if (!_selectionMode)
            _selection = -1;

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
            unichar_t prevCh = _text.charAt(p);
            int prevPos = p;

            do
            {
                p = _text.charBack(p);

                unichar_t ch = _text.charAt(p);
                if (isWordBoundary(ch, prevCh))
                {
                    p = prevPos;
                    break;
                }

                prevCh = ch;
                prevPos = p;
            }
            while (p > 0);
        }
    }

    if (p < _position)
    {
        _position = p;
        positionToLineColumn();

        if (!_selectionMode)
            _selection = -1;

        return true;
    }
    else
        return false;
}

bool Document::moveCharsForward()
{
    int p = _position;

    if (p < _text.length())
    {
        unichar_t prevCh = _text.charAt(p);
        p = _text.charForward(p);

        while (p < _text.length())
        {
            unichar_t ch = _text.charAt(p);
            if (charIsSpace(prevCh) && !charIsSpace(ch))
                break;

            prevCh = ch;
            p = _text.charForward(p);
        }
    }

    if (p > _position)
    {
        _position = p;
        positionToLineColumn();

        if (!_selectionMode)
            _selection = -1;

        return true;
    }
    else
        return false;
}

bool Document::moveCharsBack()
{
    int p = _position;

    if (p > 0)
    {
        p = _text.charBack(p);

        if (p > 0)
        {
            unichar_t prevCh = _text.charAt(p);
            int prevPos = p;

            do
            {
                p = _text.charBack(p);

                unichar_t ch = _text.charAt(p);
                if (charIsSpace(ch) && !charIsSpace(prevCh))
                {
                    p = prevPos;
                    break;
                }

                prevCh = ch;
                prevPos = p;
            }
            while (p > 0);
        }
    }

    if (p < _position)
    {
        _position = p;
        positionToLineColumn();

        if (!_selectionMode)
            _selection = -1;

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

        if (!_selectionMode)
            _selection = -1;

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

        if (!_selectionMode)
            _selection = -1;

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

        if (!_selectionMode)
            _selection = -1;

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

        if (!_selectionMode)
            _selection = -1;

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

    if (_position != prev)
    {
        if (!_selectionMode)
            _selection = -1;

        return true;
    }
    else
        return false;
}

bool Document::moveLinesDown(int lines)
{
    ASSERT(lines >= 0);

    _line += lines;

    int prev = _position;
    lineColumnToPosition();

    if (_position != prev)
    {
        if (!_selectionMode)
            _selection = -1;

        return true;
    }
    else
        return false;
}

bool Document::moveToLine(int line)
{
    ASSERT(line > 0);

    _line = line;

    int prev = _position;
    lineColumnToPosition();

    if (_position != prev)
    {
        if (!_selectionMode)
            _selection = -1;

        return true;
    }
    else
        return false;
}

bool Document::moveToLineColumn(int line, int column)
{
    ASSERT(line > 0 && column > 0);

    if (line == _line && column == _column)
        return false;

    _line = line;
    _preferredColumn = column;
    lineColumnToPosition();

    if (!_selectionMode)
        _selection = -1;

    return true;
}

void Document::insertNewLine()
{
    int p = _position, q = p;
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

    ch = _text.charAt(q);
    while (ch == ' ' || ch == '\t')
    {
        q = _text.charForward(q);
        ch = _text.charAt(q);
    }

    _text.replace(_position, _indent, q - _position);
    _position += _indent.length();

    positionToLineColumn();
    _modified = true;
    _selectionMode = false;
    _selection = -1;
}

void Document::insertChar(unichar_t ch, bool afterIdent)
{
    ASSERT(ch != 0);

    if (afterIdent)
    {
        while (charIsWord(_text.charAt(_position)))
            _position = _text.charForward(_position);
    }

    _text.insert(_position, ch);
    _position = _text.charForward(_position);

    positionToLineColumn();
    _modified = true;
    _selectionMode = false;
    _selection = -1;
}

bool Document::deleteCharForward()
{
    if (_position < _text.length())
    {
        _text.erase(_position, _text.charForward(_position) - _position);

        positionToLineColumn();
        _modified = true;
        _selectionMode = false;
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
        _selectionMode = false;
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
        _selectionMode = false;
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
        _selectionMode = false;
        _selection = -1;

        return true;
    }

    return false;
}

void Document::indentLines()
{
    changeLines(&Document::indentLine);
}

void Document::unindentLines()
{
    changeLines(&Document::unindentLine);
}

void Document::commentLines()
{
    changeLines(&Document::commentLine);
}

void Document::uncommentLines()
{
    changeLines(&Document::uncommentLine);
}

void Document::markSelection()
{
    _selectionMode = true;
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
    }

    if (start < end)
    {
        String text = _text.substr(start, end - start);

        if (!copy)
        {
            _text.erase(start, end - start);

            if (_selection < 0)
                lineColumnToPosition();
            else
            {
                _position = start;
                positionToLineColumn();
            }

            _modified = true;
        }

        _selectionMode = false;
        _selection = -1;
        return text;
    }

    _selectionMode = false;
    _selection = -1;
    return String();
}

void Document::pasteText(const String& text, bool lineSelection)
{
    ASSERT(!text.empty());

    if (lineSelection)
        _text.insert(findLineStart(_position), text);
    else
        _text.insert(_position, text);

    _position += text.length();
    positionToLineColumn();

    _modified = true;
    _selectionMode = false;
    _selection = -1;
}

String Document::currentWord() const
{
    int start = _position;

    if (charIsWord(_text.charAt(start)))
    {
        while (start > 0)
        {
            int p = _text.charBack(start);
            if (!charIsWord(_text.charAt(p)))
                break;
            start = p;
        }

        int end = _text.charForward(_position);
        while (end < _text.length())
        {
            if (!charIsWord(_text.charAt(end)))
                break;
            end = _text.charForward(end);
        }

        return _text.substr(start, end - start);
    }

    return String();
}

String Document::autocompletePrefix() const
{
    int p = _position;

    while (p > 0)
    {
        int q = _text.charBack(p);
        if (!charIsWord(_text.charAt(q)))
            break;
        p = q;
    }

    return _text.substr(p, _position - p);
}

void Document::completeWord(const String& word)
{
    ASSERT(!word.empty());

    int start = _position;
    while (start > 0)
    {
        int p = _text.charBack(start);
        if (!charIsWord(_text.charAt(p)))
            break;
        start = p;
    }

    int end = _position;
    while (end < _text.length())
    {
        if (!charIsWord(_text.charAt(end)))
            break;
        end = _text.charForward(end);
    }

    if (start < end)
    {
        _text.replace(start, word, end - start);

        _modified = true;
        _selectionMode = false;
        _selection = -1;
    }
}

bool Document::find(const String& searchStr, bool caseSesitive, bool next)
{
    ASSERT(!searchStr.empty());

    int p = findPosition(searchStr, caseSesitive, next);

	if (p != INVALID_POSITION && p != _position)
	{
		_position = p;
        positionToLineColumn();

        if (!_selectionMode)
            _selection = -1;

        return true;
    }

    return false;
}

bool Document::replace(const String& searchStr, const String& replaceStr, bool caseSesitive)
{
    ASSERT(!searchStr.empty());

    int p = findPosition(searchStr, caseSesitive, false);

	if (p == _position)
	{
        _text.replace(p, replaceStr, searchStr.length());

        _position = p + replaceStr.length();
        p = findPosition(searchStr, caseSesitive, false);
        if (p != INVALID_POSITION)
            _position = p;

        positionToLineColumn();
        _modified = true;
        _selectionMode = false;
        _selection = -1;

        return true;
    }

    return false;
}

bool Document::replaceAll(const String& searchStr, const String& replaceStr, bool caseSesitive)
{
    ASSERT(!searchStr.empty());

    _text.replaceString(searchStr, replaceStr, caseSesitive);

    lineColumnToPosition();
    _modified = true;
    _selectionMode = false;
    _selection = -1;

    return true;
}

void Document::open(const String& filename)
{
    ASSERT(!filename.empty());

    clear();
    _filename = filename;

	File file;
	if (file.open(filename, FILE_MODE_OPEN_EXISTING))
        _text.assign(file.readString(_encoding, _bom, _crLf));
}

void Document::save()
{
    if (_filename.empty())
        throw Exception(STR("filename must be specified"));

    trimTrailingWhitespace();

	File file(_filename, FILE_MODE_CREATE);
	file.writeString(_text, _encoding, _bom, _crLf);

    lineColumnToPosition();
    _modified = false;
    _selection = -1;
}

void Document::clear()
{
    _text.ensureCapacity(1);
    _text.clear();

    _position = 0;
    _modified = false;

    _encoding = TEXT_ENCODING_UTF8;
    _bom = false;
#ifdef PLATFORM_WINDOWS
    _crLf = true;
#else
    _crLf = false;
#endif

    _line = _column = 1;
    _preferredColumn = 1;

    _selection = -1;
    _selectionMode = false;
}

void Document::setDimensions(int x, int y, int width, int height)
{
    ASSERT(x > 0 && y > 0);
    ASSERT(width > 0 && height > 0);

    _x = x;
    _y = y;
    _width = width;
    _height = height;
}

void Document::draw(int screenWidth, UniCharBuffer& screen, bool unicodeLimit16)
{
    ASSERT(screenWidth > 0);

    if (_line < _top)
        _top = _line;
    else if (_line >= _top + _height)
        _top = _line - _height + 1;

    if (_column < _left)
        _left = _column;
    else if (_column >= _left + _width)
        _left = _column - _width + 1;

    int p = findLine(_top), q;
    int len = _left + _width - 1;
    unichar_t ch;

    for (int j = 1; j <= _height; ++j)
    {
        q = (_y + j - 2) * screenWidth + _x - 1;

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
                ch = 0;

            if (i >= _left)
            {
                if (unicodeLimit16 && ch > 0xffff)
                    screen[q++] = '?';
                else
                    screen[q++] = ch;
            }
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
}

void Document::positionToLineColumn()
{
    int p = 0;
    _line = 1; _column = 1;

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

int Document::findLineStart(int pos) const
{
    ASSERT(pos >= 0 && pos <= _text.length());

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
    ASSERT(pos >= 0 && pos <= _text.length());

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
    ASSERT(pos >= 0 && pos <= _text.length());

    pos = findLineEnd(pos);

    if (pos < _text.length())
        return _text.charForward(pos);
    else
        return INVALID_POSITION;
}

int Document::findPreviousLine(int pos) const
{
    ASSERT(pos >= 0 && pos <= _text.length());

    pos = findLineStart(pos);

    if (pos > 0)
    {
        pos = _text.charBack(pos);
        return findLineStart(pos);
    }
    else
        return INVALID_POSITION;
}

int Document::findPosition(const String& searchStr, bool caseSesitive, bool next) const
{
    ASSERT(!searchStr.empty());

    int p = INVALID_POSITION;

    if (_position < _text.length())
    {
        p = next ? _text.charForward(_position) : _position;

        p = _text.find(searchStr, caseSesitive, p);
        if (p == INVALID_POSITION)
            p = _text.find(searchStr, caseSesitive);
    }
    else
        p = _text.find(searchStr, caseSesitive);

    return p;
}

void Document::changeLines(int(Document::* lineOp)(int))
{
    ASSERT(lineOp);

    if (_selection < 0)
        _position = (this->*lineOp)(_position);
    else
    {
        int start, end;
        bool atStart;

        if (_selection < _position)
        {
            start = _selection;
            end = _position;
            atStart = false;
        }
        else
        {
            start = _position;
            end = _selection;
            atStart = true;
        }

        start = findLineStart(start);
        int p = end, n = 0;

        do
        {
            p = (this->*lineOp)(p);
            p = findPreviousLine(p);
            ++n;
        }
        while (p != INVALID_POSITION && p >= start);

        end = start;
        while (--n > 0)
            end = findNextLine(end);

        end = findLineEnd(end);

        if (atStart)
        {
            _position = start;
            _selection = end;
        }
        else
        {
            _selection = start;
            _position = end;
        }

        _selectionMode = false;
    }

    positionToLineColumn();
    _modified = true;
}

int Document::indentLine(int pos)
{
    ASSERT(pos >= 0 && pos <= _text.length());

    int start = findLineStart(pos), p = start;
    unichar_t ch;
    int n = 0;

    ch = _text.charAt(p);
    while (ch == ' ' || ch == '\t')
    {
        if (ch == '\t')
            n += TAB_SIZE - (p - start) % TAB_SIZE;
        else
            ++n;

        p = _text.charForward(p);
        ch = _text.charAt(p);
    }

    n = (n / TAB_SIZE + 1) * TAB_SIZE;
    _text.erase(start, p - start);
    _text.insert(start, ' ', n);

    return pos - (p - start) + n;
}

int Document::unindentLine(int pos)
{
    ASSERT(pos >= 0 && pos <= _text.length());

    int start = findLineStart(pos), p = start;
    unichar_t ch;
    int n = 0;

    ch = _text.charAt(p);
    while (ch == ' ' || ch == '\t')
    {
        if (ch == '\t')
            n += TAB_SIZE - (p - start) % TAB_SIZE;
        else
            ++n;

        p = _text.charForward(p);
        ch = _text.charAt(p);
    }

    if (n > 0)
    {
        n = (n - 1) / TAB_SIZE * TAB_SIZE;
        _text.erase(start, p - start);
        _text.insert(start, ' ', n);

        pos = pos - (p - start) + n;
        if (pos < start)
            pos = start;
    }

    return pos;
}

int Document::commentLine(int pos)
{
    ASSERT(pos >= 0 && pos <= _text.length());

    int start = findLineStart(pos), p = start;
    unichar_t ch;

    ch = _text.charAt(p);
    while (ch == ' ' || ch == '\t')
    {
        p = _text.charForward(p);
        ch = _text.charAt(p);
    }

    _text.insert(start, STR("//"));
    return start;
}

int Document::uncommentLine(int pos)
{
    ASSERT(pos >= 0 && pos <= _text.length());

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
        }
    }

    return start;
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

// Editor

Editor::Editor() :
    _document(NULL),
    _lastDocument(NULL),
    _commandLine(Document(), NULL, NULL),
    _lineSelection(false),
    _caseSesitive(true),
    _width(0), _height(0),
    _recentLocation(NULL),
    _currentSuggestion(INVALID_POSITION)
{
    Console::setLineMode(false);

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

    _documents.addLast(Document());
    _document = _documents.last();

    _document->value.open(filename);
    findUniqueWords();
}

void Editor::saveDocument(Document& document)
{
    if (document.modified())
        document.save();

    findUniqueWords();
}

void Editor::saveDocuments()
{
    for (auto doc = _documents.first(); doc; doc = doc->next)
        if (doc->value.modified())
            doc->value.save();

    findUniqueWords();
}

void Editor::run()
{
    Console::clear();
    _document = _documents.first();

    int width, height;
    Console::getSize(width, height);
    setDimensions(width, height);

    updateScreen(true);

    while (processInput());
}

void Editor::updateScreen(bool redrawAll)
{
#ifndef PLATFORM_WINDOWS
    Console::showCursor(false);
#endif

    if (_document)
    {
        Document& doc = _document->value;
        doc.draw(_width, _screen, _unicodeLimit16);

        if (_document == &_commandLine)
            _screen[(_height - 1) * _width] = ':';
        else
            updateStatusLine();

        if (redrawAll)
        {
            for (int j = 0; j < _height; ++j)
            {
                int p = j * _width;
                _output.clear();

                for (int i = 0; i < _width; ++i)
                {
                    if (_screen[p])
                        _output += _screen[p++];
                    else
                    {
#ifdef PLATFORM_WINDOWS
                        _output += ' ';
                        ++p;
#else
                        _output += STR("\x1b[K");
                        break;
#endif
                    }
                }

                Console::write(j + 1, 1, _output);
            }
        }
        else
        {
            for (int j = 0; j < _height; ++j)
            {
                int p = j * _width, start = p, end = p + _width - 1;
                _output.clear();

                while (start <= end && _screen[start] == _prevScreen[start])
                    ++start;

                while (start <= end && _screen[end] == _prevScreen[end])
                    --end;

                for (int i = start; i <= end; ++i)
                {
                    if (_screen[i])
                        _output += _screen[i];
                    else
                    {
#ifdef PLATFORM_WINDOWS
                        _output += ' ';
#else
                        _output += STR("\x1b[K");
                        break;
#endif
                    }
                }

                Console::write(j + 1, start - p + 1, _output);
            }
        }

        _prevScreen = _screen;

        Console::setCursorPosition(
            doc.line() - doc.top() + doc.y(),
            doc.column() - doc.left() + doc.x());
    }

#ifndef PLATFORM_WINDOWS
    Console::showCursor(true);
#endif
}

void Editor::updateStatusLine()
{
    if (!_message.empty())
    {
        int len = _message.charLength();
        int p = (_height - 1) * _width;

        if (len <= _width)
        {
            for (int i = 0; i < _message.length(); i = _message.charForward(i))
                _screen[p++] = _message.charAt(i);

            for (int i = 0; i < _width - len; ++i)
                _screen[p++] = ' ';
        }
        else
        {
            for (int i = 0, q = 0; i < _width - 3; ++i, q = _message.charForward(q))
                _screen[p++] = _message.charAt(q);

            _screen[p++] = '.'; _screen[p++] = '.'; _screen[p++] = '.';
        }

        _message.clear();
    }
    else
    {
        Document& doc = _document->value;
        _status = doc.filename();

        if (doc.modified())
            _status += '*';

        _status += doc.encoding() == TEXT_ENCODING_UTF8 ? STR("  UTF-8") : STR("  UTF-16");
        _status += doc.crlf() ? STR("  CRLF") : STR("  LF");
        _status.appendFormat(STR("  %d, %d"), doc.line(), doc.column());

        int len = _status.charLength();
        int p = (_height - 1) * _width;

        if (len <= _width)
        {
            for (int i = 0; i < _width - len; ++i)
                _screen[p++] = ' ';

            for (int i = 0; i < _status.length(); i = _status.charForward(i))
                _screen[p++] = _status.charAt(i);
        }
        else
        {
            _screen[p++] = '.'; _screen[p++] = '.'; _screen[p++] = '.';

            for (int i = _status.charBack(_status.length(), _width - 3);
                    i < _status.length(); i = _status.charForward(i))
                _screen[p++] = _status.charAt(i);
        }
    }
}

void Editor::setDimensions(int width, int height)
{
    ASSERT(width > 0 && height > 1);

    _width = width;
    _height = height;

    _commandLine.value.setDimensions(2, _height, _width - 1, 1);

    for (auto doc = _documents.first(); doc; doc = doc->next)
        doc->value.setDimensions(1, 1, _width, _height - 1);

    _screen.assign(_width * _height, ' ');
    _prevScreen.assign(_width * _height, ' ');
}

bool Editor::processInput()
{
    Document& doc = _document->value;
    bool update = false, modified = false, autocomplete = false;

    auto& inputEvents = Console::readInput();

    for (int i = 0; i < inputEvents.size(); ++i)
    {
        InputEvent event = inputEvents[i];

        if (_document)
        {
            if (event.eventType == INPUT_EVENT_TYPE_KEY)
            {
                KeyEvent keyEvent = event.event.keyEvent;

                if (keyEvent.keyDown)
                {
                    if (keyEvent.ctrl)
                    {
                        if (keyEvent.ch == 'b')
                        {
                            update = doc.moveWordBack();
                        }
                        else if (keyEvent.ch == 'w')
                        {
                            update = doc.moveWordForward();
                        }
                        else if (keyEvent.ch == 't')
                        {
                            doc.insertChar(0x9);
                            update = modified = true;
                        }
                        else if (keyEvent.ch == 'h')
                        {
                            update = doc.moveToLineStart();
                        }
                        else if (keyEvent.ch == 'e')
                        {
                            update = doc.moveToLineEnd();
                        }
                        else if (keyEvent.ch == 'p')
                        {
                            update = doc.moveLinesUp(_height - 1);
                        }
                        else if (keyEvent.ch == 'n')
                        {
                            update = doc.moveLinesDown(_height - 1);
                        }
                        else if (keyEvent.ch == 'd')
                        {
                            modified = update = doc.deleteWordForward();
                        }
                        else if (keyEvent.ch == ']')
                        {
                            modified = update = doc.deleteWordBack();
                        }
                        else if (keyEvent.ch == 'g')
                        {
                            _searchStr = doc.currentWord();

                            if (!_searchStr.empty())
                            {
                                _caseSesitive = true;
                                update = doc.find(_searchStr, _caseSesitive, true);
                            }
                        }
                        else if (keyEvent.ch == 'f')
                        {
                            if (!_searchStr.empty())
                                update = doc.find(_searchStr, _caseSesitive, true);
                        }
                        else if (keyEvent.ch == 'r')
                        {
                            if (!_searchStr.empty())
                                modified = update = doc.replace(_searchStr, _replaceStr, _caseSesitive);
                        }
                        else if (keyEvent.ch == 'a')
                        {
                            doc.markSelection();
                        }
                        else if (keyEvent.ch == 'x')
                        {
                            _lineSelection = doc.selection() < 0;
                            _buffer = doc.copyDeleteText(false);

                            if (!_buffer.empty())
                                modified = update = true;
                        }
                        else if (keyEvent.ch == 'c' || keyEvent.ch == 'k')
                        {
                            _lineSelection = doc.selection() < 0;
                            _buffer = doc.copyDeleteText(true);
                        }
                        else if (keyEvent.ch == 'v' || keyEvent.ch == 'l')
                        {
                            if (!_buffer.empty())
                            {
                                doc.pasteText(_buffer, _lineSelection);
                                modified = update = true;
                            }
                        }
                        else if (keyEvent.ch == '^')
                        {
                            update = moveToPrevRecentLocation();
                        }
                    }
                    else if (keyEvent.alt)
                    {
                        if (keyEvent.key == KEY_LEFT)
                        {
                            update = doc.moveWordBack();
                        }
                        else if (keyEvent.key == KEY_RIGHT)
                        {
                            update = doc.moveWordForward();
                        }
                        else if (keyEvent.key == KEY_UP)
                        {
                            update = doc.moveLinesUp(20);
                        }
                        else if (keyEvent.key == KEY_DOWN)
                        {
                            update = doc.moveLinesDown(20);
                        }
                        else if (keyEvent.key == KEY_DELETE)
                        {
                            modified = update = doc.deleteWordForward();
                        }
                        else if (keyEvent.key == KEY_BACKSPACE)
                        {
                            modified = update = doc.deleteWordBack();
                        }
                        else if (keyEvent.ch == 'h' || keyEvent.key == KEY_HOME)
                        {
                            update = doc.moveToStart();
                        }
                        else if (keyEvent.ch == 'e' || keyEvent.key == KEY_END)
                        {
                            update = doc.moveToEnd();
                        }
                        else if (keyEvent.ch == 'b')
                        {
                            update = doc.moveCharsBack();
                        }
                        else if (keyEvent.ch == 'w')
                        {
                            update = doc.moveCharsForward();
                        }
                        else if (keyEvent.ch == ',')
                        {
                            auto doc = _document->prev ? _document->prev : _documents.last();

                            if (doc != _document)
                            {
                                _document = doc;
                                update = true;
                            }
                        }
                        else if (keyEvent.ch == '.')
                        {
                            auto doc = _document->next ? _document->next : _documents.first();

                            if (doc != _document)
                            {
                                _document = doc;
                                update = true;
                            }
                        }
                        else if (keyEvent.ch == '/')
                        {
                            doc.commentLines();
                            modified = update = true;
                        }
                        else if (keyEvent.ch == '?')
                        {
                            doc.uncommentLines();
                            modified = update = true;
                        }
                        else if (keyEvent.ch == '[')
                        {
                            doc.unindentLines();
                            modified = update = true;
                        }
                        else if (keyEvent.ch == ']')
                        {
                            doc.indentLines();
                            modified = update = true;
                        }
                        else if (keyEvent.ch == '\'')
                        {
                            findUniqueWords();
                            updateScreen(true);
                        }
                    }
                    else if (keyEvent.key == KEY_F2)
                    {
                        if (_document == &_commandLine)
                            _document = _lastDocument;
                        else
                        {
                            _lastDocument = _document;
                            _document = &_commandLine;
                            _commandLine.value.clear();
                        }

                        update = true;
                    }
                    else if (keyEvent.key == KEY_F5)
                    {
                        buildProject();
                        updateScreen(true);
                    }
                    else if (keyEvent.key == KEY_F8)
                    {
                        saveDocument(doc);
                        update = true;
                    }
                    else if (keyEvent.key == KEY_F9)
                    {
                        saveDocuments();
                        update = true;
                    }
                    else if (keyEvent.key == KEY_F10)
                    {
                        saveDocuments();
                        return false;
                    }
                    else if (keyEvent.key == KEY_LEFT)
                    {
                        update = doc.moveBack();
                    }
                    else if (keyEvent.key == KEY_RIGHT)
                    {
                        update = doc.moveForward();
                    }
                    else if (keyEvent.key == KEY_UP)
                    {
                        update = doc.moveUp();
                    }
                    else if (keyEvent.key == KEY_DOWN)
                    {
                        update = doc.moveDown();
                    }
                    else if (keyEvent.key == KEY_BACKSPACE)
                    {
                        modified = update = doc.deleteCharBack();
                    }
                    else if (keyEvent.key == KEY_DELETE)
                    {
                        modified = update = doc.deleteCharForward();
                    }
                    else if (keyEvent.key == KEY_HOME)
                    {
                        update = doc.moveToLineStart();
                    }
                    else if (keyEvent.key == KEY_END)
                    {
                        update = doc.moveToLineEnd();
                    }
                    else if (keyEvent.key == KEY_PGUP)
                    {
                        update = doc.moveLinesUp(_height - 1);
                    }
                    else if (keyEvent.key == KEY_PGDN)
                    {
                        update = doc.moveLinesDown(_height - 1);
                    }
                    else if (keyEvent.key == KEY_ENTER)
                    {
                        if (_document == &_commandLine)
                        {
                            _document = _lastDocument;
                            update = true;

                            try
                            {
                                if (!_commandLine.value.text().empty())
                                    processCommand(_commandLine.value.text());
                            }
                            catch (Exception& ex)
                            {
                                _message = ex.message();
                            }
                        }
                        else
                        {
                            if (inputEvents.size() == 1)
                                doc.insertNewLine();
                            else
                                doc.insertChar('\n');
                            modified = update = true;
                        }
                    }
                    else if (keyEvent.key == KEY_TAB)
                    {
                        if (keyEvent.shift)
                        {
                            if (completeWord(false))
                                autocomplete = true;
                            else
                                doc.unindentLines();
                        }
                        else
                        {
                            if (completeWord(true))
                                autocomplete = true;
                            else
                                doc.indentLines();
                        }

                        modified = update = true;
                    }
                    else if (keyEvent.key == KEY_ESC)
                    {
                        return false;
                    }
                    else if (charIsPrint(keyEvent.ch))
                    {
                        if (charIsWord(keyEvent.ch) || _currentSuggestion == INVALID_POSITION)
                            doc.insertChar(keyEvent.ch);
                        else
                        {
                            _uniqueWords[_suggestions[_currentSuggestion].word] = INT_MAX;
                            _currentSuggestion = INVALID_POSITION;
                            doc.insertChar(keyEvent.ch, true);
                        }

                        modified = update = true;
                    }
                }
            }
            else if (event.eventType == INPUT_EVENT_TYPE_MOUSE)
            {
                MouseEvent mouseEvent = event.event.mouseEvent;

                if (mouseEvent.buttonDown)
                {
                    if (mouseEvent.button == MOUSE_BUTTON_PRIMARY)
                        update = doc.moveToLineColumn(
                            doc.top() + mouseEvent.y - 1,
                            doc.left() +  mouseEvent.x - 1);
                    else if (mouseEvent.button == MOUSE_BUTTON_WHEEL_UP)
                        update = doc.moveLinesUp(20);
                    else if (mouseEvent.button == MOUSE_BUTTON_WHEEL_DOWN)
                        update = doc.moveLinesDown(20);
                }

            }
            else if (event.eventType == INPUT_EVENT_TYPE_WINDOW)

            {
                WindowEvent windowEvent = event.event.windowEvent;
                setDimensions(windowEvent.width, windowEvent.height);
                update = true;
            }
        }
        else
        {
            if (event.eventType == INPUT_EVENT_TYPE_KEY)
            {
                KeyEvent keyEvent = event.event.keyEvent;

                if (keyEvent.keyDown)
                {
                    if (keyEvent.key == KEY_ESC)
                    {
                        return false;
                    }
                }
            }
        }
    }

    if (update)
    {
        if (_currentSuggestion != INVALID_POSITION && !autocomplete)
            _currentSuggestion = INVALID_POSITION;
        updateScreen(false);
    }

    if (modified && _document != &_commandLine)
        updateRecentLocations();

    return true;
}

void Editor::updateRecentLocations()
{
    ListNode<RecentLocation>* node;
    _recentLocation = NULL;

    for (node = _recentLocations.first(); node; node = node->next)
    {
        if (node->value.document == _document &&
                abs(node->value.line - _document->value.line()) <= 5)
        {
            node->value.line = _document->value.line();
            break;
        }
    }

    if (node)
    {
        _recentLocations.addLast(node->value);
        _recentLocations.remove(node);
    }
    else
        _recentLocations.addLast(
            RecentLocation(_document, _document->value.line()));

    if (_recentLocations.size() > 10)
        _recentLocations.removeFirst();
}

bool Editor::moveToNextRecentLocation()
{
    _recentLocation = _recentLocation && _recentLocation->next ?
        _recentLocation->next : _recentLocations.first();

    if (_recentLocation)
    {
        if (_recentLocation->value.document == _document &&
            abs(_recentLocation->value.line - _document->value.line()) <= 5)
        {
            _recentLocation = _recentLocation->next ?
                _recentLocation->next : _recentLocations.first();

            if (_recentLocation->value.document == _document &&
                    abs(_recentLocation->value.line - _document->value.line()) <= 5)
                return false;
        }

        _document = _recentLocation->value.document;
        _document->value.moveToLine(_recentLocation->value.line);

        return true;
    }

    return false;
}

bool Editor::moveToPrevRecentLocation()
{
    _recentLocation = _recentLocation && _recentLocation->prev ?
        _recentLocation->prev : _recentLocations.last();

    if (_recentLocation)
    {
        if (_recentLocation->value.document == _document &&
            abs(_recentLocation->value.line - _document->value.line()) <= 5)
        {
            _recentLocation = _recentLocation->prev ?
                _recentLocation->prev : _recentLocations.last();

            if (_recentLocation->value.document == _document &&
                    abs(_recentLocation->value.line - _document->value.line()) <= 5)
                return false;
        }

        _document = _recentLocation->value.document;
        _document->value.moveToLine(_recentLocation->value.line);

        return true;
    }

    return false;
}

void Editor::processCommand(const String& command)
{
    ASSERT(!command.empty());

    int p = 0;
    unichar_t ch = command.charAt(p);

    if (ch == 'f')
    {
        _caseSesitive = true;

        while (true)
        {
            p = command.charForward(p);
            ch = command.charAt(p);

            if (ch == 'i')
                _caseSesitive = false;
            else if (ch == ' ')
                break;
            else
                throw Exception(STR("invalid command"));
        }

        p = command.charForward(p);
        if (p < command.length())
        {
            _searchStr = command.substr(p);
            _document->value.find(_searchStr, _caseSesitive, false);
        }
        else
            throw Exception(STR("invalid command"));
    }
    else if (ch == 'r')
    {
        _caseSesitive = true;
        unichar_t replaceScope = 0;

        while (true)
        {
            p = command.charForward(p);
            ch = command.charAt(p);

            if (ch == 'i')
                _caseSesitive = false;
            else if (ch == 'd' || ch == 'a')
                replaceScope = ch;
            else if (ch == 0)
                throw Exception(STR("invalid command"));
            else
                break;
        }

        p = command.charForward(p);
        if (p < command.length())
        {
            int q = command.find(ch, true, p);
            if (q == INVALID_POSITION)
            {
                _searchStr = command.substr(p);
                _replaceStr.clear();
            }
            else
            {
                _searchStr = command.substr(p, q - p);
                _replaceStr = command.substr(q + 1);
            }

            if (replaceScope == 'd')
            {
                _document->value.replaceAll(_searchStr, _replaceStr, _caseSesitive);
            }
            else if (replaceScope == 'a')
            {
                for (auto doc = _documents.first(); doc; doc = doc->next)
                    doc->value.replaceAll(_searchStr, _replaceStr, _caseSesitive);
            }
            else
                _document->value.find(_searchStr, _caseSesitive, false);
        }
        else
            throw Exception(STR("invalid command"));
    }
    else if (ch == 'g')
    {
        p = command.charForward(p);
        if (command.charAt(p) == ' ')
        {
            p = command.charForward(p);
            int line = command.substr(p).toInt();

            if (line > 0)
                _document->value.moveToLine(line);
            else
                throw Exception(STR("invalid line number"));
        }
        else
            throw Exception(STR("invalid command"));
    }
    else
        throw Exception(STR("invalid command"));
}

void Editor::buildProject()
{
    Console::setLineMode(true);
    Console::clear();

    saveDocuments();

#ifdef PLATFORM_WINDOWS
    const wchar_t* makeCmd = _wgetenv(L"MAKE_CMD");
    _wsystem(makeCmd ? makeCmd : L"nmake.exe");
#else
    const char* makeCmd = getenv("MAKE_CMD");
    system(makeCmd ? makeCmd : "make");
#endif

    Console::writeLine(STR("Press any key to continue..."));
    Console::readLine();

    Console::setLineMode(false);
}

void Editor::findUniqueWords()
{
    Map<String, int> words;

    for (auto doc = _documents.first(); doc; doc = doc->next)
    {
        const String& text = doc->value.text();
        String word;

        for (int p = 0; p < text.length(); p = text.charForward(p))
        {
            unichar_t ch = text.charAt(p);

            if (charIsWord(ch))
                word += ch;
            else if (!word.empty())
            {
                ++words[word];
                word.clear();
            }
        }

        if (!word.empty())
            ++words[word];
    }

    auto it = _uniqueWords.constIterator();
    while (it.moveNext())
    {
        if (it.value().value == INT_MAX)
        {
            int* value = words.find(it.value().key);
            if (value)
                *value = INT_MAX;
        }
    }

    swap(words, _uniqueWords);
}

void Editor::prepareSuggestions(const String& prefix)
{
    ASSERT(!prefix.empty());

    _suggestions.clear();

    auto it = _uniqueWords.constIterator();
    while (it.moveNext())
    {
        if (it.value().key.startsWith(prefix))
            _suggestions.addLast(AutocompleteSuggestion(it.value().key, it.value().value));
    }

    _suggestions.sort();
}

bool Editor::completeWord(bool next)
{
    String prefix = _document->value.autocompletePrefix();

    if (prefix.length() > 0)
    {
        if (_currentSuggestion == INVALID_POSITION)
            prepareSuggestions(prefix);

        if (_suggestions.size() > 0)
        {
            int last = _suggestions.size() - 1;

            if (_currentSuggestion == INVALID_POSITION)
                _currentSuggestion = next ? 0 : last;
            else
            {
                if (next)
                    _currentSuggestion = _currentSuggestion < last ? _currentSuggestion + 1 : 0;
                else
                    _currentSuggestion = _currentSuggestion > 0 ? _currentSuggestion - 1 : last;
            }

            _document->value.completeWord(_suggestions[_currentSuggestion].word);
        }

        return true;
    }

    return false;
}

int MAIN(int argc, const char_t** argv)
{
    try
    {
        if (argc < 2)
        {
            Console::writeLine(STR("eve text editor version 1.4\n"
                "Copyright (C) Andrey Levichev, 2017\n\n"
                "usage: eve filename ...\n\n"));

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
