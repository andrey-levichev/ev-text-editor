#ifndef EVE_INCLUDED
#define EVE_INCLUDED

#include <foundation.h>
#include <console.h>
#include <file.h>

// Document

class Document
{
public:
    Document() :
        _position(0),
#ifdef PLATFORM_WINDOWS
        _encoding(TEXT_ENCODING_UTF16_LE),
        _bom(true),
        _crLf(true)
#else
        _encoding(TEXT_ENCODING_UTF8),
        _bom(false),
        _crLf(false)
#endif
    {
        _text.ensureCapacity(1);
    }

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

    bool moveForward();
    bool moveBack();

    bool moveWordForward();
    bool moveWordBack();

    bool moveToStart();
    bool moveToEnd();

    bool moveToLineStart();
    bool moveToLineEnd();

    void insertChar(unichar_t ch);

    bool deleteCharForward();
    bool deleteCharBack();

    bool deleteWordForward();
    bool deleteWordBack();

    String copyDeleteText(int pos, bool copy);
    void pasteText(const String& text, bool lineSelection);

    int findCurrentLineStart();
    int findCurrentLineEnd();
    int findLine(int line);

    bool findNext(const String& pattern);
    String currentWord();

    void trimTrailingWhitespace();

    void open(const String& filename);
    void save();

protected:
    static bool isIdent(unichar_t ch);

protected:
    String _text;
    int _position;

    String _filename;
    TextEncoding _encoding;
    bool _bom;
    bool _crLf;

    String _indent;
};

// Editor

class Editor
{
public:
    Editor();
    ~Editor();

    void openDocument(const char_t* filename);
    void saveDocument();

    void run();

protected:
    void positionToLineColumn();
    void lineColumnToPosition();

    void updateScreen(bool redrawAll = false);
    bool processKey();

    String getCommand(const char_t* prompt);
    void buildProject();

protected:
    Document _document;
    
    String _buffer;
    String _pattern;

    CharArray _screen;
    CharArray _window;
    String _status;

    int _width, _height, _screenHeight;
    int _top, _left;

    int _line, _column, _preferredColumn;
    int _selection;
    bool _lineSelection;
};

#endif
