HEADERS=$(wildcard *.h)
SOURCES=$(wildcard *.cpp)

all: eve.gcc

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
