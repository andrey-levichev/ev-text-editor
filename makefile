TARGET=eve
HEADERS=
SOURCES=eve.cpp

all: $(TARGET).mingw.exe

$(TARGET).vcpp.exe: $(HEADERS) $(SOURCES)
	cl /nologo /EHsc /MT /Zi /W3 /wd4996 /wd4267 /wd4244 $(SOURCES) \
		/I. /D_WIN32_WINNT=_WIN32_WINNT_WIN7 \
		/link /out:$@

$(TARGET).gcc: $(HEADERS) $(SOURCES)
	g++ -std=gnu++14 -g $(SOURCES) -o $@ -I.

$(TARGET).mingw.exe: $(HEADERS) $(SOURCES)
	g++ -std=gnu++14 -g $(SOURCES) -o $@ -I. \
		-D_WIN32_WINNT=_WIN32_WINNT_WIN7

$(TARGET).clang: $(HEADERS) $(SOURCES)
	clang++ -std=gnu++14 -g $(SOURCES) -o $@ -I.

$(TARGET).clang.exe: $(HEADERS) $(SOURCES)
	clang++ -std=gnu++14 -g $(SOURCES) -o $@ \
		-fno-color-diagnostics -fms-compatibility-version=19 -fexceptions \
		-I. -D_WIN32_WINNT=_WIN32_WINNT_WIN7 -D_CRT_SECURE_NO_WARNINGS

$(TARGET).solstudio: $(HEADERS) $(SOURCES)
	CC -std=c++11 -g $(SOURCES) -o $@ -I.

$(TARGET).xlc: $(HEADERS) $(SOURCES)
	xlC_r -qlanglvl=extended0x -g $(SOURCES) -o $@ -I.

clean:
	-rm $(TARGET).gcc $(TARGET).clang $(TARGET).solstudio $(TARGET).xlc

clean_win:
	-del *.exe *.o *.obj *.ilk *.pdb
