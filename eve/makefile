TARGET=eve
HEADERS=foundation.h console.h file.h test.h
SOURCES=foundation.cpp console.cpp file.cpp test.cpp
RM=$(if $(OS),del,rm)

all: $(TARGET).clang.exe

$(TARGET).vcpp.exe: $(HEADERS) $(SOURCES)
	cl /nologo /std:c++latest /EHsc /MT /Zi /W3 /wd4244 /wd4267 $(SOURCES) \
		/I. /DDEBUG /D_UNICODE /DUNICODE /D_WIN32_WINNT=_WIN32_WINNT_WIN7 \
		/D_CRT_SECURE_NO_WARNINGS /D_CRT_NON_CONFORMING_SWPRINTFS /link /out:$@

$(TARGET).gcc: $(HEADERS) $(SOURCES)
	g++ -g -Wall $(SOURCES) -o $@ -I. -DDEBUG

$(TARGET).mingw.exe: $(HEADERS) $(SOURCES)
	g++ -g -Wall $(SOURCES) -o $@ \
		-I. -DDEBUG -D_UNICODE -DUNICODE \
		-D_WIN32_WINNT=_WIN32_WINNT_WIN7 -municode

$(TARGET).clang: $(HEADERS) $(SOURCES)
	clang++ -std=gnu++14 -g -Wall $(SOURCES) -o $@ -I. -DDEBUG

$(TARGET).clang.exe: $(HEADERS) $(SOURCES)
	clang++ -std=gnu++14 -g -Wall $(SOURCES) -o $@ \
		-fno-color-diagnostics -fms-compatibility-version=19.10 -fexceptions \
		-I. -DDEBUG -D_UNICODE -DUNICODE \
		-D_WIN32_WINNT=_WIN32_WINNT_WIN7 \
		-D_CRT_SECURE_NO_WARNINGS -D_CRT_NON_CONFORMING_SWPRINTFS

$(TARGET).sol: $(HEADERS) $(SOURCES)
	CC -std=c++11 -g $(SOURCES) -o $@ -I. -DDEBUG

$(TARGET).xlc: $(HEADERS) $(SOURCES)
	xlC_r -qlanglvl=extended0x -g $(SOURCES) -o $@ -I. -DDEBUG

clean:
	-$(RM) $(TARGET).vcpp.exe $(TARGET).mingw.exe $(TARGET).clang.exe *.obj *.ilk *.pdb \
		$(TARGET).gcc $(TARGET).clang $(TARGET).sol $(TARGET).xlc *.o
