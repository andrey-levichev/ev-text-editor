APPLICATION_NAME=ev

FOUNDATION_HEADERS=foundation.h file.h application.h input.h console.h graphics.h
FOUNDATION_SOURCES=foundation.cpp file.cpp application.cpp input.cpp console.cpp graphics.cpp main.cpp

APPLICATION_HEADERS=$(FOUNDATION_HEADERS) editor.h
APPLICATION_SOURCES=$(FOUNDATION_SOURCES) editor.cpp

TRASH=*.exe *.obj *.ilk *.pdb *.gcc *.clang *.sol *.xlc *.o t.* *.log $(APPLICATION_NAME)

VCPP_OPTIONS=/nologo /std:c++latest /utf-8 /EHsc /W3 /wd4244 /wd4267 /wd4723 /wd4838 \
	/I. /D_UNICODE /DUNICODE /D_WIN32_WINNT=_WIN32_WINNT_WIN7 /DGUI_MODE \
	/D_CRT_SECURE_NO_WARNINGS /D_CRT_NON_CONFORMING_SWPRINTFS /Fe:$@
VCPP_LINKER_OPTIONS=/link user32.lib ole32.lib dwrite.lib d2d1.lib windowscodecs.lib

GCC_OPTIONS=-std=gnu++14 -Wall -o $@ -I. -Wno-unused-variable -Wno-unused-but-set-variable -lrt

CLANG_OPTIONS=-std=gnu++14 -Wall -o $@ -I. -Wno-unused-variable

SOL_OPTIONS=-std=c++11 +w -o $@ -I. -errtags=yes -erroff=arrowrtn2,wvarhidenmem

XLC_OPTIONS=-qlanglvl=extended0x -o $@ -I. -qsuppress=1540-0306 -qsuppress=1500-030

all: $(APPLICATION_NAME).dbg.vcpp.exe

$(APPLICATION_NAME).dbg.vcpp.exe: $(APPLICATION_HEADERS) $(APPLICATION_SOURCES) makefile
	cl $(VCPP_OPTIONS) /MT /O1 $(APPLICATION_SOURCES) $(VCPP_LINKER_OPTIONS)

$(APPLICATION_NAME).vcpp.exe: $(APPLICATION_HEADERS) $(APPLICATION_SOURCES) makefile
	cl $(VCPP_OPTIONS) /MT /O2 /GL /DDISABLE_ASSERT $(APPLICATION_SOURCES) $(VCPP_LINKER_OPTIONS) /LTCG

$(APPLICATION_NAME).dbg.gcc: $(APPLICATION_HEADERS) $(APPLICATION_SOURCES) makefile
	g++ $(GCC_OPTIONS) -Os $(APPLICATION_SOURCES)

$(APPLICATION_NAME).gcc: $(APPLICATION_HEADERS) $(APPLICATION_SOURCES) makefile
	g++ $(GCC_OPTIONS) -O3 -flto -DDISABLE_ASSERT $(APPLICATION_SOURCES)

$(APPLICATION_NAME).dbg.clang: $(APPLICATION_HEADERS) $(APPLICATION_SOURCES) makefile
	clang++ $(CLANG_OPTIONS) -Os $(APPLICATION_SOURCES)

$(APPLICATION_NAME).clang: $(APPLICATION_HEADERS) $(APPLICATION_SOURCES) makefile
	clang++ $(CLANG_OPTIONS) -O3 -flto -DDISABLE_ASSERT $(APPLICATION_SOURCES)

$(APPLICATION_NAME).dbg.sol: $(APPLICATION_HEADERS) $(APPLICATION_SOURCES) makefile
	CC $(SOL_OPTIONS) -O1 $(APPLICATION_SOURCES)

$(APPLICATION_NAME).sol: $(APPLICATION_HEADERS) $(APPLICATION_SOURCES) makefile
	CC $(SOL_OPTIONS) -O3 -xlinkopt -DDISABLE_ASSERT $(APPLICATION_SOURCES)

$(APPLICATION_NAME).dbg.xlc: $(APPLICATION_HEADERS) $(APPLICATION_SOURCES) makefile
	xlC_r $(XLC_OPTIONS) -O $(APPLICATION_SOURCES)

$(APPLICATION_NAME).xlc: $(APPLICATION_HEADERS) $(APPLICATION_SOURCES) makefile
	xlC_r $(XLC_OPTIONS) -O3 -DDISABLE_ASSERT $(APPLICATION_SOURCES)

clean:
	-del $(TRASH)
	-rm $(TRASH)

depl.win: $(APPLICATION_NAME).vcpp.exe
	move /y $(APPLICATION_NAME).vcpp.exe $(APPLICATION_NAME).exe
	"C:\Program Files\7-Zip\7z.exe" a $(APPLICATION_NAME)-win.zip $(APPLICATION_NAME).exe
	move /y $(APPLICATION_NAME)-win.zip web

depl.mac: $(APPLICATION_NAME).clang
	mv $(APPLICATION_NAME).clang $(APPLICATION_NAME)
	tar cJvf $(APPLICATION_NAME)-mac.tar.bz2 $(APPLICATION_NAME)
	mv $(APPLICATION_NAME)-mac.tar.bz2 web

depl.linux: $(APPLICATION_NAME).gcc
	mv $(APPLICATION_NAME).gcc $(APPLICATION_NAME)
	tar acvf $(APPLICATION_NAME)-linux.tar.bz2 $(APPLICATION_NAME)
	mv $(APPLICATION_NAME)-linux.tar.bz2 web

depl.solaris: $(APPLICATION_NAME).sol
	mv $(APPLICATION_NAME).sol $(APPLICATION_NAME)
	tar acvf $(APPLICATION_NAME)-solaris.tar.bz2 $(APPLICATION_NAME)
	mv $(APPLICATION_NAME)-solaris.tar.bz2 web

depl.aix: $(APPLICATION_NAME).xlc
	mv $(APPLICATION_NAME).xlc $(APPLICATION_NAME)
	tar acvf $(APPLICATION_NAME)-aix.tar.bz2 $(APPLICATION_NAME)
	mv $(APPLICATION_NAME)-aix.tar.bz2 web
