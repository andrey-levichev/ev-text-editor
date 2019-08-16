#include <editor.h>
#include <console.h>

const char_t* APPLICATION_NAME = STR("ev");

const int TAB_SIZE = 4;
const char_t* GUI_FONT_NAME = STR("Lucida Console");
const int GUI_FONT_SIZE = 13;
const Color GUI_BACKGROUND = 0xffffff;

uint32_t rgbColors[] = {
    0x000000, 0x800000, 0x008000, 0x808000, 0x000080, 0x800080, 0x008080, 0xc0c0c0,
    0x808080, 0xff0000, 0x00ff00, 0xffff00, 0x0000ff, 0xff00ff, 0x00ffff, 0xffffff
};

bool charIsWord(unichar_t ch)
{
    return charIsAlphaNum(ch) || ch == '_';
}

bool isWordBoundary(unichar_t prevCh, unichar_t ch)
{
    return (!charIsWord(prevCh) && charIsWord(ch)) ||
           ((charIsWord(prevCh) || charIsSpace(prevCh)) && !(charIsWord(ch) || charIsSpace(ch)));
}

bool isCharBoundary(unichar_t prevCh, unichar_t ch)
{
    return (charIsSpace(prevCh) && !charIsSpace(ch));
}

ForegroundColor defaultForeground()
{
#ifdef GUI_MODE
    return FOREGROUND_COLOR_BLACK;
#else
    return Console::defaultForeground();
#endif
}

BackgroundColor defaultBackground()
{
#ifdef GUI_MODE
    return BACKGROUND_COLOR_BRIGHT_WHITE;
#else
    return Console::defaultBackground();
#endif
}

// Environment

class Environment
{
public:
    static String getVariable(const String& name)
    {
        return getVariable(name.chars());
    }

    static String getVariable(const char_t* name)
    {
#ifdef PLATFORM_WINDOWS
        return String(reinterpret_cast<const char_t*>(
            _wgetenv(reinterpret_cast<const wchar_t*>(name))));
#else
        return String(getenv(name));
#endif
    }

    static int executeCommand(const String& command)
    {
        return executeCommand(command.chars());
    }

    static int executeCommand(const char_t* command)
    {
#ifdef PLATFORM_WINDOWS
        return _wsystem(reinterpret_cast<const wchar_t*>(command));
#else
        return system(command);
#endif
    }
};

// ScreenCell

ScreenCell::ScreenCell() :
#ifdef PLATFORM_WINDOWS

#ifdef GUI_MODE
    ch(' '), color(defaultForeground())
#else
    ch(' '), color(defaultForeground() | defaultBackground())
#endif

#else
    ch(0), color(defaultForeground())
#endif
{
}

// CppSyntaxHighlighter

CppSyntaxHighlighter::CppSyntaxHighlighter() : SyntaxHighlighter(DOCUMENT_TYPE_CPP)
{
    _keywords.add(String(STR("alignas")));
    _keywords.add(String(STR("alignof")));
    _keywords.add(String(STR("and")));
    _keywords.add(String(STR("and_eq")));
    _keywords.add(String(STR("asm")));
    _keywords.add(String(STR("atomic_cancel")));
    _keywords.add(String(STR("atomic_commit")));
    _keywords.add(String(STR("atomic_noexcept")));
    _keywords.add(String(STR("bitand")));
    _keywords.add(String(STR("bitor")));
    _keywords.add(String(STR("break")));
    _keywords.add(String(STR("case")));
    _keywords.add(String(STR("catch")));
    _keywords.add(String(STR("class")));
    _keywords.add(String(STR("compl")));
    _keywords.add(String(STR("concept")));
    _keywords.add(String(STR("const_cast")));
    _keywords.add(String(STR("continue")));
    _keywords.add(String(STR("co_await")));
    _keywords.add(String(STR("co_return")));
    _keywords.add(String(STR("co_yield")));
    _keywords.add(String(STR("decltype")));
    _keywords.add(String(STR("default")));
    _keywords.add(String(STR("delete")));
    _keywords.add(String(STR("do")));
    _keywords.add(String(STR("dynamic_cast")));
    _keywords.add(String(STR("else")));
    _keywords.add(String(STR("enum")));
    _keywords.add(String(STR("explicit")));
    _keywords.add(String(STR("export")));
    _keywords.add(String(STR("extern")));
    _keywords.add(String(STR("false")));
    _keywords.add(String(STR("for")));
    _keywords.add(String(STR("friend")));
    _keywords.add(String(STR("goto")));
    _keywords.add(String(STR("if")));
    _keywords.add(String(STR("import")));
    _keywords.add(String(STR("inline")));
    _keywords.add(String(STR("module")));
    _keywords.add(String(STR("mutable")));
    _keywords.add(String(STR("namespace")));
    _keywords.add(String(STR("new")));
    _keywords.add(String(STR("noexcept")));
    _keywords.add(String(STR("not")));
    _keywords.add(String(STR("not_eq")));
    _keywords.add(String(STR("nullptr")));
    _keywords.add(String(STR("operator")));
    _keywords.add(String(STR("or")));
    _keywords.add(String(STR("or_eq")));
    _keywords.add(String(STR("private")));
    _keywords.add(String(STR("protected")));
    _keywords.add(String(STR("public")));
    _keywords.add(String(STR("register")));
    _keywords.add(String(STR("reflexpr")));
    _keywords.add(String(STR("reinterpret_cast")));
    _keywords.add(String(STR("requires")));
    _keywords.add(String(STR("return")));
    _keywords.add(String(STR("sizeof")));
    _keywords.add(String(STR("static")));
    _keywords.add(String(STR("static_assert")));
    _keywords.add(String(STR("static_cast")));
    _keywords.add(String(STR("struct")));
    _keywords.add(String(STR("switch")));
    _keywords.add(String(STR("synchronized")));
    _keywords.add(String(STR("template")));
    _keywords.add(String(STR("this")));
    _keywords.add(String(STR("thread_local")));
    _keywords.add(String(STR("throw")));
    _keywords.add(String(STR("true")));
    _keywords.add(String(STR("try")));
    _keywords.add(String(STR("typedef")));
    _keywords.add(String(STR("typeid")));
    _keywords.add(String(STR("typename")));
    _keywords.add(String(STR("union")));
    _keywords.add(String(STR("using")));
    _keywords.add(String(STR("virtual")));
    _keywords.add(String(STR("while")));
    _keywords.add(String(STR("xor")));
    _keywords.add(String(STR("xor_eq")));
    _keywords.add(String(STR("override")));
    _keywords.add(String(STR("final")));
    _keywords.add(String(STR("transaction_safe")));
    _keywords.add(String(STR("transaction_safe_dynamic")));
    _keywords.add(String(STR("_Pragma")));

    _types.add(String(STR("auto")));
    _types.add(String(STR("bool")));
    _types.add(String(STR("byte")));
    _types.add(String(STR("char")));
    _types.add(String(STR("char16_t")));
    _types.add(String(STR("char32_t")));
    _types.add(String(STR("const")));
    _types.add(String(STR("constexpr")));
    _types.add(String(STR("double")));
    _types.add(String(STR("float")));
    _types.add(String(STR("int")));
    _types.add(String(STR("long")));
    _types.add(String(STR("short")));
    _types.add(String(STR("signed")));
    _types.add(String(STR("unsigned")));
    _types.add(String(STR("void")));
    _types.add(String(STR("volatile")));
    _types.add(String(STR("wchar_t")));
    _types.add(String(STR("int8_t")));
    _types.add(String(STR("int16_t")));
    _types.add(String(STR("int32_t")));
    _types.add(String(STR("int64_t")));
    _types.add(String(STR("uint8_t")));
    _types.add(String(STR("uint16_t")));
    _types.add(String(STR("uint32_t")));
    _types.add(String(STR("uint64_t")));
    _types.add(String(STR("intptr_t")));
    _types.add(String(STR("uintptr_t")));
    _types.add(String(STR("intmax_t")));
    _types.add(String(STR("uintmax_t")));
    _types.add(String(STR("size_t")));
    _types.add(String(STR("ptrdiff_t")));
    _types.add(String(STR("nullptr_t")));
    _types.add(String(STR("max_align_t")));
    _types.add(String(STR("unichar_t")));
    _types.add(String(STR("char_t")));
    _types.add(String(STR("byte_t")));

    _preprocessor.add(String(STR("if")));
    _preprocessor.add(String(STR("elif")));
    _preprocessor.add(String(STR("else")));
    _preprocessor.add(String(STR("endif")));
    _preprocessor.add(String(STR("defined")));
    _preprocessor.add(String(STR("ifdef")));
    _preprocessor.add(String(STR("ifndef")));
    _preprocessor.add(String(STR("define")));
    _preprocessor.add(String(STR("undef")));
    _preprocessor.add(String(STR("include")));
    _preprocessor.add(String(STR("line")));
    _preprocessor.add(String(STR("error")));
    _preprocessor.add(String(STR("pragma")));
}

