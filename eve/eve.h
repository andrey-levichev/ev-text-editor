#ifndef EVE_INCLUDED
#define EVE_INCLUDED

#include <foundation.h>
#include <console.h>
#include <file.h>

// Text

class Text : public String
{
public:
    Text() :
        _position(0)
    {
    }

    int& position()
    {
        return _position;
    }

    const int& position() const
    {
        return _position;
    }

    bool charForward();
    bool charBack();

    bool wordForward();
    bool wordBack();

    bool toStart();
    bool toEnd();

    bool toLineStart();
    bool toLineEnd();

    void insertChar(char_t ch);

    bool deleteCharForward();
    bool deleteCharBack();

    bool deleteWordForward();
    bool deleteWordBack();

    String copyDeleteText(int pos, bool copy);
    void pasteText(const String& text, bool lineSelection);

    char_t* findChar(char_t ch) const;
    char_t* findCharBack(char_t ch) const;
    char_t* findLine(int line) const;

    bool findNext(const String& pattern);
    String currentWord() const;

    void trimTrailingWhitespace();

protected:
    static bool isIdent(char_t ch);

protected:
    int _position;
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
    String _buffer;
    String _pattern;

    CharArray _screen;
    CharArray _window;
    CharArray _commandLine;

    String _filename;

    int _width, _height, _screenHeight;
    int _top, _left;

    int _line, _column, _preferredColumn;
    int _selection;
    bool _lineSelection;
};

#endif
