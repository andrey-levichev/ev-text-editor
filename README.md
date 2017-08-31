# eve

eve is simple cross-platform text editor written in C++ with minimal dependencies. The executable is less than a hundred kilobytes. The editor does not use C++ standard library and instead provides its own simple base class library called foundation. The editor works in a text console or terminal on five platforms (Windows, macOS, Linux, Solaris, AIX). Compiles with Visual C++, gcc, clang, Solaris Studio and IBM XL C/C++. The code is under active development and may not be stable or even compile.

eve stands for "excellent visual editor" or "eradicate vi and emacs" whichever you prefer :-)

Supported features:
* open/save documents
* basic text editing operations
* indent/unindent line, autoindent
* copy/delete/paste text blocks or lines
* multiple open documents
* basic find/replace
* go to line
* ability to execute make and use eve as a simple IDE
* basic Unicode support (can read/write UTF-8 and UTF-16, aware of multibyte characters)

Known issues:

Not all keys work in all terminals, so some editor features may be unusable. This should be fixed soon.