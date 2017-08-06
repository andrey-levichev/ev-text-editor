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
        _position = 0;
    }

    void position(int pos)
    {
        ASSERT(pos >= 0 && pos <= _length);
        _position = pos;
    }

    int position() const
    {
        return _position;
    }

    void assign(const String& str)
    {
        String::assign(str);
        _position = 0;
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

protected:
    static bool isIdent(unichar_t ch);

protected:
    int _position;
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
    int _selection;
    bool _lineSelection;
};

#endif
