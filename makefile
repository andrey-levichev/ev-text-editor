HEADERS=foundation.h console.h file.h
SOURCES=foundation.cpp console.cpp file.cpp
TEST_HEADERS=test.h $(HEADERS)
TEST_SOURCES=test.cpp $(SOURCES)
EVE_HEADERS=eve.h $(HEADERS)
EVE_SOURCES=eve.cpp $(SOURCES)

TRASH=eve.dbg.vcpp.exe eve.vcpp.exe test.dbg.vcpp.exe test.vcpp.exe *.obj *.ilk *.pdb \
	eve.dbg.gcc eve.dbg.clang eve.dbg.sol eve.dbg.xlc \
	test.dbg.gcc test.dbg.clang test.dbg.sol test.dbg.xlc \
	eve.gcc eve.clang eve.sol eve.xlc \
	test.gcc test.clang test.sol test.xlc *.o

VCPP_OPTIONS=/nologo /std:c++latest /utf-8 /EHsc /W3 /wd4244 /wd4267 /wd4723 \
	/I. /DENABLE_ASSERT /D_UNICODE /DUNICODE /D_WIN32_WINNT=_WIN32_WINNT_WIN7 \
	/D_CRT_SECURE_NO_WARNINGS /D_CRT_NON_CONFORMING_SWPRINTFS /Fe:$@
VCPP_DEBUG_OPTIONS=/RTCsu /MTd /Zi
VCPP_RELEASE_OPTIONS=/MT /Ox

GCC_OPTIONS=-Wall -o $@ -I. -Wno-unused-variable -Wno-unused-but-set-variable -lrt
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

all: eve.gcc

eve.dbg.vcpp.exe: $(EVE_HEADERS) $(EVE_SOURCES)
	cl $(VCPP_OPTIONS) $(VCPP_DEBUG_OPTIONS) $(EVE_SOURCES)

eve.dbg.gcc: $(EVE_HEADERS) $(EVE_SOURCES)
	g++ $(GCC_OPTIONS) $(GCC_DEBUG_OPTIONS) $(EVE_SOURCES)

eve.dbg.clang: $(EVE_HEADERS) $(EVE_SOURCES)
	clang++ $(CLANG_OPTIONS) $(CLANG_DEBUG_OPTIONS) $(EVE_SOURCES)

eve.dbg.sol: $(EVE_HEADERS) $(EVE_SOURCES)
	CC $(SOL_OPTIONS) $(SOL_DEBUG_OPTIONS) $(EVE_SOURCES)

eve.dbg.xlc: $(EVE_HEADERS) $(EVE_SOURCES)
	xlC_r $(XLC_OPTIONS) $(XLC_DEBUG_OPTIONS) $(EVE_SOURCES)

eve.vcpp.exe: $(EVE_HEADERS) $(EVE_SOURCES)
	cl $(VCPP_OPTIONS) $(VCPP_RELEASE_OPTIONS) $(EVE_SOURCES)

eve.gcc: $(EVE_HEADERS) $(EVE_SOURCES)
	g++ $(GCC_OPTIONS) $(GCC_RELEASE_OPTIONS) $(EVE_SOURCES)

eve.clang: $(EVE_HEADERS) $(EVE_SOURCES)
	clang++ $(CLANG_OPTIONS) $(CLANG_RELEASE_OPTIONS) $(EVE_SOURCES)

eve.sol: $(EVE_HEADERS) $(EVE_SOURCES)
	CC $(SOL_OPTIONS) $(SOL_RELEASE_OPTIONS) $(EVE_SOURCES)

eve.xlc: $(EVE_HEADERS) $(EVE_SOURCES)
	xlC_r $(XLC_OPTIONS) $(XLC_RELEASE_OPTIONS) $(EVE_SOURCES)

test.dbg.vcpp.exe: $(TEST_HEADERS) $(TEST_SOURCES)
	cl $(VCPP_OPTIONS) $(VCPP_DEBUG_OPTIONS) $(TEST_SOURCES)

test.dbg.gcc: $(TEST_HEADERS) $(TEST_SOURCES)
	g++ $(GCC_OPTIONS) $(GCC_DEBUG_OPTIONS) $(TEST_SOURCES)

test.dbg.clang: $(TEST_HEADERS) $(TEST_SOURCES)
	clang++ $(CLANG_OPTIONS) $(CLANG_DEBUG_OPTIONS) $(TEST_SOURCES)

test.dbg.sol: $(TEST_HEADERS) $(TEST_SOURCES)
	CC $(SOL_OPTIONS) $(SOL_DEBUG_OPTIONS) $(TEST_SOURCES)

test.dbg.xlc: $(TEST_HEADERS) $(TEST_SOURCES)
	xlC_r $(XLC_OPTIONS) $(XLC_DEBUG_OPTIONS) $(TEST_SOURCES)

test.vcpp.exe: $(TEST_HEADERS) $(TEST_SOURCES)
	cl $(VCPP_OPTIONS) $(VCPP_RELEASE_OPTIONS) $(TEST_SOURCES) -DENABLE_ASSERT

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

depl.win: eve.vcpp.exe
	move /y eve.vcpp.exe eve.exe
	"C:\Program Files\7-Zip\7z.exe" a eve-win.zip eve.exe
	move /y eve-win.zip docs

depl.linux: eve.gcc
	mv eve.gcc eve
	tar acvf eve-linux.tar.bz2 eve
	mv eve-linux.tar.bz2 docs

depl.solaris: eve.sol
	mv eve.sol eve
	tar acvf eve-solaris.tar.bz2 eve
	mv eve-solaris.tar.bz2 docs

depl.aix: eve.xlc
	mv eve.xlc eve
	tar acvf eve-aix.tar.bz2 eve
	mv eve-aix.tar.bz2 docs