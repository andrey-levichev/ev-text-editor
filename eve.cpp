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

void copyToClipboard(const String& text)
{
    ASSERT(!text.empty());

#ifdef PLATFORM_WINDOWS
    BOOL rc = OpenClipboard(NULL);
    ASSERT(rc);

    rc = EmptyClipboard();
    ASSERT(rc);

    ByteBuffer bytes = Unicode::stringToBytes(text, TEXT_ENCODING_UTF16_LE, false, true);

    HGLOBAL hText = GlobalAlloc(GMEM_MOVEABLE, bytes.size() + 2);
    ASSERT(hText);

    void* ptr = GlobalLock(hText);
    ASSERT(ptr);

    memset(ptr, 12, bytes.size() + 2);
    memcpy(ptr, bytes.values(), bytes.size());

    GlobalUnlock(hText);

    HANDLE hClip = SetClipboardData(CF_UNICODETEXT, hText);
    ASSERT(hClip);

    rc = CloseClipboard();
    ASSERT(rc);
#endif
}

void pasteFromClipboard(String& text)
{
#ifdef PLATFORM_WINDOWS
    if (IsClipboardFormatAvailable(CF_UNICODETEXT))
    {
        BOOL rc = OpenClipboard(NULL);
        ASSERT(rc);

        HGLOBAL hText = GetClipboardData(CF_UNICODETEXT);
        ASSERT(hText);

        void* ptr = GlobalLock(hText);
        ASSERT(ptr);

        TextEncoding encoding;
        bool bom, crLf;
        text = Unicode::bytesToString(wcslen(reinterpret_cast<wchar_t*>(ptr)) * 2,
            reinterpret_cast<byte_t*>(ptr), encoding, bom, crLf);

        GlobalUnlock(hText);

        rc = CloseClipboard();
        ASSERT(rc);
    }
    else
        text.clear();
#endif
}

// Document

Document::Document() :
    _top(1), _left(1),
    _x(1), _y(1),
    _width(100), _height(50)
{
    clear();
}

bool Document::moveForward()
{
    if (_position < _text.length())
    {
        int p = _text.charForward(_position);
        setPositionLineColumn(p);

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
        int p = _text.charBack(_position);
        setPositionLineColumn(p);

        if (!_selectionMode)
            _selection = -1;

        return true;
    }

    return false;
}

bool Document::moveWordForward()
{
    int p = findWordForward(_position);

    if (p > _position)
    {
        setPositionLineColumn(p);

        if (!_selectionMode)
            _selection = -1;

        return true;
    }
    else
        return false;
}

bool Document::moveWordBack()
{
    int p = findWordBack(_position);

    if (p < _position)
    {
        setPositionLineColumn(p);

        if (!_selectionMode)
            _selection = -1;

        return true;
    }
    else
        return false;
}

bool Document::moveCharsForward()
{
    int p = findCharsForward(_position);

    if (p > _position)
    {
        setPositionLineColumn(p);

        if (!_selectionMode)
            _selection = -1;

        return true;
    }
    else
        return false;
}

bool Document::moveCharsBack()
{
    int p = findCharsBack(_position);

    if (p < _position)
    {
        setPositionLineColumn(p);

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
        _line = _column = _preferredColumn = 1;

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
        int p = _text.length();
        setPositionLineColumn(p);

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
        setPositionLineColumn(p);

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
        setPositionLineColumn(p);

        if (!_selectionMode)
            _selection = -1;

        return true;
    }
    else
        return false;
}

bool Document::moveLines(int lines)
{
    int line = _line + lines;
    if (line < 1)
        line = 1;

    return moveToLineColumn(line, _preferredColumn);
}

bool Document::moveToLine(int line)
{
    return moveToLineColumn(line, _preferredColumn);
}