void CppSyntaxHighlighter::highlightChar(const String& text, int pos)
{
    if (_highlightingState.charsRemaining > 0)
    {
        --_highlightingState.charsRemaining;
        if (_highlightingState.charsRemaining > 0)
            return;

        if (_highlightingState.reset)
            _highlightingState.highlightingType = HIGHLIGHTING_TYPE_NONE;
    }

    unichar_t ch = text.charAt(pos);

    if (_highlightingState.highlightingType == HIGHLIGHTING_TYPE_STRING)
    {
        if (_highlightingState.prevCh == '\\')
            _highlightingState.prevCh = 0;
        else if (ch == _highlightingState.startCh)
        {
            _highlightingState.charsRemaining = 1;
            _highlightingState.reset = true;
        }
        else if (ch == '\\')
            _highlightingState.prevCh = ch;

        return;
    }
    else if (_highlightingState.highlightingType == HIGHLIGHTING_TYPE_NUMBER)
    {
        if (!(charIsDigit(ch) || ch == 'x' || ch == 'X' || ch == 'a' || ch == 'A' || ch == 'b' || ch == 'B' ||
              ch == 'c' || ch == 'C' || ch == 'd' || ch == 'D' || ch == 'e' || ch == 'E' || ch == 'f' || ch == 'F' ||
              ch == '.' || ch == '+' || ch == '-'))
            _highlightingState.highlightingType = HIGHLIGHTING_TYPE_NONE;
        else
            return;
    }
    else if (_highlightingState.highlightingType == HIGHLIGHTING_TYPE_SINGLELINE_COMMENT)
    {
        if (ch == '\n')
        {
            _highlightingState.charsRemaining = 1;
            _highlightingState.reset = true;
        }

        return;
    }
    else if (_highlightingState.highlightingType == HIGHLIGHTING_TYPE_MULTILINE_COMMENT)
    {
        if (ch == '*')
        {
            pos = text.charForward(pos);

            if (pos < text.length())
            {
                if (text.charAt(pos) == '/')
                {
                    _highlightingState.charsRemaining = 2;
                    _highlightingState.reset = true;
                }
            }
        }

        return;
    }
    else if (_highlightingState.highlightingType == HIGHLIGHTING_TYPE_PREPROCESSOR)
    {
        if (ch == '\n')
        {
            if (_highlightingState.prevCh != '\\')
            {
                _highlightingState.charsRemaining = 1;
                _highlightingState.reset = true;
            }

            _highlightingState.prevCh = 0;
        }
        else if (ch == '\\')
            _highlightingState.prevCh = ch;

        return;
    }

    if (ch == '"' || ch == '\'')
    {
        _highlightingState.startCh = ch;
        _highlightingState.highlightingType = HIGHLIGHTING_TYPE_STRING;
    }
    else if (charIsDigit(ch))
    {
        _highlightingState.highlightingType = HIGHLIGHTING_TYPE_NUMBER;
    }
    else if (charIsAlphaNum(ch) || ch == '_')
    {
        int s = pos;

        do
        {
            pos = text.charForward(pos);
            if (pos < text.length())
                ch = text.charAt(pos);
            else
                break;
        } while (charIsAlphaNum(ch) || charIsDigit(ch) || ch == '_');

        _highlightingState.word = text.substr(s, pos - s);
        _highlightingState.charsRemaining = _highlightingState.word.charLength();
        _highlightingState.reset = true;

        if (_keywords.contains(_highlightingState.word))
            _highlightingState.highlightingType = HIGHLIGHTING_TYPE_KEYWORD;
        else if (_types.contains(_highlightingState.word))
            _highlightingState.highlightingType = HIGHLIGHTING_TYPE_TYPE;
        else
            _highlightingState.highlightingType = HIGHLIGHTING_TYPE_IDENT;
    }
    else if (ch == '/')
    {
        pos = text.charForward(pos);

        if (pos < text.length())
        {
            ch = text.charAt(pos);

            if (ch == '*')
            {
                _highlightingState.highlightingType = HIGHLIGHTING_TYPE_MULTILINE_COMMENT;
                _highlightingState.charsRemaining = 2;
                _highlightingState.reset = false;
            }
            else if (ch == '/')
                _highlightingState.highlightingType = HIGHLIGHTING_TYPE_SINGLELINE_COMMENT;
        }
    }
    else if (ch == '#')
    {
        int s = pos;
        pos = text.charForward(pos);

        if (pos < text.length())
        {
            ch = text.charAt(pos);
            int q = pos;

            while (charIsAlpha(ch))
            {
                pos = text.charForward(pos);
                if (pos < text.length())
                    ch = text.charAt(pos);
                else
                    break;
            }

            _highlightingState.word = text.substr(q, pos - q);

            if (_preprocessor.contains(_highlightingState.word))
            {
                _highlightingState.highlightingType = HIGHLIGHTING_TYPE_PREPROCESSOR;

                if (_highlightingState.word == STR("else") || _highlightingState.word == STR("endif"))
                {
                    _highlightingState.charsRemaining = _highlightingState.word.charLength() + 1;
                    _highlightingState.reset = true;
                }
            }
        }
    }
}

ShellSyntaxHighlighter::ShellSyntaxHighlighter() : SyntaxHighlighter(DOCUMENT_TYPE_SHELL)
{
    _keywords.add(String(STR("case")));
    _keywords.add(String(STR("do")));
    _keywords.add(String(STR("done")));
    _keywords.add(String(STR("elif")));
    _keywords.add(String(STR("else")));
    _keywords.add(String(STR("esac")));
    _keywords.add(String(STR("fi")));
    _keywords.add(String(STR("for")));
    _keywords.add(String(STR("function")));
    _keywords.add(String(STR("if")));
    _keywords.add(String(STR("in")));
    _keywords.add(String(STR("select")));
    _keywords.add(String(STR("then")));
    _keywords.add(String(STR("time")));
    _keywords.add(String(STR("until")));
    _keywords.add(String(STR("while")));
}

void ShellSyntaxHighlighter::highlightChar(const String& text, int pos)
{
    if (_highlightingState.charsRemaining > 0)
    {
        --_highlightingState.charsRemaining;
        if (_highlightingState.charsRemaining > 0)
            return;

        if (_highlightingState.reset)
            _highlightingState.highlightingType = HIGHLIGHTING_TYPE_NONE;
    }

    unichar_t ch = text.charAt(pos);

    if (_highlightingState.highlightingType == HIGHLIGHTING_TYPE_STRING)
    {
        if (_highlightingState.prevCh == '\\')
            _highlightingState.prevCh = 0;
        else if (ch == _highlightingState.startCh)
        {
            _highlightingState.charsRemaining = 1;
            _highlightingState.reset = true;
        }
        else if (ch == '\\')
            _highlightingState.prevCh = ch;

        return;
    }
    else if (_highlightingState.highlightingType == HIGHLIGHTING_TYPE_NUMBER)
    {
        if (!(charIsDigit(ch) || ch == 'x' || ch == 'X' || ch == 'a' || ch == 'A' || ch == 'b' || ch == 'B' ||
              ch == 'c' || ch == 'C' || ch == 'd' || ch == 'D' || ch == 'e' || ch == 'E' || ch == 'f' || ch == 'F' ||
              ch == '.' || ch == '+' || ch == '-'))
            _highlightingState.highlightingType = HIGHLIGHTING_TYPE_NONE;
        else
            return;
    }
    else if (_highlightingState.highlightingType == HIGHLIGHTING_TYPE_SINGLELINE_COMMENT)
    {
        if (ch == '\n')
        {
            _highlightingState.charsRemaining = 1;
            _highlightingState.reset = true;
        }

        return;
    }
    else if (_highlightingState.highlightingType == HIGHLIGHTING_TYPE_VARIABLE_REF)
    {
        if (_highlightingState.startCh == '{')
        {
            if (ch == '}')
            {
                _highlightingState.charsRemaining = 1;
                _highlightingState.reset = true;
            }

            return;
        }
        else
        {
            if (!(charIsAlphaNum(ch) || charIsDigit(ch) || ch == '_'))
                _highlightingState.highlightingType = HIGHLIGHTING_TYPE_NONE;
            else
                return;
        }
    }

    if (ch == '"' || ch == '\'')
    {
        _highlightingState.startCh = ch;
        _highlightingState.highlightingType = HIGHLIGHTING_TYPE_STRING;
    }
    else if (charIsDigit(ch))
    {
        _highlightingState.highlightingType = HIGHLIGHTING_TYPE_NUMBER;
    }
    else if (charIsAlphaNum(ch) || ch == '_')
    {
        int s = pos;

        do
        {
            pos = text.charForward(pos);
            if (pos < text.length())
                ch = text.charAt(pos);
            else
                break;
        } while (charIsAlphaNum(ch) || charIsDigit(ch) || ch == '_');

        _highlightingState.word = text.substr(s, pos - s);
        _highlightingState.charsRemaining = _highlightingState.word.charLength();
        _highlightingState.reset = true;

        if (_keywords.contains(_highlightingState.word))
            _highlightingState.highlightingType = HIGHLIGHTING_TYPE_KEYWORD;
        else
        {
            if (pos < text.length() && ch == '=')
                _highlightingState.highlightingType = HIGHLIGHTING_TYPE_VARIABLE;
            else
                _highlightingState.highlightingType = HIGHLIGHTING_TYPE_NONE;
        }
    }
    else if (ch == '#')
    {
        _highlightingState.highlightingType = HIGHLIGHTING_TYPE_SINGLELINE_COMMENT;
    }
    else if (ch == '$')
    {
        pos = text.charForward(pos);

        if (pos < text.length())
        {
            ch = text.charAt(pos);
            _highlightingState.startCh = ch == '{' ? ch : 0;
        }

        _highlightingState.highlightingType = HIGHLIGHTING_TYPE_VARIABLE_REF;
    }
}

// XmlSyntaxHighlighter

