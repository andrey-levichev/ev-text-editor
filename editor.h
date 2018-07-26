#ifndef EDITOR_INCLUDED
#define EDITOR_INCLUDED

#include <foundation.h>
#include <console.h>
#include <file.h>

// ScreenCell

struct ScreenCell
{
    unichar_t ch;
    short color;

    ScreenCell() :
        ch(0), color(39)
    {
    }

    friend bool operator==(const ScreenCell& left, const ScreenCell& right)
    {
        return left.ch == right.ch && left.color == right.color;
    }
};

// DocumentType

enum DocumentType
{
    DOCUMENT_TYPE_TEXT,
    DOCUMENT_TYPE_CPP,
    DOCUMENT_TYPE_SHELL,
    DOCUMENT_TYPE_XML
};

// HighlightingType

enum HighlightingType
{
    HIGHLIGHTING_TYPE_NONE,
    HIGHLIGHTING_TYPE_STRING,
    HIGHLIGHTING_TYPE_NUMBER,
    HIGHLIGHTING_TYPE_IDENT,
    HIGHLIGHTING_TYPE_KEYWORD,
    HIGHLIGHTING_TYPE_TYPE,
    HIGHLIGHTING_TYPE_SINGLELINE_COMMENT,
    HIGHLIGHTING_TYPE_MULTILINE_COMMENT,
    HIGHLIGHTING_TYPE_PREPROCESSOR
};

// SyntaxHighlighter

class SyntaxHighlighter
{
public:
    SyntaxHighlighter() :
        _documentType(DOCUMENT_TYPE_TEXT),
        _highlightingType(HIGHLIGHTING_TYPE_NONE)
    {
    }

    DocumentType documentType() const
    {
        return _documentType;
    }

    HighlightingType highlightingType() const
    {
        return _highlightingType;
    }

    virtual void startHighlighting() = 0;
    virtual void highlightChar(const String& text, int pos) = 0;

protected:
    DocumentType _documentType;
    HighlightingType _highlightingType;
};

// CppSyntaxHighlighter

class CppSyntaxHighlighter : public SyntaxHighlighter
{
public:
    CppSyntaxHighlighter();

    virtual void startHighlighting();
    virtual void highlightChar(const String& text, int pos);

protected:
    int _charsRemaining, _prevPos;
    String _word;
    unichar_t _quote, _prevCh;

    Set<String> _keywords;
    Set<String> _types;
    Set<String> _preprocessor;
};

// Document

class Editor;

class Document
{
public:
    Document(Editor* editor);

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
        ASSERT(!filename.empty());
        _filename = filename;
        determineDocumentType();
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

    bool moveLines(int lines);
    bool moveToLine(int line);
    bool moveToLineColumn(int line, int column);

    void insertNewLine();
    void insertChar(unichar_t ch, bool afterIdent = false);

    bool deleteCharForward();
    bool deleteCharBack();

    bool deleteWordForward();
    bool deleteWordBack();

    bool deleteCharsForward();
    bool deleteCharsBack();

    void indentLines();
    void unindentLines();
    void commentLines();
    void uncommentLines();

    void markSelection();
    String copyDeleteText(bool copy);
    void pasteText(const String& text);
    bool toggleSelectionStart();

    String currentWord() const;
    String autocompletePrefix() const;
    void completeWord(const char_t* suffix);

    bool find(const String& searchStr, bool caseSesitive, bool next);
    bool replace(const String& searchStr, const String& replaceStr, bool caseSesitive);
    bool replaceAll(const String& searchStr, const String& replaceStr, bool caseSesitive);

    void open(const String& filename);
    void save();
    void clear();

    void setDimensions(int x, int y, int width, int height);
    void draw(int screenWidth, Buffer<ScreenCell>& screen, bool unicodeLimit16);

protected:
    void setPositionLineColumn(int pos);
    void positionToLineColumn(int startPos, int startLine, int startColumn,
        int newPos, int& line, int& column);

    void lineColumnToPosition(int startPos, int startLine, int startColumn,
        int newLine, int newColumn, int& pos, int& line, int& column);

    int findLineStart(int pos) const;
    int findLineEnd(int pos) const;
    int findNextLine(int pos) const;
    int findPreviousLine(int pos) const;

    int findWordForward(int pos) const;
    int findWordBack(int pos) const;
    int findCharsForward(int pos) const;
    int findCharsBack(int pos) const;

    int findPosition(int pos, const String& searchStr, bool caseSesitive, bool next) const;

    void changeLines(int(Document::* lineOp)(int));

    int indentLine(int pos);
    int unindentLine(int pos);
    int commentLine(int pos);
    int uncommentLine(int pos);

    void trimTrailingWhitespace();
    void determineDocumentType();

protected:
    Editor* _editor;

    String _text;
    int _position;
    bool _modified;

    String _filename;
    DocumentType _documentType;
    TextEncoding _encoding;
    bool _bom;
    bool _crLf;

    int _line, _column;
    int _preferredColumn;

    int _top, _left;
    int _topPosition;
    int _prevTopPosition;

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
    int rank;

    AutocompleteSuggestion(const String& word, int rank) :
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

    bool brightBackground() const
    {
        return _brightBackground;
    }

    bool& brightBackground()
    {
        return _brightBackground;
    }

    SyntaxHighlighter* syntaxHighlighter(DocumentType documentType)
    {
        /*for (auto node = _syntaxHighlighters.first(); node; node = node->next)
            if (node->value->documentType() == documentType)
                return node->value.ptr();

        if (documentType == DOCUMENT_TYPE_CPP)
        {
            Unique<SyntaxHighlighter> sh = createUnique<CppSyntaxHighlighter>()
            _syntaxHighlighters.addLast(sh);
        }
        else
            return NULL;

        return _syntaxHighlighters.last()->value.ptr();*/
        if (documentType == DOCUMENT_TYPE_CPP)
            return &_cppSyntaxHighlighter;
        else
            return NULL;
    }

    void newDocument(const String& filename);
    void openDocument(const String& filename);
    void saveDocument();
    void saveAllDocuments();
    void closeDocument();

    void run();

protected:
    void setDimensions();
    void updateScreen(bool redrawAll);
    void updateStatusLine();

    bool processInput();
    void showCommandLine();
    void buildProject();

    bool processCommand(const String& command);

    void updateRecentLocations();
    bool moveToNextRecentLocation();
    bool moveToPrevRecentLocation();

    void findUniqueWords();
    void prepareSuggestions(const String& prefix);
    bool completeWord(int next);

    void copyToClipboard(const String& text);
    void pasteFromClipboard(String& text);

protected:
    List<Document> _documents;
    ListNode<Document> _commandLine;
    ListNode<Document>* _document;
    ListNode<Document>* _lastDocument;

    Array<InputEvent> _inputEvents;
    bool _recordingMacro;
    Array<InputEvent> _macro;

    int _width, _height;
    bool _unicodeLimit16;
    bool _brightBackground;

    Buffer<ScreenCell> _screen;
    Buffer<ScreenCell> _prevScreen;
    String _output;

    String _status, _message;

    String _buffer;
    String _searchStr, _replaceStr;
    bool _caseSesitive;

    List<RecentLocation> _recentLocations;
    ListNode<RecentLocation>* _recentLocation;

    Map<String, int> _uniqueWords;
    Array<AutocompleteSuggestion> _suggestions;
    int _currentSuggestion;

    List<Unique<SyntaxHighlighter>> _syntaxHighlighters;
    CppSyntaxHighlighter _cppSyntaxHighlighter;
};

#endif
