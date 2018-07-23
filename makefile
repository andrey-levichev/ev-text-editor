EDITOR_NAME=ev
HEADERS=foundation.h console.h file.h
SOURCES=foundation.cpp console.cpp file.cpp
TEST_HEADERS=test.h $(HEADERS)
TEST_SOURCES=test.cpp $(SOURCES)
EDITOR_HEADERS=editor.h $(HEADERS)
EDITOR_SOURCES=editor.cpp $(SOURCES)

TRASH=editor.dbg.vcpp.exe editor.vcpp.exe test.dbg.vcpp.exe test.vcpp.exe *.obj *.ilk *.pdb \
	editor.dbg.gcc editor.dbg.clang editor.dbg.sol editor.dbg.xlc \
	test.dbg.gcc test.dbg.clang test.dbg.sol test.dbg.xlc \
	editor editor.gcc editor.clang editor.sol editor.xlc \
	test.gcc test.clang test.sol test.xlc *.o

VCPP_OPTIONS=/nologo /std:c++latest /utf-8 /EHsc /W3 /wd4244 /wd4267 /wd4723 \
	/I. /DENABLE_ASSERT /D_UNICODE /DUNICODE /D_WIN32_WINNT=_WIN32_WINNT_WIN7 \
	/D_CRT_SECURE_NO_WARNINGS /D_CRT_NON_CONFORMING_SWPRINTFS /Fe:$@
VCPP_DEBUG_OPTIONS=/RTCsu /MTd /Zi
VCPP_RELEASE_OPTIONS=/MT /Ox
VCPP_LINKER_OPTIONS=/link user32.lib

GCC_OPTIONS=-Wall -o $@ -I. -Wno-unused-variable -Wno-unused-but-set-variable -lrt -static-libstdc++
GCC_DEBUG_OPTIONS=-g -DENABLE_ASSERT
GCC_RELEASE_OPTIONS=-O3

CLANG_OPTIONS=-std=gnu++14 -Wall -o $@ -I. -Wno-unused-variable
CLANG_DEBUG_OPTIONS=-g -DENABLE_ASSERT
CLANG_RELEASE_OPTIONS=-O3

SOL_OPTIONS=-m64 -std=c++11 +w -o $@ -I. -errtags=yes -erroff=arrowrtn2,wvarhidenmem
SOL_DEBUG_OPTIONS=-g -DENABLE_ASSERT
SOL_RELEASE_OPTIONS=-O3

XLC_OPTIONS=-q64 -qlanglvl=extended0x -o $@ -I.
XLC_DEBUG_OPTIONS=-g -DENABLE_ASSERT
XLC_RELEASE_OPTIONS=-O3 -qstrict=nans:infinities

all: editor.dbg.vcpp.exe

editor.dbg.vcpp.exe: $(EDITOR_HEADERS) $(EDITOR_SOURCES)
	cl $(VCPP_OPTIONS) $(VCPP_DEBUG_OPTIONS) $(EDITOR_SOURCES) $(VCPP_LINKER_OPTIONS)

editor.dbg.gcc: $(EDITOR_HEADERS) $(EDITOR_SOURCES)
	g++ $(GCC_OPTIONS) $(GCC_DEBUG_OPTIONS) $(EDITOR_SOURCES)

editor.dbg.clang: $(EDITOR_HEADERS) $(EDITOR_SOURCES)
	clang++ $(CLANG_OPTIONS) $(CLANG_DEBUG_OPTIONS) $(EDITOR_SOURCES)

editor.dbg.sol: $(EDITOR_HEADERS) $(EDITOR_SOURCES)
	CC $(SOL_OPTIONS) $(SOL_DEBUG_OPTIONS) $(EDITOR_SOURCES)

editor.dbg.xlc: $(EDITOR_HEADERS) $(EDITOR_SOURCES)
	xlC_r $(XLC_OPTIONS) $(XLC_DEBUG_OPTIONS) $(EDITOR_SOURCES)

editor.vcpp.exe: $(EDITOR_HEADERS) $(EDITOR_SOURCES)
	cl $(VCPP_OPTIONS) $(VCPP_RELEASE_OPTIONS) $(EDITOR_SOURCES) $(VCPP_LINKER_OPTIONS)

editor.gcc: $(EDITOR_HEADERS) $(EDITOR_SOURCES)
	g++ $(GCC_OPTIONS) $(GCC_RELEASE_OPTIONS) $(EDITOR_SOURCES)

editor.clang: $(EDITOR_HEADERS) $(EDITOR_SOURCES)
	clang++ $(CLANG_OPTIONS) $(CLANG_RELEASE_OPTIONS) $(EDITOR_SOURCES)

