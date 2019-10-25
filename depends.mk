application.obj: application.cpp application.h foundation.h input.h \
 console.h
console.obj: console.cpp console.h foundation.h input.h
editor.obj: editor.cpp editor.h foundation.h application.h input.h file.h \
 graphics.h console.h
file.obj: file.cpp file.h foundation.h
foundation.obj: foundation.cpp foundation.h file.h console.h input.h
graphics.obj: graphics.cpp graphics.h foundation.h
input.obj: input.cpp input.h foundation.h
main.obj: main.cpp foundation.h console.h input.h
