#include <eve.h>
#include <dictionary.h>

const int TAB_SIZE = 4;

bool charIsWord(unichar_t ch)
{
    return charIsAlphaNum(ch) || ch == '_' || ch == '-' || ch == '\'';
}

bool isWordBoundary(unichar_t prevCh, unichar_t ch)
{
    return (!charIsWord(prevCh) && charIsWord(ch)) ||
        ((charIsWord(prevCh) || charIsSpace(prevCh)) && !(charIsWord(ch) || charIsSpace(ch))) ||
        (prevCh != '\n' && ch == '\n');
}

// Document

Document::Document()
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

void Document::toggleComment()
{
    changeLines(&Document::toggleComment);
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

int Document::findPosition(const String& searchStr, bool caseSesitive, bool next)
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

    _top = _left = 1;
    _width = 100; _height = 50;

    _selection = -1;
    _selectionMode = false;
}

void Document::setDimensions(int width, int height)
{
    ASSERT(width > 0 && height > 0);

    _width = width;
    _height = height;
}

void Document::draw(int x, int y, int width, UniCharArray& screen, bool unicodeLimit16)
{
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
        q = (y + j - 2) * width + x - 1;

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
    ASSERT(pos >= 0);

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
    ASSERT(pos >= 0);

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
            return start;
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
    findUniqueWords(_document->value);
}

void Editor::saveDocument(Document& document)
{
    if (document.modified())
    {
        document.save();
        findUniqueWords(document);
    }

    prepareSuggestions();
}

void Editor::saveDocuments()
{
    for (auto doc = _documents.first(); doc; doc = doc->next)
    {
        if (doc->value.modified())
        {
            doc->value.save();
            findUniqueWords(doc->value);
        }
    }

    prepareSuggestions();
}

void Editor::run()
{
    _document = _documents.first();

    int width, height;
    Console::getSize(width, height);
    setDimensions(width, height);

    prepareSuggestions();
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
        _screen.assign(_width * _height, ' ');
        _output.clear();

        int x = 2, y = 2;
        _document->value.draw(x, y, _width, _screen, _unicodeLimit16);

        for (int i = 0; i < _width * (_height - 1); ++i)
            _output += _screen[i];

        _status = _document->value.filename();

        if (_document->value.modified())
            _status += '*';

        _status += _document->value.encoding() == TEXT_ENCODING_UTF8 ? STR("  UTF-8") : STR("  UTF-16");
        _status += _document->value.crlf() ? STR("  CRLF") : STR("  LF");
        _status.appendFormat(STR("  %d, %d"), _document->value.line(), _document->value.column());

        int len = _status.charLength();

        if (len <= _width)
        {
            _output.append(' ', _width - len);
            _output.append(_status);
        }
        else
        {
            int p = _status.charBack(_status.length(), _width - 3);
            _output.append(STR("..."), 3);
            _output.append(_status.chars() + p, _status.length() - p);
        }

        ASSERT(_output.charLength() == _width * _height);

        Console::write(1, 1, _output);
        Console::setCursorPosition(
            _document->value.line() - _document->value.top() + x,
            _document->value.column() - _document->value.left() + y);
    }
    else
        Console::clear();

#ifndef PLATFORM_WINDOWS
    Console::showCursor(true);
#endif
}

void Editor::setDimensions(int width, int height)
{
    ASSERT(width > 0 && height > 1);

    _width = width;
    _height = height;

    for (auto doc = _documents.first(); doc; doc = doc->next)
        doc->value.setDimensions(20, 10);

    _screen.ensureCapacity(_width * _height);
}

