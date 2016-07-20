HEADERS=$(wildcard *.hpp)
SOURCES=$(wildcard *.cpp)

all: eve.linux

eve.linux: $(HEADERS) $(SOURCES)
	g++ -std=gnu++14 -g $(SOURCES) -o $@ -I.

eve.solaris: $(HEADERS) $(SOURCES)
	CC -std=c++14 -g $(SOURCES) -o $@ -I.

eve.aix: $(HEADERS) $(SOURCES)
	xlC_r -qlanglvl=extended0x -g $(SOURCES) -o $@ -I.

clean:
	-rm eve.linux eve.solaris eve.aix

depl:
	cp eve.linux ~/bin/Linux/eve
	cp eve.solaris ~/bin/SunOS/eve
	cp eve.aix ~/bin/AIX/eve