void XmlSyntaxHighlighter::highlightChar(const String& text, int pos)
{
    if (_highlightingState.charsRemaining > 0)
    {
        --_highlightingState.charsRemaining;
        if (_highlightingState.charsRemaining > 0)
            return;

        if (_highlightingState.reset)
            _highlightingState.highlightingType = HIGHLIGHTING_TYPE_NONE;
    }

    unichar_t ch = text.charAt(pos);

    if (_highlightingState.highlightingType == HIGHLIGHTING_TYPE_TAG)
    {
        if (ch == '>')
        {
            _highlightingState.charsRemaining = 1;
            _highlightingState.reset = true;
        }
        else if (charIsSpace(ch))
            _highlightingState.highlightingType = HIGHLIGHTING_TYPE_ATTRIBUTE;

        return;
    }
    else if (_highlightingState.highlightingType == HIGHLIGHTING_TYPE_ATTRIBUTE)
    {
        if (ch == '=')
            _highlightingState.highlightingType = HIGHLIGHTING_TYPE_ATTRIBUTE_EQUAL;
        else if (ch == '>')
        {
            _highlightingState.highlightingType = HIGHLIGHTING_TYPE_TAG;
            _highlightingState.charsRemaining = 1;
            _highlightingState.reset = true;
        }
        else if (ch == '/')
            _highlightingState.highlightingType = HIGHLIGHTING_TYPE_TAG;

        return;
    }
    else if (_highlightingState.highlightingType == HIGHLIGHTING_TYPE_ATTRIBUTE_EQUAL)
    {
        if (ch == '\'' || ch == '"')
        {
            _highlightingState.startCh = ch;
            _highlightingState.highlightingType = HIGHLIGHTING_TYPE_ATTRIBUTE_VALUE;
        }
        else if (!charIsSpace(ch))
        {
            _highlightingState.startCh = 0;
            _highlightingState.highlightingType = HIGHLIGHTING_TYPE_ATTRIBUTE_VALUE;
        }

        return;
    }
    else if (_highlightingState.highlightingType == HIGHLIGHTING_TYPE_ATTRIBUTE_VALUE)
    {
        if (_highlightingState.prevCh != 0)
        {
            _highlightingState.prevCh = 0;

            if (ch == '>')
            {
                _highlightingState.highlightingType = HIGHLIGHTING_TYPE_TAG;
                _highlightingState.charsRemaining = 1;
                _highlightingState.reset = true;
            }
            else if (ch == '/')
                _highlightingState.highlightingType = HIGHLIGHTING_TYPE_TAG;
            else
                _highlightingState.highlightingType = HIGHLIGHTING_TYPE_ATTRIBUTE;
        }
        else if (_highlightingState.startCh != 0)
        {
            if (ch == _highlightingState.startCh)
                _highlightingState.prevCh = 1;
        }
        else if (_highlightingState.startCh == 0)
        {
            if (ch == '>')
            {
                _highlightingState.highlightingType = HIGHLIGHTING_TYPE_TAG;
                _highlightingState.charsRemaining = 1;
                _highlightingState.reset = true;
            }
            else if (ch == '/')
                _highlightingState.highlightingType = HIGHLIGHTING_TYPE_TAG;
            else if (charIsSpace(ch))
                _highlightingState.highlightingType = HIGHLIGHTING_TYPE_ATTRIBUTE;
        }

        return;
    }
    else if (_highlightingState.highlightingType == HIGHLIGHTING_TYPE_MULTILINE_COMMENT)
    {
        if (ch == '-')
        {
            pos = text.charForward(pos);

            if (pos < text.length())
            {
                if (text.charAt(pos) == '-')
                {
                    pos = text.charForward(pos);

                    if (pos < text.length())
                    {
                        if (text.charAt(pos) == '>')
                        {
                            _highlightingState.charsRemaining = 3;
                            _highlightingState.reset = true;
                        }
                    }
                }
            }
        }

        return;
    }

    if (ch == '<')
    {
        pos = text.charForward(pos);

        if (pos < text.length())
        {
            if (text.charAt(pos) == '!')
            {
                pos = text.charForward(pos);

                if (pos < text.length())
                {
                    if (text.charAt(pos) == '-')
                    {
                        pos = text.charForward(pos);

                        if (pos < text.length())
                        {
                            if (text.charAt(pos) == '-')
                            {
                                _highlightingState.highlightingType = HIGHLIGHTING_TYPE_MULTILINE_COMMENT;
                                return;
                            }
                        }
                    }
                }
            }
        }

        _highlightingState.highlightingType = HIGHLIGHTING_TYPE_TAG;
    }
}

// Document

Document::Document(Editor* editor) : _editor(editor)
{
    clear();
    setDimensions(1, 1, 1, 1);
}

bool Document::moveForward()
{
    if (_position < _text.length())
    {
        int p = _text.charForward(_position);
        setPositionLineColumn(p);

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
        int p = _text.charBack(_position);
        setPositionLineColumn(p);

        if (!_selectionMode)
            _selection = -1;

        return true;
    }

    return false;
}

bool Document::moveWordForward()
{
    int p = findWordForward(_position);

    if (p > _position)
    {
        setPositionLineColumn(p);

        if (!_selectionMode)
            _selection = -1;

        return true;
    }
    else
        return false;
}

bool Document::moveWordBack()
{
    int p = findWordBack(_position);

    if (p < _position)
    {
        setPositionLineColumn(p);

        if (!_selectionMode)
            _selection = -1;

        return true;
    }
    else
        return false;
}

bool Document::moveCharsForward()
{
    int p = findCharsForward(_position);

    if (p > _position)
    {
        setPositionLineColumn(p);

        if (!_selectionMode)
            _selection = -1;

        return true;
    }
    else
        return false;
}

bool Document::moveCharsBack()
{
    int p = findCharsBack(_position);

    if (p < _position)
    {
        setPositionLineColumn(p);

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
        _line = _column = _preferredColumn = 1;

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
        int p = _text.length();
        setPositionLineColumn(p);

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
        setPositionLineColumn(p);

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
        setPositionLineColumn(p);

        if (!_selectionMode)
            _selection = -1;

        return true;
    }
    else
        return false;
}

bool Document::moveLines(int lines)
{
    int line = _line + lines;
    if (line < 1)
        line = 1;

    return moveToLine(line);
}

bool Document::moveToLine(int line)
{
    ASSERT(line > 0);

    int prev = _position;
    lineColumnToPosition(_position, _line, _column, line, _preferredColumn, _position, _line, _column);

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

    int prev = _position;
    lineColumnToPosition(_position, _line, _column, line, column, _position, _line, _column);

    _preferredColumn = _column;

    if (_position != prev)
    {
        if (!_selectionMode)
            _selection = -1;

        return true;
    }
    else
        return false;
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
    setPositionLineColumn(_position + _indent.length());

    _modified = true;
    _selectionMode = false;
    _selection = -1;
}

void Document::insertChar(unichar_t ch, bool afterIdent)
{
    ASSERT(ch != 0);

    int p = _position;

    if (afterIdent)
    {
        while (charIsWord(_text.charAt(p)))
            p = _text.charForward(p);
    }

    _text.insert(p, ch);
    p = _text.charForward(p);
    setPositionLineColumn(p);

    _modified = true;
    _selectionMode = false;
    _selection = -1;
}

bool Document::deleteCharForward()
{
    if (_position < _text.length())
    {
        _text.erase(_position, _text.charForward(_position) - _position);

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
        int p = _text.charBack(_position);
        int prev = _position;

        setPositionLineColumn(p);
        _text.erase(_position, prev - _position);

        _modified = true;
        _selectionMode = false;
        _selection = -1;

        return true;
    }

    return false;
}

bool Document::deleteWordForward()
{
    int p = findWordForward(_position);

    if (p > _position)
    {
        _text.erase(_position, p - _position);

        _modified = true;
        _selectionMode = false;
        _selection = -1;

        return true;
    }

    return false;
}

bool Document::deleteWordBack()
{
    int p = findWordBack(_position);

    if (p < _position)
    {
        int prev = _position;

        setPositionLineColumn(p);
        _text.erase(_position, prev - _position);

        _modified = true;
        _selectionMode = false;
        _selection = -1;

        return true;
    }

    return false;
}

bool Document::deleteCharsForward()
{
    int p = findCharsForward(_position);

    if (p > _position)
    {
        _text.erase(_position, p - _position);

        _modified = true;
        _selectionMode = false;
        _selection = -1;

        return true;
    }

    return false;
}