bool Editor::processInput()
{
    bool update = false;
    bool modified = false;
    const Array<InputEvent>& inputEvents = Console::readInput();

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
                        if (keyEvent.key == KEY_LEFT)
                        {
                            update = _document->value.moveWordBack();
                        }
                        else if (keyEvent.key == KEY_RIGHT)
                        {
                            update = _document->value.moveWordForward();
                        }
                        else if (keyEvent.key == KEY_UP)
                        {
                            update = _document->value.moveLinesUp(10);
                        }
                        else if (keyEvent.key == KEY_DOWN)
                        {
                            update = _document->value.moveLinesDown(10);
                        }
                        else if (keyEvent.key == KEY_TAB || keyEvent.ch == 't')
                        {
                            _document->value.insertChar(0x9);
                            update = modified = true;
                        }
                    }
                    else if (keyEvent.alt)
                    {
                        if (keyEvent.key == KEY_LEFT)
                        {
                            update = _document->value.moveCharsBack();
                        }
                        else if (keyEvent.key == KEY_RIGHT)
                        {
                            update = _document->value.moveCharsForward();
                        }
                        else if (keyEvent.key == KEY_UP)
                        {
                            moveToPrevRecentLocation();
                        }
                        else if (keyEvent.key == KEY_DOWN)
                        {
                            moveToNextRecentLocation();
                        }
                        else if (keyEvent.key == KEY_DELETE)
                        {
                            modified = update = _document->value.deleteWordForward();
                        }
                        else if (keyEvent.key == KEY_BACKSPACE)
                        {
                            modified = update = _document->value.deleteWordBack();
                        }
                        else if (keyEvent.key == KEY_HOME)
                        {
                            update = _document->value.moveToStart();
                        }
                        else if (keyEvent.key == KEY_END)
                        {
                            update = _document->value.moveToEnd();
                        }
                        else if (keyEvent.key == KEY_PGUP)
                        {
                            update = _document->value.moveLinesUp(_height / 2);
                        }
                        else if (keyEvent.key == KEY_PGDN)
                        {
                            update = _document->value.moveLinesDown(_height / 2);
                        }
                        else if (keyEvent.ch == ',')
                        {
                            auto doc = _document->prev ? _document->prev : _documents.last();
                            if (doc != _document)
                            {
                                _document = doc;
                                updateScreen(true);
                            }
                        }
                        else if (keyEvent.ch == '.')
                        {
                            auto doc = _document->next ? _document->next : _documents.first();
                            if (doc != _document)
                            {
                                _document = doc;
                                updateScreen(true);
                            }
                        }
                        else if (keyEvent.ch == 'q')
                        {
                            return false;
                        }
                        else if (keyEvent.ch == 's')
                        {
                            saveDocument(_document->value);
                            update = true;
                        }
                        else if (keyEvent.ch == 'a')
                        {
                            saveDocuments();
                            update = true;
                        }
                        else if (keyEvent.ch == 'x')
                        {
                            saveDocuments();
                            return false;
                        }
                        else if (keyEvent.ch == 'd')
                        {
                            _lineSelection = _document->value.selection() < 0;
                            _buffer = _document->value.copyDeleteText(false);

                            if (!_buffer.empty())
                                modified = update = true;
                        }
                        else if (keyEvent.ch == 'c')
                        {
                            _lineSelection = _document->value.selection() < 0;
                            _buffer = _document->value.copyDeleteText(true);
                        }
                        else if (keyEvent.ch == 'p')
                        {
                            if (!_buffer.empty())
                            {
                                _document->value.pasteText(_buffer, _lineSelection);
                                modified = update = true;
                            }
                        }
                        else if (keyEvent.ch == 'b')
                        {
                            buildProject();
                            return true;
                        }
                        else if (keyEvent.ch == 'm')
                        {
                            _document->value.markSelection();
                        }
                        else if (keyEvent.ch == 'w')
                        {
                            _searchStr = _document->value.currentWord();
                            _caseSesitive = true;
                            update = _document->value.find(_searchStr, _caseSesitive, true);
                        }
                        else if (keyEvent.ch == 'f')
                        {
                            update = _document->value.find(_searchStr, _caseSesitive, true);
                        }
                        else if (keyEvent.ch == 'r')
                        {
                            modified = update = _document->value.replace(_searchStr, _replaceStr, _caseSesitive);
                        }
                        else if (keyEvent.ch == '/')
                        {
                            _document->value.toggleComment();
                            modified = update = true;
                        }
                        else if (keyEvent.ch == '[')
                        {
                            completeWord(false);
                            modified = update = true;
                        }
                        else if (keyEvent.ch == ']')
                        {
                            completeWord(true);
                            modified = update = true;
                        }
                        else if (keyEvent.ch == '`')
                        {
                            try
                            {
                                processCommand();
                                update = true;
                            }
                            catch (Exception& ex)
                            {
                                Console::write(_height, 1, ex.message());
                            }
                        }
                    }
                    else if (keyEvent.shift && keyEvent.key == KEY_TAB)
                    {
                        _document->value.unindentLines();
                        modified = update = true;
                    }
                    else if (keyEvent.key == KEY_BACKSPACE)
                    {
                        modified = update = _document->value.deleteCharBack();
                    }
                    else if (keyEvent.key == KEY_DELETE)
                    {
                        modified = update = _document->value.deleteCharForward();
                    }
                    else if (keyEvent.key == KEY_LEFT)
                    {
                        update = _document->value.moveBack();
                    }
                    else if (keyEvent.key == KEY_RIGHT)
                    {
                        update = _document->value.moveForward();
                    }
                    else if (keyEvent.key == KEY_UP)
                    {
                        update = _document->value.moveUp();
                    }
                    else if (keyEvent.key == KEY_DOWN)
                    {
                        update = _document->value.moveDown();
                    }
                    else if (keyEvent.key == KEY_HOME)
                    {
                        update = _document->value.moveToLineStart();
                    }
                    else if (keyEvent.key == KEY_END)
                    {
                        update = _document->value.moveToLineEnd();
                    }
                    else if (keyEvent.key == KEY_PGUP)
                    {
                        update = _document->value.moveLinesUp(_height - 1);
                    }
                    else if (keyEvent.key == KEY_PGDN)
                    {
                        update = _document->value.moveLinesDown(_height - 1);
                    }
                    else if (keyEvent.key == KEY_ENTER)
                    {
                        if (inputEvents.size() == 1)
                            _document->value.insertNewLine();
                        else
                            _document->value.insertChar('\n');
                        modified = update = true;
                    }
                    else if (keyEvent.key == KEY_TAB)
                    {
                        _document->value.indentLines();
                        modified = update = true;
                    }
                    else if (keyEvent.key == KEY_ESC)
                    {
                        cancelCompletion();
                        modified = update = true;
                    }
                    else if (charIsPrint(keyEvent.ch))
                    {
                        if (inputEvents.size() == 1)
                        {
                            if (charIsWord(keyEvent.ch))
                            {
                                _document->value.insertChar(keyEvent.ch);

                                _currentSuggestion = INVALID_POSITION;
                                completeWord(true);
                            }
                            else
                            {
                                if (_currentSuggestion != INVALID_POSITION)
                                {
                                    _currentSuggestion = INVALID_POSITION;
                                    _document->value.insertChar(keyEvent.ch, true);
                                }
                                else
                                    _document->value.insertChar(keyEvent.ch);
                            }
                        }
                        else
                            _document->value.insertChar(keyEvent.ch);

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
                        update = _document->value.moveToLineColumn(
                            _document->value.top() + mouseEvent.y - 1,
                            _document->value.left() +  mouseEvent.x - 1);
                    else if (mouseEvent.button == MOUSE_BUTTON_WHEEL_UP)
                        update = _document->value.moveLinesUp(10);
                    else if (mouseEvent.button == MOUSE_BUTTON_WHEEL_DOWN)
                        update = _document->value.moveLinesDown(10);
                }

            }
            else if (event.eventType == INPUT_EVENT_TYPE_WINDOW)

            {
                WindowEvent windowEvent = event.event.windowEvent;
                setDimensions(windowEvent.width, windowEvent.height);
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
                    if (keyEvent.alt)
                    {
                        if (keyEvent.ch == 'q')
                        {
                            return false;
                        }
                    }
                }
            }
        }
    }

    if (update)
        updateScreen();

    if (modified)
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