bool Document::moveToLineColumn(int line, int column)
{
    ASSERT(line > 0 && column > 0);

    int prev = _position;
    lineColumnToPosition(_position, _line, _column,
        line, column, _position, _line, _column);

    if (_position != prev)
    {
        if (!_selectionMode)
            _selection = -1;

        return true;
    }
    else
        return false;
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
    setPositionLineColumn(_position + _indent.length());

    _modified = true;
    _selectionMode = false;
    _selection = -1;
}

void Document::insertChar(unichar_t ch, bool afterIdent)
{
    ASSERT(ch != 0);

    int p = _position;

    if (afterIdent)
    {
        while (charIsWord(_text.charAt(p)))
            p = _text.charForward(p);
    }

    _text.insert(p, ch);
    p = _text.charForward(p);
    setPositionLineColumn(p);

    _modified = true;
    _selectionMode = false;
    _selection = -1;
}

bool Document::deleteCharForward()
{
    if (_position < _text.length())
    {
        _text.erase(_position, _text.charForward(_position) - _position);

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
        int p = _text.charBack(_position);
        int prev = _position;

        setPositionLineColumn(p);
        _text.erase(_position, prev - _position);

        _modified = true;
        _selectionMode = false;
        _selection = -1;

        return true;
    }

    return false;
}

bool Document::deleteWordForward()
{
    int p = findWordForward(_position);

    if (p > _position)
    {
        _text.erase(_position, p - _position);

        _modified = true;
        _selectionMode = false;
        _selection = -1;

        return true;
    }

    return false;
}

bool Document::deleteWordBack()
{
    int p = findWordBack(_position);

    if (p < _position)
    {
        int prev = _position;

        setPositionLineColumn(p);
        _text.erase(_position, prev - _position);

        _modified = true;
        _selectionMode = false;
        _selection = -1;

        return true;
    }

    return false;
}

bool Document::deleteCharsForward()
{
    int p = findCharsForward(_position);

    if (p > _position)
    {
        _text.erase(_position, p - _position);

        _modified = true;
        _selectionMode = false;
        _selection = -1;

        return true;
    }

    return false;
}