bool Document::deleteCharsBack()
{
    int p = findCharsBack(_position);

    if (p < _position)
    {
        int prev = _position;

        setPositionLineColumn(p);
        _text.erase(_position, prev - _position);

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

void Document::commentLines()
{
    changeLines(&Document::commentLine);
}

void Document::uncommentLines()
{
    changeLines(&Document::uncommentLine);
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
            if (_selection < 0)
            {
                _text.erase(start, end - start);
                lineColumnToPosition(start, _line, 1, _line, _preferredColumn, _position, _line, _column);
            }
            else
            {
                setPositionLineColumn(start);
                _text.erase(start, end - start);
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

void Document::pasteText(const String& text)
{
    ASSERT(!text.empty());

    if (text.charAt(text.charBack(text.length())) == '\n')
    {
        int start = findLineStart(_position);
        _text.insert(start, text);
        lineColumnToPosition(start, _line, 1, _line, _preferredColumn, _position, _line, _column);
    }
    else
    {
        _text.insert(_position, text);
        setPositionLineColumn(_position + text.length());
    }

    _modified = true;
    _selectionMode = false;
    _selection = -1;
}

bool Document::toggleSelectionStart()
{
    if (_selection >= 0 && _position != _selection)
    {
        int p = _position;
        setPositionLineColumn(_selection);
        _selection = p;
        return true;
    }

    return false;
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

void Document::completeWord(const char_t* suffix)
{
    int end = _position;
    while (end < _text.length())
    {
        if (!charIsWord(_text.charAt(end)))
            break;
        end = _text.charForward(end);
    }

    _text.replace(_position, suffix, end - _position);

    _modified = true;
    _selectionMode = false;
    _selection = -1;
}

bool Document::find(const String& searchStr, bool caseSesitive, bool next)
{
    ASSERT(!searchStr.empty());

    int p = findPosition(_position, searchStr, caseSesitive, next);

    if (p != INVALID_POSITION && p != _position)
    {
        setPositionLineColumn(p);

        if (!_selectionMode)
            _selection = -1;

        return true;
    }

    return false;
}

bool Document::replace(const String& searchStr, const String& replaceStr, bool caseSesitive)
{
    ASSERT(!searchStr.empty());

    int p = findPosition(_position, searchStr, caseSesitive, false);

    if (p == _position)
    {
        _text.replace(p, replaceStr, searchStr.length());
        p += replaceStr.length();

        int q = findPosition(p, searchStr, caseSesitive, false);
        if (q != INVALID_POSITION)
            p = q;

        setPositionLineColumn(p);

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
    lineColumnToPosition(0, 1, 1, _line, _column, _position, _line, _column);

    _modified = true;
    _selectionMode = false;
    _selection = -1;
    _topPosition = -1;

    return true;
}

void Document::open(const String& filename)
{
    ASSERT(!filename.empty());

    clear();

    File file(filename, FILE_MODE_READ | FILE_MODE_CREATE);
    _text.assign(Unicode::bytesToString(file.read(), _encoding, _bom, _crLf));

    _filename = filename;
    determineDocumentType(file.isExecutable());
}

void Document::save()
{
    ASSERT(!_filename.empty());

    if (_editor->trimWhitespace())
        trimTrailingWhitespace();

    File file(_filename, FILE_MODE_WRITE | FILE_MODE_CREATE | FILE_MODE_TRUNCATE);
    file.write(Unicode::stringToBytes(_text, _encoding, _bom, _crLf));

    _modified = false;
    _selectionMode = false;
    _selection = -1;
}

void Document::clear()
{
    _text.ensureCapacity(1);
    _text.clear();

    _position = 0;
    _modified = false;

    _filename.clear();
    _documentType = DOCUMENT_TYPE_TEXT;
    _encoding = TEXT_ENCODING_UTF8;
    _bom = false;
    _crLf = CRLF;

    _line = _column = 1;
    _preferredColumn = 1;

    _top = _left = 1;
    _topPosition = 0;

    _selectionMode = false;
    _selection = -1;
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

    lineColumnToPosition(0, 1, 1, _line, _column, _position, _line, _column);

    _modified = true;
    _selectionMode = false;
    _selection = -1;
    _topPosition = -1;
}

void Document::setDimensions(int x, int y, int width, int height)
{
    ASSERT(x > 0 && y > 0);
    ASSERT(width > 0 && height > 0);

    _x = x;
    _y = y;
    _width = width;
    _height = height;
}

void Document::draw(int screenWidth, Buffer<ScreenCell>& screen, bool unicodeLimit16)
{
    ASSERT(screenWidth > 0);

    int l;
    bool highlightFromStart = true;

    if (_line < _top)
        lineColumnToPosition(_position, _line, _column, _line, 1, _topPosition, _top, l);
    else if (_line >= _top + _height)
        lineColumnToPosition(_position, _line, _column, _line - _height + 1, 1, _topPosition, _top, l);
    else if (_topPosition < 0)
        lineColumnToPosition(_position, _line, _column, _top, 1, _topPosition, _top, l);
    else
        highlightFromStart = false;

    if (_column < _left)
        _left = _column;
    else if (_column >= _left + _width)
        _left = _column - _width + 1;

    ASSERT(_position >= 0 && _position <= _text.length());
    ASSERT(_line > 0 && _column > 0);
    ASSERT(_preferredColumn > 0);
    ASSERT(_top > 0 && _left > 0);
    ASSERT(_topPosition >= 0 && _topPosition <= _text.length());
    ASSERT(_x > 0 && _y > 0);
    ASSERT(_width > 0 && _height > 0);

    int p = _topPosition;
    int len = _left + _width - 1;

    const ForegroundColor brightBackgroundColors[] = {
        defaultForeground(), FOREGROUND_COLOR_YELLOW, FOREGROUND_COLOR_RED, defaultForeground(),
        FOREGROUND_COLOR_BLUE, FOREGROUND_COLOR_CYAN,FOREGROUND_COLOR_BRIGHT_BLACK, FOREGROUND_COLOR_BRIGHT_BLACK,
        FOREGROUND_COLOR_MAGENTA, FOREGROUND_COLOR_CYAN, FOREGROUND_COLOR_MAGENTA, FOREGROUND_COLOR_BLUE,
        FOREGROUND_COLOR_CYAN, defaultForeground(), FOREGROUND_COLOR_YELLOW
    };

    const ForegroundColor darkBackgroundColors[] = {
        defaultForeground(), FOREGROUND_COLOR_BRIGHT_YELLOW, FOREGROUND_COLOR_BRIGHT_RED, defaultForeground(),
        FOREGROUND_COLOR_BRIGHT_GREEN, FOREGROUND_COLOR_BRIGHT_CYAN, FOREGROUND_COLOR_WHITE, FOREGROUND_COLOR_WHITE,
        FOREGROUND_COLOR_BRIGHT_MAGENTA, FOREGROUND_COLOR_BRIGHT_CYAN, FOREGROUND_COLOR_BRIGHT_MAGENTA, FOREGROUND_COLOR_BRIGHT_GREEN,
        FOREGROUND_COLOR_BRIGHT_CYAN, defaultForeground(), FOREGROUND_COLOR_BRIGHT_YELLOW
    };

    SyntaxHighlighter* syntaxHighlighter = _editor->syntaxHighlighter(_documentType);
    const ForegroundColor* colors = _editor->brightBackground() ? brightBackgroundColors : darkBackgroundColors;

    if (syntaxHighlighter)
    {
        if (highlightFromStart)
        {
            p = 0;
            syntaxHighlighter->highlightingState() = HighlightingState();

            while (p < _topPosition)
            {
                syntaxHighlighter->highlightChar(_text, p);
                p = _text.charForward(p);
            }

            _highlightingState = syntaxHighlighter->highlightingState();
        }
        else
            syntaxHighlighter->highlightingState() = _highlightingState;
    }

    for (int j = 1; j <= _height; ++j)
    {
        int q = (_y + j - 2) * screenWidth + _x - 1;
        unichar_t ch = 0;
        bool eol = false;

        for (int i = 1; i <= len || !eol; ++i)
        {
            if (!eol)
            {
                ch = _text.charAt(p);
                if (syntaxHighlighter)
                    syntaxHighlighter->highlightChar(_text, p);

                if (ch == '\t')
                {
                    ch = ' ';
                    if (i == ((i - 1) / TAB_SIZE + 1) * TAB_SIZE)
                        p = _text.charForward(p);
                }
                else if (!ch || ch == '\n')
                {
                    eol = true;
                    if (ch == '\n')
                        p = _text.charForward(p);

#ifdef PLATFORM_WINDOWS
                    ch = ' ';
#else
                    ch = 0;
#endif
                }
                else
                    p = _text.charForward(p);
            }

            if (i >= _left && i <= len)
            {
                screen[q].ch = unicodeLimit16 && ch > 0xffff ? '?' : ch;

#if defined(PLATFORM_WINDOWS) && !defined(GUI_MODE)
                if (syntaxHighlighter)
                    screen[q].color =
                        defaultBackground() | colors[syntaxHighlighter->highlightingState().highlightingType];
                else
                    screen[q].color = defaultBackground() | defaultForeground();
#else
                if (syntaxHighlighter)
                    screen[q].color = colors[syntaxHighlighter->highlightingState().highlightingType];
                else
                    screen[q].color = defaultForeground();
#endif

                ++q;
            }
        }
    }
}

void Document::setPositionLineColumn(int pos)
{
    positionToLineColumn(_position, _line, _column, pos, _line, _column);
    _position = pos;
    _preferredColumn = _column;
}

void Document::positionToLineColumn(int startPos, int startLine, int startColumn, int newPos, int& line, int& column)
{
    ASSERT(startPos >= 0 && startPos <= _text.length());
    ASSERT(startLine > 0 && startColumn > 0);
    ASSERT(newPos >= 0 && newPos <= _text.length());

    int p = startPos;
    line = startLine;
    column = startColumn;

    if (p > newPos)
    {
        while (p > newPos)
        {
            p = _text.charBack(p);
            if (_text.charAt(p) == '\n')
                --line;
        }

        p = findLineStart(p);
        column = 1;
    }
    else if (p == newPos)
        return;

    while (p < newPos)
    {
        unichar_t ch = _text.charAt(p);

        if (ch == '\n')
        {
            ++line;
            column = 1;
        }
        else if (ch == '\t')
            column = ((column - 1) / TAB_SIZE + 1) * TAB_SIZE + 1;
        else
            ++column;

        p = _text.charForward(p);
    }
}

void Document::lineColumnToPosition(int startPos, int startLine, int startColumn, int newLine, int newColumn, int& pos,
                                    int& line, int& column)
{
    ASSERT(startPos >= 0 && startPos <= _text.length());
    ASSERT(startLine > 0 && startColumn > 0);
    ASSERT(newLine > 0 && newColumn > 0);

    pos = startPos;
    line = startLine;
    column = 1;

    if (line < newLine)
    {
        while (pos < _text.length() && line < newLine)
        {
            if (_text.charAt(pos) == '\n')
                ++line;

            pos = _text.charForward(pos);
        }
    }
    else
    {
        while (pos > 0)
        {
            int p = _text.charBack(pos);
            if (_text.charAt(p) == '\n')
            {
                if (line > newLine)
                    --line;
                else
                    break;
            }

            pos = p;
        }
    }

    while (pos < _text.length() && column < newColumn)
    {
        unichar_t ch = _text.charAt(pos);

        if (ch == '\n')
            break;
        else if (ch == '\t')
            column = ((column - 1) / TAB_SIZE + 1) * TAB_SIZE + 1;
        else
            ++column;

        pos = _text.charForward(pos);
    }
}

int Document::findLineStart(int pos) const
{
    ASSERT(pos >= 0 && pos <= _text.length());

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
    ASSERT(pos >= 0 && pos <= _text.length());

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
    ASSERT(pos >= 0 && pos <= _text.length());

    pos = findLineEnd(pos);

    if (pos < _text.length())
        return _text.charForward(pos);
    else
        return INVALID_POSITION;
}

int Document::findPreviousLine(int pos) const
{
    ASSERT(pos >= 0 && pos <= _text.length());

    pos = findLineStart(pos);

    if (pos > 0)
    {
        pos = _text.charBack(pos);
        return findLineStart(pos);
    }
    else
        return INVALID_POSITION;
}

int Document::findWordForward(int pos) const
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

    return p;
}

int Document::findWordBack(int pos) const
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
            } while (p > 0);
        }
    }

    return p;
}

int Document::findCharsForward(int pos) const
{
    int p = _position;

    if (p < _text.length())
    {
        unichar_t prevCh = _text.charAt(p);
        p = _text.charForward(p);

        while (p < _text.length())
        {
            unichar_t ch = _text.charAt(p);
            if (isCharBoundary(prevCh, ch))
                break;

            prevCh = ch;
            p = _text.charForward(p);
        }
    }

    return p;
}

int Document::findCharsBack(int pos) const
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
                if (isCharBoundary(ch, prevCh))
                {
                    p = prevPos;
                    break;
                }

                prevCh = ch;
                prevPos = p;
            } while (p > 0);
        }
    }

    return p;
}

int Document::findPosition(int pos, const String& searchStr, bool caseSesitive, bool next) const
{
    ASSERT(!searchStr.empty());

    int p = INVALID_POSITION;

    if (pos < _text.length())
    {
        p = next ? _text.charForward(pos) : pos;

        p = _text.find(searchStr, caseSesitive, p);
        if (p == INVALID_POSITION)
            p = _text.find(searchStr, caseSesitive);
    }
    else
        p = _text.find(searchStr, caseSesitive);

    return p;
}