void Editor::moveToNextRecentLocation()
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
                return;
        }

        _document = _recentLocation->value.document;
        _document->value.moveToLine(_recentLocation->value.line);
        updateScreen(true);
    }
}

void Editor::moveToPrevRecentLocation()
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
                return;
        }

        _document = _recentLocation->value.document;
        _document->value.moveToLine(_recentLocation->value.line);
        updateScreen(true);
    }
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
            Console::write(_height, 1, cmdLine);
            Console::write(_height, len + 1, ' ', _width - len);
        }
        else
        {
            p = _width;
            Console::write(_height, 1,
                cmdLine.chars() + cmdLine.charBack(cmdLine.length(), _width - 1), _width - 1);
        }

        Console::setCursorPosition(_height, p);

        const Array<InputEvent>& inputEvents = Console::readInput();

        for (int i = 0; i < inputEvents.size(); ++i)
        {
            InputEvent event = inputEvents[i];

            if (event.eventType == INPUT_EVENT_TYPE_KEY)
            {
                KeyEvent keyEvent = event.event.keyEvent;

                if (keyEvent.keyDown)
                {
                    if (keyEvent.key == KEY_ENTER)
                    {
                        return cmdLine.substr(promptLength);
                    }
                    else if (keyEvent.key == KEY_ESC)
                    {
                        return String();
                    }
                    else if (keyEvent.key == KEY_BACKSPACE)
                    {
                        if (cmdLine.length() > promptLength)
                            cmdLine.erase(cmdLine.charBack(cmdLine.length()));
                    }
                    else if (charIsPrint(keyEvent.ch))
                    {
                        cmdLine += keyEvent.ch;
                    }
                }
            }
        }
    }
}

