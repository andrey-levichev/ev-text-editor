#ifndef EDITOR_INCLUDED
#define EDITOR_INCLUDED

#include <foundation.h>
#include <application.h>
#include <file.h>

#ifdef GUI_MODE
#include <graphics.h>
#endif

// ScreenCell

struct ScreenCell
{
    char16_t ch;
    uint16_t color;

    ScreenCell();

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
    DOCUMENT_TYPE_BATCH,
    DOCUMENT_TYPE_POWERSHELL,
    DOCUMENT_TYPE_XML,
    DOCUMENT_TYPE_HTML,
    DOCUMENT_TYPE_PYTHON,
    DOCUMENT_TYPE_JAVASCRIPT
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
    HIGHLIGHTING_TYPE_PREPROCESSOR,
    HIGHLIGHTING_TYPE_VARIABLE,
    HIGHLIGHTING_TYPE_VARIABLE_REF,
    HIGHLIGHTING_TYPE_TAG,
    HIGHLIGHTING_TYPE_ATTRIBUTE,
    HIGHLIGHTING_TYPE_ATTRIBUTE_EQUAL,
    HIGHLIGHTING_TYPE_ATTRIBUTE_VALUE
};

// HighlightingState

struct HighlightingState
{
    HighlightingType highlightingType = HIGHLIGHTING_TYPE_NONE;
    int charsRemaining = 0;
    bool reset = false;
    unichar_t startCh = 0, prevCh = 0;
    String word;
};

// SyntaxHighlighter

class SyntaxHighlighter
{
public:
    SyntaxHighlighter(DocumentType documentType = DOCUMENT_TYPE_TEXT) : _documentType(documentType)
    {
    }

    virtual ~SyntaxHighlighter()
    {
    }

    DocumentType documentType() const
    {
        return _documentType;
    }

    const HighlightingState& highlightingState() const
    {
        return _highlightingState;
    }

    HighlightingState& highlightingState()
    {
        return _highlightingState;
    }

    virtual void highlightChar(const String& text, int pos) = 0;

protected:
    DocumentType _documentType;
    HighlightingState _highlightingState;
};

// CppSyntaxHighlighter

class CppSyntaxHighlighter : public SyntaxHighlighter
{
public:
    CppSyntaxHighlighter();
    void highlightChar(const String& text, int pos) override;

protected:
    Set<String> _keywords;
    Set<String> _types;
    Set<String> _preprocessor;
};

// ShellSyntaxHighlighter

class ShellSyntaxHighlighter : public SyntaxHighlighter
{
public:
    ShellSyntaxHighlighter();
    void highlightChar(const String& text, int pos) override;

protected:
    Set<String> _keywords;
};

// XmlSyntaxHighlighter

class XmlSyntaxHighlighter : public SyntaxHighlighter
{
public:
    XmlSyntaxHighlighter() : SyntaxHighlighter(DOCUMENT_TYPE_XML)
    {
    }

    void highlightChar(const String& text, int pos) override;
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
        determineDocumentType(false);
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
    bool movePage(bool down);
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
    void trimTrailingWhitespace();

    void setDimensions(int x, int y, int width, int height);
    void draw(int screenWidth, Buffer<ScreenCell>& screen, bool unicodeLimit16);

protected:
    void setPositionLineColumn(int pos);
    void positionToLineColumn(int startPos, int startLine, int startColumn, int newPos, int& line, int& column);

    void lineColumnToPosition(int startPos, int startLine, int startColumn, int newLine, int newColumn, int& pos,
                              int& line, int& column);

    int findLineStart(int pos) const;
    int findLineEnd(int pos) const;
    int findNextLine(int pos) const;
    int findPreviousLine(int pos) const;

    int findWordForward(int pos) const;
    int findWordBack(int pos) const;
    int findCharsForward(int pos) const;
    int findCharsBack(int pos) const;

    int findPosition(int pos, const String& searchStr, bool caseSesitive, bool next) const;

    void changeLines(int (Document::*lineOp)(int));

    int indentLine(int pos);
    int unindentLine(int pos);
    int commentLine(int pos);
    int uncommentLine(int pos);

    void determineDocumentType(bool fileExecutable);

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

    int _x, _y;
    int _width, _height;

    int _selection;
    bool _selectionMode;

    String _indent;
    HighlightingState _highlightingState;
};

// RecentLocation

struct RecentLocation
{
    ListNode<Document>* document;
    int line;

    RecentLocation(ListNode<Document>* document, int line) : document(document), line(line)
    {
    }
};

// AutocompleteSuggestion

struct AutocompleteSuggestion
{
    String word;
    int rank;

    AutocompleteSuggestion(const String& word, int rank) : word(word), rank(rank)
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

class Editor : public Application
{
public:
    Editor(const Array<String>& args);
    Editor(const Editor&) = delete;
    Editor& operator=(const Editor&) = delete;

    bool brightBackground() const
    {
        return _brightBackground;
    }

    bool trimWhitespace() const
    {
        return _trimWhitespace;
    }

    int indentSize() const
    {
        return _indentSize;
    }

    SyntaxHighlighter* syntaxHighlighter(DocumentType documentType);

    void newDocument(const String& filename);
    void openDocument(const String& filename);
    void saveDocument();
    void saveAllDocuments();
    void closeDocument();

    bool start() override;

protected:
    void onCreate() override;
    void onDestroy() override;
    void onPaint(uintptr_t context) override;
    void onResize(int width, int height) override;
    void onInput(const Array<InputEvent>& inputEvents) override;

    void measureCharSize();
    void computeWidthHeight();
    void setDimensions();

#ifdef GUI_MODE
    Rect rectFromLineCol(int left, int top, int right, int bottom);
#endif

    void drawBlockCursor(bool on);
    void updateScreen(bool redrawAll);
    void updateStatusLine();

    void showCommandLine();
    bool executeCommand(const String& command);
    void executeProjectCommand(const String& command);

    void updateRecentLocations();
    bool moveToNextRecentLocation();
    bool moveToPrevRecentLocation();

    void findUniqueWords();
    void prepareSuggestions(const String& prefix);
    bool completeWord(int next);

    void copyToClipboard(const String& text);
    void pasteFromClipboard(String& text);

    void readConfigFile(const String& filename);

protected:
    List<Document> _documents;
    ListNode<Document> _commandLine;
    ListNode<Document>* _document;
    ListNode<Document>* _lastDocument;

    bool _recordingMacro;
    Array<InputEvent> _macro;

    int _width, _height;
    int _cursorLine, _cursorColumn;
    float _charWidth, _charHeight;
    float _offsetX, _offsetY;
    bool _unicodeLimit16;

    Buffer<ScreenCell> _screen;
    Buffer<ScreenCell> _prevScreen;
    String _output;

#ifdef GUI_MODE
    Unique<Graphics> _graphics;
#endif

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

    bool _brightBackground = true;
    bool _trimWhitespace = true;
    int _indentSize = 4;
    float _guiFontSize = 13;
    String _guiFontName = STR("Lucida Console");
    bool _startMaximized = false;

#ifdef PLATFORM_WINDOWS
    String _buildCommand = STR("nmake.exe & pause");
    String _runCommand = STR("nmake.exe run & pause");
    String _cleanCommand = STR("nmake.exe clean & pause");
#else
    String _buildCommand = STR("make; echo 'Press ENTER to continue...'; read");
    String _runCommand = STR("make run; echo 'Press ENTER to continue...'; read");
    String _cleanCommand = STR("make clean; echo 'Press ENTER to continue...'; read");
#endif
};

#endif