void Document::changeLines(int (Document::*lineOp)(int))
{
    ASSERT(lineOp);

    if (_selection < 0)
    {
        int start = findLineStart(_position), p = _position;
        setPositionLineColumn(start);

        setPositionLineColumn((this->*lineOp)(p));
    }
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
        setPositionLineColumn(start);

        int p = end, n = 0;

        do
        {
            p = (this->*lineOp)(p);
            p = findPreviousLine(p);
            ++n;
        } while (p != INVALID_POSITION && p >= start);

        end = start;
        while (--n > 0)
            end = findNextLine(end);

        end = findLineEnd(end);

        if (atStart)
            _selection = end;
        else
        {
            _selection = start;
            setPositionLineColumn(end);
        }

        _selectionMode = false;
        _topPosition = -1;
    }

    _modified = true;
}

int Document::indentLine(int pos)
{
    ASSERT(pos >= 0 && pos <= _text.length());

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
    ASSERT(pos >= 0 && pos <= _text.length());

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

int Document::commentLine(int pos)
{
    ASSERT(pos >= 0 && pos <= _text.length());

    int start = findLineStart(pos), p = start;
    unichar_t ch;

    ch = _text.charAt(p);
    while (ch == ' ' || ch == '\t')
    {
        p = _text.charForward(p);
        ch = _text.charAt(p);
    }

    _text.insert(start, STR("//"));
    return start;
}

int Document::uncommentLine(int pos)
{
    ASSERT(pos >= 0 && pos <= _text.length());

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
        }
    }

    return start;
}

void Document::determineDocumentType(bool fileExecutable)
{
    if (_filename.endsWith(STR(".c")) || _filename.endsWith(STR(".h")) || _filename.endsWith(STR(".cpp")) ||
        _filename.endsWith(STR(".hpp")))
        _documentType = DOCUMENT_TYPE_CPP;
    else if (_filename.endsWith(STR(".sh")) || _filename.endsWith(STR(".ksh")))
        _documentType = DOCUMENT_TYPE_SHELL;
    else if (_filename.endsWith(STR(".bat")) || _filename.endsWith(STR(".cmd")))
        _documentType = DOCUMENT_TYPE_BATCH;
    else if (_filename.endsWith(STR(".ps1")))
        _documentType = DOCUMENT_TYPE_POWERSHELL;
    else if (_filename.endsWith(STR(".xml")) || _filename.endsWith(STR(".xsd")) || _filename.endsWith(STR(".htm")) ||
             _filename.endsWith(STR(".html")) || _text.startsWith(STR("<?xml")))
        _documentType = DOCUMENT_TYPE_XML;
    else if (_filename.endsWith(STR(".py")))
        _documentType = DOCUMENT_TYPE_PYTHON;
    else if (_filename.endsWith(STR(".js")))
        _documentType = DOCUMENT_TYPE_JAVASCRIPT;
    else
        _documentType = DOCUMENT_TYPE_TEXT;

    if (_documentType == DOCUMENT_TYPE_TEXT && fileExecutable)
        _documentType = DOCUMENT_TYPE_SHELL;
}

// Editor

Editor::Editor(const Array<String>& args) :
    Application(args, STR("ev")), _commandLine(Document(this), NULL, NULL), _document(NULL), _lastDocument(NULL),
    _recordingMacro(false), _width(2), _height(2), _cursorLine(0), _cursorColumn(0),
    _charWidth(1), _charHeight(1), _brightBackground(true), _caseSesitive(true), _recentLocation(NULL),
    _currentSuggestion(INVALID_POSITION), _trimWhitespace(true)
{
#ifdef PLATFORM_WINDOWS
    _unicodeLimit16 = true;
#else
    String term = Environment::getVariable(STR("TERM"));
    _unicodeLimit16 = term.contains(STR("xterm"));
#endif
}

SyntaxHighlighter* Editor::syntaxHighlighter(DocumentType documentType)
{
    for (auto node = _syntaxHighlighters.first(); node; node = node->next)
        if (node->value->documentType() == documentType)
            return node->value.ptr();

    if (documentType == DOCUMENT_TYPE_CPP)
        _syntaxHighlighters.addLast(createUnique<CppSyntaxHighlighter>());
    else if (documentType == DOCUMENT_TYPE_SHELL)
        _syntaxHighlighters.addLast(createUnique<ShellSyntaxHighlighter>());
    else if (documentType == DOCUMENT_TYPE_XML)
        _syntaxHighlighters.addLast(createUnique<XmlSyntaxHighlighter>());
    else
        return NULL;

    return _syntaxHighlighters.last()->value.ptr();
}

void Editor::newDocument(const String& filename)
{
    ASSERT(!filename.empty());

    _documents.addLast(Document(this));
    _document = _documents.last();

    _document->value.setDimensions(1, 1, _width, _height - 1);
    _document->value.filename(filename);
}

void Editor::openDocument(const String& filename)
{
    ASSERT(!filename.empty());

    Document doc(this);
    doc.setDimensions(1, 1, _width, _height - 1);

    try
    {
        doc.open(filename);

        _documents.addLast(doc);
        _document = _documents.last();

        findUniqueWords();
    }
    catch (Exception& ex)
    {
        _message = ex.message();
    }
}

void Editor::saveDocument()
{
    if (_document)
    {
        try
        {
            if (_document->value.modified())
                _document->value.save();
        }
        catch (Exception& ex)
        {
            _message = ex.message();
        }

        findUniqueWords();
    }
}

void Editor::saveAllDocuments()
{
    for (auto doc = _documents.first(); doc; doc = doc->next)
    {
        try
        {
            if (doc->value.modified())
                doc->value.save();
        }
        catch (Exception& ex)
        {
            _message = ex.message();
        }
    }

    findUniqueWords();
}

void Editor::closeDocument()
{
    if (_document)
    {
        auto doc = _document->next;
        _documents.remove(_document);
        _document = doc;

        findUniqueWords();
    }
}