void Editor::processCommand()
{
    _command = getCommand(STR(":"));

    if (_command.empty())
        return;

    int p = 0;
    unichar_t ch = _command.charAt(p);

    if (ch == 'f')
    {
        _caseSesitive = true;

        while (true)
        {
            p = _command.charForward(p);
            ch = _command.charAt(p);

            if (ch == 'i')
                _caseSesitive = false;
            else if (ch == ' ')
                break;
            else
                throw Exception(STR("invalid command"));
        }

        p = _command.charForward(p);
        if (p < _command.length())
        {
            _searchStr = _command.substr(p);
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
            p = _command.charForward(p);
            ch = _command.charAt(p);

            if (ch == 'i')
                _caseSesitive = false;
            else if (ch == 'd' || ch == 'a')
                replaceScope = ch;
            else if (ch == 0)
                throw Exception(STR("invalid command"));
            else
                break;
        }

        p = _command.charForward(p);
        if (p < _command.length())
        {
            int q = _command.find(ch, true, p);
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
        p = _command.charForward(p);
        if (_command.charAt(p) == ' ')
        {
            p = _command.charForward(p);
            int line = _command.substr(p).toInt();

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
    system("nmake");
#else
    system("make");
#endif

    Console::writeLine(STR("Press any key to continue..."));
    Console::readChar();

    Console::setLineMode(false);
    updateScreen(true);
}

void Editor::findUniqueWords(const Document& document)
{
    const String& text = document.text();
    String word;

    for (int p = 0; p < text.length(); p = text.charForward(p))
    {
        unichar_t ch = text.charAt(p);

        if (charIsWord(ch))
            word += ch;
        else if (!word.empty())
        {
            ++_uniqueWords[word];
            word.clear();
        }
    }

    if (!word.empty())
        ++_uniqueWords[word];
}

void Editor::prepareSuggestions()
{
    int n = 0;
    while (*dictionary[n])
        ++n;

    _suggestions.clear();
    _suggestions.ensureCapacity(_uniqueWords.size() + n);

    auto it = _uniqueWords.constIterator();
    while (it.moveNext())
        _suggestions.addLast(AutocompleteSuggestion(it.value().key, n + it.value().value));

    for (int i = 0; i < n; ++i)
        _suggestions.addLast(AutocompleteSuggestion(dictionary[i], n - i));

    _suggestions.sort();
}

int Editor::findNextSuggestion(const String& prefix, int currentSuggestion) const
{
    ASSERT(!prefix.empty());
    ASSERT(currentSuggestion == INVALID_POSITION ||
        (currentSuggestion >= 0 && currentSuggestion < _suggestions.size()));

    if (_suggestions.size() > 0)
    {
        int i = currentSuggestion == INVALID_POSITION ? 0 : currentSuggestion + 1;

        for (; i < _suggestions.size(); ++i)
            if (_suggestions[i].word.startsWith(prefix))
                break;

        if (i < _suggestions.size())
            return i;

        if (currentSuggestion != INVALID_POSITION)
        {
            for (i = 0; i < currentSuggestion; ++i)
                if (_suggestions[i].word.startsWith(prefix))
                    break;

            if (i < currentSuggestion)
                return i;
        }
    }

    return INVALID_POSITION;
}

int Editor::findPrevSuggestion(const String& prefix, int currentSuggestion) const
{
    ASSERT(!prefix.empty());
    ASSERT(currentSuggestion == INVALID_POSITION ||
        (currentSuggestion >= 0 && currentSuggestion < _suggestions.size()));

    if (_suggestions.size() > 0)
    {
        int i = currentSuggestion == INVALID_POSITION ?
            _suggestions.size() - 1 : currentSuggestion - 1;

        for (; i >= 0; --i)
            if (_suggestions[i].word.startsWith(prefix))
                break;

        if (i >= 0)
            return i;

        if (currentSuggestion != INVALID_POSITION)
        {
            for (i = _suggestions.size() - 1; i > currentSuggestion; --i)
                if (_suggestions[i].word.startsWith(prefix))
                    break;

            if (i > currentSuggestion)
                return i;
        }
    }

    return INVALID_POSITION;
}

void Editor::completeWord(bool next)
{
    String prefix = _document->value.autocompletePrefix();

    if (prefix.length() > 1)
    {
        int suggestion = next ?
            findNextSuggestion(prefix, _currentSuggestion) :
            findPrevSuggestion(prefix, _currentSuggestion);

        _currentSuggestion = suggestion;
        _document->value.completeWord(suggestion == INVALID_POSITION ?
            prefix : _suggestions[suggestion].word);
    }
}

void Editor::cancelCompletion()
{
    _currentSuggestion = INVALID_POSITION;
    String prefix = _document->value.autocompletePrefix();

    if (prefix.length() > 1)
        _document->value.completeWord(prefix);
}

int MAIN(int argc, const char_t** argv)
{
    try
    {
        if (argc < 2)
        {
            Console::writeLine(STR("eve text editor version 1.2\n"
                "Copyright (C) Andrey Levichev, 2017\n\n"
                "usage: eve filename ...\n\n"
                "keys:\n"
                "arrows - move cursor\n"
                "ctrl+left/right - word (identifier) left/right\n"
                "ctrl+up/down - 10 lines up/down\n"
                "alt+left/right - word (space separated) left/right\n"
                "alt+up/down - previous/next edited location\n"
                "home/end - start/end of line\n"
                "alt+home/end - start/end of file\n"
                "pgup/pgdn - page up/down\n"
                "alt+pgup/pgdn - half page up/down\n"
                "tab - indent line/selection\n"
                "shift+tab - unindent line/selection\n"
                "delete - delete character at cursor position\n"
                "backspace - delete character to the left of cursor position\n"
                "alt+del - delete word at cursor position\n"
                "alt+backspace - delete word to the left of cursor position\n"
                "alt+m - mark selection start\n"
                "alt+d - delete line/selection\n"
                "alt+c - copy line/selection\n"
                "alt+p - paste line/selection\n"
                "alt+w - find word at cursor\n"
                "alt+f - find again\n"
                "alt+r - replace and find again\n"
                "alt+/ - toggle comment for line/selection\n"
                "alt+b - build with make\n"
                "alt+, - go to previous document\n"
                "alt+. - go to next document\n"
                "alt+s - save\n"
                "alt+a - save all\n"
                "alt+x - save all and quit\n"
                "alt+q - quit without saving\n"
                "alt+` - command line\n"
                "alt+[ - previous autocomplete suggestion\n"
                "alt+] - next autocomplete suggestion\n"
                "ESC - cancel current suggestion\n\n"
                "commands (optional parameters in square brackets):\n"
                "g <number> - go to line number\n"
                "f[i] <string> - find string\n"
                "\ti - ignore case\n"
                "r[ida] <searchString> <replaceString> - replace string\n"
                "\ti - ignore case\n"
                "\td - replace all matches in current document\n"
                "\ta - replace all matches in all documents\n"
                "\tuse any character as string separator instead of space\n"));

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
