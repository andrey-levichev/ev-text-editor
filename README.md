# eve

eve is simple cross-platform text editor written in C++ with minimal dependencies. The executable is less than a hundred kilobytes. The editor does not use C++ standard library and instead provides its own simple base class library called foundation. The editor works in a text console or terminal on five platforms (Windows, macOS, Linux, Solaris, AIX). Compiles with Visual C++, gcc, clang, Solaris Studio and IBM XL C/C++. The code is under active development and may not be stable or even compile.

eve stands for "excellent visual editor", "extremely versatile editor" or "eradicate vi and emacs" whichever you prefer :-)

Supported features (run eve without parameters to see keyboard shortcuts):
* open/save documents
* multiple open documents
* basic text editing operations
* find/replace
* copy/delete/paste text blocks or lines
* indent/unindent line, autoindent
* toggle comment
* go to line
* ability to execute make and use eve as a simple IDE
* basic Unicode support (can read/write UTF-8 and UTF-16, aware of multibyte characters)
* autocomplete based on words in open documents

Known issues:

Not all keys work in all terminals, so some editor features may be unusable. This should be fixed soon.