bool Editor::start()
{
    for (int i = 1; i < _args.size(); ++i)
    {
        if (_args[i] == STR("--version"))
        {
#ifdef GUI_MODE
            showMessage(
#else
            Console::writeLine(
#endif
                STR("ev text editor version 2.0\n"
                    "web: evtext.org\n"
                    "Copyright (C) Andrey Levichev, 2019\n\n"
                    "usage: ev [OPTIONS] [FILE]...\n\n"
                    "OPTIONS:\n\n"
                    "--version - print version information and exit\n"
                    "--dark - assume dark screen background\n"
                    "--bright - assume bright screen background\n"));

            return false;
        }
        else if (_args[i] == STR("--bright"))
            _brightBackground = true;
        else if (_args[i] == STR("--dark"))
            _brightBackground = false;
        else
            openDocument(_args[i]);
    }

    _document = _documents.first();
    return true;
}

void Editor::onCreate()
{
#ifdef GUI_MODE
    _graphics.create(_window);

    Size scrSize = _graphics->size();

    TextBlock textBlock = _graphics->createTextBlock(GUI_FONT_NAME, GUI_FONT_SIZE, false, STR("w"), scrSize, true);
    Size chrSize = textBlock.size();

    _charWidth = chrSize.width;
    _charHeight = chrSize.height;
    ASSERT(_charWidth > 0 && _charHeight > 0);

    _width = scrSize.width / _charWidth;
    _height = scrSize.height / _charHeight;
#else
    _brightBackground = Console::brightBackground();
    Console::getSize(_width, _height);

    Console::setLineMode(false);
#endif

    setDimensions();
}

void Editor::onDestroy()
{
#ifdef GUI_MODE
    _graphics.reset();
#else
    Console::setLineMode(true);
    Console::clear();
#endif
}

void Editor::onPaint()
{
    updateScreen(true);
}

void Editor::onResize(int width, int height)
{
#ifdef GUI_MODE
    _graphics->resize(width, height);

    Size size = _graphics->size();
    _width = size.width / _charWidth;
    _height = size.height / _charHeight;
#else
    _width = width;
    _height = height;
#endif

    setDimensions();
}

void Editor::onInput(const Array<InputEvent>& inputEvents)
{
    bool update = false, modified = false;
    bool autocomplete = false, redrawAll = false;
    bool multipleInputEvents = inputEvents.size() > 1;

    for (int i = 0; i < inputEvents.size(); ++i)
    {
        InputEvent event = inputEvents[i];

        if (_document)
        {
            Document& doc = _document->value;

            if (event.eventType == INPUT_EVENT_TYPE_KEY)
            {
                KeyEvent keyEvent = event.event.keyEvent;

                if (_recordingMacro)
                {
                    if (!(keyEvent.alt && (keyEvent.ch == 'r' || keyEvent.ch == 'm')))
                        _macro.addLast(event);
                }

                if (keyEvent.ctrl)
                {
                    if (keyEvent.key == KEY_LEFT || keyEvent.ch == 'b')
                    {
                        update = doc.moveWordBack();
                    }
                    else if (keyEvent.key == KEY_RIGHT || keyEvent.ch == 'w')
                    {
                        update = doc.moveWordForward();
                    }
                    else if (keyEvent.key == KEY_UP || keyEvent.ch == '^')
                    {
                        update = moveToPrevRecentLocation();
                    }
                    else if (keyEvent.key == KEY_DOWN)
                    {
                        update = moveToNextRecentLocation();
                    }
                    else if (keyEvent.key == KEY_DELETE || keyEvent.ch == 'd')
                    {
                        modified = update = doc.deleteWordForward();
                    }
                    else if (keyEvent.key == KEY_BACKSPACE || keyEvent.ch == ']')
                    {
                        modified = update = doc.deleteWordBack();
                    }
                    else if (keyEvent.key == KEY_PGUP)
                    {
                        update = doc.moveLines(20);
                    }
                    else if (keyEvent.key == KEY_PGDN)
                    {
                        update = doc.moveLines(20);
                    }
                    else if (keyEvent.key == KEY_HOME)
                    {
                        update = doc.moveToStart();
                    }
                    else if (keyEvent.key == KEY_END)
                    {
                        update = doc.moveToEnd();
                    }
                    else if (keyEvent.ch == 't')
                    {
                        doc.insertChar(0x9);
                        update = modified = true;
                    }
                    else if (keyEvent.ch == 'h')
                    {
                        update = doc.moveToLineStart();
                    }
                    else if (keyEvent.ch == 'e')
                    {
                        update = doc.moveToLineEnd();
                    }
                    else if (keyEvent.ch == 'p')
                    {
                        update = doc.moveLines(-(_height - 1));
                    }
                    else if (keyEvent.ch == 'n')
                    {
                        update = doc.moveLines(_height - 1);
                    }
                    else if (keyEvent.ch == 'g')
                    {
                        _searchStr = doc.currentWord();

                        if (!_searchStr.empty())
                        {
                            _caseSesitive = true;
                            update = doc.find(_searchStr, _caseSesitive, true);
                        }
                    }
                    else if (keyEvent.ch == 'f')
                    {
                        if (!_searchStr.empty())
                            update = doc.find(_searchStr, _caseSesitive, true);
                    }
                    else if (keyEvent.ch == 'r')
                    {
                        if (!_searchStr.empty())
                            modified = update = doc.replace(_searchStr, _replaceStr, _caseSesitive);
                    }
                    else if (keyEvent.ch == 'a')
                    {
                        doc.markSelection();
                    }
                    else if (keyEvent.ch == 'x')
                    {
                        _buffer = doc.copyDeleteText(false);
                        copyToClipboard(_buffer);

                        if (!_buffer.empty())
                            modified = update = true;
                    }
                    else if (keyEvent.ch == 'c' || keyEvent.ch == 'k')
                    {
                        _buffer = doc.copyDeleteText(true);
                        copyToClipboard(_buffer);
                    }
                    else if (keyEvent.ch == 'v' || keyEvent.ch == 'l')
                    {
                        pasteFromClipboard(_buffer);

                        if (!_buffer.empty())
                        {
                            doc.pasteText(_buffer);
                            modified = update = true;
                        }
                    }
                }
                else if (keyEvent.alt)
                {
                    if (keyEvent.key == KEY_LEFT)
                    {
                        update = doc.moveWordBack();
                    }
                    else if (keyEvent.key == KEY_RIGHT)
                    {
                        update = doc.moveWordForward();
                    }
                    else if (keyEvent.key == KEY_UP)
                    {
                        update = moveToPrevRecentLocation();
                    }
                    else if (keyEvent.key == KEY_DOWN)
                    {
                        update = moveToNextRecentLocation();
                    }
                    else if (keyEvent.key == KEY_DELETE)
                    {
                        modified = update = doc.deleteWordForward();
                    }
                    else if (keyEvent.key == KEY_BACKSPACE)
                    {
                        modified = update = doc.deleteWordBack();
                    }
                    else if (keyEvent.key == KEY_PGUP || keyEvent.ch == 'p')
                    {
                        update = doc.moveLines(-20);
                    }
                    else if (keyEvent.key == KEY_PGDN || keyEvent.ch == 'n')
                    {
                        update = doc.moveLines(20);
                    }
                    else if (keyEvent.key == KEY_HOME || keyEvent.ch == 'h')
                    {
                        update = doc.moveToStart();
                    }
                    else if (keyEvent.key == KEY_END || keyEvent.ch == 'e')
                    {
                        update = doc.moveToEnd();
                    }
                    else if (keyEvent.ch == 'a')
                    {
                        update = doc.toggleSelectionStart();
                    }
                    else if (keyEvent.ch == 'b')
                    {
                        update = doc.moveCharsBack();
                    }
                    else if (keyEvent.ch == 'w')
                    {
                        update = doc.moveCharsForward();
                    }
                    else if (keyEvent.ch == 'd')
                    {
                        modified = update = doc.deleteCharsForward();
                    }
                    else if (keyEvent.ch == ']')
                    {
                        modified = update = doc.deleteCharsBack();
                    }
                    else if (keyEvent.ch == ',')
                    {
                        auto doc = _document->prev ? _document->prev : _documents.last();

                        if (doc != _document)
                        {
                            _document = doc;
                            update = true;
                        }
                    }
                    else if (keyEvent.ch == '.')
                    {
                        auto doc = _document->next ? _document->next : _documents.first();

                        if (doc != _document)
                        {
                            _document = doc;
                            update = true;
                        }
                    }
                    else if (keyEvent.ch == '/')
                    {
                        doc.commentLines();
                        modified = update = true;
                    }
                    else if (keyEvent.ch == '\\')
                    {
                        doc.uncommentLines();
                        modified = update = true;
                    }
                    else if (keyEvent.ch == '\'')
                    {
                        findUniqueWords();
                        updateScreen(true);
                    }
                    else if (keyEvent.ch >= '0' && keyEvent.ch <= '9')
                    {
                        int n = keyEvent.ch == '0' ? 10 : keyEvent.ch - '0';

                        for (auto doc = _documents.first(); doc; doc = doc->next)
                        {
                            if (--n == 0)
                            {
                                if (doc != _document)
                                {
                                    _document = doc;
                                    update = true;
                                }
                                break;
                            }
                        }
                    }
                    else if (keyEvent.ch == 'r')
                    {
                        if (_recordingMacro)
                            _recordingMacro = false;
                        else
                        {
                            _recordingMacro = true;
                            _macro.clear();
                        }

                        update = true;
                    }
                    else if (keyEvent.ch == 'm')
                    {
                        if (!_recordingMacro && _macro.size() > 0)
                        {
                            multipleInputEvents = false;
                            onInput(_macro);
                        }
                    }
                }
                else if (keyEvent.key == KEY_F2)
                {
                    showCommandLine();
                    update = true;
                }
                else if (keyEvent.key == KEY_F5)
                {
                    buildProject(PROJECT_COMMAND_BUILD);
                    updateScreen(true);
                }
                else if (keyEvent.key == KEY_F6)
                {
                    buildProject(PROJECT_COMMAND_CLEAN);
                    updateScreen(true);
                }
                else if (keyEvent.key == KEY_F8)
                {
                    saveDocument();
                    update = true;
                }
                else if (keyEvent.key == KEY_F9)
                {
                    saveAllDocuments();
                    update = true;
                }
                else if (keyEvent.key == KEY_F10)
                {
                    saveAllDocuments();
                    destroyWindow();
                    return;
                }
                else if (keyEvent.key == KEY_LEFT)
                {
                    update = doc.moveBack();
                }
                else if (keyEvent.key == KEY_RIGHT)
                {
                    update = doc.moveForward();
                }
                else if (keyEvent.key == KEY_UP)
                {
                    update = doc.moveLines(-1);
                }
                else if (keyEvent.key == KEY_DOWN)
                {
                    update = doc.moveLines(1);
                }
                else if (keyEvent.key == KEY_BACKSPACE)
                {
                    modified = update = doc.deleteCharBack();

                    if (_currentSuggestion != INVALID_POSITION)
                    {
                        _currentSuggestion = INVALID_POSITION;
                        if (completeWord(1))
                            autocomplete = true;
                    }
                }
                else if (keyEvent.key == KEY_DELETE)
                {
                    modified = update = doc.deleteCharForward();
                }
                else if (keyEvent.key == KEY_HOME)
                {
                    update = doc.moveToLineStart();
                }
                else if (keyEvent.key == KEY_END)
                {
                    update = doc.moveToLineEnd();
                }
                else if (keyEvent.key == KEY_PGUP)
                {
                    redrawAll = update = doc.moveLines(-(_height - 1));
                }
                else if (keyEvent.key == KEY_PGDN)
                {
                    redrawAll = update = doc.moveLines(_height - 1);
                }
                else if (keyEvent.key == KEY_ENTER)
                {
                    if (_document == &_commandLine)
                    {
                        _document = _lastDocument;
                        update = true;

                        try
                        {
                            if (!_commandLine.value.text().empty())
                            {
                                if (!processCommand(_commandLine.value.text()))
                                {
                                    destroyWindow();
                                    return;
                                }
                            }
                        }
                        catch (Exception& ex)
                        {
                            _message = ex.message();
                        }
                    }
                    else
                    {
                        if (multipleInputEvents)
                            doc.insertChar('\n');
                        else
                            doc.insertNewLine();
                        modified = update = true;
                    }
                }
                else if (keyEvent.key == KEY_TAB)
                {
                    if (keyEvent.shift)
                    {
                        if (completeWord(-1))
                            autocomplete = true;
                        else
                            doc.unindentLines();
                    }
                    else
                    {
                        if (completeWord(1))
                            autocomplete = true;
                        else
                            doc.indentLines();
                    }

                    modified = update = true;
                }
                else if (keyEvent.key == KEY_ESC)
                {
                    if (_document == &_commandLine)
                    {
                        _document = _lastDocument;
                        update = true;
                    }
                    else
                    {
                        destroyWindow();
                        return;
                    }
                }
                else if (charIsPrint(keyEvent.ch))
                {
                    if (_currentSuggestion == INVALID_POSITION)
                        doc.insertChar(keyEvent.ch);
                    else
                    {
                        if (charIsWord(keyEvent.ch))
                        {
                            doc.insertChar(keyEvent.ch);
                            if (completeWord(0))
                                autocomplete = true;
                        }
                        else
                        {
                            doc.insertChar(keyEvent.ch, true);
                            _uniqueWords[_suggestions[_currentSuggestion].word] = INT_MAX;
                            _currentSuggestion = INVALID_POSITION;
                        }
                    }

                    modified = update = true;
                }
            }
            else if (event.eventType == INPUT_EVENT_TYPE_MOUSE)
            {
                MouseEvent mouseEvent = event.event.mouseEvent;

                if (mouseEvent.buttonDown)
                {
                    if (mouseEvent.button == MOUSE_BUTTON_PRIMARY)
                        update = doc.moveToLineColumn(doc.top() + mouseEvent.y - 1, doc.left() + mouseEvent.x - 1);
                    else if (mouseEvent.button == MOUSE_BUTTON_WHEEL_UP)
                        update = doc.moveLines(-20);
                    else if (mouseEvent.button == MOUSE_BUTTON_WHEEL_DOWN)
                        update = doc.moveLines(20);
                }
            }
        }
        else
        {
            if (event.eventType == INPUT_EVENT_TYPE_KEY)
            {
                KeyEvent keyEvent = event.event.keyEvent;

                if (keyEvent.key == KEY_F2)
                {
                    showCommandLine();
                    update = true;
                }
                else if (keyEvent.key == KEY_ESC)
                {
                    if (_document == &_commandLine)
                    {
                        _document = _lastDocument;
                        update = true;
                    }
                    else
                    {
                        destroyWindow();
                        return;
                    }
                }
            }
        }
    }

    if (update)
    {
        if (_currentSuggestion != INVALID_POSITION && !autocomplete)
            _currentSuggestion = INVALID_POSITION;
        updateScreen(redrawAll);
    }

    if (modified && _document != &_commandLine)
        updateRecentLocations();
}

void Editor::setDimensions()
{
    if (_width < 2)
        _width = 2;

    if (_height < 2)
        _height = 2;

    _screen.assign(_width * _height, ScreenCell());
    _commandLine.value.setDimensions(2, _height, _width - 1, 1);

    for (auto doc = _documents.first(); doc; doc = doc->next)
        doc->value.setDimensions(1, 1, _width, _height - 1);
}

void Editor::updateScreen(bool redrawAll)
{
    int line, col;

    _prevScreen = _screen;

    if (_document)
    {
        if (_document == &_commandLine)
            _screen[(_height - 1) * _width].ch = ':';
        else
            updateStatusLine();

        Document& doc = _document->value;
        doc.draw(_width, _screen, _unicodeLimit16);

        line = doc.line() - doc.top() + doc.y();
        col = doc.column() - doc.left() + doc.x();
    }
    else
    {
        _screen.assign(_width * _height, ScreenCell());
        updateStatusLine();
        line = col = 1;
    }

#ifdef PLATFORM_WINDOWS

#ifdef GUI_MODE
    _graphics->beginDraw();
#else
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    ASSERT(handle);

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    BOOL rc = GetConsoleScreenBufferInfo(handle, &csbi);
    ASSERT(rc);

    COORD size;
    size.X = _width;
    size.Y = _height;

    if (!redrawAll)
    {
        int changed = 0;

        for (int j = 0; j < _height; ++j)
        {
            int jw = j * _width, start = jw, end = start + _width - 1;

            while (start <= end && _screen[start] == _prevScreen[start])
                ++start;

            while (start <= end && _screen[end] == _prevScreen[end])
                --end;

            if (start <= end)
                ++changed;
        }

        redrawAll = changed > 2;
    }
#endif

#else
    Console::showCursor(false);
#endif

    if (redrawAll)
    {
#ifdef PLATFORM_WINDOWS

#ifdef GUI_MODE
        _graphics->clear();

        for (int j = 0; j < _height; ++j)
        {
            int jw = j * _width, start = jw, end = start + _width;
            int color = -1, ii = 0;

            _output.clear();

            for (int i = start; i < end; ++i)
            {
                if (color != _screen[i].color)
                {
                    if (color >= 0)
                    {
                        Rect rect = { ii * _charWidth, j * _charHeight, (i - jw) * _charWidth, (j + 1) * _charHeight };
                        _graphics->drawText(GUI_FONT_NAME, GUI_FONT_SIZE, _output, rect, rgbColors[color]);

                        _output.clear();
                        ii = i - jw;
                    }

                    color = _screen[i].color;
                }

                _output += _screen[i].ch;
            }

            Rect rect = { ii * _charWidth, j * _charHeight, _width * _charWidth, (j + 1) * _charHeight };
            _graphics->drawText(GUI_FONT_NAME, GUI_FONT_SIZE, _output, rect, rgbColors[color]);
        }
#else
        SMALL_RECT rect;
        rect.Top = csbi.srWindow.Top;
        rect.Left = csbi.srWindow.Left;
        rect.Bottom = rect.Top + _height - 1;
        rect.Right = rect.Left + _width - 1;

        rc = WriteConsoleOutput(handle, reinterpret_cast<CHAR_INFO*>(_screen.values()), size, { 0, 0 }, &rect);
        ASSERT(rc);
#endif

#else
        for (int j = 0; j < _height; ++j)
        {
            int jw = j * _width, start = jw, end = start + _width;
            int color = -1;

            _output.clear();

            for (int i = start; i < end; ++i)
            {
                if (color != _screen[i].color)
                {
                    color = _screen[i].color;
                    _output.appendFormat(STR("\x1b[0;%dm"), color);
                }

                if (_screen[i].ch)
                    _output += _screen[i].ch;
                else
                {
                    _output += STR("\x1b[K");
                    break;
                }
            }

            Console::write(j + 1, 1, _output);
        }
#endif
    }
    else
    {
        for (int j = 0; j < _height; ++j)
        {
            int jw = j * _width, start = jw, end = start + _width - 1;
            int color = -1;

            _output.clear();

            while (start <= end && _screen[start] == _prevScreen[start])
                ++start;

            while (start <= end && _screen[end] == _prevScreen[end])
                --end;

            if (start <= end)
            {
#ifdef PLATFORM_WINDOWS

#ifdef GUI_MODE
                int ii = start - jw;

                for (int i = start; i <= end; ++i)
                {
                    if (color != _screen[i].color)
                    {
                        if (color >= 0)
                        {
                            Rect rect = { ii * _charWidth, j * _charHeight, (i - jw) * _charWidth, (j + 1) * _charHeight };
                            _graphics->fillRectangle(rect, GUI_BACKGROUND);
                            _graphics->drawText(GUI_FONT_NAME, GUI_FONT_SIZE, _output, rect, rgbColors[color]);

                            _output.clear();
                            ii = i - jw;
                        }

                        color = _screen[i].color;
                    }

                    _output += _screen[i].ch;
                }

                Rect rect = { ii * _charWidth, j * _charHeight, (end - jw + 1) * _charWidth, (j + 1) * _charHeight };
                _graphics->fillRectangle(rect, GUI_BACKGROUND);
                _graphics->drawText(GUI_FONT_NAME, GUI_FONT_SIZE, _output, rect, rgbColors[color]);
#else
                COORD pos;
                pos.X = start - jw;
                pos.Y = j;

                SMALL_RECT rect;
                rect.Top = rect.Bottom = csbi.srWindow.Top + pos.Y;
                rect.Left = csbi.srWindow.Left + pos.X;
                rect.Right = rect.Left + end - jw;

                rc = WriteConsoleOutput(handle, reinterpret_cast<CHAR_INFO*>(_screen.values()), size, pos, &rect);
                ASSERT(rc);
#endif

#else
                for (int i = start; i <= end; ++i)
                {
                    if (color != _screen[i].color)
                    {
                        color = _screen[i].color;
                        _output.appendFormat(STR("\x1b[0;%dm"), color);
                    }

                    if (_screen[i].ch)
                        _output += _screen[i].ch;
                    else
                    {
                        _output += STR("\x1b[K");
                        break;
                    }
                }

                Console::write(j + 1, start - jw + 1, _output);
#endif
            }
        }
    }

#ifdef PLATFORM_WINDOWS

#ifdef GUI_MODE
    if (_cursorLine > 0)
    {
        int i = (_cursorLine - 1) * _width + _cursorColumn - 1;
        _output = _screen[i].ch;

        Rect rect = { (_cursorColumn - 1) * _charWidth, (_cursorLine - 1) * _charHeight,
                    _cursorColumn * _charWidth, _cursorLine * _charHeight };

        _graphics->fillRectangle(rect, GUI_BACKGROUND);
        _graphics->drawText(GUI_FONT_NAME, GUI_FONT_SIZE, _output, rect, rgbColors[_screen[i].color]);
    }

    _cursorLine = line;
    _cursorColumn = col;

    _graphics->fillRectangle({ (col - 1) * _charWidth, (line - 1) * _charHeight,
        (col - 1) * _charWidth + 2, line * _charHeight }, 0);

    _graphics->endDraw();
#else
    Console::setCursorPosition(line, col);
#endif

#else
    Console::setCursorPosition(line, col);
    Console::showCursor(true);
#endif
}

void Editor::updateStatusLine()
{
    for (int p = (_height - 1) * _width; p < _screen.size(); ++p)
#if defined(PLATFORM_WINDOWS) && !defined(GUI_MODE)
        _screen[p].color = defaultBackground() | defaultForeground();
#else
        _screen[p].color = defaultForeground();
#endif

    if (!_message.empty())
    {
        int len = _message.charLength();
        int p = (_height - 1) * _width;

        if (len <= _width)
        {
            for (int i = 0; i < _message.length(); i = _message.charForward(i))
                _screen[p++].ch = _message.charAt(i);

            for (int i = 0; i < _width - len; ++i)
                _screen[p++].ch = ' ';
        }
        else
        {
            for (int i = 0, q = 0; i < _width - 3; ++i, q = _message.charForward(q))
                _screen[p++].ch = _message.charAt(q);

            _screen[p++].ch = '.';
            _screen[p++].ch = '.';
            _screen[p++].ch = '.';
        }

        _message.clear();
    }
    else if (_document)
    {
        Document& doc = _document->value;
        _status = doc.filename();

        if (doc.modified())
            _status += '*';

        if (_recordingMacro)
            _status += STR("  REC");

        int percent = doc.text().length() == 0 ? 100 : doc.position() * 100 / doc.text().length();

        _status += doc.encoding() == TEXT_ENCODING_UTF8 ? STR("  UTF-8") : STR("  UTF-16");
        _status += doc.crlf() ? STR("  CRLF") : STR("  LF");
        _status.appendFormat(STR("  %d, %d  %d%%"), doc.line(), doc.column(), percent);

        int len = _status.charLength();
        int p = (_height - 1) * _width;

        if (len <= _width)
        {
            for (int i = 0; i < _width - len; ++i)
                _screen[p++].ch = ' ';

            for (int i = 0; i < _status.length(); i = _status.charForward(i))
                _screen[p++].ch = _status.charAt(i);
        }
        else
        {
            _screen[p++].ch = '.';
            _screen[p++].ch = '.';
            _screen[p++].ch = '.';

            for (int i = _status.charBack(_status.length(), _width - 3); i < _status.length();
                 i = _status.charForward(i))
                _screen[p++].ch = _status.charAt(i);
        }
    }
}

void Editor::showCommandLine()
{
    if (_document != &_commandLine)
    {
        _lastDocument = _document;
        _document = &_commandLine;
        _commandLine.value.clear();
    }
}

void Editor::buildProject(Projectommand command)
{
    saveAllDocuments();

#ifndef GUI_MODE
    Console::setLineMode(true);
    Console::setColor(defaultForeground(), defaultBackground());
    Console::clear();
#endif

    String cmd;

    switch (command)
    {
    case PROJECT_COMMAND_BUILD:
        cmd = Environment::getVariable(STR("EV_MAKE_CMD"));
        if (cmd.empty())
#ifdef PLATFORM_WINDOWS
            cmd = STR("nmake.exe & pause");
#else
            cmd = STR("make");
#endif
        break;
    case PROJECT_COMMAND_CLEAN:
        cmd = Environment::getVariable(STR("EV_CLEAN_CMD"));
#ifdef PLATFORM_WINDOWS
            cmd = STR("nmake.exe clean & pause");
#else
            cmd = STR("make clean");
#endif
        break;
    default:
        throw Exception(STR("invalid project command"));
    }

    Environment::executeCommand(cmd);

#ifndef GUI_MODE
    Console::setLineMode(false);
#endif
}

bool Editor::processCommand(const String& command)
{
    ASSERT(!command.empty());

    if (command == STR("s"))
    {
        saveDocument();
        return true;
    }
    else if (command == STR("sa"))
    {
        saveAllDocuments();
        return true;
    }
    else if (command == STR("c"))
    {
        closeDocument();
        return true;
    }
    else if (command == STR("q"))
    {
        return false;
    }
    else if (command == STR("qs"))
    {
        saveAllDocuments();
        return false;
    }
    else if (command == STR("tw on"))
    {
        _trimWhitespace = true;
        return true;
    }
    else if (command == STR("tw off"))
    {
        _trimWhitespace = false;
        return true;
    }

    int p = 0;
    unichar_t ch = command.charAt(p);

    if (ch == 'f')
    {
        _caseSesitive = true;

        while (true)
        {
            p = command.charForward(p);
            ch = command.charAt(p);

            if (ch == 'i')
                _caseSesitive = false;
            else if (ch == ' ')
                break;
            else
                throw Exception(STR("invalid command"));
        }

        p = command.charForward(p);
        if (p < command.length())
        {
            _searchStr = command.substr(p);
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
            p = command.charForward(p);
            ch = command.charAt(p);

            if (ch == 'i')
                _caseSesitive = false;
            else if (ch == 'd' || ch == 'a')
                replaceScope = ch;
            else if (ch == 0)
                throw Exception(STR("invalid command"));
            else
                break;
        }

        p = command.charForward(p);
        if (p < command.length())
        {
            int q = command.find(ch, true, p);
            if (q == INVALID_POSITION)
            {
                _searchStr = command.substr(p);
                _replaceStr.clear();
            }
            else
            {
                _searchStr = command.substr(p, q - p);
                _replaceStr = command.substr(q + 1);
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
        p = command.charForward(p);
        if (command.charAt(p) == ' ')
        {
            p = command.charForward(p);
            int line = command.substr(p).toInt();

            if (line > 0)
                _document->value.moveToLine(line);
            else
                throw Exception(STR("invalid line number"));
        }
        else
            throw Exception(STR("invalid command"));
    }
    else if (ch == 'n')
    {
        p = command.charForward(p);
        if (command.charAt(p) == ' ')
        {
            p = command.charForward(p);
            String filename = command.substr(p);

            if (!filename.empty())
                newDocument(filename);
            else
                throw Exception(STR("invalid filename"));
        }
        else
            throw Exception(STR("invalid command"));
    }
    else if (ch == 'o')
    {
        p = command.charForward(p);
        if (command.charAt(p) == ' ')
        {
            p = command.charForward(p);
            String filename = command.substr(p);

            if (!filename.empty())
                openDocument(filename);
            else
                throw Exception(STR("invalid filename"));
        }
        else
            throw Exception(STR("invalid command"));
    }
    else
        throw Exception(STR("invalid command"));

    return true;
}

void Editor::updateRecentLocations()
{
    ListNode<RecentLocation>* node;
    _recentLocation = NULL;

    for (node = _recentLocations.first(); node; node = node->next)
    {
        if (node->value.document == _document && abs(node->value.line - _document->value.line()) <= 5)
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
        _recentLocations.addLast(RecentLocation(_document, _document->value.line()));

    if (_recentLocations.size() > 10)
        _recentLocations.removeFirst();
}

bool Editor::moveToNextRecentLocation()
{
    _recentLocation = _recentLocation && _recentLocation->next ? _recentLocation->next : _recentLocations.first();

    if (_recentLocation)
    {
        if (_recentLocation->value.document == _document &&
            abs(_recentLocation->value.line - _document->value.line()) <= 5)
        {
            _recentLocation = _recentLocation->next ? _recentLocation->next : _recentLocations.first();

            if (_recentLocation->value.document == _document &&
                abs(_recentLocation->value.line - _document->value.line()) <= 5)
                return false;
        }

        _document = _recentLocation->value.document;
        _document->value.moveToLine(_recentLocation->value.line);

        return true;
    }

    return false;
}

bool Editor::moveToPrevRecentLocation()
{
    _recentLocation = _recentLocation && _recentLocation->prev ? _recentLocation->prev : _recentLocations.last();

    if (_recentLocation)
    {
        if (_recentLocation->value.document == _document &&
            abs(_recentLocation->value.line - _document->value.line()) <= 5)
        {
            _recentLocation = _recentLocation->prev ? _recentLocation->prev : _recentLocations.last();

            if (_recentLocation->value.document == _document &&
                abs(_recentLocation->value.line - _document->value.line()) <= 5)
                return false;
        }

        _document = _recentLocation->value.document;
        _document->value.moveToLine(_recentLocation->value.line);

        return true;
    }

    return false;
}

void Editor::findUniqueWords()
{
    Map<String, int> words;

    for (auto doc = _documents.first(); doc; doc = doc->next)
    {
        const String& text = doc->value.text();
        String word;

        for (int p = 0; p < text.length(); p = text.charForward(p))
        {
            unichar_t ch = text.charAt(p);

            if (charIsWord(ch))
                word += ch;
            else if (!word.empty())
            {
                ++words[word];
                word.clear();
            }
        }

        if (!word.empty())
            ++words[word];
    }

    auto it = _uniqueWords.constIterator();
    while (it.moveNext())
    {
        if (it.value().value == INT_MAX)
        {
            int* value = words.find(it.value().key);
            if (value)
                *value = INT_MAX;
        }
    }

    swap(words, _uniqueWords);
}

void Editor::prepareSuggestions(const String& prefix)
{
    ASSERT(!prefix.empty());

    _suggestions.clear();

    auto it = _uniqueWords.constIterator();
    while (it.moveNext())
    {
        if (it.value().key.startsWith(prefix))
            _suggestions.addLast(AutocompleteSuggestion(it.value().key, it.value().value));
    }

    _suggestions.sort();
}

bool Editor::completeWord(int next)
{
    String prefix = _document->value.autocompletePrefix();

    if (prefix.length() > 0)
    {
        if (_currentSuggestion == INVALID_POSITION)
            prepareSuggestions(prefix);
        else if (!_suggestions[_currentSuggestion].word.startsWith(prefix))
        {
            prepareSuggestions(prefix);
            _currentSuggestion = INVALID_POSITION;
        }

        if (_suggestions.size() > 0)
        {
            int last = _suggestions.size() - 1;

            if (_currentSuggestion == INVALID_POSITION)
                _currentSuggestion = next >= 0 ? 0 : last;
            else
            {
                if (next > 0)
                    _currentSuggestion = _currentSuggestion < last ? _currentSuggestion + 1 : 0;
                else if (next < 0)
                    _currentSuggestion = _currentSuggestion > 0 ? _currentSuggestion - 1 : last;
            }

            _document->value.completeWord(_suggestions[_currentSuggestion].word.chars() + prefix.length());
        }

        return true;
    }

    return false;
}

void Editor::copyToClipboard(const String& text)
{
#ifdef PLATFORM_WINDOWS
    BOOL rc = OpenClipboard(NULL);
    ASSERT(rc);

    rc = EmptyClipboard();
    ASSERT(rc);

    ByteBuffer bytes = Unicode::stringToBytes(text, TEXT_ENCODING_UTF16_LE, false, true);

    HGLOBAL hText = GlobalAlloc(GMEM_MOVEABLE, bytes.size() + 2);
    ASSERT(hText);

    void* ptr = GlobalLock(hText);
    ASSERT(ptr);

    memcpy(ptr, bytes.values(), bytes.size());
    *(reinterpret_cast<wchar_t*>(ptr) + bytes.size() / 2) = 0;

    GlobalUnlock(hText);

    HANDLE hClip = SetClipboardData(CF_UNICODETEXT, hText);
    ASSERT(hClip);

    rc = CloseClipboard();
    ASSERT(rc);
#endif
}

void Editor::pasteFromClipboard(String& text)
{
#ifdef PLATFORM_WINDOWS
    if (IsClipboardFormatAvailable(CF_UNICODETEXT))
    {
        BOOL rc = OpenClipboard(NULL);
        ASSERT(rc);

        HGLOBAL hText = GetClipboardData(CF_UNICODETEXT);
        ASSERT(hText);

        void* ptr = GlobalLock(hText);
        ASSERT(ptr);

        TextEncoding encoding;
        bool bom, crLf;
        text = Unicode::bytesToString(wcslen(reinterpret_cast<wchar_t*>(ptr)) * 2, reinterpret_cast<byte_t*>(ptr),
                                      encoding, bom, crLf);

        GlobalUnlock(hText);

        rc = CloseClipboard();
        ASSERT(rc);
    }
    else
        text.clear();
#endif
}

void run(const Array<String>& args)
{
    Editor app(args);

    if (app.start())
        app.run();
}
