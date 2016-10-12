TARGET=eve
HEADERS=test.h foundation.h
SOURCES=test.cpp foundation.cpp

all: $(TARGET).vcpp.exe

$(TARGET).vcpp.exe: $(HEADERS) $(SOURCES)
	cl /nologo /EHsc /MT /Zi /W3 /wd4244 /wd4267 $(SOURCES) \
		/I. /D_UNICODE /DUNICODE /D_WIN32_WINNT=_WIN32_WINNT_WIN7 \
		/D_CRT_SECURE_NO_WARNINGS /link /out:$@

$(TARGET).gcc: $(HEADERS) $(SOURCES)
	g++ -std=gnu++11 -g $(SOURCES) -o $@ -I.

$(TARGET).mingw.exe: $(HEADERS) $(SOURCES)
	g++ -std=gnu++11 -g -Wall $(SOURCES) -o $@ -I. \
		-D_WIN32_WINNT=_WIN32_WINNT_WIN7

$(TARGET).clang: $(HEADERS) $(SOURCES)
	clang++ -std=gnu++11 -g -Wall $(SOURCES) -o $@ -I.

$(TARGET).clang.exe: $(HEADERS) $(SOURCES)
	clang++ -std=gnu++11 -g -Wall $(SOURCES) -o $@ \
		-fno-color-diagnostics -fms-compatibility-version=19 -fexceptions \
		-I. -D_UNICODE -DUNICODE -D_WIN32_WINNT=_WIN32_WINNT_WIN7 -D_CRT_SECURE_NO_WARNINGS

$(TARGET).solstudio: $(HEADERS) $(SOURCES)
	CC -std=c++11 -g $(SOURCES) -o $@ -I.

$(TARGET).xlc: $(HEADERS) $(SOURCES)
	xlC_r -qlanglvl=extended0x -g $(SOURCES) -o $@ -I.

clean:
ifeq ($(OS),Windows_NT)
	-del $(TARGET).vcpp.exe $(TARGET).mingw.exe $(TARGET).clang.exe *.obj *.ilk *.pdb
else
	-rm $(TARGET).gcc $(TARGET).clang $(TARGET).solstudio $(TARGET).xlc *.o
endif