bool Document::deleteCharsBack()
{
    int p = findCharsBack(_position);

    if (p < _position)
    {
        int prev = _position;

        setPositionLineColumn(p);
        _text.erase(_position, prev - _position);

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
                lineColumnToPosition(start, _line, 1, _line, _column,  _position, _line, _column);
            else
                setPositionLineColumn(start);

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

void Document::pasteText(const String& text)
{
    ASSERT(!text.empty());

    if (text.charAt(text.charBack(text.length())) == '\n')
        _text.insert(findLineStart(_position), text);
    else
        _text.insert(_position, text);

    setPositionLineColumn(_position + text.length());

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

    int p = findPosition(_position, searchStr, caseSesitive, next);

	if (p != INVALID_POSITION && p != _position)
	{
        setPositionLineColumn(p);

        if (!_selectionMode)
            _selection = -1;

        return true;
    }

    return false;
}

bool Document::replace(const String& searchStr, const String& replaceStr, bool caseSesitive)
{
    ASSERT(!searchStr.empty());

    int p = findPosition(_position, searchStr, caseSesitive, false);

	if (p == _position)
	{
        _text.replace(p, replaceStr, searchStr.length());
        p += replaceStr.length();

        int q = findPosition(p, searchStr, caseSesitive, false);
        if (q != INVALID_POSITION)
            p = q;

        setPositionLineColumn(p);

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
    lineColumnToPosition(0, 1, 1, _line, _column, _position, _line, _column);

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
	if (file.open(filename))
	{
        ByteBuffer bytes = file.read();
        _text.assign(Unicode::bytesToString(bytes, _encoding, _bom, _crLf));
    }
}

void Document::save()
{
    if (_filename.empty())
        throw Exception(STR("filename must be specified"));

    trimTrailingWhitespace();

	File file(_filename, FILE_MODE_WRITE | FILE_MODE_CREATE | FILE_MODE_TRUNCATE);
	file.write(Unicode::stringToBytes(_text, _encoding, _bom, _crLf));

    lineColumnToPosition(0, 1, 1, _line, _column, _position, _line, _column);

    _modified = false;
    _selectionMode = false;
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

    _selectionMode = false;
    _selection = -1;
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

    int p, q;
    lineColumnToPosition(_position, _line, _column, _top, _left, p, _top, _left);

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

void Document::setPositionLineColumn(int pos)
{
    positionToLineColumn(_position, _line, _column, pos, _line, _column);
    _position = pos;
    _preferredColumn = _column;
}

void Document::positionToLineColumn(int startPos, int startLine, int startColumn,
        int newPos, int& line, int& column)
{
    ASSERT(startPos >= 0 && startPos <= _text.length());
    ASSERT(startLine > 0 && startColumn > 0);
    ASSERT(newPos >= 0 && newPos <= _text.length());

    int p = startPos;
    line = startLine;
    column = startColumn;

    if (p < newPos)
    {
        while (p < newPos)
        {
            unichar_t ch = _text.charAt(p);

            if (ch == '\n')
            {
                ++line;
                column = 1;
            }
            else if (ch == '\t')
                column = ((column - 1) / TAB_SIZE + 1) * TAB_SIZE + 1;
            else
                ++column;

            p = _text.charForward(p);
        }
    }
    else if (p > newPos)
    {
        while (p > newPos)
        {
            p = _text.charBack(p);
            if (_text.charAt(p) == '\n')
                --line;
            --column;
        }

        if (line != startLine)
        {
            _column = 1;
            p = findLineStart(p);

            while (p < newPos)
            {
                unichar_t ch = _text.charAt(p);

                if (ch == '\t')
                    column = ((column - 1) / TAB_SIZE + 1) * TAB_SIZE + 1;
                else
                    ++column;

                p = _text.charForward(p);
            }
        }
    }
}

void Document::lineColumnToPosition(int startPos, int startLine, int startColumn,
        int newLine, int newColumn, int& pos, int& line, int& column)
{
    ASSERT(startPos >= 0 && startPos <= _text.length());
    ASSERT(startLine > 0 && startColumn > 0);
    ASSERT(newLine > 0 && newColumn > 0);

    pos = startPos;
    line = startLine;
    column = 1;

    if (line < newLine)
    {
        while (pos < _text.length() && line < newLine)
        {
            if (_text.charAt(pos) == '\n')
                ++line;

            pos = _text.charForward(pos);
        }
    }
    else
    {
        while (pos > 0)
        {
            int p = _text.charBack(pos);
            if (_text.charAt(p) == '\n')
            {
                if (line > newLine)
                    --line;
                else
                    break;
            }

            pos = p;
        }
    }

    unichar_t ch = _text.charAt(pos);

    while (pos < _text.length() && ch != '\n' && column < newColumn)
    {
        if (ch == '\t')
            column = ((column - 1) / TAB_SIZE + 1) * TAB_SIZE + 1;
        else
            ++column;

        pos = _text.charForward(pos);
        ch = _text.charAt(pos);
    }
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

int Document::findWordForward(int pos) const
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

    return p;
}

int Document::findWordBack(int pos) const
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

    return p;
}

int Document::findCharsForward(int pos) const
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

    return p;
}

int Document::findCharsBack(int pos) const
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

    return p;
}

int Document::findPosition(int pos, const String& searchStr, bool caseSesitive, bool next) const
{
    ASSERT(!searchStr.empty());

    pos = INVALID_POSITION;

    if (pos < _text.length())
    {
        pos = next ? _text.charForward(pos) : pos;

        pos = _text.find(searchStr, caseSesitive, pos);
        if (pos == INVALID_POSITION)
            pos = _text.find(searchStr, caseSesitive);
    }
    else
        pos = _text.find(searchStr, caseSesitive);

    return pos;
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

    setPositionLineColumn(_position);
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
    _commandLine(Document(), NULL, NULL),
    _document(NULL),
    _lastDocument(NULL),
    _recordingMacro(false),
    _caseSesitive(true),
    _recentLocation(NULL),
    _currentSuggestion(INVALID_POSITION)
{
    Console::setLineMode(false);
    Console::getSize(_width, _height);

    setDimensions();

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

void Editor::newDocument(const String& filename)
{
    ASSERT(!filename.empty());

    _documents.addLast(Document());
    _document = _documents.last();

    _document->value.setDimensions(1, 1, _width, _height - 1);
    _document->value.filename(filename);
}

void Editor::openDocument(const String& filename)
{
    ASSERT(!filename.empty());

    _documents.addLast(Document());
    _document = _documents.last();

    _document->value.setDimensions(1, 1, _width, _height - 1);
    _document->value.open(filename);

    findUniqueWords();
}

void Editor::saveDocument()
{
    if (_document)
    {
        if (_document->value.modified())
            _document->value.save();

        findUniqueWords();
    }
}

void Editor::saveAllDocuments()
{
    if (!_documents.empty())
    {
        for (auto doc = _documents.first(); doc; doc = doc->next)
            if (doc->value.modified())
                doc->value.save();

        findUniqueWords();
    }
}

void Editor::closeDocument()
{
    if (_document)
    {
        auto doc = _document->next;
        _documents.remove(_document);
        _document = doc;

        findUniqueWords();
    }
}

void Editor::run()
{
    _document = _documents.first();

    setDimensions();
    updateScreen(true);

    while (processInput());
}

void Editor::setDimensions()
{
    ASSERT(_width > 0 && _height > 1);

    _screen.assign(_width * _height, static_cast<unichar_t>(0));
    _commandLine.value.setDimensions(2, _height, _width - 1, 1);

    for (auto doc = _documents.first(); doc; doc = doc->next)
        doc->value.setDimensions(1, 1, _width, _height - 1);
}

void Editor::updateScreen(bool redrawAll)
{
    int line, col;
    _prevScreen = _screen;

#ifndef PLATFORM_WINDOWS
    Console::showCursor(false);
#endif

    if (_document)
    {
        if (_document == &_commandLine)
            _screen[(_height - 1) * _width] = ':';
        else
            updateStatusLine();

        Document& doc = _document->value;
        doc.draw(_width, _screen, _unicodeLimit16);

        line = doc.line() - doc.top() + doc.y();
        col = doc.column() - doc.left() + doc.x();
    }
    else
    {
        _screen.assign(_width * _height, static_cast<unichar_t>(0));
        updateStatusLine();
        line = col = 1;
    }

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

            if (!_output.empty())
                Console::write(j + 1, start - p + 1, _output);
        }
    }

    Console::setCursorPosition(line, col);

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
    else if (_document)
    {
        Document& doc = _document->value;
        _status = doc.filename();

        if (doc.modified())
            _status += '*';

        _status += doc.encoding() == TEXT_ENCODING_UTF8 ? STR("  UTF-8") : STR("  UTF-16");
        _status += doc.crlf() ? STR("  CRLF") : STR("  LF");
        _status.appendFormat(STR("  %d, %d (%d)"), doc.line(), doc.column(), doc.position());

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

bool Editor::processInput()
{
    bool update = false, modified = false, autocomplete = false;

    _inputEvents = Console::readInput();
    bool multipleInputEvents = _inputEvents.size() > 1;

    for (int i = 0; i < _inputEvents.size(); ++i)
    {
        InputEvent event = _inputEvents[i];

        if (_document)
        {
            Document& doc = _document->value;

            if (event.eventType == INPUT_EVENT_TYPE_KEY)
            {
                KeyEvent keyEvent = event.event.keyEvent;

                if (keyEvent.keyDown)
                {
                    if (_recordingMacro)
                    {
                        if (!(keyEvent.alt && (keyEvent.ch == 'r' || keyEvent.ch == 'm')))
                            _macro.addLast(event);
                    }

                    if (keyEvent.ctrl)
                    {
                        if (keyEvent.key == KEY_LEFT || keyEvent.ch == 'b')
                        {
                            update = doc.moveWordBack();
                        }
                        else if (keyEvent.key == KEY_RIGHT || keyEvent.ch == 'w')
                        {
                            update = doc.moveWordForward();
                        }
                        else if (keyEvent.key == KEY_UP || keyEvent.ch == '^')
                        {
                            update = moveToPrevRecentLocation();
                        }
                        else if (keyEvent.key == KEY_DOWN)
                        {
                            update = moveToNextRecentLocation();
                        }
                        else if (keyEvent.key == KEY_DELETE || keyEvent.ch == 'd')
                        {
                            modified = update = doc.deleteWordForward();
                        }
                        else if (keyEvent.key == KEY_BACKSPACE || keyEvent.ch == ']')
                        {
                            modified = update = doc.deleteWordBack();
                        }
                        else if (keyEvent.key == KEY_PGUP)
                        {
                            update = doc.moveLines(20);
                        }
                        else if (keyEvent.key == KEY_PGDN)
                        {
                            update = doc.moveLines(20);
                        }
                        else if (keyEvent.key == KEY_HOME)
                        {
                            update = doc.moveToStart();
                        }
                        else if (keyEvent.key == KEY_END)
                        {
                            update = doc.moveToEnd();
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
                            update = doc.moveLines(-(_height - 1));
                        }
                        else if (keyEvent.ch == 'n')
                        {
                            update = doc.moveLines(_height - 1);
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
                            _buffer = doc.copyDeleteText(false);
                            copyToClipboard(_buffer);

                            if (!_buffer.empty())
                                modified = update = true;
                        }
                        else if (keyEvent.ch == 'c' || keyEvent.ch == 'k')
                        {
                            _buffer = doc.copyDeleteText(true);
                            copyToClipboard(_buffer);
                        }
                        else if (keyEvent.ch == 'v' || keyEvent.ch == 'l')
                        {
                            pasteFromClipboard(_buffer);

                            if (!_buffer.empty())
                            {
                                doc.pasteText(_buffer);
                                modified = update = true;
                            }
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
                            update = moveToPrevRecentLocation();
                        }
                        else if (keyEvent.key == KEY_DOWN)
                        {
                            update = moveToNextRecentLocation();
                        }
                        else if (keyEvent.key == KEY_DELETE)
                        {
                            modified = update = doc.deleteWordForward();
                        }
                        else if (keyEvent.key == KEY_BACKSPACE)
                        {
                            modified = update = doc.deleteWordBack();
                        }
                        else if (keyEvent.key == KEY_PGUP || keyEvent.ch == 'p')
                        {
                            update = doc.moveLines(-20);
                        }
                        else if (keyEvent.key == KEY_PGDN || keyEvent.ch == 'n')
                        {
                            update = doc.moveLines(20);
                        }
                        else if (keyEvent.key == KEY_HOME || keyEvent.ch == 'h')
                        {
                            update = doc.moveToStart();
                        }
                        else if (keyEvent.key == KEY_END || keyEvent.ch == 'e')
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
                        else if (keyEvent.ch == 'd')
                        {
                            modified = update = doc.deleteCharsForward();
                        }
                        else if (keyEvent.ch == ']')
                        {
                            modified = update = doc.deleteCharsBack();
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
                        else if (keyEvent.ch == '\\')
                        {
                            doc.uncommentLines();
                            modified = update = true;
                        }
                        else if (keyEvent.ch == '-')
                        {
                            doc.unindentLines();
                            modified = update = true;
                        }
                        else if (keyEvent.ch == '=')
                        {
                            doc.indentLines();
                            modified = update = true;
                        }
                        else if (keyEvent.ch == '\'')
                        {
                            findUniqueWords();
                            updateScreen(true);
                        }
                        else if (keyEvent.ch >= '0' && keyEvent.ch <= '9')
                        {
                            int n = keyEvent.ch == '0' ? 10 : keyEvent.ch - '0';

                            for (auto doc = _documents.first(); doc; doc = doc->next)
                            {
                                if (--n == 0)
                                {
                                    if (doc != _document)
                                    {
                                        _document = doc;
                                        update = true;
                                    }
                                    break;
                                }
                            }
                        }
                        else if (keyEvent.ch == 'r')
                        {
                            if (_recordingMacro)
                                _recordingMacro = false;
                            else
                            {
                                _recordingMacro = true;
                                _macro.clear();
                            }
                        }
                        else if (keyEvent.ch == 'm')
                        {
                            if (!_recordingMacro && _macro.size() > 0)
                            {
                                multipleInputEvents = false;
                                for (int j = 0; j < _macro.size(); ++j)
                                    _inputEvents.addLast(_macro[j]);
                            }
                        }
                    }
                    else if (keyEvent.key == KEY_F2)
                    {
                        showCommandLine();
                        update = true;
                    }
                    else if (keyEvent.key == KEY_F5)
                    {
                        buildProject();
                        updateScreen(true);
                    }
                    else if (keyEvent.key == KEY_F8)
                    {
                        saveDocument();
                        update = true;
                    }
                    else if (keyEvent.key == KEY_F9)
                    {
                        saveAllDocuments();
                        update = true;
                    }
                    else if (keyEvent.key == KEY_F10)
                    {
                        saveAllDocuments();
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
                        update = doc.moveLines(-1);
                    }
                    else if (keyEvent.key == KEY_DOWN)
                    {
                        update = doc.moveLines(1);
                    }
                    else if (keyEvent.key == KEY_BACKSPACE)
                    {
                        modified = update = doc.deleteCharBack();

                        if (_currentSuggestion != INVALID_POSITION)
                        {
                            _currentSuggestion = INVALID_POSITION;
                            if (completeWord(1))
                                autocomplete = true;
                        }
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
                        update = doc.moveLines(-(_height - 1));
                    }
                    else if (keyEvent.key == KEY_PGDN)
                    {
                        update = doc.moveLines(_height - 1);
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
                                {
                                    if (!processCommand(_commandLine.value.text()))
                                        return false;
                                }
                            }
                            catch (Exception& ex)
                            {
                                _message = ex.message();
                            }
                        }
                        else
                        {
                            if (multipleInputEvents)
                                doc.insertChar('\n');
                            else
                                doc.insertNewLine();
                            modified = update = true;
                        }
                    }
                    else if (keyEvent.key == KEY_TAB)
                    {
                        if (keyEvent.shift)
                        {
                            if (completeWord(-1))
                                autocomplete = true;
                            else
                                doc.unindentLines();
                        }
                        else
                        {
                            if (completeWord(1))
                                autocomplete = true;
                            else
                                doc.indentLines();
                        }

                        modified = update = true;
                    }
                    else if (keyEvent.key == KEY_ESC)
                    {
                        if (_document == &_commandLine)
                        {
                            _document = _lastDocument;
                            update = true;
                        }
                        else
                            return false;
                    }
                    else if (charIsPrint(keyEvent.ch))
                    {
                        if (_currentSuggestion == INVALID_POSITION)
                            doc.insertChar(keyEvent.ch);
                        else
                        {
                            if (charIsWord(keyEvent.ch))
                            {
                                doc.insertChar(keyEvent.ch);
                                if (completeWord(0))
                                    autocomplete = true;
                            }
                            else
                            {
                                doc.insertChar(keyEvent.ch, true);
                                _uniqueWords[_suggestions[_currentSuggestion].word] = INT_MAX;
                                _currentSuggestion = INVALID_POSITION;
                            }
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
                        update = doc.moveLines(-20);
                    else if (mouseEvent.button == MOUSE_BUTTON_WHEEL_DOWN)
                        update = doc.moveLines(20);
                }

            }
            else if (event.eventType == INPUT_EVENT_TYPE_WINDOW)
            {
                WindowEvent windowEvent = event.event.windowEvent;

                _width = windowEvent.width;
                _height = windowEvent.height;

                setDimensions();
                updateScreen(true);
            }
        }
        else
        {
            if (event.eventType == INPUT_EVENT_TYPE_KEY)
            {
                KeyEvent keyEvent = event.event.keyEvent;

                if (keyEvent.keyDown)
                {
                    if (keyEvent.key == KEY_F2)
                    {
                        showCommandLine();
                        update = true;
                    }
                    else if (keyEvent.key == KEY_F5)
                    {
                        buildProject();
                        updateScreen(true);
                    }
                    else if (keyEvent.key == KEY_ESC)
                    {
                        if (_document == &_commandLine)
                        {
                            _document = _lastDocument;
                            update = true;
                        }
                        else
                            return false;
                    }
                }
            }
            else if (event.eventType == INPUT_EVENT_TYPE_WINDOW)
            {
                WindowEvent windowEvent = event.event.windowEvent;

                _width = windowEvent.width;
                _height = windowEvent.height;

                setDimensions();
                updateScreen(true);
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

void Editor::showCommandLine()
{
    if (_document != &_commandLine)
    {
        _lastDocument = _document;
        _document = &_commandLine;
        _commandLine.value.clear();
    }
}

void Editor::buildProject()
{
    Console::setLineMode(true);
    Console::clear();

    saveAllDocuments();

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

bool Editor::processCommand(const String& command)
{
    ASSERT(!command.empty());

    if (command == STR("s"))
    {
        saveDocument();
        return true;
    }
    else if (command == STR("sa"))
    {
        saveAllDocuments();
        return true;
    }
    else if (command == STR("c"))
    {
        closeDocument();
        return true;
    }
    else if (command == STR("q"))
    {
        return false;
    }
    else if (command == STR("qs"))
    {
        saveAllDocuments();
        return false;
    }

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
    else if (ch == 'n')
    {
        p = command.charForward(p);
        if (command.charAt(p) == ' ')
        {
            p = command.charForward(p);
            String filename = command.substr(p);

            if (!filename.empty())
                newDocument(filename);
            else
                throw Exception(STR("invalid filename"));
        }
        else
            throw Exception(STR("invalid command"));
    }
    else if (ch == 'o')
    {
        p = command.charForward(p);
        if (command.charAt(p) == ' ')
        {
            p = command.charForward(p);
            String filename = command.substr(p);

            if (!filename.empty())
                openDocument(filename);
            else
                throw Exception(STR("invalid filename"));
        }
        else
            throw Exception(STR("invalid command"));
    }
    else
        throw Exception(STR("invalid command"));

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

bool Editor::completeWord(int next)
{
    String prefix = _document->value.autocompletePrefix();

    if (prefix.length() > 0)
    {
        if (_currentSuggestion == INVALID_POSITION)
            prepareSuggestions(prefix);
        else if (!_suggestions[_currentSuggestion].word.startsWith(prefix))
        {
            prepareSuggestions(prefix);
            _currentSuggestion = INVALID_POSITION;
        }

        if (_suggestions.size() > 0)
        {
            int last = _suggestions.size() - 1;

            if (_currentSuggestion == INVALID_POSITION)
                _currentSuggestion = next >= 0 ? 0 : last;
            else
            {
                if (next > 0)
                    _currentSuggestion = _currentSuggestion < last ? _currentSuggestion + 1 : 0;
                else if (next < 0)
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
        if (argc == 2 && strCompare(argv[1], STR("--version")) == 0)
        {
            Console::writeLine(STR("eve text editor version 1.5\n"
                "web: andrewshark.github.io/eve\n"
                "Copyright (C) Andrey Levichev, 2018\n\n"
                "usage: eve [FILE]...\n\n"));
        }
        else
        {
            Editor editor;

            for (int i = 1; i < argc; ++i)
                editor.openDocument(argv[i]);

            editor.run();
        }
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
