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

    const String& text() const
    {
        return _text;
    }

    int position() const
    {
        return _position;
    }

    bool modified() const
    {
        return _modified;
    }

    void filename(const String& filename)
    {
        _filename = filename;
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

    int top() const
    {
        return _top;
    }

    int left() const
    {
        return _left;
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
    bool moveToLine(int line);

    void insertNewLine();
    void insertChar(unichar_t ch);

    bool deleteCharForward();
    bool deleteCharBack();

    bool deleteWordForward();
    bool deleteWordBack();

    void indentLines();
    void unindentLines();
    void toggleComment();

    void markSelection();
    String copyDeleteText(bool copy);
    void pasteText(const String& text, bool lineSelection);

    void lineColumnToTopLeft(int width, int height);
    int findLine(int line) const;

    String currentWord() const;

    int findPosition(const String& searchStr, bool next);
    bool find(const String& searchStr, bool next);
    bool replace(const String& searchStr, const String& replaceStr);

    void open(const String& filename);
    void save();
    void clear();

protected:
    int findLineStart(int pos) const;
    int findLineEnd(int pos) const;
    int findNextLine(int pos) const;
    int findPreviousLine(int pos) const;

    void changeLines(int(Document::* lineOp)(int));

    int indentLine(int pos);
    int unindentLine(int pos);
    int toggleComment(int pos);

    void positionToLineColumn();
    void lineColumnToPosition();

    void trimTrailingWhitespace();

    static bool charIsIdent(unichar_t ch);
    static bool isWordBoundary(unichar_t prevCh, unichar_t ch);

protected:
    String _text;
    int _position;
    bool _modified;

    String _filename;
    TextEncoding _encoding;
    bool _bom;
    bool _crLf;

    int _line, _column, _preferredColumn;
    int _top, _left;

    int _selection;
    bool _selectionMode;

    String _indent;
};

// Editor

class Editor
{
public:
    Editor();
    ~Editor();

    void openDocument(const char_t* filename);
    void saveDocuments();

    void run();

protected:
    void updateScreen(bool redrawAll = false);
    bool processKey();

    String getCommand(const char_t* prompt);
    void processCommand();

    void buildProject();

protected:
    List<Document> _documents;
    ListNode<Document>* _document;

    String _buffer;
    bool _lineSelection;

    String _command;
    String _searchStr, _replaceStr;

    String _screen;
    String _window;
    String _status;

    int _width, _height, _screenHeight;
    bool _unicodeLimit16;
};

#endif
