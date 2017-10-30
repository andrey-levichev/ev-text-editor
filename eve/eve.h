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

    int x() const
    {
        return _x;
    }

    int y() const
    {
        return _y;
    }

    int width() const
    {
        return _width;
    }

    int height() const
    {
        return _height;
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

    bool moveCharsForward();
    bool moveCharsBack();

    bool moveToStart();
    bool moveToEnd();

    bool moveToLineStart();
    bool moveToLineEnd();

    bool moveLinesUp(int lines);
    bool moveLinesDown(int lines);
    bool moveToLine(int line);
    bool moveToLineColumn(int line, int column);

    void insertNewLine();
    void insertChar(unichar_t ch, bool afterIdent = false);

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

    String currentWord() const;
    String autocompletePrefix() const;
    void completeWord(const String& word);

    int findLine(int line) const;
    int findPosition(const String& searchStr, bool caseSesitive, bool next);
    bool find(const String& searchStr, bool caseSesitive, bool next);

    bool replace(const String& searchStr, const String& replaceStr, bool caseSesitive);
    bool replaceAll(const String& searchStr, const String& replaceStr, bool caseSesitive);

    void open(const String& filename);
    void save();
    void clear();

    void setDimensions(int x, int y, int width, int height);
    void draw(int screenWidth, UniCharArray& screen, bool unicodeLimit16);

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

protected:
    String _text;
    int _position;
    bool _modified;

    String _filename;
    TextEncoding _encoding;
    bool _bom;
    bool _crLf;

    int _line, _column;
    int _preferredColumn;

    int _top, _left;
    int _x, _y;
    int _width, _height;

    int _selection;
    bool _selectionMode;

    String _indent;
};

// RecentLocation

struct RecentLocation
{
    ListNode<Document>* document;
    int line;

    RecentLocation(ListNode<Document>* document, int line) :
        document(document), line(line)
    {
    }
};

// AutocompleteSuggestion

struct AutocompleteSuggestion
{
    String word;
    float rank;

    AutocompleteSuggestion(const String& word, float rank) :
        word(word), rank(rank)
    {
    }

    friend void swap(AutocompleteSuggestion& left, AutocompleteSuggestion& right)
    {
        swap(left.word, right.word);
        swap(left.rank, right.rank);
    }

    friend bool operator<(const AutocompleteSuggestion& left, const AutocompleteSuggestion& right)
    {
        if (left.rank > right.rank)
            return true;
        else if (left.rank == right.rank)
            return left.word > right.word;
        else
            return false;
    }
};

// Editor

class Editor
{
public:
    Editor();
    ~Editor();

    void openDocument(const char_t* filename);
    void saveDocument(Document& docment);
    void saveDocuments();

    void run();

protected:
    void updateScreen();
    void updateStatusLine();

    void setDimensions(int width, int height);
    bool processInput();

    void updateRecentLocations();
    bool moveToNextRecentLocation();
    bool moveToPrevRecentLocation();

    void processCommand(const String& command);
    void buildProject();

    void findUniqueWords(const Document& document);
    void prepareSuggestions();

    int findNextSuggestion(const String& prefix, int currentSuggestion) const;
    int findPrevSuggestion(const String& prefix, int currentSuggestion) const;
    void completeWord(bool next);
    void cancelCompletion();

protected:
    List<Document> _documents;
    ListNode<Document>* _document;
    ListNode<Document>* _lastDocument;
    ListNode<Document> _commandLine;

    String _buffer;
    bool _lineSelection;

    String _searchStr, _replaceStr;
    bool _caseSesitive;

    UniCharArray _screen;
    UniCharArray _prevScreen;
    String _output;

    String _status, _message;

    int _width, _height;
    bool _unicodeLimit16;

    List<RecentLocation> _recentLocations;
    ListNode<RecentLocation>* _recentLocation;

    Map<String, int> _uniqueWords;
    Array<AutocompleteSuggestion> _suggestions;
    int _currentSuggestion;
};

#endif