editor.sol: $(EDITOR_HEADERS) $(EDITOR_SOURCES)
	CC $(SOL_OPTIONS) $(SOL_RELEASE_OPTIONS) $(EDITOR_SOURCES)

editor.xlc: $(EDITOR_HEADERS) $(EDITOR_SOURCES)
	xlC_r $(XLC_OPTIONS) $(XLC_RELEASE_OPTIONS) $(EDITOR_SOURCES)

test.dbg.vcpp.exe: $(TEST_HEADERS) $(TEST_SOURCES)
	cl $(VCPP_OPTIONS) $(VCPP_DEBUG_OPTIONS) $(TEST_SOURCES) $(VCPP_LINKER_OPTIONS)

test.dbg.gcc: $(TEST_HEADERS) $(TEST_SOURCES)
	g++ $(GCC_OPTIONS) $(GCC_DEBUG_OPTIONS) $(TEST_SOURCES)

test.dbg.clang: $(TEST_HEADERS) $(TEST_SOURCES)
	clang++ $(CLANG_OPTIONS) $(CLANG_DEBUG_OPTIONS) $(TEST_SOURCES)

test.dbg.sol: $(TEST_HEADERS) $(TEST_SOURCES)
	CC $(SOL_OPTIONS) $(SOL_DEBUG_OPTIONS) $(TEST_SOURCES)

test.dbg.xlc: $(TEST_HEADERS) $(TEST_SOURCES)
	xlC_r $(XLC_OPTIONS) $(XLC_DEBUG_OPTIONS) $(TEST_SOURCES)

test.vcpp.exe: $(TEST_HEADERS) $(TEST_SOURCES)
	cl $(VCPP_OPTIONS) $(VCPP_RELEASE_OPTIONS) $(TEST_SOURCES) -DENABLE_ASSERT $(VCPP_LINKER_OPTIONS)

test.gcc: $(TEST_HEADERS) $(TEST_SOURCES)
	g++ $(GCC_OPTIONS) $(GCC_RELEASE_OPTIONS) $(TEST_SOURCES) -DENABLE_ASSERT

test.clang: $(TEST_HEADERS) $(TEST_SOURCES)
	clang++ $(CLANG_OPTIONS) $(CLANG_RELEASE_OPTIONS) $(TEST_SOURCES) -DENABLE_ASSERT

test.sol: $(TEST_HEADERS) $(TEST_SOURCES)
	CC $(SOL_OPTIONS) $(SOL_RELEASE_OPTIONS) $(TEST_SOURCES) -DENABLE_ASSERT

test.xlc: $(TEST_HEADERS) $(TEST_SOURCES)
	xlC_r $(XLC_OPTIONS) $(XLC_RELEASE_OPTIONS) $(TEST_SOURCES) -DENABLE_ASSERT

clean:
	-del $(TRASH)
	-rm $(TRASH)

depl.win: editor.vcpp.exe
	move /y editor.vcpp.exe $(EDITOR_NAME).exe
	"C:\Program Files\7-Zip\7z.exe" a $(EDITOR_NAME)-win.zip $(EDITOR_NAME).exe
	move /y $(EDITOR_NAME)-win.zip docs
	move /y $(EDITOR_NAME).exe C:\Apps

depl.mac: editor.clang
	mv editor.clang $(EDITOR_NAME)
	tar cJvf $(EDITOR_NAME)-mac.tar.bz2 $(EDITOR_NAME)
	mv $(EDITOR_NAME)-mac.tar.bz2 docs
	sudo mv $(EDITOR_NAME) /opt/bin

depl.linux: editor.gcc
	mv editor.gcc $(EDITOR_NAME)
	tar acvf $(EDITOR_NAME)-linux.tar.bz2 $(EDITOR_NAME)
	mv $(EDITOR_NAME)-linux.tar.bz2 docs
	mv $(EDITOR_NAME) ~/bin/Linux

depl.solaris: editor.sol
	mv editor.sol $(EDITOR_NAME)
	tar acvf $(EDITOR_NAME)-solaris.tar.bz2 $(EDITOR_NAME)
	mv $(EDITOR_NAME)-solaris.tar.bz2 docs
	mv $(EDITOR_NAME) ~/bin/SunOS

depl.aix: editor.xlc
	mv editor.xlc $(EDITOR_NAME)
	tar acvf $(EDITOR_NAME)-aix.tar.bz2 $(EDITOR_NAME)
	mv $(EDITOR_NAME)-aix.tar.bz2 docs
	mv $(EDITOR_NAME) ~/bin/AIX
