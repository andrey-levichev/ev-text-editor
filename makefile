all: editor.linux

editor.linux: editor.c
	gcc -std=gnu11 -g $^ -o $@

editor.solaris: editor.c
	cc -g $^ -o $@

editor.aix: editor.c
	xlc -g $^ -o $@

clean:
	-rm editor.linux editor.solaris editor.aix

depl:
	cp editor.linux ~/bin/Linux/e
	cp editor.solaris ~/bin/SunOS/e
	cp editor.aix ~/bin/AIX/e

