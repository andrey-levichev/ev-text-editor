SOURCES=eve.cpp Foundation.cpp

all: eve.linux

eve.linux: $(SOURCES)
	g++ -std=gnu++11 -Wall -O2 $^ -o $@ -I.

eve.solaris: $(SOURCES)
	CC -std=c++14 +w -O2 $^ -o $@ -I.

eve.aix: $(SOURCES)
	xlC_r -qflag=w -qlanglvl=extended0x -O2 $^ -o $@ -I.

clean:
	-rm eve.linux eve.solaris eve.aix

depl:
	cp eve.linux ~/bin/Linux/eve
	cp eve.solaris ~/bin/SunOS/eve
	cp eve.aix ~/bin/AIX/eve

