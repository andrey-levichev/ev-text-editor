windres editor.rc -O coff -o editor.res
g++ -o ev -O2 -DGUI_MODE -I. -municode -mwindows -static application.cpp console.cpp editor.cpp file.cpp foundation.cpp input.cpp main.cpp graphics.cpp editor.res -lole32 -ldwrite -ld2d1 -lwindowscodecs
