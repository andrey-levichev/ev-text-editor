#ifndef EVE_INCLUDED
#define EVE_INCLUDED

#include <foundation.h>
#include <console.h>
#include <file.h>

// Text

class Text : public String
{
public:
    Text()
    {
        ensureCapacity(1);
        _position = _chars;
    }

    void position(char_t* pos)
    {
        ASSERT(_chars <= pos && pos <= _chars + _length);
        _position = pos;
    }

    char_t* position() const
    {
        return _position;
    }

    void assign(const String& str)
    {
        String::assign(str);
        _position = _chars;
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

    String copyDeleteText(char_t* pos, bool copy);
    void pasteText(const String& text, bool lineSelection);

    char_t* findCurrentLineStart();
    char_t* findCurrentLineEnd();
    char_t* findLine(int line);

    bool findNext(const String& pattern);
    String currentWord();

    void trimTrailingWhitespace();

protected:
    static bool isIdent(unichar_t ch);

protected:
    char_t* _position;
    String _indent;
};

// Editor

class Editor
{
public:
    Editor(const char_t* filename);
    ~Editor();

    void run();

protected:
    void positionToLineColumn();
    void lineColumnToPosition();

    void updateScreen(bool redrawAll = false);
    bool processKey();

    void openFile();
    void saveFile();

    String getCommand(const char_t* prompt);
    void buildProject();

protected:
    Text _text;
    TextEncoding _encoding;
    bool _bom;
    bool _crLf;
    
    String _buffer;
    String _pattern;

    CharArray _screen;
    CharArray _window;
    String _filename;
    String _status;

    int _width, _height, _screenHeight;
    int _top, _left;

    int _line, _column, _preferredColumn;
    char_t* _selection;
    bool _lineSelection;
};

#endif
