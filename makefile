HEADERS=$(wildcard *.h)
SOURCES=$(wildcard *.cpp)

all: eve.vcpp.exe

eve.vcpp.exe: $(HEADERS) $(SOURCES)
	cl /nologo /EHsc /MT /Zi /W3 /wd4996 /wd4267 /wd4244 test.cpp foundation.cpp \
		/I. /D_UNICODE /DUNICODE /D_WIN32_WINNT=_WIN32_WINNT_WIN7 \
		/link /out:$@

eve.gcc: $(HEADERS) $(SOURCES)
	g++ -std=gnu++14 -g $(SOURCES) -o $@ -I.

eve.clang: $(HEADERS) $(SOURCES)
	clang++ -std=gnu++14 -g $(SOURCES) -o $@ -I.

eve.solstudio: $(HEADERS) $(SOURCES)
	CC -std=c++11 -g $(SOURCES) -o $@ -I.

eve.xlc: $(HEADERS) $(SOURCES)
	xlC_r -qlanglvl=extended0x -g $(SOURCES) -o $@ -I.

clean:
	-rm eve.gcc eve.clang eve.solstudio eve.xlc

depl:
	cp eve.gcc ~/bin/Linux/eve
	cp eve.solstudio ~/bin/SunOS/eve
	cp eve.xlc ~/bin/AIX/eve
