#ifndef EVE_INCLUDED
#define EVE_INCLUDED

#include <foundation.h>
#include <console.h>
#include <file.h>

// Document

class Document
{
public:
    Document();

    int length() const
    {
        return _text.length();
    }

    int charLength() const
    {
        return _text.charLength();
    }

    const char_t* chars() const
    {
        return _text.chars();
    }

    unichar_t charAt(int pos) const
    {
        return _text.charAt(pos);
    }

    int charForward(int pos, int n = 1) const
    {
        return _text.charForward(pos, n);
    }

    int charBack(int pos, int n = 1) const
    {
        return _text.charBack(pos, n);
    }

    void position(int pos)
    {
        ASSERT(pos >= 0 && pos <= _text.length());
        _position = pos;
    }

    int position() const
    {
        return _position;
    }

    const String& filename() const
    {
        return _filename;
    }

    TextEncoding encoding() const
    {
        return _encoding;
    }

    bool crlf() const
    {
        return _crLf;
    }

    int line() const
    {
        return _line;
    }
    
    int column() const
    {
        return _column;
    }

    int selection() const
    {
        return _selection;
    }

    bool moveUp();
    bool moveDown();

    bool moveForward();
    bool moveBack();

    bool moveWordForward();
    bool moveWordBack();

    bool moveToStart();
    bool moveToEnd();

    bool moveToLineStart();
    bool moveToLineEnd();

    bool moveLinesUp(int lines);
    bool moveLinesDown(int lines);

    void insertChar(unichar_t ch);

    bool deleteCharForward();
    bool deleteCharBack();

    bool deleteWordForward();
    bool deleteWordBack();

    void markSelection();
    String copyDeleteText(bool copy);
    void pasteText(const String& text, bool lineSelection);

    int findCurrentLineStart() const;
    int findCurrentLineEnd() const;
    int findLine(int line) const;

    bool findNext(const String& pattern);
    String currentWord() const;

    void trimTrailingWhitespace();

    void positionToLineColumn();
    void lineColumnToPosition();

    void open(const String& filename);
    void save();

protected:
    static bool isIdent(unichar_t ch);

protected:
    String _text;
    int _position;
    bool _modified;

    String _filename;
    TextEncoding _encoding;
    bool _bom;
    bool _crLf;

    int _line, _column, _preferredColumn;
    int _selection;

    String _indent;
};

// Editor

class Editor
{
public:
    Editor();
    ~Editor();

    void openDocument(const char_t* filename)
    {
        _document.open(filename);
    }

    void saveDocument()
    {
        _document.save();
    }

    void run();

protected:
    void updateScreen(bool redrawAll = false);
    bool processKey();

    String getCommand(const char_t* prompt);
    void buildProject();

protected:
    Document _document;
    
    String _buffer;
    bool _lineSelection;

    String _pattern;

    CharArray _screen;
    CharArray _window;
    String _status;

    int _width, _height, _screenHeight;
    int _top, _left;
};

#endif
